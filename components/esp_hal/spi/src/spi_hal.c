#include "spi_hal.h"

esp_err_t spi_bus_init(hal_spi_bus_handle_t *bus_handle, const hal_spi_bus_config_t *bus_config)
{

    const spi_bus_config_t config = {
        .mosi_io_num = bus_config->mosi_io_num,
        .miso_io_num = bus_config->miso_io_num,
        .sclk_io_num = bus_config->sclk_io_num,
        .quadwp_io_num = bus_config->quadwp_io_num,
        .quadhd_io_num = bus_config->quadhd_io_num,
    };

    esp_err_t ret = spi_bus_initialize(bus_config->spi_instance, &config, SPI_DMA_CH_AUTO); /* DMA CH AUTO hard coded as of now */

    if (ret == ESP_OK)
    {
        bus_handle->host_id = bus_config->spi_instance;
    }

    return ret;
}

esp_err_t spi_bus_deinit(hal_spi_bus_handle_t *bus_handle)
{
    return spi_bus_free(bus_handle->host_id);
}

esp_err_t spi_add_device(hal_spi_bus_handle_t *bus_handle, const hal_spi_device_config_t *device_config, hal_spi_device_handle_t *device_handle)
{
    const spi_device_interface_config_t dev_config = {
        .clock_speed_hz = device_config->clk_freq_hz,
    };

    esp_err_t ret = spi_bus_add_device(bus_handle->host_id, &dev_config, &device_handle->esp_handle);

    return ret;
}

esp_err_t spi_remove_device(hal_spi_device_handle_t *device_handle)
{
    return spi_bus_remove_device(device_handle->esp_handle);
}

esp_err_t spi_transfer(hal_spi_device_handle_t *device_handle, hal_spi_transaction_t *transaction)
{

    spi_transaction_t trans = {
        .tx_buffer = transaction->tx_buffer,
        .rx_buffer = transaction->rx_buffer,
        .length = transaction->tx_length,
        .rxlength = transaction->rx_length,
    };

    return spi_device_polling_transmit(device_handle->esp_handle, &trans);
}