
#include "ILI9341.h"
#include "gpio_hal.h"

int8_t ili9341_init(ili9341_config_t config, ili9341_handle_t *handle)
{

    /* Initialize the display with the provided configuration */

    // config and set all the io pins
    gpio_hal_pin_config_t pin_config = {
        .pin_num = config.dc_io_num,
        .mode = GPIO_HAL_OUTPUT,
        .pull = GPIO_HAL_NOPULL};
    gpio_hal_config_pin(&pin_config);

    pin_config.pin_num = config.cs_io_num;
    gpio_hal_config_pin(&pin_config);

    pin_config.pin_num = config.reset_io_num;
    gpio_hal_config_pin(&pin_config);

    return 0;
}

int8_t ili9341_deinit()
{

    /* Deinitialize the display and free resources */

    return 0;
}

int8_t ili9341_send_command(uint8_t *cmd, ili9341_handle_t *handle)
{

    /* Send command to the display */

    ili9341_config_t *config = handle;
    hal_spi_transaction_t transaction;
    transaction.tx_buffer = cmd;
    transaction.rx_buffer = NULL;
    transaction.tx_length = 8; // Command is 1 byte
    transaction.rx_length = 0;

    // assert cmd io
    gpio_hal_set_level(config->dc_io_num, 0); // Command mode

    spi_transfer(&config->display_handle, &transaction);

    return 0;
}

int8_t ili9341_send_data(uint8_t *data, uint32_t length, ili9341_handle_t *handle)
{

    /* Send data to the display */

    ili9341_config_t *config = handle;
    hal_spi_transaction_t transaction;
    transaction.tx_buffer = data;
    transaction.rx_buffer = NULL;
    transaction.tx_length = length * 8; // Length in bits
    transaction.rx_length = 0;

    // assert data io
    gpio_hal_set_level(config->dc_io_num, 1); // Data mode

    spi_transfer(&config->display_handle, &transaction);

    return 0;
}