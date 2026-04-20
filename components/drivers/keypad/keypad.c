#include "string.h"
#include "stdlib.h"
#include "event_manager.h"
#include "io_expander/PCF8574/PCF8574.h"
#include "keypad.h"
/* ─────────────────────────────────────────
 * Internal task parameter struct
 * kept private — not exposed in header
 * ───────────────────────────────────────── */
typedef struct
{
    uint8_t cid;
    keypad_handle_t *handle;
} keypad_task_params_t;

SemaphoreHandle_t debounce_timer_mutex;

/* ─────────────────────────────────────────
 * Forward declarations
 * ───────────────────────────────────────── */
static void keypad_listen_task(void *pvParameters);
void key_timer_cb(TimerHandle_t xTimer);

/* ─────────────────────────────────────────
 * keypad_listen_task
 * ───────────────────────────────────────── */
static void keypad_listen_task(void *pvParameters)
{
    keypad_task_params_t *params = (keypad_task_params_t *)pvParameters;
    keypad_handle_t *handle = params->handle;
    uint8_t cid = params->cid;

    /* params was malloc'd in init — free it now
     * we have extracted what we need                */
    free(params);

    uint8_t buf_size = (handle->config.buttons / 8) + 1;

    printf("\nKeypad_listen_task started...");
    printf("\nKeypad_listen_task CID %d", cid);

    while (1)
    {
        EventDescription_t ev_desc;
        event_manager_wait_event(EVENT_TYPE_IO_EXP, cid, &ev_desc);
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
                    handle->keys[key_index].raw_state = (new_state == 0) ? SW_DOWN : SW_UP;

                    // start debounce timer
                    // check the timer state
                    if (handle->keys[key_index].timer_state == IDEL)
                    {
                        handle->keys[key_index].timer_state = DEBOUNCE_TIMER;
                        // printf("\nStarting debounce timer for key %d\n", handle->keys[key_index].key_id);
                        handle->keys[key_index].timer = xTimerCreate("DebounceTimer", pdMS_TO_TICKS(20), pdFALSE, (void *)&handle->keys[key_index], key_timer_cb);
                        xTimerStart(handle->keys[key_index].timer, 0);
                    }
                    else if (handle->keys[key_index].timer_state == DEBOUNCE_TIMER)
                    {
                        // if the timer is already running, reset it
                        // printf("\nResetting debounce timer for key %d\n", handle->keys[key_index].key_id);
                        xTimerReset(handle->keys[key_index].timer, 0);
                    }
                    else if (handle->keys[key_index].timer_state == LONG_PRESS_TIMER)
                    {
                        // if the long press timer is running, stop it and start debounce timer
                        // printf("\nStopping long press timer for key %d\n", handle->keys[key_index].key_id);
                        xTimerStop(handle->keys[key_index].timer, 0);
                        handle->keys[key_index].timer_state = DEBOUNCE_TIMER;
                        xTimerChangePeriod(handle->keys[key_index].timer, pdMS_TO_TICKS(20), 0);
                        xTimerStart(handle->keys[key_index].timer, 0);
                    }

                    // EventDescription_t ev_desc = {
                    //     .event_source = 2,
                    //     .event_type = EVENT_TYPE_KEYPAD,
                    //     .keypad = {
                    //         .action = (new_state == 0) ? KEYPAD_PRESSED : KEY_RELEASED,
                    //         .key_number = handle->keys[key_index].key_id,
                    //     },
                    // };
                    // event_manager_post_event(ev_desc.event_type, ev_desc);

                    /* TODO — feed into debounce timer here */
                }
            }

            /* update prev for next comparison */
            handle->prev_raw[port] = handle->cur_raw[port];
        }
    }
}

void post_key_event(uint8_t key_id, keypad_action_t action)
{
    EventDescription_t ev_desc = {
        .event_source = 2,
        .event_type = EVENT_TYPE_KEYPAD,
        .keypad = {
            .action = action,
            .key_number = key_id,
        },
    };
    event_manager_post_event(ev_desc.event_type, ev_desc);
}

void key_timer_cb(TimerHandle_t xTimer)
{
    /* TODO — implement debounce timer callback */
    // get the key index from timer ID
    // last raw state is the debounced state

    // get the key from the key index
    keypad_button_attrs_t *key = (keypad_button_attrs_t *)pvTimerGetTimerID(xTimer);
    switch (key->timer_state)
    {
    case DEBOUNCE_TIMER:
        // means the debounce timer is expired and the switch is stable
        if (key->raw_state == SW_DOWN)
        {
            key->stable_state = KEY_PRESSED;
            // start long press timer
            key->timer_state = LONG_PRESS_TIMER;
            xTimerChangePeriod(key->timer, pdMS_TO_TICKS(1000), 0);
            xTimerStart(key->timer, 0);
            post_key_event(key->key_id, KEYPAD_PRESSED);
        }
        else
        {
            key->stable_state = KEY_RELEASED;
            post_key_event(key->key_id, KEYPAD_RELEASED);
            // stopr all timer and reset timer state to idle
            xTimerStop(key->timer, 0);
            key->timer_state = IDEL;
            // no need to start long press timer since the key is released
        }
        break;
    case LONG_PRESS_TIMER:
        // means the long press timer is expired and the key is still down, so it's a long press
        if (key->stable_state == KEY_PRESSED)
        {
            post_key_event(key->key_id, KEYPAD_LONG_PRESS);
            xTimerStop(key->timer, 0); // stop timer after firing
            key->timer_state = IDEL;   // reset state
        }

    default:
        break;
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
        handle->keys[i].key_id = i;
        handle->keys[i].stable_state = KEY_RELEASED;
        handle->keys[i].raw_state = KEY_RELEASED;
        handle->keys[i].gesture_state = KEY_GESTURE_NONE;
        handle->keys[i].timer = NULL;
        handle->keys[i].timer_state = IDEL;
        // handle->keys[i].long_press_tmr = NULL;
        // handle->keys[i].double_click_tmr = NULL;
    }

    /* allocate raw port buffers */
    uint8_t buf_size = (config->buttons / 8) + 1;

    handle->prev_raw = (uint8_t *)malloc(buf_size);
    handle->cur_raw = (uint8_t *)malloc(buf_size);

    if (handle->prev_raw == NULL || handle->cur_raw == NULL)
    {
        printf("\nkeypad_init: raw buffer malloc failed");
        free(handle->keys);
        handle->keys = NULL;
        return -4;
    }

    /* all pins high = all keys released (active low) */
    memset(handle->prev_raw, 0xFF, buf_size);
    memset(handle->cur_raw, 0xFF, buf_size);

    /* register for io exp event */
    if (event_manager_register_event(EVENT_TYPE_IO_EXP, &handle->consumer_id) != 0)
    {
        printf("\nkeypad_init: event registration failed");
        free(handle->keys);
        free(handle->prev_raw);
        free(handle->cur_raw);
        handle->keys = NULL;
        handle->prev_raw = NULL;
        handle->cur_raw = NULL;
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

    params->cid = handle->consumer_id;
    params->handle = handle;

    /* create listener task */
    BaseType_t ret = xTaskCreate(
        keypad_listen_task,
        "keypad_task",
        2048,
        (void *)params,
        5,
        &handle->task_handle);

    if (ret != pdPASS)
    {
        printf("\nkeypad_init: task creation failed");
        free(params);
        free(handle->keys);
        free(handle->prev_raw);
        free(handle->cur_raw);
        handle->keys = NULL;
        handle->prev_raw = NULL;
        handle->cur_raw = NULL;
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
    event_manager_unregister_event(EVENT_TYPE_IO_EXP, handle->consumer_id);

    /* free all allocations */
    free(handle->keys);
    free(handle->prev_raw);
    free(handle->cur_raw);

    handle->keys = NULL;
    handle->prev_raw = NULL;
    handle->cur_raw = NULL;

    printf("\nkeypad_deinit: success");
    return 0;
}