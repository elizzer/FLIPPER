#ifndef WD_LABEL_H
#define WD_LABEL_H

#include <stdint.h>

typedef void (*draw_function_t)(uint16_t x, uint16_t y, uint16_t color);

typedef enum {
    LABEL_ALIGN_LEFT,
    LABEL_ALIGN_CENTER,
    LABEL_ALIGN_RIGHT
} label_align_t;

typedef struct label_attr_s{
    char text[32];
    uint16_t x;
    uint16_t y;
    uint16_t color;
    uint16_t bg_color; // how to represent transparent background
    uint16_t width;
    uint16_t height;
    uint16_t padding;
    label_align_t align;
    draw_function_t draw_pixel;
}label_attr_t;

void draw_label(label_attr_t attr);

#endif // WD_LABEL_H