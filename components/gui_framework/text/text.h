#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>
#include "display_manager.h"

typedef void (*draw_pixel_func_t)(void *, uint16_t, uint16_t, uint16_t);

int8_t render_text( draw_pixel_func_t draw_pixel_func, void *display_handle, const char *text, uint16_t x, uint16_t y, uint16_t text_color, uint16_t bg_color);

#endif