#include "ILI9341.h"
#include "spi_hal.h"
#include "hw_include.h"

void ili9341_lcd_disp_test(void)
{
    /* Test function to display something on the ILI9341 LCD */

    hal_spi_bus_config_t bus_config = {
        .spi_instance = SPI_NUM_3,
        .mosi_io_num = IO_NUM_23,
        .miso_io_num = IO_NUM_19,
        .sclk_io_num = IO_NUM_18,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = (240*320*2), // 2 KB
    };

    hal_spi_bus_handle_t bus_handle;
    esp_err_t ret = spi_bus_init(&bus_handle, &bus_config);
    if (ret != ESP_OK)
    {
        printf("Failed to initialize SPI bus\n");
        return;
    }

    hal_spi_device_config_t device_config = {
        .cs_io_num = IO_NUM_5,
        .clk_freq_hz = 1 * 1000 * 1000, // 1 MHz
    };

    hal_spi_device_handle_t device_handle;
    ret = spi_add_device(&bus_handle, &device_config, &device_handle);
    if (ret != ESP_OK)
    {
        printf("Failed to add SPI device\n");
        spi_bus_deinit(&bus_handle);
        return;
    }

    /* Example: Fill the screen with a solid color */
    uint16_t color = 0xF800; // Red color in RGB565

    ili9341_config_t config = {
        .width = 240,
        .height = 320,
        .rotation = 0,
        .display_handle = &device_handle,
        .dc_io_num = IO_NUM_25,
        .cs_io_num = IO_NUM_5,
        .reset_io_num = IO_NUM_17,
        .backlight_io_num = IO_NUM_16,
        .color_mode = 16, // RGB565
        .command_length = 8,
    };

    ili9341_handle_t handle;

    ret = ili9341_init(config, &handle);
    if (ret != 0)
    {
        printf("Failed to initialize ILI9341 display\n");
        return;
    }

   

    // Fill screen with red color
    uint32_t total_pixels = config.width * config.height;
    printf("Total pixels: %ld\n", total_pixels);
    uint16_t *pixel_data = malloc(total_pixels * sizeof(uint16_t));
    printf("Allocated pixel data buffer at %p\n", pixel_data);
    if (pixel_data == NULL)
    {
        printf("Failed to allocate memory for pixel data\n");
        return;
    }
    for (uint32_t i = 0; i < total_pixels; i++)
    {
        pixel_data[i] = color;
    }

    printf("Setting column and page addresses\n");

    ili9341_set_col_addr(0, config.width - 1,&handle);
    printf("Column address set\n");
    ili9341_set_page_addr(0, config.height - 1,&handle);
    printf("Page address set\n");
    ili9341_send_565_pxl_data(pixel_data, total_pixels, &handle);
    free(pixel_data);
}