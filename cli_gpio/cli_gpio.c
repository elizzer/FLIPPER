#include <stdio.h>
#define LOG_TAG "gpio"
#include "cli_log.h"
#include "cli_gpio.h"
#include "interface_registry.h"


typedef int8_t (*gpio_cmd_func_t)(gpioHandle_t, char *);

cmdEntry_t gpio_cmds[] = {
    {"set_pin", (generic_fp_t)cli_gpio_set_pin,""},
    {"get_config", (generic_fp_t)cli_gpio_get_config,""},
    {"set_dir", (generic_fp_t)cli_gpio_set_dir,""},
    {"set_pull", (generic_fp_t)cli_gpio_set_pull,""},
    {"set", (generic_fp_t)cli_gpio_set,""},
    {"clear", (generic_fp_t)cli_gpio_clear,""},
    {"", NULL,""} // sentinel
};

int8_t cli_gpio_register()
{
    LOG_INFO("Registring GPIO interface for CLI");
    InterfaceRegistryEntry_t entry;
    entry.id = INTERFACE_GPIO;
    strcpy(entry.name, "gpio");
    entry.init = (InterfaceHandlerInitCB_t)cli_gpio_init;
    entry.de_init = (InterfaceHandlerDeInitCB_t)cli_gpio_deinit;
    entry.cmd_handler = (InterfaceHandlerCmdCB_t)cli_gpio_cmd_dispatch;
    entry.help_handler = (InterfaceHandlerHelpCB_t)cli_gpio_help; // can be implemented later
    int8_t reg_sts = interface_registry_register(&entry);
    if (reg_sts == 0)
    {
        LOG_INFO("CLI GPIO registration success");
        return 0;
    }
    else
    {
        LOG_ERR("CLI GPIO registration fail");
        return 1;
    }
}

int8_t cli_gpio_init(gpioHandle_t *handle)
{
    // For this simple implementation, we just allocate a struct to hold the config
    gpioconfig_t *gpio = (gpioconfig_t *)malloc(sizeof(gpioconfig_t));
    if (!gpio)
        return -1; // allocation failed
    memset(gpio, 0, sizeof(gpioconfig_t));
    // set the pin to ff
    gpio->pin = 0xFF;
    *handle = (void *)gpio;
    return 0;
}

int8_t cli_gpio_deinit(gpioHandle_t handle)
{
    if (handle)
    {
        free(handle);
        return 0;
    }
    return -1; // invalid handle
}

int8_t cli_gpio_cmd_dispatch(gpioHandle_t handle, const char *cmd)
{
    if (handle == NULL)
    {
        return -1;
    }
    char key[32];
    char args[128];
    cmd_parse(cmd, key, 32, args, 128);
    int8_t cb_idx = cmd_tbl_search(key, gpio_cmds, sizeof(gpio_cmds) / sizeof(cmdEntry_t));
    if (cb_idx == -1)
    {
        LOG_ERR("\n\r Unknown GPIO command \"%s\" not found", key);
        return -1;
    }
    ((gpio_cmd_func_t)gpio_cmds[cb_idx].func)(handle, args);
    return 0;
}

void cli_gpio_help(void)
{
    printf("\n\r--- GPIO Interface Commands ---\n\r");
    printf("  set_pin <IO_x>                        : Set active GPIO pin (e.g. set_pin IO_5)\n\r");
    printf("  set_dir <input|output>                : Set pin direction\n\r");
    printf("  set_pull <no_pull|pull_up|pull_down>  : Set pin pull mode\n\r");
    printf("  get_config                            : Print current pin, dir, pull, level\n\r");
    printf("--------------------------------\n\r");
}

static int8_t get_io_num(char *io)
{
    // io string is defined as IO_x
    // can we use sscanf to get the number??
    int num = -1;
    if (io != NULL)
    {

        sscanf(io, "IO_%d", &num);
        return num;
    }
    return -1;
}

int8_t cli_gpio_set_pin(gpioHandle_t handle, char *args)
{
    if (handle == NULL)
    {
        return -1;
    }
    // must validate io number
    gpioconfig_t *gpio = (gpioconfig_t *)handle;
    int8_t retVal = get_io_num(args);
    if (retVal != -1)
    {
        gpio->pin = retVal;
        LOG_INFO("GPIO pin set to %d", gpio->pin);
        return 0;
    }
    else
    {

        gpio->pin = 0xFF;
        LOG_ERR("Invalid IO number");
        return -1;
    }
}

static void print_config(gpioconfig_t *config)
{
    printf("pin=%d  mode=%d  pull=%d  level=%d\n",
           config->pin, config->dir, config->pull, config->level);
}
// this fuction is to get the set values for interface
int8_t cli_gpio_get_config(gpioHandle_t handle)
{
    // handle null check
    if (handle == NULL)
    {
        return -1;
    }
    gpioconfig_t *gpio = (gpioconfig_t *)handle;
    // print all the configs
    print_config(gpio);
    return 0;
}

int8_t cli_gpio_set_dir(gpioHandle_t handle, char *args)
{
    if (handle == NULL)
    {
        return -1;
    }
    gpioconfig_t *gpio = (gpioconfig_t *)handle;

    if (strcmp(args, "output") == 0)
    {
        gpio->dir = 0;
        KxGpio_SetDirection(gpio->pin, KX_GPIO_DIR_OUTPUT);
    }
    else if (strcmp(args, "input") == 0)
    {
        gpio->dir = 1;
        KxGpio_SetDirection(gpio->pin, KX_GPIO_DIR_INPUT);
    }
    else
    {
        LOG_ERR("\n\rInvalid pin direction");
    }
    return 0;
}

int8_t cli_gpio_set_pull(gpioHandle_t handle, char *args)
{
    if (handle == NULL)
    {
        return -1;
    }
    gpioconfig_t *gpio = (gpioconfig_t *)handle;
    if (strcmp(args, "no_pull") == 0)
    {
        gpio->pull = 0;
        KxGpio_SetPull(gpio->pin, KX_GPIO_PULL_NONE);
    }
    else if (strcmp(args, "pull_up") == 0)
    {
        gpio->pull = 1;
        KxGpio_SetPull(gpio->pin, KX_GPIO_PULL_UP);
    }
    else if (strcmp(args, "pull_down") == 0)
    {
        gpio->pull = 2;
        KxGpio_SetPull(gpio->pin, KX_GPIO_PULL_DOWN);
    }
    else
    {
        LOG_ERR("\n\rInvalid PULL mode");
    }
    return 0;
}

int8_t cli_gpio_set(gpioHandle_t handle, char *args)
{
    if (handle == NULL)
    {
        return -1;
    }
    gpioconfig_t *gpio = (gpioconfig_t *)handle;
    if (gpio->pin == 0xFF)
    {
        LOG_WARN("IO is not set for this pin");
        return KX_HAL_ERR_FAIL;
    }

    Kx_ErrorCode retVal;
    retVal = KxGpio_Set(gpio->pin);
    if (retVal != KX_HAL_OK)
    {
        LOG_ERR("Unable to set the pin");
        return KX_HAL_ERR_FAIL;
    }

    return KX_HAL_OK;
}

int8_t cli_gpio_clear(gpioHandle_t handle, char *args)
{
    if (handle == NULL)
    {
        return -1;
    }
    gpioconfig_t *gpio = (gpioconfig_t *)handle;
    if (gpio->pin == 0xFF)
    {
        LOG_WARN("IO is not set for this pin");
        return KX_HAL_ERR_FAIL;
    }

    Kx_ErrorCode retVal;
    retVal = KxGpio_Clear(gpio->pin);
    if (retVal != KX_HAL_OK)
    {
        LOG_ERR("Unable to set the pin");
        return KX_HAL_ERR_FAIL;
    }

    return KX_HAL_OK;
}