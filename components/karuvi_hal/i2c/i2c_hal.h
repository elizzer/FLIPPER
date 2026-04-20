#ifndef I2C_HAL_H
#define I2C_HAL_H

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

typedef uint8_t I2C_7BitAddress_t;

typedef struct I2cHalConfig_s
{
    i2c_port_t i2c_instance;
    uint8_t scl_io_num;
    uint8_t sda_io_num;
    uint32_t clk_speed_hz;

} I2cHalConfig_t;

typedef enum I2cHalAddrBitLen_e{
    I2C_HAL_ADDR_BIT_LEN_7,
    I2C_HAL_ADDR_BIT_LEN_10
}I2cHalAddrBitLen_t;

typedef struct I2cHalBusHandle_s
{
    i2c_port_t port;
    i2c_master_bus_handle_t bus_handle;

} I2cHalBusHandle_t;

typedef struct I2cHalTransaction_s
{
    /* data */
    uint8_t *tx_buffer; /*!< Pointer to transmit buffer */
    uint8_t *rx_buffer; /*!< Pointer to receive buffer */
    size_t tx_length;   /*!< Length of data to be transmitted, in bytes */
    size_t rx_length;   /*!< Length of data to be received, in bytes */

}I2cHalTransaction_t;

typedef struct I2cHalDeviceConfig_s
{
    I2cHalAddrBitLen_t addr_len; 
    uint16_t device_address;
    uint32_t clk_freq_hz;

} I2cHalDeviceConfig_t;

typedef struct I2cHalDeviceHandle_s
{
    uint16_t address;
    i2c_master_dev_handle_t dev_handle;

} I2cHalDeviceHandle_t;


esp_err_t i2c_hal_init(const I2cHalConfig_t *config, I2cHalBusHandle_t *handle);
esp_err_t i2c_hal_deinit(I2cHalBusHandle_t *handle);

// esp_err_t i2c_hal_add_device(const I2cHalDeviceConfig_t *config, I2cHalDeviceHandle_t *handle);
esp_err_t i2c_hal_add_device(const I2cHalDeviceConfig_t *config, I2cHalDeviceHandle_t *dev_handle, I2cHalBusHandle_t *bus_handle);
esp_err_t i2c_hal_remove_device(I2cHalDeviceHandle_t *handle);

esp_err_t i2c_hal_transaction_init(I2cHalTransaction_t *transaction);
esp_err_t i2c_hal_transfer(I2cHalTransaction_t *transaction, I2cHalDeviceHandle_t *handle);


esp_err_t i2c_hal_device_probe(I2cHalBusHandle_t *bus_handle, I2cHalDeviceHandle_t *dev_handle);
esp_err_t i2c_hal_probe(I2cHalBusHandle_t *bus_handle, uint8_t i2c_addr);



#endif // I2C_HAL_H