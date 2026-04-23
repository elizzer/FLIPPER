#ifndef GPIO_CLOCK_GEN_H
#define GPIO_CLOCK_GEN_H

#include <stdint.h>
#include "gpio_hal.h"
#include "clock_gen_hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct{
    TaskHandle_t  clock_task_handle;
    volatile bool is_running;
}GpioClockGenHandle_t;

int8_t gpio_clock_config(ClockGenHalHandle_t *handle);
int8_t gpio_clock_start(ClockGenHalHandle_t *handle) ;
int8_t gpio_clock_stop(ClockGenHalHandle_t *handle) ;
int8_t gpio_clock_deinit(ClockGenHalHandle_t *handle);

#endif /* GPIO_CLOCK_GEN_H */