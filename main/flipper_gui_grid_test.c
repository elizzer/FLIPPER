#include <stdio.h>

#include "spi_hal.h"
#include "event_manager.h"
#include "gpio_hal.h"
#include "shapes.h"
#include "display_manager.h"

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

int8_t draw_square_grid(int start_x, int start_y,
                        int rows, int cols,
                        int square_size,
                        int h_gap, int v_gap,
                        int filled_index,
                        uint16_t color)
{
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {

            int x = start_x + col * (square_size + h_gap);
            int y = start_y + row * (square_size + v_gap);
            int index = row * cols + col;

            if (index == filled_index)
            {
                draw_filled_rectangle(x, y, square_size, square_size, color);
            }
            else
            {
                draw_rectangle(x, y, square_size, square_size, color);
            }
        }
    }
    return 0;
}

void app_main(void)
{
    event_manager_init();
    hardware_init();
    display_handle = get_display_handle();
    uint8_t keypad_cid = 0xff;
    event_manager_register_event(EVENT_TYPE_KEYPAD, &keypad_cid);

    EventDescription_t keypad_desc;

    shape_init(draw_pixel_wrapper);

    display_clearBuffer(display_handle);

    uint8_t gap = 2;
    uint8_t n = 10;
    uint8_t height = 10;
    uint8_t rows = 20;
    uint8_t cols = 15;

    Shape_t rect = MAKE_FILLED_RECTANGLE(10, 10, 200, 100, 0xF800);

    display_writeBuffer(display_handle);

    uint8_t selected_box = 0;
    // draw_shape(&rect);

    while (1)
    {
        display_clearBuffer(display_handle);
        event_manager_wait_event(EVENT_TYPE_KEYPAD, keypad_cid, &keypad_desc);

        // process only press event
        if (keypad_desc.keypad.action == KEYPAD_PRESSED)
        {
            switch (keypad_desc.keypad.key_number)
            {
            case 1: // UP
                if (selected_box >= cols)
                    selected_box -= cols;
                break;

            case 3: // DOWN
                if (selected_box < (rows - 1) * cols)
                    selected_box += cols;
                break;

            case 0: // LEFT
                if (selected_box % cols != 0)
                    selected_box -= 1;
                break;

            case 4: // RIGHT
                if (selected_box % cols != (cols - 1))
                    selected_box += 1;
                break;

            default:
                break;
            }
            draw_square_grid(10, 10, rows, cols, 10, 2, 2, selected_box, 0xF800);
            display_writeBuffer(display_handle);
        }
    }
}
