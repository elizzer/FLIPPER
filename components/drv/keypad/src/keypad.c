#include "keypad.h"
#include "string.h"
#include "stdlib.h"
#include "PCF8574_IoExp.h"
#include "event_manager.h"


/* ─────────────────────────────────────────
 * Internal task parameter struct
 * kept private — not exposed in header
 * ───────────────────────────────────────── */
typedef struct {
    uint8_t          cid;
    keypad_handle_t *handle;
} keypad_task_params_t;

/* ─────────────────────────────────────────
 * Forward declarations
 * ───────────────────────────────────────── */
static void keypad_listen_task(void *pvParameters);

/* ─────────────────────────────────────────
 * keypad_listen_task
 * ───────────────────────────────────────── */
static void keypad_listen_task(void *pvParameters)
{
    keypad_task_params_t *params = (keypad_task_params_t *)pvParameters;
    keypad_handle_t      *handle = params->handle;
    uint8_t               cid    = params->cid;

    /* params was malloc'd in init — free it now
     * we have extracted what we need                */
    free(params);

    uint8_t buf_size = (handle->config.buttons / 8) + 1;

    printf("\nKeypad_listen_task started...");
    printf("\nKeypad_listen_task CID %d",cid);

    while (1)
    {
        wait_semaphore(EVENT_TYPE_IO_EXP, cid);
        printf("\nIO EXP event trigreed");
        /* read current port state from io expander */
        for (uint8_t port = 0; port < buf_size; port++)
        {
            uint8_t value;
            PCF8574_IoExp_readPort(handle->config.ioExp_handle,
                                   port, &value);
            handle->cur_raw[port] = value;
        }

        /* detect changes per port */
        for (uint8_t port = 0; port < buf_size; port++)
        {
            uint8_t changed = handle->prev_raw[port] ^
                              handle->cur_raw[port];

            for (uint8_t i = 0; i < 8; i++)
            {
                if ((changed >> i) & 0x01)
                {
                    uint8_t key_index = (port * 8) + i;

                    if (key_index >= handle->config.buttons)
                        break;

                    uint8_t new_state = (handle->cur_raw[port] >> i) & 0x01;

                    /* update raw state for this key */
                    handle->keys[key_index].raw_state =
                        (new_state == 0) ? KEY_PRESSED : KEY_RELEASED;

                    printf("Key %d changed to %s\n",
                           handle->keys[key_index].key_id,
                           (new_state == 0) ? "PRESSED" : "RELEASED");

                    /* TODO — feed into debounce timer here */
                }
            }

            /* update prev for next comparison */
            handle->prev_raw[port] = handle->cur_raw[port];
        }
    }
}

/* ─────────────────────────────────────────
 * keypad_init
 * ───────────────────────────────────────── */
int8_t keypad_init(keypad_config_t *config, keypad_handle_t *handle)
{
    if (config == NULL || handle == NULL)
    {
        printf("\nkeypad_init: null argument");
        return -1;
    }

    if (config->buttons == 0 || config->ioExp_handle == NULL)
    {
        printf("\nkeypad_init: invalid config");
        return -2;
    }

    /* copy config into handle — handle owns its own copy */
    handle->config = *config;

    /* allocate per key state array */
    handle->keys = (keypad_button_attrs_t *)malloc(
                       sizeof(keypad_button_attrs_t) * config->buttons);
    if (handle->keys == NULL)
    {
        printf("\nkeypad_init: keys malloc failed");
        return -3;
    }

    /* initialise each key */
    for (uint8_t i = 0; i < config->buttons; i++)
    {
        handle->keys[i].key_id       = i;
        handle->keys[i].stable_state = KEY_RELEASED;
        handle->keys[i].raw_state    = KEY_RELEASED;
        handle->keys[i].timer_state  = KEY_TIMER_NONE;
        handle->keys[i].timer        = NULL;
    }

    /* allocate raw port buffers */
    uint8_t buf_size = (config->buttons / 8) + 1;

    handle->prev_raw = (uint8_t *)malloc(buf_size);
    handle->cur_raw  = (uint8_t *)malloc(buf_size);

    if (handle->prev_raw == NULL || handle->cur_raw == NULL)
    {
        printf("\nkeypad_init: raw buffer malloc failed");
        free(handle->keys);
        handle->keys = NULL;
        return -4;
    }

    /* all pins high = all keys released (active low) */
    memset(handle->prev_raw, 0xFF, buf_size);
    memset(handle->cur_raw,  0xFF, buf_size);

    /* register for io exp event */
    if (register_event(EVENT_TYPE_IO_EXP, &handle->consumer_id) != 0)
    {
        printf("\nkeypad_init: event registration failed");
        free(handle->keys);
        free(handle->prev_raw);
        free(handle->cur_raw);
        handle->keys     = NULL;
        handle->prev_raw = NULL;
        handle->cur_raw  = NULL;
        return -5;
    }

    /* pack task parameters */
    keypad_task_params_t *params = (keypad_task_params_t *)malloc(
                                       sizeof(keypad_task_params_t));
    if (params == NULL)
    {
        printf("\nkeypad_init: params malloc failed");
        free(handle->keys);
        free(handle->prev_raw);
        free(handle->cur_raw);
        return -6;
    }

    params->cid    = handle->consumer_id;
    params->handle = handle;

    /* create listener task */
    BaseType_t ret = xTaskCreate(
        keypad_listen_task,
        "keypad_task",
        2048,
        (void *)params,
        5,
        &handle->task_handle
    );

    if (ret != pdPASS)
    {
        printf("\nkeypad_init: task creation failed");
        free(params);
        free(handle->keys);
        free(handle->prev_raw);
        free(handle->cur_raw);
        handle->keys     = NULL;
        handle->prev_raw = NULL;
        handle->cur_raw  = NULL;
        return -7;
    }

    printf("\nkeypad_init: success, %d keys registered", config->buttons);
    return 0;
}

/* ─────────────────────────────────────────
 * keypad_deinit
 * ───────────────────────────────────────── */
int8_t keypad_deinit(keypad_handle_t *handle)
{
    if (handle == NULL)
    {
        printf("\nkeypad_deinit: null handle");
        return -1;
    }

    /* delete listener task */
    if (handle->task_handle != NULL)
    {
        vTaskDelete(handle->task_handle);
        handle->task_handle = NULL;
    }

    /* delete per key timers */
    for (uint8_t i = 0; i < handle->config.buttons; i++)
    {
        if (handle->keys[i].timer != NULL)
        {
            xTimerDelete(handle->keys[i].timer, 0);
            handle->keys[i].timer = NULL;
        }
    }

    /* unregister from event manager */
    unregister_event(EVENT_TYPE_IO_EXP, handle->consumer_id);

    /* free all allocations */
    free(handle->keys);
    free(handle->prev_raw);
    free(handle->cur_raw);

    handle->keys     = NULL;
    handle->prev_raw = NULL;
    handle->cur_raw  = NULL;

    printf("\nkeypad_deinit: success");
    return 0;
}