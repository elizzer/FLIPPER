
#include "wd_graph.h"

void wd_graph_draw(const GraphConfig_t* config, GraphPoint_t* data_buffer, size_t data_buffer_size){
    //draw the background reactbangle
    Shape_t bg_rect = MAKE_FILLED_RECTANGLE(config->x, config->y, config->width, config->height, config->background_color);
    draw_shape(&bg_rect);
    //draw the axis
    Shape_t x_axis = MAKE_HORIZONTAL_LINE(config->x, config->x + config->width, config->y + config->height, config->line_color);
    Shape_t y_axis = MAKE_VERTICAL_LINE(config->x, config->y, config->y + config->height, config->line_color);
    draw_shape(&x_axis);
    draw_shape(&y_axis);
    //plot the points
    for(size_t i = 0; i < data_buffer_size; i++){
        int plot_x = config->x + (int)(data_buffer[i].x * config->width);
        int plot_y = config->y + config->height - (int)(data_buffer[i].y * config->height);
        config->draw_function(plot_x, plot_y, config->line_color);
    }
}