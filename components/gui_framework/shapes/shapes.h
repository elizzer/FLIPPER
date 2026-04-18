#ifndef SHAPES_H
#define SHAPES_H

#include <stdint.h>

typedef void (*draw_func_t)(uint16_t x, uint16_t y, uint16_t color);

#define MAKE_HORIZONTAL_LINE(X1, X2, Y, COLOR)                           \
    (Shape_t)                                                            \
    {                                                                    \
        .type = SHAPE_HORIZONTAL_LINE, .shape.h_line = {.x1 = X1,        \
                                                        .x2 = X2,        \
                                                        .y = Y,          \
                                                        .color = COLOR } \
    }

#define MAKE_VERTICAL_LINE(X, Y1, Y2, COLOR)                           \
    (Shape_t)                                                          \
    {                                                                  \
        .type = SHAPE_VERTICAL_LINE, .shape.v_line = {.x = X,          \
                                                      .y1 = Y1,        \
                                                      .y2 = Y2,        \
                                                      .color = COLOR } \
    }

#define MAKE_LINE(X1, Y1, X2, Y2, COLOR)                    \
    (Shape_t)                                               \
    {                                                       \
        .type = SHAPE_LINE, .shape.line = {.x1 = X1,        \
                                           .y1 = Y1,        \
                                           .x2 = X2,        \
                                           .y2 = Y2,        \
                                           .color = COLOR } \
    }

#define MAKE_RECTANGLE(X, Y, W, H, COLOR)                             \
    (Shape_t)                                                         \
    {                                                                 \
        .type = SHAPE_RECTANGLE, .shape.rectangle = {.x = X,          \
                                                     .y = Y,          \
                                                     .width = W,      \
                                                     .height = H,     \
                                                     .color = COLOR } \
    }

#define MAKE_FILLED_RECTANGLE(X, Y, W, H, COLOR)                                    \
    (Shape_t)                                                                       \
    {                                                                               \
        .type = SHAPE_FILLED_RECTANGLE, .shape.filled_rectangle = {.x = X,          \
                                                                   .y = Y,          \
                                                                   .width = W,      \
                                                                   .height = H,     \
                                                                   .color = COLOR } \
    }

#define MAKE_CIRCLE(X, Y, R, COLOR)                             \
    (Shape_t)                                                   \
    {                                                           \
        .type = SHAPE_CIRCLE, .shape.circle = {.x_center = X,   \
                                               .y_center = Y,   \
                                               .radius = R,     \
                                               .color = COLOR } \
    }

#define MAKE_FILLED_CIRCLE(X, Y, R, COLOR)                                    \
    (Shape_t)                                                                 \
    {                                                                         \
        .type = SHAPE_FILLED_CIRCLE, .shape.filled_circle = {.x_center = X,   \
                                                             .y_center = Y,   \
                                                             .radius = R,     \
                                                             .color = COLOR } \
    }

typedef enum
{
    SHAPE_HORIZONTAL_LINE,
    SHAPE_VERTICAL_LINE,
    SHAPE_LINE,
    SHAPE_RECTANGLE,
    SHAPE_FILLED_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_FILLED_CIRCLE,
} ShapeType_t;

typedef struct
{
    int x1, x2, y;
    uint16_t color;
} shape_HorizontalLine_t;

typedef struct
{
    int x, y1, y2;
    uint16_t color;
} shape_VerticalLine_t;

typedef struct
{
    int x1, y1, x2, y2;
    uint16_t color;
} shape_Line_t;

typedef struct
{
    int x, y, width, height;
    uint16_t color;
} shape_Rectangle_t;

typedef struct
{
    int x, y, width, height;
    uint16_t color;
} shape_FilledRectangle_t;

typedef struct
{
    int x_center, y_center, radius;
    uint16_t color;
} shape_Circle_t;

typedef struct
{
    int x_center, y_center, radius;
    uint16_t color;
} shape_FilledCircle_t;

typedef struct
{
    ShapeType_t type;
    union
    {
        shape_HorizontalLine_t h_line;
        shape_VerticalLine_t v_line;
        shape_Line_t line;
        shape_Rectangle_t rectangle;
        shape_FilledRectangle_t filled_rectangle;
        shape_Circle_t circle;
        shape_FilledCircle_t filled_circle;
    } shape;
} Shape_t;

int8_t shape_init(draw_func_t draw_pixel_func);
int8_t shape_deinit();

int8_t draw_shape(Shape_t *);

int8_t draw_horizontal_line(int x1, int x2, int y, uint16_t color);
int8_t draw_vertical_line(int x, int y1, int y2, uint16_t color);
int8_t draw_line(int x1, int y1, int x2, int y2, uint16_t color);
int8_t draw_rectangle(int x, int y, int width, int height, uint16_t color);
int8_t draw_filled_rectangle(int x, int y, int width, int height, uint16_t color);
int8_t draw_circle(int x_center, int y_center, int radius, uint16_t color);
int8_t draw_filled_circle(int x_center, int y_center, int radius, uint16_t color);

#endif