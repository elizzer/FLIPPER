#include <driver/gpio.h>
#include "i2c_hal.h"
#include "io_expander/PCF8574/PCF8574.h"
#include "display/ILI9341/ILI9341.h"
#include "spi_hal.h"
#include "hw_include.h"
#include "display_manager.h"
#include "keypad/keypad.h"



static SpiHalBusHandle_t SPI3_bus_handle;
static SpiHalDeviceHandle_t tft_display_spi_handle;
static ili9341_handle_t tft_display_handle;
static displayHandle_t display_handle;
static I2cHalBusHandle_t I2C0_bus_handle;
static I2cHalDeviceHandle_t io_exp_i2c_handle;
static PCF8574_handle_t io_exp_handle;
static keypad_handle_t keypad_handle;


displayHandle_t* get_display_handle(){
    return &display_handle;
}


int8_t hardware_init()
{
    gpio_hal_init();

    SpiHalBusConfig_t bus_config = {
        .spi_instance = SPI_NUM_3,
        .mosi_io_num = IO_NUM_23,
        .miso_io_num = IO_NUM_19,
        .sclk_io_num = IO_NUM_18,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 2 * 240*320, // 2 KB
    };

    esp_err_t ret = spi_hal_bus_init(&SPI3_bus_handle, &bus_config);
    if (ret != ESP_OK)
    {
        printf("Failed to initialize SPI bus\n");
        return -1;
    }

    SpiHalDeviceConfig_t device_config = {
        .cs_io_num = IO_NUM_5,
        .clk_freq_hz = 30 * 1000 * 1000, // 1 MHz
    };

    ret = spi_hal_add_device(&SPI3_bus_handle, &device_config, &tft_display_spi_handle);
    if (ret != ESP_OK)
    {
        printf("Failed to add SPI device\n");
        spi_hal_bus_deinit(&SPI3_bus_handle);
        return -1;
    }

    ili9341_config_t config = {
        .width = 240,
        .height = 320,
        .rotation = 0,
        .display_handle = &tft_display_spi_handle,
        .dc_io_num = IO_NUM_25,
        .cs_io_num = IO_NUM_5,
        .reset_io_num = IO_NUM_17,
        .backlight_io_num = IO_NUM_16,
        .color_mode = 16, // RGB565
        .command_length = 8,
    };

    ret = ili9341_init(config, &tft_display_handle);
    if (ret != 0)
    {
        printf("Failed to initialize ILI9341 display\n");
        return -1;
    }

    displayConfig_t disp_config = {
        .width = config.width,
        .height = config.height,
        .frame_buffer = NULL,                         // This will be allocated in display_init
        .write_buffer_func = ili9341_send_dips_buf,   // Set the function pointer to send buffer to ILI9341
        .display_device_handle = &tft_display_handle, // Pass the ILI9341 handle to the display config
    };

    int8_t status;
    status = display_init(&disp_config, &display_handle);
    if (status != 0)
    {
        printf("Failed to initialize display manager\n");
        return -1;
    }

    const I2cHalConfig_t i2c_bus_config = {
        .clk_speed_hz = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = GPIO_NUM_22,
        .sda_io_num = GPIO_NUM_21,
        .i2c_instance = I2C_NUM_0,
    };
    i2c_hal_init(&i2c_bus_config, &I2C0_bus_handle);

    const I2cHalDeviceConfig_t io_exp_i2c_dev_config = {
        .addr_len = I2C_HAL_ADDR_BIT_LEN_7,
        .clk_freq_hz = 100000,
        .device_address = 32,
    };

    i2c_hal_add_device(&io_exp_i2c_dev_config, &io_exp_i2c_handle, &I2C0_bus_handle);

    PCF8574_config_t io_exp_config = {
        .i2c_bus_handle = &I2C0_bus_handle,
        .i2c_device_handle = &io_exp_i2c_handle,
        .max_pins = 8,
        .max_port = 1,
        .init_gpio = GPIO_NUM_26,
    };

    PCF8574_IoExp_init(&io_exp_handle, &io_exp_config);
    keypad_config_t keypad_config = {
        .buttons = 5,
        .ioExp_handle = &io_exp_handle};
    keypad_init(&keypad_config, &keypad_handle);

    return 0;
}