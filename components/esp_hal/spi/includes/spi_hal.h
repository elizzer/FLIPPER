#ifndef ESP_HAL_SPI_H
#define ESP_HAL_SPI_H

#include <stdint.h>

#include "esp_err.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"

typedef struct hal_spi_bus_handle_s
{

    uint8_t host_id;

} hal_spi_bus_handle_t;

typedef struct hal_spi_device_handle_s
{
    spi_device_handle_t esp_handle;
} hal_spi_device_handle_t;

typedef struct hal_spi_transaction_s
{

    uint8_t *tx_buffer; /*!< Pointer to transmit buffer */
    uint8_t *rx_buffer; /*!< Pointer to receive buffer */
    size_t tx_length;   /*!< Length of data to be transmitted, in bits */
    size_t rx_length;   /*!< Length of data to be received, in bits */

} hal_spi_transaction_t;

typedef struct hal_spi_bus_config_s
{
    uint8_t spi_instance;
    uint8_t mosi_io_num;   /*!< GPIO number for MOSI signal */
    uint8_t miso_io_num;   /*!< GPIO number for MISO signal */
    uint8_t sclk_io_num;   /*!< GPIO number for SCLK signal */
    int8_t quadwp_io_num; /*!< GPIO number for WP signal */
    int8_t quadhd_io_num; /*!< GPIO number for HD signal */
    uint32_t max_transfer_sz; /*!< Maximum transfer size in bytes */

} hal_spi_bus_config_t;

typedef struct hal_spi_device_config_s
{

    uint8_t cs_io_num;
    uint32_t clk_freq_hz;

} hal_spi_device_config_t;

esp_err_t spi_bus_init(hal_spi_bus_handle_t *bus_handle, const hal_spi_bus_config_t *bus_config);
esp_err_t spi_bus_deinit(hal_spi_bus_handle_t *bus_handle);

esp_err_t spi_add_device(hal_spi_bus_handle_t *bus_handle, const hal_spi_device_config_t *device_config, hal_spi_device_handle_t *device_handle);
esp_err_t spi_remove_device(hal_spi_device_handle_t *device_handle);

esp_err_t spi_transfer(hal_spi_device_handle_t *device_handle, hal_spi_transaction_t *transaction);

#endif // ESP_HAL_SPI_H