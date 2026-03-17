#include "i2c_hal.h"
#include "hw_include.h"
#include <stdint.h>


esp_err_t i2c_hal_init(const i2c_hal_config_t *config, i2c_hal_bus_handle_t *handle)
{

    if (config == NULL || handle == NULL)
    {
        printf("Invalid pointers");
        return -1;
    }

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = config->i2c_instance,
        .scl_io_num = config->scl_io_num,
        .sda_io_num = config->sda_io_num,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &(handle->bus_handle)));

    handle->port = config->i2c_instance;

    return 0;
}

esp_err_t i2c_hal_deinit(i2c_hal_bus_handle_t *handle)
{
    return 0;

}

esp_err_t i2c_hal_add_device(const i2c_hal_device_config_t *config, i2c_hal_device_handle_t *dev_handle, i2c_hal_bus_handle_t *bus_handle)
{
    if (config == NULL || dev_handle == NULL || bus_handle == NULL)
    {
        printf("Invalid pointers");
        return -1;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = config->addr_len,
        .device_address = config->device_address,
        .scl_speed_hz = config->clk_freq_hz,
    };


    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle->bus_handle, &dev_cfg, &(dev_handle->dev_handle)));

    dev_handle->address = config->device_address;

    return 0;

}

esp_err_t i2c_hal_remove_device(i2c_hal_device_handle_t *handle)
{
    return 0;

}

esp_err_t i2c_hal_transaction_init(i2c_hal_transaction_t *transaction)
{
    if (transaction == NULL)
    {
        printf("Invalid pointers as parameter");
        return -1;
    }

    transaction->rx_buffer = NULL;
    transaction->rx_length = 0;
    transaction->tx_buffer = NULL;
    transaction->tx_length = 0;

    return 0;
}

esp_err_t i2c_hal_transfer(i2c_hal_transaction_t *transaction, i2c_hal_device_handle_t *dev_handle)
{

    esp_err_t status;

    if (transaction == NULL || dev_handle == NULL)
    {
        printf("Invalid pointers as parameter");
        return -1;
    }

    /*if tx buffer avilable perform tx*/
    if (transaction->tx_buffer != NULL && transaction->tx_length != 0)
    {
        status = i2c_master_transmit(dev_handle->dev_handle, transaction->tx_buffer, transaction->tx_length, -1); // Wait forever
        if (status != 0)
        {
            return status;
        }
    }

    /*if rx buffer avilable perform rx*/
    if (transaction->rx_buffer != NULL && transaction->rx_length != 0)
    {
        status = i2c_master_receive(dev_handle->dev_handle, transaction->rx_buffer, transaction->rx_length, -1); // Wait forever
        if (status != 0)
        {
            return status;
        }
    }

    return 0;

}

esp_err_t i2c_hal_device_probe(i2c_hal_bus_handle_t *bus_handle, i2c_hal_device_handle_t *dev_handle)
{
    esp_err_t status;

    if (dev_handle == NULL)
    {
        printf("Invalid pointers as parameter");
        return -1;
    }

    status = i2c_master_probe(bus_handle->bus_handle, dev_handle->address, -1);

    return status;
}


esp_err_t i2c_hal_probe(i2c_hal_bus_handle_t *bus_handle, uint8_t i2c_addr)
{
    esp_err_t status;

    if (bus_handle == NULL)
    {
        printf("Invalid pointers as parameter");
        return -1;
    }

    status = i2c_master_probe(bus_handle->bus_handle, i2c_addr, -1);

    return status;
}
