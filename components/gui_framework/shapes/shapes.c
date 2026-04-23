#include "shapes.h"
#include <stddef.h>
# include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

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
    for (int x = x1; x <= x2; x++)
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
    for (int y = y1; y <= y2; y++)
    {
        draw_pixel(x, y, color);
    }
    return 0;
}
int8_t draw_line (int x1, int y1, int x2, int y2, uint16_t color) {
	int8_t result = validate_draw_pixel ();
	if (result != 0) {
		printf ("Error : draw_pixel function pointer is not set \n");
		return -1; //Failure
	}
	int dx =  abs(x2-x1), sx = x1<x2 ? 1 : -1;
   int dy = -abs(y2-y1), sy = y1<y2 ? 1 : -1; 
   int err = dx+dy, e2; /* error value e_xy */

	if (dy == 0 && dx == 0) { //Single point
		draw_pixel (x1, y1, color);
		return 0; //Success
	}
	if (dy == 0) { //horizontal line
		draw_horizontal_line (x1, x2, y1, color);
		return 0; //Success
	}
	if (dx == 0) { //vertical line
		draw_vertical_line (x1, y1, y2, color);
		return 0; //Success
	}
	
	//Bresenham's line drawing algorithm

   for(;;){  /* loop */
      draw_pixel(x1, y1, color);
      e2 = 2*err;
      if (x1 == x2 && y1 == y2) break;
      if (e2 >= dy) { err += dy; x1 += sx; } /* e_xy+e_x > 0 */
      if (e2 <= dx) { err += dx; y1 += sy; } /* e_xy+e_y < 0 */
   }
    //printf ("Drawing Bresenham's : x: %d y:%d", x, y);

	return 0; //success
}

int8_t draw_rectangle(int x, int y, int width, int height, uint16_t color)
{
		int8_t result = validate_draw_pixel();
    if (result != 0)
    {
       	printf("Error: draw_pixel function pointer is not set\n");
       	return -1; // Error: draw_pixel function pointer is not set
    }

    draw_horizontal_line(x, x + width - 1, y, color);
    draw_horizontal_line(x, x + width - 1, y + height - 1, color);
    draw_vertical_line(x, y, y + height - 1, color);
    draw_vertical_line(x + width - 1, y, y + height - 1, color);
    return 0;
}

int8_t draw_filled_rectangle(int x, int y, int width, int height, uint16_t color){
	int8_t result = validate_draw_pixel();
    if (result != 0)
    {
       	printf("Error: draw_pixel function pointer is not set\n");
       	return -1; // Error: draw_pixel function pointer is not set
    }
	if (width == 0 && height == 0) { //Point
		draw_pixel (x, y, color);
		return 0;
	}
	if (width == 0) { //Vertical line
		draw_vertical_line (x, y, y + height, color);
		return 0;
	}
	if (height == 0) { //Horizontal line
		draw_horizontal_line (x, x + width, y, color);
		return 0;
	}

	//filling rectangle
	for (int i = 0; i < height; i++, y++) {
		draw_horizontal_line (x, x + width, y, color);
	}
	return 0;
}

int8_t draw_circle(int x_center, int y_center, int radius, uint16_t color) {
  int8_t result = validate_draw_pixel();
  if (result != 0)
    {
       	printf("Error: draw_pixel function pointer is not set\n");
       	return -1; // Error: draw_pixel function pointer is not set
    }
	if (radius <= 0) { //Point
		draw_pixel (x_center, y_center, color);
		return 0;
	}

	int x = -radius, y = 0, err = 2-2*radius ; /* II. Quadrant */ 
   do {
      draw_pixel(x_center - x, y_center + y, color); /*   I. Quadrant */
      draw_pixel(x_center - y, y_center - x, color); /*  II. Quadrant */
      draw_pixel(x_center + x, y_center - y, color); /* III. Quadrant */
      draw_pixel(x_center + y, y_center + x, color); /*  IV. Quadrant */
      radius = err;
      if (radius <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
      if (radius > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
   } while (x < 0);
	return 0;
}

int8_t draw_filled_circle(int x_center, int y_center, int radius, uint16_t color){
	int8_t result = validate_draw_pixel ();

	if (result != 0){
		printf ("Error : draw_pixel function pointer is not set\n");
		return -1;
	}

	if (radius <= 0) {//Point
		draw_pixel (x_center, y_center, color);
		return 0;
	}

		int x = radius;
    int y = 0;
    int xChange = 1 - (radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        for (int i = x_center - x; i <= x_center + x; i++)
        {
            draw_pixel(i, y_center + y, color);
            draw_pixel(i, y_center - y, color);
        }
        for (int i = x_center - y; i <= x_center + y; i++)
        {
            draw_pixel(i, y_center + x, color);
            draw_pixel(i, y_center - x, color);
        }

        y++;
				radiusError += 2 * y + 1;
				if (2 * (radiusError - x) + 1 > 0)
				{
    			x--;
    			radiusError += -2 * x + 1;
				}
    }
	return 0;
}

int8_t draw_polygon (uint16_t color, int count, ...) {
	int result = validate_draw_pixel();
	if (result != 0){
		printf ("Error : draw_pixel function pointer not set\n");
		return -1;
	}

	va_list coordinate;
	va_start (coordinate, count);

	printf ("\nDrawing the polygon\n");
	int points_array [count*2];
	for (int i = 0; i < count*2; i++){
		points_array [i] = va_arg (coordinate, int);
		if (i%2 == 1 && i > 1){
			//draw_pixel(points_array[i-1], points_array[i], color);
			draw_line(
									points_array[i-3],
									points_array[i-2],
									points_array[i-1],
									points_array[i],
									color);
			/*printf ("\nDrawing line %d with points :\n x1: %d y1: %d\n x2: %d y2: %d",
							i, points_array[i-3], points_array[i-2], points_array[i-1],
							points_array[i]);*/
		}
		//printf ("points array %d: %d\n", i, points_array[i]);
	}
	va_end(coordinate);

		/*draw_pixel (x1, y1, 0xFFFF);
		draw_line (x1, y1, x2, y2, 0xF800);
		draw_pixel (x2, y2, 0xFFF0);
		draw_line (x2, y2, x3, y3, 0xF800);
		draw_pixel (x3, y3, 0x0F10);
		draw_line (x1, y1, x3, y3, 0xF800);*/

	return 0;
}

int8_t draw_filled_polygon (uint16_t color, int count, ...) {
	int result = validate_draw_pixel();
	if (result != 0){
		printf ("Error : draw_pixel function pointer not set\n");
		return -1;
	}

	va_list coordinate;
	va_start (coordinate, count);

	printf ("\nDrawing the filled polygon\n");
	int points_array [count*2], dx[count], dy[count], k = 0;
	for (int i = 0; i < count*2; i++){
		points_array [i] = va_arg (coordinate, int);
		if (i%2 == 0) {
			dx[k] = points_array[i] - points_array[i-2];
			k++;
		}
		else {
			dy[k] = points_array[i] - points_array[i-2];
		}
	}
		//printf ("points array %d: %d\n", i, points_array[i]);
	va_end(coordinate);
	printf ("\ndx1: %d dx2: %d dx3: %d dy1: %d dy2: %d dy3: %d\n", dx[1], dx[2],
					dx[3], dy[1], dy[2], dy[3]);
	switch (count) {

		case 3:
			//for (int i = points_array[]; i < ) {

			//}
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		default :
			printf ("\n TODO cases\n");
	}
		/*draw_pixel (x1, y1, 0xFFFF);
		draw_line (x1, y1, x2, y2, 0xF800);
		draw_pixel (x2, y2, 0xFFF0);
		draw_line (x2, y2, x3, y3, 0xF800);
		draw_pixel (x3, y3, 0x0F10);
		draw_line (x1, y1, x3, y3, 0xF800);*/

	return 0;
}

int8_t draw_arc(int quad, int x_center, int y_center, int radius, uint16_t color) {
  int8_t result = validate_draw_pixel();
  if (result != 0)
    {
       	printf("Error: draw_pixel function pointer is not set\n");
       	return -1; // Error: draw_pixel function pointer is not set
    }
	if (radius <= 0) { //Point
		draw_pixel (x_center, y_center, color);
		return 0;
	}


	int x = -radius, y = 0, err = 2-2*radius ; /* II. Quadrant */ 
   do {
			switch (quad) {
				case 1:
      		draw_pixel(x_center - x, y_center + y, color); /*   I. Quadrant */
					break;
				case 2:
      		draw_pixel(x_center - y, y_center - x, color); /*  II. Quadrant */
					break;
				case 3:
      		draw_pixel(x_center + x, y_center - y, color); /* III. Quadrant */
					break;
				case 4:
      		draw_pixel(x_center + y, y_center + x, color); /*  IV. Quadrant */
					break;
				default:
					printf ("invalid Quardrant number : %d", quad);
					return -1;
			}
      radius = err;
      if (radius <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
      if (radius > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
  } while (x < 0);
	return 0;
}

/*int8_t draw_alphabet_seven_seg (int number, uint16_t color) {
	if (number > 26 || number < 1){
		printf ("Error : Invalid alphabet number %d\nTry a valid alphabet between 1 and 26\n", number);
		return -1;
	}
	else if (number == 10 || number == 13 || number == 14 || number == 17 ||
					 number == 20 || number == 22 || number == 23 ) {
		printf ("\nCan't draw the letter %c, kindly try someother letter\n", number+65);
		return -1;
	}
	switch (number){

		case 1:
			draw_line (80, 200, 85, 200, color);
			draw_line (85, 200, 85, 212, color);
			draw_line (80, 200, 80, 212, color);
			draw_line (80, 207, 85, 207, color);
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 11:
		case 12:
		case 15:
		case 16:
		case 18:
		case 19:
		case 21:
		case 24:
		case 25:
		case 26:

		default :
	}
	return 0;
}*/

int8_t shape_deinit()
{
    draw_pixel = NULL;
    return 0; // Success
}
