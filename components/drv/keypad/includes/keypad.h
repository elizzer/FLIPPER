#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

/* ─────────────────────────────────────────
 * Key state
 * ───────────────────────────────────────── */
typedef enum {
    KEY_PRESSED,
    KEY_RELEASED,
} key_state_t;

/* ─────────────────────────────────────────
 * Active timer type on a key
 * ───────────────────────────────────────── */
typedef enum {
    KEY_TIMER_NONE,
    KEY_TIMER_DEBOUNCE,
    KEY_TIMER_DOUBLE_PRESS,
    KEY_TIMER_LONG_PRESS,
} key_timer_state_t;

/* ─────────────────────────────────────────
 * Per key runtime state — internal use only
 * exposed here so handle struct can embed it
 * ───────────────────────────────────────── */
typedef struct {
    uint8_t           key_id;
    key_state_t       stable_state;      // last confirmed debounced state
    key_state_t       raw_state;         // latest raw reading
    key_timer_state_t timer_state;       // which timer is currently active
    TimerHandle_t     timer;             // FreeRTOS software timer handle
} keypad_button_attrs_t;

/* ─────────────────────────────────────────
 * Config — caller owns, passed into init
 * ───────────────────────────────────────── */
typedef struct {
    uint8_t  buttons;          // total number of keys
    void    *ioExp_handle;     // handle to the io expander driver instance
} keypad_config_t;

/* ─────────────────────────────────────────
 * Handle — module owns, caller holds pointer
 * treat as opaque — do not access fields directly
 * ───────────────────────────────────────── */
typedef struct {
    keypad_config_t        config;         // copy of caller config
    keypad_button_attrs_t *keys;           // allocated per key state array
    uint8_t               *prev_raw;       // previous raw port reading
    uint8_t               *cur_raw;        // current raw port reading
    uint8_t                consumer_id;    // event manager consumer slot
    TaskHandle_t           task_handle;    // internal listener task
} keypad_handle_t;

/* ─────────────────────────────────────────
 * Public API
 * ───────────────────────────────────────── */
int8_t keypad_init(keypad_config_t *config, keypad_handle_t *handle);
int8_t keypad_deinit(keypad_handle_t *handle);

#endif // KEYPAD_H