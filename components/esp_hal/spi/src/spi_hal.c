#include "spi_hal.h"


esp_err_t spi_hal_bus_init(SpiHalBusHandle_t *bus_handle, const SpiHalBusConfig_t *bus_config)
{

    spi_bus_config_t config = {
        .mosi_io_num = bus_config->mosi_io_num,
        .miso_io_num = bus_config->miso_io_num,
        .sclk_io_num = bus_config->sclk_io_num,
        // .quadwp_io_num = bus_config->quadwp_io_num,
        // .quadhd_io_num = bus_config->quadhd_io_num,
         .max_transfer_sz = bus_config->max_transfer_sz,
    };

 

    esp_err_t ret = spi_bus_initialize(bus_config->spi_instance, &config, SPI_DMA_CH_AUTO); /* DMA CH AUTO hard coded as of now */

   

    if (ret == ESP_OK)
    {
        bus_handle->host_id = bus_config->spi_instance;
    }

    return ret;
}

esp_err_t spi_hal_bus_deinit(SpiHalBusHandle_t *bus_handle)
{
    return spi_bus_free(bus_handle->host_id);
}

esp_err_t spi_hal_add_device(SpiHalBusHandle_t *bus_handle, const SpiHalDeviceConfig_t *device_config, SpiHalDeviceHandle_t *device_handle)
{
    const spi_device_interface_config_t dev_config = {
        .mode = 0,
        .clock_speed_hz = device_config->clk_freq_hz,
        .spics_io_num = device_config->cs_io_num,
        .queue_size = 5
    };


    esp_err_t ret = spi_bus_add_device(bus_handle->host_id, &dev_config, &device_handle->esp_handle);

    return ret;
}

esp_err_t spi_hal_remove_device(SpiHalDeviceHandle_t *device_handle)
{
    return spi_bus_remove_device(device_handle->esp_handle);
}

esp_err_t spi_hal_transfer(SpiHalDeviceHandle_t *device_handle, SpiHalTransaction_t *transaction)
{

    if(device_handle==NULL){
        printf("NULL handle\n");
        return -1;
    }

    spi_transaction_t trans = {
        .tx_buffer = transaction->tx_buffer,
        .rx_buffer = transaction->rx_buffer,
        .length = transaction->tx_length,
        .rxlength = transaction->rx_length,
    };

    return spi_device_polling_transmit(device_handle->esp_handle, &trans);
}