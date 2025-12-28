#include <stdio.h>

#include "esp_hal_spi.h"

extern void hal_spi_test(void);

void app_main(void)
{
    printf("Starting SPI HAL test...\n");

    hal_spi_test();
}
