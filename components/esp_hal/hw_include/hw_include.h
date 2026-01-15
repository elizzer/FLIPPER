#ifndef HW_INCLUDE_H
#define HW_INCLUDE_H

#include "driver/spi_common.h"
#include <driver/gpio.h>


#define MAX_GPIO_NUM 40
#define MAX_SPI_NUM 3

typedef enum
{
    SPI_NUM_1 = SPI1_HOST,
    SPI_NUM_2 = SPI2_HOST,
    SPI_NUM_3 = SPI3_HOST,
} spi_port_t;

#define MAX_I2C_NUM 2
// typedef enum
// {
//     I2C_NUM_0 = 0,
//     I2C_NUM_1 = 1,
// } i2c_port_t;

typedef enum
{
    IO_NUM_0 = 0,
    IO_NUM_1 = 1,
    IO_NUM_2,
    IO_NUM_3,
    IO_NUM_4,
    IO_NUM_5,
    IO_NUM_6,
    IO_NUM_7,
    IO_NUM_8,
    IO_NUM_9,
    IO_NUM_10,
    IO_NUM_11,
    IO_NUM_12,
    IO_NUM_13,
    IO_NUM_14,
    IO_NUM_15,
    IO_NUM_16,
    IO_NUM_17,
    IO_NUM_18,
    IO_NUM_19,
    IO_NUM_20,
    IO_NUM_21,
    IO_NUM_22,
    IO_NUM_23,
    IO_NUM_24,
    IO_NUM_25,
    IO_NUM_26 = GPIO_NUM_26,
} io_num_t;

#endif // HW_INCLUDE_H