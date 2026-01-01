#include <stdio.h>

#include "spi_hal.h"

extern void hal_spi_test(void);

void app_main(void)
{
    printf("Starting SPI HAL test...\n");

    hal_spi_test();
}
