#include "i2c_hal.h"
#include "PCF8574_IoExp.h"
#include <stdint.h>
#include <driver/gpio.h>
#include "event_manager.h"

PCF8574_handle_t io_exp_handle;

void IO_exp_event_handler()
{
    uint8_t readValue;
    int8_t status;
    status = PCF8574_IoExp_readPin(&io_exp_handle, 1, &readValue);
    printf("\nRead Value 0x%20x", readValue);
}

void IoExp_test()
{

    // Create i2c bus
    const i2c_hal_config_t i2c_bus_config = {
        .clk_speed_hz = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = GPIO_NUM_22,
        .sda_io_num = GPIO_NUM_21,
        .i2c_instance = I2C_NUM_0,
    };

    i2c_hal_bus_handle_t i2c_bus_handle;

    i2c_hal_init(&i2c_bus_config, &i2c_bus_handle);

    // scan the bus to gat all address
    int8_t addr_status;
    printf("\nI2C bus scanning...");
    for (uint8_t addr = 0; addr < 0x7F; addr++)
    {
        addr_status = i2c_hal_probe(&i2c_bus_handle, addr);
        if (addr_status == 0)
        {
            printf("\n I2C device found at 0x%20X", addr);
        }
    }
    printf("\nI2C bus scanning complete");

    const i2c_hal_device_config_t io_exp_i2c_dev_config = {
        .addr_len = I2C_HAL_ADDR_BIT_LEN_7,
        .clk_freq_hz = 100000,
        .device_address = 32,
    };

    i2c_hal_device_handle_t io_exp_i2c_dev_handle;

    i2c_hal_add_device(&io_exp_i2c_dev_config, &io_exp_i2c_dev_handle, &i2c_bus_handle);

    i2c_hal_transaction_t transaction;
    i2c_hal_transaction_init(&transaction);

    uint8_t sendData;

    transaction.rx_buffer = &readValue;
    transaction.rx_length = 1;

    // // add device
    PCF8574_config_t io_exp_config = {
        .i2c_bus_handle = &i2c_bus_handle,
        .i2c_device_handle = &io_exp_i2c_dev_handle,
        .max_pins = 8,
        .max_port = 1,
        .init_gpio = GPIO_NUM_32,
    };

    // init io exp
    PCF8574_IoExp_init(&io_exp_handle, &io_exp_config);

    uint8_t cid;

    register_event(KEYPAD_EVENT,&cid);

    printf("\n Keypad event registerd consumer id is %d",cid);

    while (1)
    {
        
    }
}