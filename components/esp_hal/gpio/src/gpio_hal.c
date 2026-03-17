#include "gpio_hal.h"

#include "driver/gpio.h"
#include "esp_err.h"

/*
 * GPIO HAL implementation with interrupt support
 *
 * Assumptions:
 *  - gpio_hal_init() is called once at startup before any ISR registration
 *  - Caller ensures valid pin numbers and usage discipline
 *  - ISR handlers run in IRAM-safe context; keep them minimal
 *
 * TODO:
 *  - Add pin ownership tracking
 *  - Add validation for input-only / flash pins
 *  - Add power management hooks
 *  - Add logging and error translation
 */

/* Default ISR service CPU affinity flag — allocate on the core that calls init */
#define GPIO_HAL_ISR_SERVICE_FLAGS  0

esp_err_t gpio_hal_init(void)
{
    /*
     * Install the GPIO ISR service once. This allows per-pin handlers to be
     * registered independently via gpio_isr_handler_add().
     *
     * ESP_ERR_INVALID_STATE means the service is already installed, which is
     * harmless — treat it as success so that multiple callers are tolerated.
     */
    esp_err_t ret = gpio_install_isr_service(GPIO_HAL_ISR_SERVICE_FLAGS);
    if (ret == ESP_ERR_INVALID_STATE)
    {
        ret = ESP_OK;
    }

    return ret;
}

static gpio_int_type_t translate_intr_type(gpio_hal_intr_type_t intr_type)
{
    switch (intr_type)
    {
    case GPIO_HAL_INTR_POSEDGE:    return GPIO_INTR_POSEDGE;
    case GPIO_HAL_INTR_NEGEDGE:    return GPIO_INTR_NEGEDGE;
    case GPIO_HAL_INTR_ANYEDGE:    return GPIO_INTR_ANYEDGE;
    case GPIO_HAL_INTR_LOW_LEVEL:  return GPIO_INTR_LOW_LEVEL;
    case GPIO_HAL_INTR_HIGH_LEVEL: return GPIO_INTR_HIGH_LEVEL;
    case GPIO_HAL_INTR_DISABLE:    /* fall-through */
    default:                       return GPIO_INTR_DISABLE;
    }
}

esp_err_t gpio_hal_config_pin(const gpio_hal_pin_config_t *pin_config)
{
    if (pin_config == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << pin_config->pin_num),
        .mode         = GPIO_MODE_DISABLE,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
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

    /* Configure interrupt trigger */
    cfg.intr_type = translate_intr_type(pin_config->intr_type);

    /*
     * Interrupt on an output-only pin is silently ignored by the hardware,
     * but warn-worthy — left as a future logging hook.
     */

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

    *level = (uint8_t)gpio_get_level((gpio_num_t)pin_num);
    return ESP_OK;
}

esp_err_t gpio_hal_isr_register(uint8_t pin_num, gpio_hal_isr_t isr, void *arg)
{
    if (isr == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    /* Remove any previously registered handler for this pin first */
    gpio_isr_handler_remove((gpio_num_t)pin_num);

    return gpio_isr_handler_add((gpio_num_t)pin_num, isr, arg);
}

esp_err_t gpio_hal_isr_deregister(uint8_t pin_num)
{
    esp_err_t ret = gpio_isr_handler_remove((gpio_num_t)pin_num);
    if (ret != ESP_OK)
    {
        return ret;
    }

    /* Disable the interrupt at the hardware level */
    return gpio_set_intr_type((gpio_num_t)pin_num, GPIO_INTR_DISABLE);
}