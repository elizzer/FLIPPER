
#ifndef KX_GPIO_HAL_H
#define KX_GPIO_HAL_H

#include <stdint.h>
#include "kx_hal_types.h"

typedef uint8_t Kx_GpioPin;

// Pin direction
typedef enum Kx_GpioDirection {
    KX_GPIO_DIR_INPUT = 0,
    KX_GPIO_DIR_OUTPUT,
    KX_GPIO_DIR_INPUT_OUTPUT,   // open-drain style bidirectional
} Kx_GpioDirection_t;

// Pin pull configuration
typedef enum Kx_GpioPull {
    KX_GPIO_PULL_NONE = 0,
    KX_GPIO_PULL_UP,
    KX_GPIO_PULL_DOWN,
    KX_GPIO_PULL_UP_DOWN,       // both enabled, if HW supports it
} Kx_GpioPull_t;

// Pin logic state
typedef enum Kx_GpioState {
    KX_GPIO_STATE_LOW = 0,
    KX_GPIO_STATE_HIGH,
} Kx_GpioState_t;

// Pin operation (for interrupt/edge config, or read-vs-write intent)
typedef enum Kx_GpioOperation {
    KX_GPIO_OP_READ = 0,
    KX_GPIO_OP_WRITE,
    KX_GPIO_OP_TOGGLE,
} Kx_GpioOperation_t;

Kx_ErrorCode KxGpio_Init();
Kx_ErrorCode KxGpio_DeInit();
Kx_ErrorCode KxGpio_SetDirection(Kx_GpioPin,Kx_GpioDirection_t);
Kx_ErrorCode KxGpio_SetPull(Kx_GpioPin,Kx_GpioPull_t);
Kx_ErrorCode KxGpio_Set(Kx_GpioPin);
Kx_ErrorCode KxGpio_Clear(Kx_GpioPin);
Kx_ErrorCode KxGpio_Toggle(Kx_GpioPin);
Kx_ErrorCode KxGpio_Read(Kx_GpioPin, Kx_GpioState_t*);

//register intrupt
//enable intrrupt
//disable intrrupt
//un reigstser intrrupt


#endif //KX_GPIO_HAL_H
