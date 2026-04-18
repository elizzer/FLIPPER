#include "text.h"
#include "font_manager.h"

int8_t render_text(draw_pixel_func_t draw_pixel_func, const char *text, uint16_t x, uint16_t y, uint16_t text_color, uint16_t bg_color)
{
    // perform NULL checks for the input parameters
    if (draw_pixel_func == NULL || text == NULL)
    {
        return -1;
    }

    uint8_t char_bitmap[FONT_HEIGHT][FONT_BYTES_PER_ROW];
    uint16_t char_width = FONT_WIDTH;
    uint16_t char_height = FONT_HEIGHT;

    // start a loop to render each character of the text string
    for (size_t i = 0; i < strlen(text); i++)
    {
        if (get_font_bitmap(text[i], char_bitmap) == 0)
        {
            for (uint8_t bitMap_row = 0; bitMap_row < char_height; bitMap_row++)
            {
                for (uint8_t bitMap_col = 0; bitMap_col < char_width; bitMap_col++)
                {
                    if (char_bitmap[bitMap_row][bitMap_col / 8] & (1 << (7 - (bitMap_col % 8))))
                    {
                        // Set pixel to text color
                        draw_pixel_func(x + bitMap_col, y + bitMap_row, text_color);
                    }
                    else
                    {
                        // Set pixel to background color
                        draw_pixel_func(x + bitMap_col, y + bitMap_row, bg_color);
                    }
                }
            }
            x += char_width; // Move to the next character position
        }
    }
    // for each character, get the bitmap from the font manager
    // then loop through the bitmap and call the draw_pixel_func for each pixel in the bitmap, passing the appropriate color (text_color or bg_color) based on whether the pixel is set or not in the bitmap
    return 0;
}