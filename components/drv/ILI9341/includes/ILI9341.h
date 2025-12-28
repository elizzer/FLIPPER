#ifndef ILI9341_H
#define ILI9341_H

#include <stdint.h>
#include "spi_hal.h"

typedef ili9341_config_t ili9341_handle_t;

typedef struct {

    uint16_t width;
    uint16_t height;
    uint8_t rotation;
    hal_spi_device_handle_t display_handle;
    uint8_t dc_io_num;
    uint8_t reset_io_num;
    uint8_t backlight_io_num;
    uint8_t color_mode;
    uint8_t command_length;

} ili9341_config_t;

int8_t ili9341_init(ili9341_config_t, ili9341_handle_t*);
int8_t ili9341_deinit();

int8_t ili9341_send_command(uint8_t cmd);
int8_t ili9341_send_data(uint8_t* data, uint32_t length);




#endif // ILI9341_H