#include <stdio.h>

#include "spi_hal.h"
#include "event_manager.h"
#include "gpio_hal.h"
#include "shapes.h"
#include "display_manager.h"
#include "wd_label.h"

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

struct menu_item
{
    char text[32];
    void (*action)(void);
    uint8_t selected;

    
};

struct menu_item menu_items[] = {
    {"Logic Analyzer",  NULL, 1},
    {"Serial Monitor",  NULL, 0},
    {"Signal Gen",      NULL, 0},
    {"Power Profiler",  NULL, 0},
    {"I2C Scanner",     NULL, 0},
    {"SPI Inspector",   NULL, 0},
    {"UART Bridge",     NULL, 0},
    {"Settings",        NULL, 0},
};

label_attr_t selected_label_attr = {
    .x = 0,
    .y = 0,
    .color = 0xFFFF,    // White color in RGB565
    .bg_color = 0x0000, // Black background
    .width = 240,
    .height = 50,
    .padding = 5,
    .align = LABEL_ALIGN_CENTER,
    .draw_pixel = draw_pixel_wrapper,
};

label_attr_t unselected_label_attr = {
    .x = 0,
    .y = 0,
    .color = 0x0000,    // White color in RGB565
    .bg_color = 0xFFFF, // Black background
    .width = 240,
    .height = 50,
    .padding = 5,
    .align = LABEL_ALIGN_LEFT,
    .draw_pixel = draw_pixel_wrapper,
};
uint8_t options_count = 8;

void app_main(void)
{
    event_manager_init();
    hardware_init();
    display_handle = get_display_handle();

    shape_init(draw_pixel_wrapper);
    display_clearBuffer(display_handle);

    display_writeBuffer(display_handle);

    uint8_t keypad_cid = 0xff;
    event_manager_register_event(EVENT_TYPE_KEYPAD, &keypad_cid);
    EventDescription_t keypad_desc;
    int8_t selected_index = 0;
    uint8_t max_visible_options = 6;
    uint8_t render_window_start = 0;
    while (1)
    {
        // Wait for keypad events and update the menu selection accordingly
        // For demonstration, we'll just toggle the selection every 2 seconds
        event_manager_wait_event(EVENT_TYPE_KEYPAD, keypad_cid, &keypad_desc);
        // Process only press events
        if (keypad_desc.keypad.action == KEYPAD_PRESSED)
        {
            printf("\nKeypad event received: key_number=%d, action=%d",
                   keypad_desc.keypad.key_number, keypad_desc.keypad.action);
            switch (keypad_desc.keypad.key_number)
            {
            case 1:                                                                    // UP
                selected_index = (selected_index - 1 + options_count) % options_count; // Wrap around
                break;

            case 3:                                                    // DOWN
                selected_index = (selected_index + 1) % options_count; // Wrap around
                break;

            default:
                break;
            }
        }
        if (selected_index < max_visible_options)
        {
            render_window_start = 0;
        }
        else if (selected_index >= max_visible_options)
        {
            render_window_start = selected_index - max_visible_options + 1;
        }
        printf("\nSelected index: %d, Render window start: %d", selected_index, render_window_start);
        // Redraw the menu
        display_clearBuffer(display_handle);

        for (int i = render_window_start; i <= render_window_start + max_visible_options - 1; i++)
        {
            label_attr_t label_attr = (i == selected_index) ? selected_label_attr : unselected_label_attr;
            label_attr.y = (i - render_window_start) * 50; // Position each label vertically
            strncpy(label_attr.text, menu_items[i].text, sizeof(label_attr.text) - 1);
            draw_label(label_attr);
        }

        display_writeBuffer(display_handle);
    }
}
