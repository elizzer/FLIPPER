#ifndef HW_INCLUDE_H
#define HW_INCLUDE_H

#define MAX_GPIO_NUM 40
#define MAX_SPI_NUM 3

typedef enum {
    SPI_NUM_0 = 0,
    SPI_NUM_1 = 1,
    SPI_NUM_2 = 2,
} spi_port_t;

#define MAX_I2C_NUM 2
typedef enum {
    I2C_NUM_0 = 0,
    I2C_NUM_1 = 1,
} i2c_port_t;


#endif // HW_INCLUDE_H