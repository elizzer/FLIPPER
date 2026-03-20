#ifndef IO_EXPANDER
#define IO_EXPANDER

#include <stdint.h>
#include "freertos/semphr.h"


typedef enum IoExpanderPinMode_e
{
    IO_EXPANDER_PIN_MODE_OUTPUT,
    IO_EXPANDER_PIN_MODE_INPUT,
} IoExpanderPinMode_t;

typedef enum IoExpanderPinState_e
{
    IO_EXPANDER_PIN_STATE_LOW,
    IO_EXPANDER_PIN_STATE_HIGH,
} IoExpanderPinValue_t;

typedef uint8_t IoExpanderPort_t;
typedef uint8_t IoExpanderPin_t;

typedef struct IoExpanderOps_s
{
    int8_t (*init)(void *ctx);
    int8_t (*deinit)(void *ctx);
    int8_t (*pin_mode)(void *ctx, IoExpanderPin_t pin, IoExpanderPinMode_t mode);
    int8_t (*read_pin)(void *ctx, IoExpanderPin_t pin, IoExpanderPinValue_t *value);
    int8_t (*read_port)(void *ctx, IoExpanderPort_t port, IoExpanderPin_t pin, IoExpanderPinValue_t *value);
    int8_t (*set_pin)(void *ctx, IoExpanderPin_t pin, IoExpanderPinValue_t value);
    int8_t (*set_port)(void *ctx, IoExpanderPort_t port, IoExpanderPin_t pin, IoExpanderPinValue_t value);
} IoExpanderOps_t;

typedef struct IoExpanderDevConfig_s
{
    const IoExpanderOps_t *ops;
    void * ctx;
    SemaphoreHandle_t    lock; 
} IoExpanderDevConfig_t;

#endif // IO_EXPANDER