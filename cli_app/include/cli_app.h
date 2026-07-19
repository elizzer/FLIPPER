#ifndef CLI_APP_H
#define CLI_APP_H

#include <stdio.h>
#include <string.h>
#include "cmd_parser.h"
#include "interface_registry.h"

// struct to hold name agains handle
typedef struct
{
    char name[32];
    void * inf_handle;
    int8_t in_use;
    InterfaceId_e inf;
} interface_instance_t;

void cmd_help(char *args);
void cmd_create(char *args);
void cmd_use(char *args);

//init function to register all commands and set default mode
void app_init(void);

#endif /* CLI_APP_H */

