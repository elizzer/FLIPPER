#include "shapes.h"
#include <stddef.h>
#include <stdio.h>

draw_func_t draw_pixel = NULL;

static int8_t validate_draw_pixel()
{
    if (draw_pixel == NULL)
    {
        return -1; // Error: draw_pixel function pointer is not set
    }
    return 0; // Success
}

int8_t shape_init(draw_func_t draw_pixel_func)
{
    if (draw_pixel_func == NULL)
    {
        return -1; // Error: draw_pixel_func is NULL
    }
    draw_pixel = draw_pixel_func;
    return 0; // Success
}

int8_t draw_horizontal_line(int x1, int x2, int y, uint16_t color)
{
    // return validate_draw_pixel() == 0 ? 0 : -1;
    int8_t result = validate_draw_pixel();
    if (result != 0)
    {
        printf("Error: draw_pixel function pointer is not set\n");
        return -1; // Error: draw_pixel function pointer is not set
    }
    if (x1 > x2)
    {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    for (int x = x1; x < x2; x++)
    {
        draw_pixel(x, y, color);
    }
    return 0;
}
int8_t draw_vertical_line(int x, int y1, int y2, uint16_t color)
{
    int8_t result = validate_draw_pixel();
    if (result != 0)
    {
        printf("Error: draw_pixel function pointer is not set\n");
        return -1; // Error: draw_pixel function pointer is not set
    }
    if (y1 > y2)
    {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }
    for (int y = y1; y < y2; y++)
    {
        draw_pixel(x, y, color);
    }
    return 0;
}
// int8_t draw_line (int x1, int y1, int x2, int y2, uint16_t color) {
// 	int8_t result = validate_draw_pixel ();
// 	if (result != 0) {
// 		printf ("Error : draw_pixel function pointer is not set \n");
// 		return -1; //Failure
// 	}

// 	if (y2 - y1 == 0 && x2 - x1 == 0) { //Single point
// 		draw_pixel (x1, y1, color);
// 		return 0; //Success
// 	}
// 	if (y2 - y1 == 0) { //horizontal line
// 		draw_horizontal_line (x1, x2, y1, color);
// 		return 0; //Success
// 	}
// 	if (x2 - x1 == 0) { //vertical line
// 		draw_vertical_line (x1, y1, y2, color);
// 		return 0; //Success
// 	}

// 	//Skew line
// 	if (x1 > x2)
// 	{
//         	int temp = x1;
//         	x1 = x2;
//         	x2 = temp;
// 	}
// 	if (x1 > x2)
// 	{
//         	int temp = x1;
// 	        x1 = x2;
//         	x2 = temp;
// 	}
// 	int8_t dx = (x2 - x1), dy = (y2 - y1);
// 	for (int x = x1; x < x2; x += dx, y += dy) {
// 		draw_pixel (x, y, color);
// 	}
// 	return 0; //success
// }
int8_t draw_line(int x1, int y1, int x2, int y2, uint16_t color)
{
    int8_t result = validate_draw_pixel();
    if (result != 0)
    {
        printf("Error: draw_pixel function pointer is not set\n");
        return -1; // Failure
    }

    // Single point
    if (x1 == x2 && y1 == y2)
    {
        draw_pixel(x1, y1, color);
        return 0;
    }

    // Horizontal line
    if (y1 == y2)
    {
        draw_horizontal_line(x1, x2, y1, color);
        return 0;
    }

    // Vertical line
    if (x1 == x2)
    {
        draw_vertical_line(x1, y1, y2, color);
        return 0;
    }

    // Skewed line — Bresenham's algorithm
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1; // step direction X
    int sy = (y1 < y2) ? 1 : -1; // step direction Y
    int err = dx + dy;

    while (1)
    {
        draw_pixel(x1, y1, color);

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y1 += sy;
        }
    }

    return 0; // Success
}
int8_t draw_rectangle(int x, int y, int width, int height, uint16_t color)
{
    // return validate_draw_pixel() == 0 ? 0 : -1;
    // printf("Drawing rectangle at x=%d, y=%d, width=%d, height=%d\n", x, y, width, height);
    // printf("Drawing horizontal line from (%d, %d) to (%d, %d)\n", x, y, x + width - 1, y);
    draw_horizontal_line(x, x + width - 1, y, color);
    // printf("Drawing horizontal line from (%d, %d) to (%d, %d)\n", x, y + height - 1, x + width - 1, y + height - 1);
    draw_horizontal_line(x, x + width - 1, y + height - 1, color);
    // printf("Drawing vertical line from (%d, %d) to (%d, %d)\n", x, y, x, y + height - 1);
    draw_vertical_line(x, y, y + height - 1, color);
    // printf("Drawing vertical line from (%d, %d) to (%d, %d)\n", x + width - 1, y, x + width - 1, y + height - 1);
    draw_vertical_line(x + width - 1, y, y + height - 1, color);
    return 0;
}

int8_t draw_filled_rectangle(int x, int y, int width, int height, uint16_t color)
{
    int8_t result = validate_draw_pixel();
    if (result != 0)
    {
        printf("Error: draw_pixel function pointer is not set\n");
        return -1; // Error: draw_pixel function pointer is not set
    }
    if (width == 0 && height == 0)
    { // Point
        draw_pixel(x, y, color);
        return 0;
    }
    if (width == 0)
    { // Vertical line
        draw_vertical_line(x, y, y + height, color);
        return 0;
    }
    if (height == 0)
    { // Horizontal line
        draw_horizontal_line(x, x + width, y, color);
        return 0;
    }

    // filling rectangle
    for (int i = 0; i < height; i++, y++)
    {
        draw_horizontal_line(x, x + width, y, color);
    }
    return 0;
}

int8_t draw_circle(int x_center, int y_center, int radius, uint16_t color)
{
    return 0;
}
int8_t draw_filled_circle(int x_center, int y_center, int radius, uint16_t color)
{
    return 0;
}

int8_t draw_shape(Shape_t *s)
{
    if (s == NULL)
        return -1;

    switch (s->type)
    {
    case SHAPE_HORIZONTAL_LINE:
        return draw_horizontal_line(s->shape.h_line.x1,
                                    s->shape.h_line.x2,
                                    s->shape.h_line.y,
                                    s->shape.h_line.color);

    case SHAPE_VERTICAL_LINE:
        return draw_vertical_line(s->shape.v_line.x,
                                  s->shape.v_line.y1,
                                  s->shape.v_line.y2,
                                  s->shape.v_line.color);

    case SHAPE_LINE:
        return draw_line(s->shape.line.x1, s->shape.line.y1,
                         s->shape.line.x2, s->shape.line.y2,
                         s->shape.line.color);

    case SHAPE_RECTANGLE:
        return draw_rectangle(s->shape.rectangle.x,
                              s->shape.rectangle.y,
                              s->shape.rectangle.width,
                              s->shape.rectangle.height,
                              s->shape.rectangle.color);

    case SHAPE_FILLED_RECTANGLE:
        return draw_filled_rectangle(s->shape.filled_rectangle.x,
                                     s->shape.filled_rectangle.y,
                                     s->shape.filled_rectangle.width,
                                     s->shape.filled_rectangle.height,
                                     s->shape.filled_rectangle.color);

    case SHAPE_CIRCLE:
        return draw_circle(s->shape.circle.x_center,
                           s->shape.circle.y_center,
                           s->shape.circle.radius,
                           s->shape.circle.color);

    case SHAPE_FILLED_CIRCLE:
        return draw_filled_circle(s->shape.filled_circle.x_center,
                                  s->shape.filled_circle.y_center,
                                  s->shape.filled_circle.radius,
                                  s->shape.filled_circle.color);

    default:
        return -1;
    }
}

int8_t shape_deinit()
{
    draw_pixel = NULL;
    return 0; // Success
}
