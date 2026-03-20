#include "event_manager.h"
#include <stdio.h>
// right now need not worry about data transfer, just the keypad intr trigger

// create a semaphore for each event type

// each type of event can have 10 different semaphore
SemaphoreHandle_t consumer_sem_mapping[EVENT_TYPE_MAX][MAX_SEM_PER_EVENT_TYPE];
static SemaphoreHandle_t mapping_mutex;

int8_t event_manager_init()
{

    for (uint8_t i = 0; i < EVENT_TYPE_MAX; i++)
    {
        for (uint8_t j = 0; j < MAX_SEM_PER_EVENT_TYPE; j++)
        {
            consumer_sem_mapping[i][j] = NULL;
        }
    }
    mapping_mutex = xSemaphoreCreateMutex();
    return 0;
}

int8_t event_manager_register_event(EventType_t event_type, uint8_t *consumer_id)
{
    if (event_type >= EVENT_TYPE_MAX)
    {
        printf("\nInvalid event type");
        return -1;
    }
    xSemaphoreTake(mapping_mutex, portMAX_DELAY);

    for (uint8_t i = 0; i < MAX_SEM_PER_EVENT_TYPE; i++)
    {
        if (consumer_sem_mapping[event_type][i] == NULL)
        {
            consumer_sem_mapping[event_type][i] = xSemaphoreCreateBinary();
            *consumer_id = i;
            xSemaphoreGive(mapping_mutex);
            return 0;
        }
    }
    xSemaphoreGive(mapping_mutex);
    printf("\nEvent consumer full...");
    return -1;
}

int8_t event_manager_unregister_event(EventType_t event_type, uint8_t consumer_id)
{
    if (event_type >= EVENT_TYPE_MAX)
    {
        printf("\nInvalid event type");
        return -1;
    }
    if (consumer_id >= MAX_SEM_PER_EVENT_TYPE)
    {
        printf("\nInvalid consumer id. it is above alloed max value");
        return -1;
    }
    xSemaphoreTake(mapping_mutex, portMAX_DELAY);
    vSemaphoreDelete(consumer_sem_mapping[event_type][consumer_id]);
    consumer_sem_mapping[event_type][consumer_id] = NULL;
    xSemaphoreGive(mapping_mutex);
    return 0;
}

int8_t event_manager_post_event(EventType_t event_type)
{
    // check event_type is valid
    if (event_type >= EVENT_TYPE_MAX)
    {
        printf("\nInvalid event type");
        return -1;
    }
xSemaphoreTake(mapping_mutex, portMAX_DELAY);
    for (uint8_t i = 0; i < MAX_SEM_PER_EVENT_TYPE; i++)
    {
        if (consumer_sem_mapping[event_type][i] != NULL)
        {
            xSemaphoreGive(consumer_sem_mapping[event_type][i]);
        }
    }
    xSemaphoreGive(mapping_mutex);
    return 0;
}

// Add an ISR-safe post function
int8_t event_manager_post_event_from_isr(EventType_t event_type)
{
    if (event_type >= EVENT_TYPE_MAX)
        return -1;

    BaseType_t xHPTW = pdFALSE;
    for (uint8_t i = 0; i < MAX_SEM_PER_EVENT_TYPE; i++)
    {
        if (consumer_sem_mapping[event_type][i] != NULL)
        {
            xSemaphoreGiveFromISR(consumer_sem_mapping[event_type][i], &xHPTW);
        }
    }
    portYIELD_FROM_ISR(xHPTW);
    return 0;
}

int8_t event_manager_wait_event(EventType_t event_type, uint8_t consumer_id)
{
    if (event_type >= EVENT_TYPE_MAX)
    {
        printf("\nInvalid event type");
        return -1;
    }
    if (consumer_id >= MAX_SEM_PER_EVENT_TYPE)
    {
        printf("\nInvalid consumer id. it is above alloed max value");
        return -1;
    }
    if (consumer_sem_mapping[event_type][consumer_id] == NULL)
    {
        printf("\nSemaphore not registered");
        return -1;
    }
    xSemaphoreTake(consumer_sem_mapping[event_type][consumer_id], portMAX_DELAY);
    return 0;
}
