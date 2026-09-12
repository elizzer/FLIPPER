#include <stdio.h>
#define LOG_TAG "app"
#include "cli_log.h"
#include "cli_app.h"
#include "cli_gpio.h"
#include "cli_pwm.h"
#include "cli_i2c.h"


// global variable to store current mode

interface_instance_t interfaces[10]; // global array to hold interface handles

void app_init(void)
{
    cli_gpio_register();
    cli_pwm_register();
    cli_i2c_register();
    return;
}

void cmd_help(void * handle,char *args)
{

    if (strlen(args) != 0)
    {
        InterfaceRegistryEntry_t *inf = interface_registry_lookup_name(args);
        if (inf != NULL)
        {
            inf->help_handler();
        }
    }
    else
    {
        printf("\r\n--- Karuvi X CLI Commands ---\r\n");
        printf("  hello <name>                 - Print a greeting\r\n");
        printf("  print_banner                 - Show the startup banner\r\n");
        printf("  time                         - Show uptime since boot\r\n");
        printf("  sysinfo                      - Show chip and heap information\r\n");
        printf("  temp                         - Show chip temperature\r\n");
        printf("  panic                        - Break me on purpose and trigger fault handling\r\n");
        printf("  all                          - Show uptime and system info\r\n");
        printf("  create <interface> <name>    - Create a named interface instance\r\n");
        printf("                                  e.g. create gpio io1\r\n");
        printf("  use <name> <cmd> [args]      - Run a command on a created interface\r\n");
        printf("                                  e.g. use io1 set_pin IO_5\r\n");
        printf("  help                         - Show this help message\r\n");
        printf("  help <interface>             - Show help message of the interface\r\n");
        printf("  reboot                       - Restart the device\r\n");
        printf("-----------------------------\r\n");
    }
}

static int8_t get_interface_handle(char *name)
{
    for (int i = 0; i < 10; i++)
    {
        if (interfaces[i].in_use && strcmp(interfaces[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1; // no handle found for this mode
}

static int8_t get_free_handle_index()
{
    for (int i = 0; i < 10; i++)
    {
        if (interfaces[i].in_use == 0)
        {
            return i;
        }
    }
    return -1; // no handle found for this mode
}

// create <interface> <name>
// call the init function for the specified interface and store the handle in a global variable for that mode

void cmd_create(void * handle,char *args)
{
    char interface[32];
    char name[32];
    cmd_parse(args, interface, 32, name, 32);
    InterfaceId_e inf;
    InterfaceRegistryEntry_t *entry = interface_registry_lookup_name(interface);
    if (entry == NULL)
    {
        LOG_ERR("interface '%s' not found in registry", interface);
        return;
    }

    // if name is empty, must not allow
    if (strlen(name) == 0)
    {
        LOG_ERR("Name must be atleast 1 char");
        return;
    }

    // check for name duplication
    int8_t retVal = get_interface_handle(name);
    if (retVal != -1)
    {
        LOG_ERR("interface name '%s' already exists", name);
        return;
    }

    int8_t new_inf_idx = get_free_handle_index();
    if (new_inf_idx == -1)
    {
        LOG_ERR("no free handle slots available");
        return;
    }

    entry->init(&interfaces[new_inf_idx].inf_handle);
    if (interfaces[new_inf_idx].inf_handle == NULL)
    {
        LOG_ERR("failed to create interface '%s', returned NULL handle", interface);
        return;
    }

    interfaces[new_inf_idx].in_use = 1;
    interfaces[new_inf_idx].inf = entry->id;
    strncpy(interfaces[new_inf_idx].name, name, 32);
    LOG_INFO("interface '%s' created with name '%s'", interface, name);
}

void cmd_use(void * handle,char *args)
{
    // the args have the name, find the handle, call the interface_cmd_dispatch function for the current mode with the handle and the rest of the args
    char name[32];
    char cmdArgs[128];
    cmd_parse(args, name, 32, cmdArgs, 128);
    int8_t indx = get_interface_handle(name);
    if (indx == -1)
    {
        LOG_ERR("no interface found with name '%s'", name);
        return;
    }

    InterfaceRegistryEntry_t *inf_entry = interface_registry_lookup_id(interfaces[indx].inf);

    if (inf_entry == NULL)
    {
        LOG_ERR("interface '%s' not found in registry", name);
        return;
    }

    inf_entry->cmd_handler(interfaces[indx].inf_handle, cmdArgs);
}