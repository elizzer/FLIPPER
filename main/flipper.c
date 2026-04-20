#include <stdio.h>

#include "spi_hal.h"
#include "event_manager.h"
#include "gpio_hal.h"
#include "shapes.h"
#include "display_manager.h"
#include "wd_label.h"
#include "wd_graph.h"

#define SWAP_16BIT_BYTE_ORDER(d) (((d) >> 8) | ((d) << 8))

extern int8_t hardware_init();
extern displayHandle_t *get_display_handle();
displayHandle_t *display_handle;

void draw_pixel_wrapper(uint16_t x, uint16_t y, uint16_t color)
{
    // printf("\nWriting pxl value 0x%04x",SWAP_16BIT_BYTE_ORDER(color));
    // printf("\nWriting pxl value 0x%04x",color);
    int8_t ret = display_drawPixel(display_handle, x, y, color);
    if (ret != 0)
    {
        printf("Failed to draw pixel at (%d, %d)\n", x, y);
    }
}



void app_main(void)
{
    event_manager_init();
    hardware_init();
    display_handle = get_display_handle();

    shape_init(draw_pixel_wrapper);
    display_clearBuffer(display_handle);

    display_writeBuffer(display_handle);

   GraphConfig_t graph_config = {
        .x = 10,
        .y = 10,
        .width = 200,
        .height = 200,
        .line_color = 0xFFFF, // White
        .background_color = 0x0000, // Black
        .draw_function = draw_pixel_wrapper
    };

    GraphPoint_t data_buffer[5] = {
        {0.1f, 0.2f},
        {0.2f, 0.4f},
        {0.3f, 0.1f},
        {0.4f, 0.5f},
        {0.5f, 0.3f}
    };

    wd_graph_draw(&graph_config, data_buffer, 5);

    display_writeBuffer(display_handle);


}
