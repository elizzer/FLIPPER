#ifndef I2C_HAL_H
#define I2C_HAL_H

#include <stdint.h>
#include "hw_include.h"
#include <stddef.h>
#include "esp_err.h"


typedef struct i2c_hal_config_s
{
    i2c_port_t i2c_instance;
    uint8_t scl_io_num;
    uint8_t sda_io_num;
    uint32_t clk_speed_hz;

} i2c_hal_config_t;

typedef struct i2c_hal_handle_s
{
    i2c_port_t port;

} i2c_hal_handle_t;

typedef struct i2c_hal_transaction_s
{
    /* data */
    uint8_t *tx_buffer; /*!< Pointer to transmit buffer */
    uint8_t *rx_buffer; /*!< Pointer to receive buffer */
    size_t tx_length;   /*!< Length of data to be transmitted, in bytes */
    size_t rx_length;   /*!< Length of data to be received, in bytes */

}i2c_hal_transaction_t;

typedef struct i2c_hal_device_config_s
{
    uint8_t device_address;
    uint8_t clk_freq_hz;

} i2c_hal_device_config_t;

typedef struct i2c_hal_device_handle_s
{
    uint8_t address;

} i2c_hal_device_handle_t;


esp_err_t i2c_init(const i2c_hal_config_t *config, i2c_hal_handle_t *handle);
esp_err_t i2c_deinit(i2c_hal_handle_t *handle);

esp_err_t i2c_add_device(const i2c_hal_device_config_t *config, i2c_hal_device_handle_t *handle);
esp_err_t i2c_remove_device(i2c_hal_device_handle_t *handle);

esp_err_t i2c_hal_transfer(i2c_hal_transaction_t *transaction, i2c_hal_handle_t *handle);

#endif // I2C_HAL_H