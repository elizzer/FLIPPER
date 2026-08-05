#include "kx_gpio_hal.h"
#include "driver/gpio.h"

// --- internal helpers: map Kx enums to ESP-IDF enums ---

static gpio_mode_t Kx_ToEspGpioMode(Kx_GpioDirection_t direction)
{
    switch (direction) {
        case KX_GPIO_DIR_INPUT:         return GPIO_MODE_INPUT;
        case KX_GPIO_DIR_OUTPUT:        return GPIO_MODE_OUTPUT;
        case KX_GPIO_DIR_INPUT_OUTPUT:  return GPIO_MODE_INPUT_OUTPUT_OD;
        default:                        return GPIO_MODE_DISABLE;
    }
}

Kx_ErrorCode KxGpio_Init(void)
{
    // ESP-IDF's gpio driver is initialized implicitly on first use;
    // nothing global to set up here yet. Reserved for future use
    // (e.g. installing the ISR service for interrupt support).
    return KX_HAL_OK;
}

Kx_ErrorCode KxGpio_DeInit(void)
{
    return KX_HAL_OK;
}

Kx_ErrorCode KxGpio_SetDirection(Kx_GpioPin pin, Kx_GpioDirection_t direction)
{
    esp_err_t err = gpio_set_direction((gpio_num_t)pin, Kx_ToEspGpioMode(direction));
    if (err != ESP_OK) {
        return KX_HAL_ERR_FAIL;
    }
    return KX_HAL_OK;
}

Kx_ErrorCode KxGpio_SetPull(Kx_GpioPin pin, Kx_GpioPull_t pull)
{
    esp_err_t err = ESP_OK;

    switch (pull) {
        case KX_GPIO_PULL_NONE:
            err = gpio_set_pull_mode((gpio_num_t)pin, GPIO_FLOATING);
            break;
        case KX_GPIO_PULL_UP:
            err = gpio_set_pull_mode((gpio_num_t)pin, GPIO_PULLUP_ONLY);
            break;
        case KX_GPIO_PULL_DOWN:
            err = gpio_set_pull_mode((gpio_num_t)pin, GPIO_PULLDOWN_ONLY);
            break;
        case KX_GPIO_PULL_UP_DOWN:
            err = gpio_set_pull_mode((gpio_num_t)pin, GPIO_PULLUP_PULLDOWN);
            break;
        default:
            return KX_HAL_ERR_INVALID_ARG;
    }

    if (err != ESP_OK) {
        return KX_HAL_ERR_FAIL;
    }
    return KX_HAL_OK;
}

Kx_ErrorCode KxGpio_Set(Kx_GpioPin pin)
{
    esp_err_t err = gpio_set_level((gpio_num_t)pin, 1);
    if (err != ESP_OK) {
        return KX_HAL_ERR_FAIL;
    }
    return KX_HAL_OK;
}

Kx_ErrorCode KxGpio_Clear(Kx_GpioPin pin)
{
    esp_err_t err = gpio_set_level((gpio_num_t)pin, 0);
    if (err != ESP_OK) {
        return KX_HAL_ERR_FAIL;
    }
    return KX_HAL_OK;
}
Kx_ErrorCode KxGpio_Read(Kx_GpioPin pin, Kx_GpioState_t *state)
{
    if (state == NULL) {
        return KX_HAL_ERR_INVALID_ARG;
    }

    int level = gpio_get_level((gpio_num_t)pin);
    *state = level ? KX_GPIO_STATE_HIGH : KX_GPIO_STATE_LOW;

    return KX_HAL_OK;
}
Kx_ErrorCode KxGpio_Toggle(Kx_GpioPin pin)
{
    int current_level = gpio_get_level((gpio_num_t)pin);
    esp_err_t err = gpio_set_level((gpio_num_t)pin, !current_level);
    if (err != ESP_OK) {
        return KX_HAL_ERR_FAIL;
    }
    return KX_HAL_OK;
}

