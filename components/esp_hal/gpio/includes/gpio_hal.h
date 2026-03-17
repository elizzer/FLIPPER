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

typedef enum
{
    GPIO_HAL_INTR_DISABLE,
    GPIO_HAL_INTR_POSEDGE,
    GPIO_HAL_INTR_NEGEDGE,
    GPIO_HAL_INTR_ANYEDGE,
    GPIO_HAL_INTR_LOW_LEVEL,
    GPIO_HAL_INTR_HIGH_LEVEL,
} gpio_hal_intr_type_t;

typedef void (*gpio_hal_isr_t)(void *arg);

typedef struct gpio_hal_pin_config_s
{
    uint8_t            pin_num;
    gpio_hal_mode_t    mode;
    gpio_hal_pull_t    pull;
    gpio_hal_intr_type_t intr_type;   /* GPIO_HAL_INTR_DISABLE if not used */
} gpio_hal_pin_config_t;

typedef uint8_t gpio_hal_pin;

esp_err_t gpio_hal_init(void);

esp_err_t gpio_hal_config_pin(const gpio_hal_pin_config_t *pin_config);
esp_err_t gpio_hal_set_level(uint8_t pin_num, uint8_t level);
esp_err_t gpio_hal_get_level(uint8_t pin_num, uint8_t *level);

/**
 * @brief  Install a per-pin ISR handler.
 *
 * gpio_hal_init() must have been called first (it installs the ISR service).
 * The handler runs in ISR context — keep it short and defer work to a task
 * via a queue or task notification.
 *
 * @param pin_num  GPIO number
 * @param isr      Handler function
 * @param arg      Opaque argument forwarded to the handler
 * @return ESP_OK on success, or an esp_err_t error code
 */
esp_err_t gpio_hal_isr_register(uint8_t pin_num, gpio_hal_isr_t isr, void *arg);

/**
 * @brief  Remove the ISR handler for a pin and disable its interrupt.
 *
 * @param pin_num  GPIO number
 * @return ESP_OK on success, or an esp_err_t error code
 */
esp_err_t gpio_hal_isr_deregister(uint8_t pin_num);

#endif // GPIO_HAL_H