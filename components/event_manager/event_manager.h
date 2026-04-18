#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define MAX_CONSUMER_PER_EVENT_TYPE 10
#define QUEUES_PER_EVENT 5

typedef enum
{
    KEYPAD_PRESSED,
    KEYPAD_RELEASED,
    KEYPAD_LONG_PRESS,
} keypad_action_t;

typedef enum EventType_e
{
    EVENT_TYPE_IO_EXP,
    EVENT_TYPE_KEYPAD,
    EVENT_TYPE_MAX
} EventType_t;

typedef struct keypadEventDesc_s
{
    keypad_action_t action;
    uint8_t key_number;
} keypadEventDesc_t;

typedef struct EventDescription_s
{
    EventType_t event_type;
    uint8_t event_source;
    union
    {
        keypadEventDesc_t keypad;
    };
} EventDescription_t;

int8_t event_manager_init(void);
int8_t event_manager_register_event(EventType_t event_type, uint8_t *consumer_id);
int8_t event_manager_unregister_event(EventType_t event_type, uint8_t consumer_id);

/* Posts an event to all registered consumers of that event type */
int8_t event_manager_post_event(EventType_t event_type, EventDescription_t event_desc);

/* ISR-safe variant — builds the descriptor inside, caller fills what it can */
int8_t event_manager_post_event_from_isr(EventType_t event_type, EventDescription_t event_desc);

/* Blocks until an event arrives; received descriptor written to *event_desc_out */
int8_t event_manager_wait_event(EventType_t event_type, uint8_t consumer_id,
                                EventDescription_t *event_desc_out);

#endif // EVENT_MANAGER_H