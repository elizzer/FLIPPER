#ifndef ESP_HAL_SPI_H
#define ESP_HAL_SPI_H

#include <stdint.h>

#include "esp_err.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"

typedef struct SpiHalBusHandle_s
{   
    uint8_t host_id;

} SpiHalBusHandle_t;

typedef struct SpiHalDeviceHandle_s
{
    spi_device_handle_t esp_handle;
} SpiHalDeviceHandle_t;

typedef struct SpiHalTransaction_s
{

    uint8_t *tx_buffer; /*!< Pointer to transmit buffer */
    uint8_t *rx_buffer; /*!< Pointer to receive buffer */
    size_t tx_length;   /*!< Length of data to be transmitted, in bits */
    size_t rx_length;   /*!< Length of data to be received, in bits */

} SpiHalTransaction_t;

typedef struct SpiHalBusConfig_s
{
    uint8_t spi_instance;
    uint8_t mosi_io_num;   /*!< GPIO number for MOSI signal */
    uint8_t miso_io_num;   /*!< GPIO number for MISO signal */
    uint8_t sclk_io_num;   /*!< GPIO number for SCLK signal */
    int8_t quadwp_io_num; /*!< GPIO number for WP signal */
    int8_t quadhd_io_num; /*!< GPIO number for HD signal */
    uint32_t max_transfer_sz; /*!< Maximum transfer size in bytes */

} SpiHalBusConfig_t;

typedef struct SpiHalDeviceConfig_s
{

    uint8_t cs_io_num;
    uint32_t clk_freq_hz;

} SpiHalDeviceConfig_t;

esp_err_t spi_hal_bus_init(SpiHalBusHandle_t *bus_handle, const SpiHalBusConfig_t *bus_config);
esp_err_t spi_hal_bus_deinit(SpiHalBusHandle_t *bus_handle);

esp_err_t spi_hal_add_device(SpiHalBusHandle_t *bus_handle, const SpiHalDeviceConfig_t *device_config, SpiHalDeviceHandle_t *device_handle);
esp_err_t spi_hal_remove_device(SpiHalDeviceHandle_t *device_handle);

esp_err_t spi_hal_transfer(SpiHalDeviceHandle_t *device_handle, SpiHalTransaction_t *transaction);

#endif // ESP_HAL_SPI_H