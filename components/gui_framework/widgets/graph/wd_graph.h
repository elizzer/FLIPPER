#ifndef WD_GRAPH_H
#define WD_GRAPH_H

#include <stdint.h>
#include "shapes.h"
#include "text.h"



typedef struct
{
    int x, y, width, height;
    uint16_t line_color;
    uint16_t background_color;
    char title[32];
    char x_axis_label[16];
    char y_axis_label[16];
    void (*draw_function)(uint16_t x, uint16_t y, uint16_t color);
} GraphConfig_t;

typedef struct{
    float x;
    float y;
} GraphPoint_t;

void wd_graph_draw(const GraphConfig_t* config, GraphPoint_t* data_buffer, size_t data_buffer_size);

#endif // WD_GRAPH_H