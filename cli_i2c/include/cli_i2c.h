#ifndef CLI_I2C_H
#define CLI_I2C_H
#include "kx_hal_types.h"
#include "kx_i2c_hal.h"


typedef struct 
{
    Kx_IO sda_pin;
    Kx_IO scl_pin;
    uint8_t addr;
    KxI2C_Mode_t mode; // 0 for master, 1 for slave
    KxI2C_Speed_t speed;
    KxI2C_Handle_t hal_handle; // handle to the underlying HAL I2C interface
}cliI2CConfig_t;

typedef cliI2CConfig_t* cliI2CHandle_t;

int8_t cli_i2c_register();

int8_t cli_i2c_init(cliI2CHandle_t* handle);
int8_t cli_i2c_deinit(cliI2CHandle_t handle);
void cli_i2c_help();
int8_t cli_i2c_cmd_dispatch(cliI2CHandle_t handle, const char * cmd);

int8_t cli_i2c_alloc_instance(cliI2CHandle_t handle,char *args);
int8_t cli_i2c_set_sda(cliI2CHandle_t handle, char *args);
int8_t cli_i2c_set_scl(cliI2CHandle_t handle, char *args);
int8_t cli_i2c_set_addr(cliI2CHandle_t handle, char *args);
int8_t cli_i2c_set_mode(cliI2CHandle_t handle, char *args);
int8_t cli_i2c_set_speed(cliI2CHandle_t handle, char *args);
int8_t cli_i2c_read(cliI2CHandle_t handle, char *args);
int8_t cli_i2c_write(cliI2CHandle_t handle, char *args);
int8_t cli_i2c_scan(cliI2CHandle_t handle, char *args);
int8_t cli_i2c_probe(cliI2CHandle_t handle, char *args);

#endif //CLI_I2C_H