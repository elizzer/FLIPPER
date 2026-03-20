#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define MAX_SEM_PER_EVENT_TYPE 10

typedef enum EventType_e
{
    EVENT_TYPE_IO_EXP,
    EVENT_TYPE_KEYPAD,
    EVENT_TYPE_MAX
} EventType_t;

typedef struct EventDescription_s
{
    Event_type_t event_type;
    uint8_t event_source;
} EventDescription_t;


int8_t event_manager_init();
int8_t event_manager_post_event(EventType_t);
int8_t event_manager_post_event_from_isr(EventType_t event_type);
int8_t event_manager_wait_event(EventType_t, uint8_t);
int8_t event_manager_register_event(EventType_t, uint8_t*);
int8_t event_manager_unregister_event(EventType_t, uint8_t);


#endif // EVENT_MANAGER_H