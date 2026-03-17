#include "ILI9341.h"
#include "spi_hal.h"
#include "hw_include.h"
#include "font_manager.h"
#include "display_manager.h"
#include "text.h"
#include "esp_timer.h"
#include "shapes.h"

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

#define RECT_WIDTH 100
#define RECT_HEIGHT 60

static displayHandle_t disp_handle;

void draw_pixel_wrapper(uint16_t x, uint16_t y, uint16_t color)
{
    display_drawPixel(&disp_handle, x, y, color);
}

typedef struct
{
    int x;
    int y;
    int vx;
    int vy;
} Rectangle;

// Render function (called every iteration)
void render(Rectangle rect,uint16_t color)
{
    // In real graphics, this would draw to screen
    // Here we just print coordinates
    // printf("Drawing rectangle at (%d, %d)\n", rect.x, rect.y);
    display_clearBuffer(&disp_handle);
    draw_rectangle(rect.x, rect.y, RECT_WIDTH, RECT_HEIGHT, color);
    display_writeBuffer(&disp_handle);
}

// Update physics
void update(Rectangle *rect)
{
    // Move rectangle
    rect->x += rect->vx;
    rect->y += rect->vy;

    // Bounce on left/right edges
    if (rect->x <= 0 || rect->x + RECT_WIDTH >= SCREEN_WIDTH)
    {
        rect->vx = -rect->vx;
    }

    // Bounce on top/bottom edges
    if (rect->y <= 0 || rect->y + RECT_HEIGHT >= SCREEN_HEIGHT)
    {
        rect->vy = -rect->vy;
    }
}

void ili9341_lcd_disp_test(void)
{
    /* Test function to display something on the ILI9341 LCD */

    hal_spi_bus_config_t bus_config = {
        .spi_instance = SPI_NUM_3,
        .mosi_io_num = IO_NUM_23,
        .miso_io_num = IO_NUM_19,
        .sclk_io_num = IO_NUM_18,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = (240 * 320 * 2), // 2 KB
    };

    hal_spi_bus_handle_t bus_handle;
    esp_err_t ret = spi_bus_init(&bus_handle, &bus_config);
    if (ret != ESP_OK)
    {
        printf("Failed to initialize SPI bus\n");
        return;
    }

    hal_spi_device_config_t device_config = {
        .cs_io_num = IO_NUM_5,
        .clk_freq_hz = 30 * 1000 * 1000, // 1 MHz
    };

    hal_spi_device_handle_t device_handle;
    ret = spi_add_device(&bus_handle, &device_config, &device_handle);
    if (ret != ESP_OK)
    {
        printf("Failed to add SPI device\n");
        spi_bus_deinit(&bus_handle);
        return;
    }

    /* Example: Fill the screen with a solid color */
    uint16_t color = 0x0000; // Red color in RGB565
    // uint16_t bg_color = 0x0000; // Black color in RGB565

    ili9341_config_t config = {
        .width = 240,
        .height = 320,
        .rotation = 0,
        .display_handle = &device_handle,
        .dc_io_num = IO_NUM_25,
        .cs_io_num = IO_NUM_5,
        .reset_io_num = IO_NUM_17,
        .backlight_io_num = IO_NUM_16,
        .color_mode = 16, // RGB565
        .command_length = 8,
    };

    ili9341_handle_t handle;

    ret = ili9341_init(config, &handle);
    if (ret != 0)
    {
        printf("Failed to initialize ILI9341 display\n");
        return;
    }

    displayConfig_t disp_config = {
        .width = config.width,
        .height = config.height,
        .frame_buffer = NULL,                       // This will be allocated in display_init
        .write_buffer_func = ili9341_send_dips_buf, // Set the function pointer to send buffer to ILI9341
        .display_device_handle = &handle,           // Pass the ILI9341 handle to the display config
    };

    // displayHandle_t disp_handle;
    int8_t status;
    status = display_init(&disp_config, &disp_handle);
    if (status != 0)
    {
        printf("Failed to initialize display manager\n");
        return;
    }

    status = shape_init(draw_pixel_wrapper);
    if (status != 0)
    {
        printf("Failed to initialize shapes module\n");
        return;
    }

    Rectangle rect;

    // Initial position
    rect.x = 10;
    rect.y = 10;

    // Initial velocity
    rect.vx = 3;
    rect.vy = 2;

    Rectangle prevRect;
    display_clearBuffer(&disp_handle);
    uint16_t rect_color = 0x0000;
    while (1)
    {
        // render(prevRect,0x0000);

        update(&rect);
        render(rect,rect_color++);
        vTaskDelay(pdMS_TO_TICKS(16)); 
        prevRect.x = rect.x;
        prevRect.y = rect.y;
        // prevRect.vx = rect.vx;
        // prevRect.vy = rect.vy;

        // Control speed (~60 FPS)
        // usleep(16000);
    }

    display_deinit(&disp_handle);
}