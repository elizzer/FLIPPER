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

int8_t cli_i2c_init(void** handle);
int8_t cli_i2c_deinit(void* handle);
void cli_i2c_help();
int8_t cli_i2c_cmd_dispatch(void* handle, const char * cmd);

int8_t cli_i2c_alloc_instance(void* handle,char *args);
int8_t cli_i2c_set_sda(void* handle, char *args);
int8_t cli_i2c_set_scl(void* handle, char *args);
int8_t cli_i2c_set_addr(void* handle, char *args);
int8_t cli_i2c_set_mode(void* handle, char *args);
int8_t cli_i2c_set_speed(void* handle, char *args);
int8_t cli_i2c_read(void* handle, char *args);
int8_t cli_i2c_write(void* handle, char *args);
int8_t cli_i2c_scan(void* handle, char *args);
int8_t cli_i2c_probe(void* handle, char *args);

#endif //CLI_I2C_H