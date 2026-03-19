#include <stdio.h>

#include "spi_hal.h"
#include "event_manager.h"
#include "gpio_hal.h"

extern void hal_spi_test(void);
extern void ili9341_lcd_disp_test(void);
extern void IoExp_test();

void app_main(void)
{
    printf("Starting SPI HAL test...\n");
    // ili9341_lcd_disp_test();
    gpio_hal_init();
    init_eventManager();
    IoExp_test();

}
