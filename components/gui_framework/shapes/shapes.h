#ifndef SHAPES_H
#define SHAPES_H

#include <stdint.h>

typedef void (*draw_func_t)(uint16_t x, uint16_t y, uint16_t color);

int8_t shape_init(draw_func_t draw_pixel_func);
int8_t shape_deinit();

int8_t draw_horizontal_line(int x1, int x2, int y, uint16_t color);
int8_t draw_vertical_line(int x, int y1, int y2, uint16_t color);
int8_t draw_line(int x1, int y1, int x2, int y2, uint16_t color);
int8_t draw_rectangle(int x, int y, int width, int height, uint16_t color);
int8_t draw_filled_rectangle(int x, int y, int width, int height, uint16_t color);
int8_t draw_circle(int x_center, int y_center, int radius, uint16_t color);
int8_t draw_filled_circle(int x_center, int y_center, int radius, uint16_t color);
int8_t draw_polygon (uint16_t color, int points, ...);
int8_t draw_filled_polygon (uint16_t color, int points, ...);
//int8_t draw_alphabet_seven_seg (int number, uint16_t color);
int8_t draw_arc(int quad, int x_center, int y_center, int radius, uint16_t color);


#endif
