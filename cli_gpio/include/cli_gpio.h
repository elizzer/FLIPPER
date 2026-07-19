#ifndef CLI_GPIO_H
#define CLI_GPIO_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cmd_parser.h"
#include "cli_app.h"




//define a struct to have a configs needed to operate a single gpio

typedef struct
{
    uint8_t pin;        // GPIO pin number
    uint8_t dir;       // GPIO mode (input, output, etc.) 0=>output 1=>input
    uint8_t pull;       // Pull-up/pull-down configuration 0=> no_pull 1=>pull_up 2=>pull_down
    uint8_t level;      // Output level (0 or 1)
} gpioconfig_t;

typedef gpioconfig_t* gpioHandle_t;
//define gpio specific commands function pairs

int8_t cli_gpio_register();

int8_t cli_gpio_init(gpioHandle_t *handle);
int8_t cli_gpio_deinit(gpioHandle_t  handle);
void cli_gpio_help(void);
int8_t cli_gpio_cmd_dispatch(gpioHandle_t  handle,const char *cmd);


int8_t cli_gpio_set_pin(gpioHandle_t  handle, char * );
int8_t cli_gpio_get_config(gpioHandle_t  handle);
int8_t cli_gpio_set_dir(gpioHandle_t  handle, char *args);
int8_t cli_gpio_set_pull(gpioHandle_t  handle, char *args);


#endif // CLI_GPIO_H