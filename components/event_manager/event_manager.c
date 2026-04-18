#include "event_manager.h"
#include <stdio.h>

/* One queue per (event_type, consumer) slot */
static QueueHandle_t consumer_queue_mapping[EVENT_TYPE_MAX][MAX_CONSUMER_PER_EVENT_TYPE];
static SemaphoreHandle_t mapping_mutex;

/* ------------------------------------------------------------------ */
int8_t event_manager_init(void)
{
    for (uint8_t i = 0; i < EVENT_TYPE_MAX; i++)
        for (uint8_t j = 0; j < MAX_CONSUMER_PER_EVENT_TYPE; j++)
            consumer_queue_mapping[i][j] = NULL;

    mapping_mutex = xSemaphoreCreateMutex();
    if (mapping_mutex == NULL)
    {
        printf("\nFailed to create mapping mutex");
        return -1;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
int8_t event_manager_register_event(EventType_t event_type, uint8_t *consumer_id)
{
    if (event_type >= EVENT_TYPE_MAX)
    {
        printf("\nInvalid event type");
        return -1;
    }

    xSemaphoreTake(mapping_mutex, portMAX_DELAY);

    for (uint8_t i = 0; i < MAX_CONSUMER_PER_EVENT_TYPE; i++)
    {
        if (consumer_queue_mapping[event_type][i] == NULL)
        {
            consumer_queue_mapping[event_type][i] =
                xQueueCreate(QUEUES_PER_EVENT, sizeof(EventDescription_t));

            if (consumer_queue_mapping[event_type][i] == NULL)
            {
                xSemaphoreGive(mapping_mutex);
                printf("\nFailed to create queue");
                return -1;
            }

            *consumer_id = i;
            xSemaphoreGive(mapping_mutex);
            return 0;
        }
    }

    xSemaphoreGive(mapping_mutex);
    printf("\nEvent consumer slots full");
    return -1;
}

/* ------------------------------------------------------------------ */
int8_t event_manager_unregister_event(EventType_t event_type, uint8_t consumer_id)
{
    if (event_type >= EVENT_TYPE_MAX)
    {
        printf("\nInvalid event type");
        return -1;
    }
    if (consumer_id >= MAX_CONSUMER_PER_EVENT_TYPE)
    {
        printf("\nInvalid consumer id");
        return -1;
    }

    xSemaphoreTake(mapping_mutex, portMAX_DELAY);

    if (consumer_queue_mapping[event_type][consumer_id] != NULL)
    {
        vQueueDelete(consumer_queue_mapping[event_type][consumer_id]);
        consumer_queue_mapping[event_type][consumer_id] = NULL;
    }

    xSemaphoreGive(mapping_mutex);
    return 0;
}

/* ------------------------------------------------------------------ */
int8_t event_manager_post_event(EventType_t event_type, EventDescription_t event_desc)
{
    if (event_type >= EVENT_TYPE_MAX)
    {
        printf("\nInvalid event type");
        return -1;
    }

    xSemaphoreTake(mapping_mutex, portMAX_DELAY);

    for (uint8_t i = 0; i < MAX_CONSUMER_PER_EVENT_TYPE; i++)
    {
        if (consumer_queue_mapping[event_type][i] != NULL)
        {
            /* 0 timeout: drop the event if the consumer's queue is full
               rather than blocking while holding the mutex               */
            if (xQueueSend(consumer_queue_mapping[event_type][i], &event_desc, 0) != pdTRUE)
            {
                printf("\nQueue full for consumer %d, event dropped", i);
            }
        }
    }

    xSemaphoreGive(mapping_mutex);
    return 0;
}

/* ------------------------------------------------------------------ */
int8_t event_manager_post_event_from_isr(EventType_t event_type, EventDescription_t event_desc)
{
    if (event_type >= EVENT_TYPE_MAX)
        return -1;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    for (uint8_t i = 0; i < MAX_CONSUMER_PER_EVENT_TYPE; i++)
    {
        if (consumer_queue_mapping[event_type][i] != NULL)
        {
            /* xQueueSendFromISR is the correct ISR-safe queue API */
            xQueueSendFromISR(consumer_queue_mapping[event_type][i],
                              &event_desc,
                              &xHigherPriorityTaskWoken);
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    return 0;
}

/* ------------------------------------------------------------------ */
int8_t event_manager_wait_event(EventType_t event_type, uint8_t consumer_id,
                                EventDescription_t *event_desc_out)
{
    if (event_type >= EVENT_TYPE_MAX)
    {
        printf("\nInvalid event type");
        return -1;
    }
    if (consumer_id >= MAX_CONSUMER_PER_EVENT_TYPE)
    {
        printf("\nInvalid consumer id");
        return -1;
    }
    if (consumer_queue_mapping[event_type][consumer_id] == NULL)
    {
        printf("\nQueue not registered");
        return -1;
    }

    /* Block forever until an event descriptor arrives */
    if (xQueueReceive(consumer_queue_mapping[event_type][consumer_id],
                      event_desc_out,
                      portMAX_DELAY) != pdTRUE)
    {
        return -1;
    }
    return 0;
}