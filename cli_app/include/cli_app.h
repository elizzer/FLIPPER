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

extern cmdEntry_t g_cmd_table[];

void main_cmd_dispatch(const char * cmd);

void cmd_help(void *,char *args);
void cmd_create(void *,char *args);
void cmd_use(void *,char *args);
void cmd_print_banner(void *handle, char *args);
void cmd_time(void *handle, char *args);
void cmd_sysinfo(void *handle, char *args);
void cmd_panic(void *handle, char *args);
void cmd_panic(void *handle, char *args);
void cmd_all(void *handle, char *args);
void cmd_reboot(void *handle, char *args);
void cmd_console_clear(void *handle, char *args);

//init function to register all commands and set default mode
void app_init(void);

#endif /* CLI_APP_H */

