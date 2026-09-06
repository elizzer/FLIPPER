#ifndef KX_I2C_HAL_H
#define KX_I2C_HAL_H
#include "kx_hal_types.h"
#include <stdint.h>
#include <stddef.h>

typedef enum
{
    KX_I2C_MODE_MASTER = 0,
    KX_I2C_MODE_SLAVE = 1
} KxI2C_Mode_t;

typedef enum
{
    KX_I2C_SPEED_STANDARD, // 100 kHz
    KX_I2C_SPEED_FAST,     // 400 kHz
    KX_I2C_SPEED_FAST_PLUS, // 1 MHz
    KX_I2C_SPEED_HIGH     // 3.4 MHz
} KxI2C_Speed_t;

typedef enum
{
    KX_I2C_INSTANCE_0 = 0,
    KX_I2C_INSTANCE_1 = 1,
    KX_I2C_INSTANCE_MAX
} KxI2C_instance_t;

typedef enum{

    KX_I2C_7_BIT_ADDR = 0,
    KX_I2C_10_BIT_ADDR,

}KxI2C_AddrMode_t;

typedef struct 
{
    KxI2C_instance_t instance; // I2C instance (I2C0, I2C1, etc.);
    bool is_used; // Flag to indicate if the I2C instance is initialized
    bool is_initialized; // Flag to indicate if the I2C instance is initialized
    KxI2C_Speed_t speed_mode;
    KxI2C_Mode_t device_mode;
}KxI2C_ChannelConfig_t;

typedef KxI2C_ChannelConfig_t* KxI2C_Handle_t;

Kx_ErrorCode kx_i2c_init();
Kx_ErrorCode kx_i2c_deinit();
Kx_ErrorCode i2c_alloc_instance(KxI2C_Handle_t *handle, int8_t instance);
Kx_ErrorCode KxI2C_free_instance(KxI2C_Handle_t handle);
Kx_ErrorCode KxI2C_set_device_mode(KxI2C_Handle_t handle, KxI2C_Mode_t mode);
Kx_ErrorCode KxI2C_set_speed(KxI2C_Handle_t handle, KxI2C_Speed_t speed);
Kx_ErrorCode KxI2C_set_sda(KxI2C_Handle_t handle, Kx_IO sda);
Kx_ErrorCode KxI2C_set_scl(KxI2C_Handle_t handle, Kx_IO scl);
Kx_ErrorCode KxI2C_set_slave_addr(KxI2C_Handle_t handle, KxI2C_AddrMode_t addr_mode, uint16_t addr);
Kx_ErrorCode KxI2C_probe(KxI2C_Handle_t handle, uint16_t addr);
Kx_ErrorCode KxI2C_master_read(KxI2C_Handle_t handle, uint16_t s_addr, uint8_t* data, size_t length);
Kx_ErrorCode KxI2C_master_write(KxI2C_Handle_t handle, uint16_t s_addr, uint8_t* data, size_t length);
Kx_ErrorCode KxI2C_slave_read(KxI2C_Handle_t handle, uint8_t* data, size_t length);
Kx_ErrorCode KxI2C_slave_write(KxI2C_Handle_t handle, uint8_t* data, size_t length);


#endif // KX_I2C_HAL_H