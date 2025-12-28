#include "gpio_hal.h"

#include "driver/gpio.h"
#include "esp_err.h"

/*
 * Minimal GPIO HAL implementation
 *
 * Assumptions:
 *  - gpio_hal_init() is called once at startup
 *  - Caller ensures valid pin numbers and usage discipline
 *
 * TODO:
 *  - Add pin ownership tracking
 *  - Add validation for input-only / flash pins
 *  - Add interrupt support
 *  - Add power management hooks
 *  - Add logging and error translation
 */

esp_err_t gpio_hal_init(void)
{
    /* Nothing required for minimal GPIO usage */
    /* Placeholder for future ISR service or global init */

    return ESP_OK;
}

esp_err_t gpio_hal_config_pin(const gpio_hal_pin_config_t *pin_config)
{
    if (pin_config == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << pin_config->pin_num),
        .mode = GPIO_MODE_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    /* Configure mode */
    switch (pin_config->mode)
    {
    case GPIO_HAL_INPUT:
        cfg.mode = GPIO_MODE_INPUT;
        break;

    case GPIO_HAL_OUTPUT:
        cfg.mode = GPIO_MODE_OUTPUT;
        break;

    case GPIO_HAL_INPUT_OUTPUT:
        cfg.mode = GPIO_MODE_INPUT_OUTPUT;
        break;

    default:
        return ESP_ERR_INVALID_ARG;
    }

    /* Configure pull */
    switch (pin_config->pull)
    {
    case GPIO_HAL_NOPULL:
        break;

    case GPIO_HAL_PULLUP:
        cfg.pull_up_en = GPIO_PULLUP_ENABLE;
        break;

    case GPIO_HAL_PULLDOWN:
        cfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
        break;

    default:
        return ESP_ERR_INVALID_ARG;
    }

    return gpio_config(&cfg);
}

esp_err_t gpio_hal_set_level(uint8_t pin_num, uint8_t level)
{
    return gpio_set_level((gpio_num_t)pin_num, level ? 1 : 0);
}

esp_err_t gpio_hal_get_level(uint8_t pin_num, uint8_t *level)
{
    if (level == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    *level = gpio_get_level((gpio_num_t)pin_num);
    return ESP_OK;
}
