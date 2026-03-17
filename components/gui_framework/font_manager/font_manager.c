
#include "font_manager.h"

int8_t get_font_bitmap(char c, uint8_t *char_bitMap){
    uint8_t char_index = font_ascii_lookup[(uint8_t)c];
    if(char_index == -1) {
        // Return an empty bitmap for unsupported characters
        memset(char_bitMap, 0, FONT_HEIGHT * FONT_BYTES_PER_ROW);
        return -1;
    }
    memcpy(char_bitMap, font_bitmaps[char_index], FONT_HEIGHT * FONT_BYTES_PER_ROW);
    return 0;
}