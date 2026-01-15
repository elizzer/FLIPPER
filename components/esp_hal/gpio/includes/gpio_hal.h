#ifndef GPIO_HAL_H
#define GPIO_HAL_H

#include <stdint.h>

#include "esp_err.h"
#include "driver/gpio.h"

typedef enum
{
    GPIO_HAL_NOPULL,
    GPIO_HAL_PULLUP,
    GPIO_HAL_PULLDOWN,
} gpio_hal_pull_t;

typedef enum
{
    GPIO_HAL_INPUT,
    GPIO_HAL_OUTPUT,
    GPIO_HAL_INPUT_OUTPUT,
} gpio_hal_mode_t;

typedef struct gpio_hal_pin_config_s
{
    uint8_t pin_num;
    gpio_hal_mode_t mode;
    gpio_hal_pull_t pull;
 
} gpio_hal_pin_config_t;

typedef uint8_t gpio_hal_pin;

esp_err_t gpio_hal_init(void);

esp_err_t gpio_hal_config_pin(const gpio_hal_pin_config_t *pin_config);
esp_err_t gpio_hal_set_level(uint8_t pin_num, uint8_t level);
esp_err_t gpio_hal_get_level(uint8_t pin_num, uint8_t *level);

#endif // GPIO_HAL_H