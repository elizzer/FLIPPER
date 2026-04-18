#include "display_manager.h"
#include <stdio.h>



int8_t display_init(displayConfig_t *config, displayHandle_t *handle)
{

    if (config == NULL || handle == NULL)
    {
        return -1;
    }

    if (config->write_buffer_func == NULL)
    {
        printf("Error: write_buffer_func is NULL in display config\n");
        return -1;
    }

    if (config->display_device_handle == NULL)
    {
        printf("Error: display_device_handle is NULL in display config\n");
        return -1;
    }

    // Assign the disp buffer as per config
    printf("Initializing display with width: %d, height: %d\n", config->width, config->height);
    config->frame_buffer = (uint16_t *)malloc(config->width * config->height * sizeof(uint16_t));
    if (config->frame_buffer == NULL)
    {
        return -1;
    }
    printf("Display buffer allocated at address: %p\n", (void *)config->frame_buffer);

   

    // copy the config to the handle, the memory for the handle is allocated in the caller function
    memcpy(handle, config, sizeof(displayConfig_t));
    printf("Display handle initialized with config\n");

    return 0;
}


int8_t display_deinit(displayHandle_t *handle)
{
    // Free the frame buffer memory
    if (handle == NULL)
    {
        return -1;
    }
    displayConfig_t *disp_config = (displayConfig_t *)(handle);

    if (disp_config->frame_buffer != NULL)
    {
        free(disp_config->frame_buffer);
        disp_config->frame_buffer = NULL;
        return 0;
    }
    else
    {
        return -1;
    }
}

int8_t display_writeBuffer(displayHandle_t *handle) // writes the frame buffer to the display, this is a blocking call and should be called after all the drawing operations are done on the frame buffer
{
    if (handle == NULL)
    {
        return -1;
    }

    // now send the complete frame buffer to the display using the function pointer in the config
    // later we can optimize this by only sending the updated regions of the frame buffer to the display instead of the complete buffer
    displayConfig_t *disp_config = (displayConfig_t *)(handle);
    if (disp_config->write_buffer_func == NULL)
    {
        printf("Error: write_buffer_func is NULL in display config\n");
        return -1;
    }
    if (disp_config->display_device_handle == NULL)
    {
        printf("Error: display_device_handle is NULL in display config\n");
        return -1;
    }
    // printf("Writing buffer to display using function pointer\n");
 
    
    int8_t result = disp_config->write_buffer_func(disp_config->frame_buffer , 0, 0, disp_config->width, disp_config->height, disp_config->display_device_handle);
    if (result != 0)    {
        printf("Error: write_buffer_func failed to write buffer to display\n");
        return -1;
    }

    return 0;
}

int8_t display_clearBuffer(displayHandle_t *handle)
{
    if (handle == NULL)
    {
        return -1;
    }

    displayConfig_t *disp_config = (displayConfig_t *)(handle);

    memset(disp_config->frame_buffer, 0x00, (disp_config->width) * (disp_config->height) * sizeof(uint16_t));
    return 0;
}

int8_t display_drawPixel(void *handle, uint16_t x, uint16_t y, uint16_t color)
{
    if (handle == NULL)
    {
        return -1;
    }
    displayConfig_t *disp_config = (displayConfig_t *)(handle);

    // check for NULL frame buffer
    if (disp_config->frame_buffer == NULL)
    {
        return -1;
    }

    if (x >= disp_config->width || y >= disp_config->height)
    {
        return -1;
    }

    // Set the pixel color in the frame buffer
    disp_config->frame_buffer[y * disp_config->width + x] = SWAP_16BIT_BYTE_ORDER(color);
 
    return 0;
}

int8_t display_getBufferPtr(displayHandle_t *handle, uint16_t **buffer)
{
    if (handle == NULL || buffer == NULL)
    {
        return -1;
    }

    displayConfig_t *disp_config = (displayConfig_t *)(handle);

    *buffer = disp_config->frame_buffer;
    printf("Frame buffer pointer retrieved: %p\n", disp_config->frame_buffer);
    printf("Display buffer pointer retrieved: %p\n", (void *)(*buffer));
    return 0;
}