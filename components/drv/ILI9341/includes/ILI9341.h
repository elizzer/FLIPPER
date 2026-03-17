#ifndef ILI9341_H
#define ILI9341_H

#include <stdint.h>
#include "spi_hal.h"
#include "ILI9341_hw.h"

// Macros for Command/Data Mode
#define ILI9341_CMD_MODE(x) gpio_hal_set_level(x, ILI9341_CMD)
#define ILI9341_DATA_MODE(x) gpio_hal_set_level(x, ILI9341_DATA)

#define SWAP_32BIT_BYTE_ORDER(d) (((d) >> 24) | (((d) >> 8) & 0xFF00) | (((d) << 8) & 0xFF0000) | (((d) << 24) & 0xFF000000))
#define SWAP_16BIT_BYTE_ORDER(d) (((d) >> 8) | ((d) << 8))

typedef struct ili9341_config_s ili9341_handle_t;

typedef struct ili9341_config_s
{

    uint16_t width;
    uint16_t height;
    uint8_t rotation;
    hal_spi_device_handle_t *display_handle;
    uint8_t dc_io_num;
    uint8_t cs_io_num;
    uint8_t reset_io_num;
    uint8_t backlight_io_num;
    uint8_t color_mode;
    uint8_t command_length;

} ili9341_config_t;

struct ILI9341_cmds
{
    uint8_t cmd;
    uint32_t msDelay;
    uint8_t *tx_data;
    uint32_t tx_data_len;
    uint8_t rx_data;
    uint32_t rx_data_len;
};

int8_t ili9341_init(ili9341_config_t, ili9341_handle_t *);
int8_t ili9341_deinit();

int8_t ili9341_send_command(uint8_t cmd, ili9341_handle_t *handle);
int8_t ili9341_send_data(uint8_t *data, uint32_t length, ili9341_handle_t *handle);

void ili9341_send_565_pxl_data(uint16_t *data, size_t len, ili9341_handle_t *handle);
void ili9341_set_col_addr(uint16_t start, uint16_t end, ili9341_handle_t *handle);
void ili9341_set_page_addr(uint16_t start, uint16_t end, ili9341_handle_t *handle);

int8_t ili9341_send_dips_buf(uint16_t *buf, uint16_t x, uint16_t y, uint16_t width, uint16_t height, void *handle);


#endif // ILI9341_H