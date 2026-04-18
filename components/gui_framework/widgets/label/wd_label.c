#include "wd_label.h"
#include "font_manager.h"
#include "text.h"
#include "shapes.h"

uint8_t char_bitMap[FONT_HEIGHT * FONT_BYTES_PER_ROW];

void draw_label(label_attr_t attr)
{
    int8_t ret;
    Shape_t bg_rect = {
        .type = SHAPE_FILLED_RECTANGLE,
        .shape.filled_rectangle = {
            .x = attr.x,
            .y = attr.y,
            .width = attr.width,
            .height = attr.height,
            .color = attr.bg_color}};
    // draw background rectangle
    draw_shape(&bg_rect);
    // render text
    // need to add calculation to put the text in the center of the label
    uint16_t text_width = FONT_WIDTH * strlen(attr.text);
    uint16_t text_height = FONT_HEIGHT;
    uint16_t text_x = attr.x + (attr.width - text_width) / 2;
    uint16_t text_y = attr.y + (attr.height - text_height) / 2;
    switch (attr.align)
    {
        case LABEL_ALIGN_LEFT:
            text_x = attr.x + attr.padding;
            break;
        case LABEL_ALIGN_CENTER:
            text_x = attr.x + (attr.width - text_width) / 2;
            break;
        case LABEL_ALIGN_RIGHT:
            text_x = attr.x + attr.width - text_width - attr.padding;
            break;
    }
    render_text(attr.draw_pixel, attr.text, text_x, text_y, attr.color, attr.bg_color);
}
