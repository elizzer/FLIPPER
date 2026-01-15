
#include "ILI9341.h"
#include "gpio_hal.h"
#include <string.h>

struct ILI9341_cmds ili9341_init_seq[] = {
    // Software Reset
    {
        .cmd = 0x01,     // Command: Software Reset
        .msDelay = 5,    // Delay: 5 ms
        .tx_data = NULL, // No data to transmit
        .tx_data_len = 0,
        .rx_data = 0, // No data to receive
        .rx_data_len = 0},
    // Exit Sleep Mode
    {
        .cmd = 0x11,    // Command: Exit Sleep Mode
        .msDelay = 120, // Delay: 120 ms
        .tx_data = NULL,
        .tx_data_len = 0,
        .rx_data = 0,
        .rx_data_len = 0},
    // Set Pixel Format
    {
        .cmd = 0x3A,                  // Command: Set Pixel Format
        .msDelay = 0,                 // No delay needed
        .tx_data = (uint8_t[]){0x55}, // Data: 16-bit pixel format (0x55)
        .tx_data_len = 1,
        .rx_data = 0,
        .rx_data_len = 0},
    // Frame Rate Control (Normal Mode)
    {
        .cmd = 0xB1,
        .msDelay = 0,
        .tx_data = (uint8_t[]){0x00, 0x10}, // Data: Frame rate settings
        .tx_data_len = 2,
        .rx_data = 0,
        .rx_data_len = 0},
    // Display Inversion OFF
    {
        .cmd = 0x20,
        .msDelay = 0,
        .tx_data = NULL,
        .tx_data_len = 0,
        .rx_data = 0,
        .rx_data_len = 0},
    // Memory Access Control (MADCTL)
    {
        .cmd = 0x36,
        .msDelay = 0,
        .tx_data = (uint8_t[]){0x00}, // Data: Orientation and RGB order
        .tx_data_len = 1,
        .rx_data = 0,
        .rx_data_len = 0},
    // Turn ON Display
    {
        .cmd = 0x29,
        .msDelay = 0,
        .tx_data = NULL,
        .tx_data_len = 0,
        .rx_data = 0,
        .rx_data_len = 0},

};

int8_t ili9341_init(ili9341_config_t config, ili9341_handle_t *handle)
{

    /* Initialize the display with the provided configuration */
    printf("\n\rILI9341 GPIO Configuration\n");
    // config and set all the io pins
    gpio_hal_pin_config_t pin_config = {
        .pin_num = config.dc_io_num,
        .mode = GPIO_HAL_OUTPUT,
        .pull = GPIO_HAL_NOPULL};
    gpio_hal_config_pin(&pin_config);
    gpio_hal_set_level(config.dc_io_num, 0);

    // pin_config.pin_num = config.cs_io_num;
    // gpio_hal_config_pin(&pin_config);

    pin_config.pin_num = config.reset_io_num;
    gpio_hal_config_pin(&pin_config);
    gpio_hal_set_level(config.reset_io_num, 1);

    pin_config.pin_num = config.backlight_io_num;
    gpio_hal_config_pin(&pin_config);
    gpio_hal_set_level(config.backlight_io_num, 1);

    gpio_hal_set_level(config.dc_io_num, 1);


    printf("\n\rILI9341 Initialization Started");
    
    // copy config to handle
    // *((ili9341_config_t *)handle) = config;
    memcpy(handle, &config, sizeof(ili9341_config_t));

    for (uint32_t i = 0; i < sizeof(ili9341_init_seq) / sizeof(ili9341_init_seq[0]); i++)
    {
        printf("\n\rSending command :%x", ili9341_init_seq[i].cmd);
        ili9341_send_command(ili9341_init_seq[i].cmd, handle);
        if (ili9341_init_seq[i].tx_data_len != 0)
        {
            ili9341_send_data(ili9341_init_seq[i].tx_data, ili9341_init_seq[i].tx_data_len, handle);
            printf("\n\rSending data ");
        }
        vTaskDelay(ili9341_init_seq[i].msDelay / portTICK_PERIOD_MS);
    }

    gpio_hal_set_level(config.dc_io_num, 0);


    return 0;
}

int8_t ili9341_deinit()
{

    /* Deinitialize the display and free resources */

    return 0;
}

int8_t ili9341_send_command(uint8_t cmd, ili9341_handle_t *handle)
{

    /* Send command to the display */

    // check for null handle
    if (handle == NULL)
    {
        return -1;
    }

    ili9341_config_t *config = (ili9341_config_t *)handle;
    hal_spi_transaction_t transaction;
    transaction.tx_buffer = &cmd;
    transaction.rx_buffer = NULL;
    transaction.tx_length = 8; // Command is 1 byte
    transaction.rx_length = 0;

    // assert cmd io
    // ILI9341_CMD_MODE(config->dc_io_num); // Command mode
    gpio_hal_set_level(config->dc_io_num, ILI9341_CMD);
    spi_transfer(config->display_handle, &transaction);

    return 0;
}

int8_t ili9341_send_data(uint8_t *data, uint32_t length, ili9341_handle_t *handle)
{

    /* Send data to the display */

    ili9341_config_t *config = (ili9341_config_t *)handle;
    hal_spi_transaction_t transaction;
    transaction.tx_buffer = data;
    transaction.rx_buffer = NULL;
    transaction.tx_length = length * 8; // Length in bits
    transaction.rx_length = 0;

    // assert data io
    // ILI9341_DATA_MODE(config->dc_io_num); // Data mode
    gpio_hal_set_level(config->dc_io_num, ILI9341_DATA);
    spi_transfer(config->display_handle, &transaction);

    return 0;
}

void ili9341_send_565_pxl_data(uint16_t *data, size_t len, ili9341_handle_t *handle)
{
    ili9341_config_t *config = (ili9341_config_t *)handle;

    ili9341_send_command(MEM_WRITE,handle);
  
    printf("Sending pixel data...\n");
    hal_spi_transaction_t transaction;
   
    esp_err_t ret;
    ret = ili9341_send_data((uint8_t *)data, len * 2, handle); // Each pixel is 2 bytes
    printf("Pixel data sent.\n");
    if (ret != ESP_OK)
    {
        printf("Failed to send pixel data\n");
    }

    return;
}

void ili9341_set_col_addr(uint16_t start, uint16_t end, ili9341_handle_t *handle)
{

    printf("Setting column address\n");

    ili9341_send_command(COL_ADDR_SET, handle);

    printf("Setting column address: start=%d, end=%d\n", start, end);

    ili9341_config_t *config = (ili9341_config_t *)handle;

    uint32_t t_data = ((start << 16) | (end));
    t_data = SWAP_32BIT_BYTE_ORDER(t_data);
    // ILI9341_send_data(&t_data,1);
    esp_err_t ret;
    printf("Transferring column address data\n");
    ret = ili9341_send_data((uint8_t *)&t_data, 4, handle);
    printf("Column address data transferred\n");
}

void ili9341_set_page_addr(uint16_t start, uint16_t end, ili9341_handle_t *handle) //row
{

    ili9341_send_command(PAGE_ADDR_SET, handle);

    ili9341_config_t *config = (ili9341_config_t *)handle;

    uint32_t t_data = ((start << 16) | (end));
    t_data = SWAP_32BIT_BYTE_ORDER(t_data);
    // ILI9341_send_data(&t_data,1);

    esp_err_t ret;
   
    ret = ili9341_send_data((uint8_t *)&t_data, 4, handle);

}


void ili9341_send_dips_buf(uint16_t *buf, uint8_t x, uint8_t y, uint8_t width, uint8_t height){

}
