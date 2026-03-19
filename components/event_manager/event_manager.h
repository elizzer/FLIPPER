#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define MAX_SEM_PER_EVENT_TYPE 10

typedef enum Event_type_e
{
    IO_EXP_EVENT,
    KEYPAD_EVENT,
    EVENT_TYPE_MAX
} Event_type_t;

typedef struct Event_s
{
    Event_type_t event_type;
    uint8_t event_source;
} Event_t;

// creates a event queue
int8_t init_eventManager();
int8_t post_semaphore(Event_type_t);
int8_t post_semaphore_from_isr(Event_type_t event_type);
int8_t wait_semaphore(Event_type_t, uint8_t);
int8_t register_event(Event_type_t, uint8_t*);
int8_t unregister_event(Event_type_t, uint8_t);

#endif // EVENT_MANAGER_H