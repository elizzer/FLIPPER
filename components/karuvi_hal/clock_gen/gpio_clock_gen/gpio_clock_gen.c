#include "gpio_clock_gen.h"

void gpio_clock_task(void *arg)
{
    ClockGenHalHandle_t *handle = (ClockGenHalHandle_t *)arg;
    uint32_t frequency_hz = handle->config.frequency_hz;
    uint32_t period_us = 1000 / frequency_hz; // Period in microseconds
    GpioClockGenHandle_t *gpio_handle = (GpioClockGenHandle_t *)handle->clk_src_handle;
    // use the mutex to start and stop the clock safely when the frequency is updated
    printf("GPIO clock generator task started for pin %d with frequency %ld Hz calculated time per cycle: %ld us\n", handle->config.output_pin, frequency_hz, period_us);
    while (1)
    {
        // block here until start is called
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        printf("GPIO clock generator task unblocked, starting clock output\n");
        while (gpio_handle->is_running)
        {   
            printf("Toggling GPIO pin %d for clock output\n", handle->config.output_pin);
            gpio_hal_set_level(handle->config.output_pin, GPIO_HAL_HIGH);
            vTaskDelay(pdMS_TO_TICKS(period_us / 2)); // High for half the period
            gpio_hal_set_level(handle->config.output_pin, GPIO_HAL_LOW);
            vTaskDelay(pdMS_TO_TICKS(period_us / 2)); // Low for the other half
        }
    }
}

int8_t gpio_clock_start(ClockGenHalHandle_t *handle)
{
    if (handle == NULL || handle->clk_src_handle == NULL)
        return -1;
    
    printf("Starting GPIO clock generator on pin %d with frequency %ld Hz\n", handle->config.output_pin, handle->config.frequency_hz);
    GpioClockGenHandle_t *gpio_handle = (GpioClockGenHandle_t *)handle->clk_src_handle;
    gpio_handle->is_running = true;
    xTaskNotifyGive(gpio_handle->clock_task_handle); // unblock task
    return 0;
}
int8_t gpio_clock_stop(ClockGenHalHandle_t *handle)
{
    if (handle == NULL || handle->clk_src_handle == NULL)
        return -1;

    GpioClockGenHandle_t *gpio_handle = (GpioClockGenHandle_t *)handle->clk_src_handle;

    gpio_handle->is_running = false; // task exits inner loop naturally
    return 0;
}

int8_t gpio_clock_config(ClockGenHalHandle_t *handle)
{

    if (handle == NULL)
    {
        printf("Invalid handle in gpio_clock_config\n");
        return -1;
    }

    // check the clock source is gpio
    if (handle->clk_src != CLK_SRC_GPIO)
    {
        printf("Invalid clock source for gpio_clock_config\n");
        return -1;
    }

    GpioClockGenHandle_t *gpio_handle = (GpioClockGenHandle_t *)malloc(sizeof(GpioClockGenHandle_t));
    if (gpio_handle == NULL)
    {
        printf("Failed to allocate memory for gpio clock gen handle\n");
        return -1;
    }
    handle->clk_src_handle = (void *)gpio_handle;

    // setup the pin configuration using gpio_hal
    gpioHalPinConfig_t pin_config = {
        .pin_num = handle->config.output_pin,
        .mode = GPIO_HAL_OUTPUT,
        .pull = GPIO_HAL_NOPULL,
        .intr_type = GPIO_HAL_INTR_DISABLE};

    esp_err_t ret = gpio_hal_config_pin(&pin_config);
    if (ret != ESP_OK)
    {
        printf("Failed to configure GPIO pin for clock output\n");
        return -1;
    }

    // create a task to toggle the gpio pin at the specified frequency
    xTaskCreate(gpio_clock_task, "gpio_clock_task", 2048, handle, 5, &gpio_handle->clock_task_handle);
    return 0;
}

int8_t gpio_clock_deinit(ClockGenHalHandle_t *handle)
{
    if (handle == NULL || handle->clk_src_handle == NULL)
    {
        printf("Invalid handle in gpio_clock_deinit\n");
        return -1;
    }

    GpioClockGenHandle_t *gpio_handle = (GpioClockGenHandle_t *)handle->clk_src_handle;

    // delete the clock task
    vTaskDelete(gpio_handle->clock_task_handle);

    // free the gpio handle memory
    free(gpio_handle);
    handle->clk_src_handle = NULL;

    return 0;
}