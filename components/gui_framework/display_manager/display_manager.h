#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <string.h>
#include <stdint.h>

// #ifndef SWAP_16BIT_BYTE_ORDER
#define SWAP_16BIT_BYTE_ORDER(d) (((d) >> 8) | ((d) << 8))
// #endif

typedef struct displayConfig displayHandle_t;

typedef struct displayConfig {
    uint16_t width;
    uint16_t height;
    uint16_t *frame_buffer;
    int8_t (*write_buffer_func)(uint16_t *buf, uint16_t x, uint16_t y, uint16_t width, uint16_t height, void *handle); //function pointer to write the frame buffer to the display, this is a blocking call and should be called after all the drawing operations are done on the frame buffer
    void *display_device_handle; 

} displayConfig_t;

int8_t display_init(displayConfig_t *config, displayHandle_t *handle);
int8_t display_deinit(displayHandle_t *handle);
int8_t display_writeBuffer(displayHandle_t *handle); //writes the frame buffer to the display, this is a blocking call and should be called after all the drawing operations are done on the frame buffer
int8_t display_clearBuffer(displayHandle_t *handle);
int8_t display_drawPixel(void *handle, uint16_t x, uint16_t y, uint16_t color);
int8_t display_getBufferPtr(displayHandle_t *handle, uint16_t **buffer);

#endif