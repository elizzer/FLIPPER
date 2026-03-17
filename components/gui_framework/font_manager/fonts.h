#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>
#include <string.h>

#define FONT_WIDTH  12
#define FONT_HEIGHT 12
#define FONT_BYTES_PER_ROW 5

extern const uint8_t font_bitmaps[95][FONT_HEIGHT][FONT_BYTES_PER_ROW];
extern const int16_t font_ascii_lookup[256];


#endif