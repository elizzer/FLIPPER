#include <stdio.h>
#include "cli_pwm.h"
#include "interface_registry.h"
#include "kx_pwm_hal.h"
#define LOG_TAG "CLI_PWM"
#include "cli_log.h"
#include "cli_app.h"

typedef int8_t (*pwm_cmd_func_t)(cliPWMHandle_t, char *);

// define to check not null handle
#define CHECK_HANDLE_NULL(handle)      \
    do                                 \
    {                                  \
        if (handle == NULL)            \
        {                              \
            LOG_ERR("Handle is NULL"); \
            return -1;                 \
        }                              \
    } while (0)

cmdEntry_t pwm_cmds[] = {
    {"set_pin", cli_pwm_set_pin, "set_pin <IO_x>                        : Set active PWM pin (e.g. set_pin IO_5)"},
    {"set_freq_hz", cli_pwm_set_freq_hz, "set_freq_hz <freq_hz>                 : Set PWM frequency in Hz"},
    {"set_freq_Mhz", cli_pwm_set_freq_Mhz, "set_freq_Mhz <freq_Mhz>               : Set PWM frequency in MHz"},
    {"get_freq_hz", cli_pwm_get_freq, "get_freq_hz                           : Print current PWM frequency in Hz"},
    {"set_duty_cycle", cli_pwm_set_duty_cycle, "set_duty_cycle <0-100>                : Set PWM duty cycle in percent"},
    {"get_duty_cycle", cli_pwm_get_duty_cycle, "get_duty_cycle                        : Print current PWM duty cycle in percent"},
    {"start", cli_pwm_start, "start                                  : Start PWM output on active pin"},
    {"stop", cli_pwm_stop, "stop                                   : Stop PWM output on active pin"},
    {"", NULL, ""} // sentinel
};

int8_t cli_pwm_register()
{
    LOG_INFO("Registring PWM interface for CLI");
    InterfaceRegistryEntry_t entry;
    entry.id = INTERFACE_PWM;
    strcpy(entry.name, "pwm");
    entry.init = cli_pwm_init;
    entry.de_init = cli_pwm_deinit;
    entry.cmd_handler = cli_pwm_cmd_dispatch;
    entry.help_handler = cli_pwm_help; // can be implemented later
    int8_t reg_sts = interface_registry_register(&entry);

    if (KX_HAL_OK == reg_sts)
    {
        LOG_INFO("CLI PWM registration success");
        return KX_HAL_OK;
    }
    else
    {
        LOG_ERR("CLI PWM registration fail");
        return KX_HAL_ERR_FAIL;
    }
}

int8_t cli_pwm_init(void **handle)
{
    Kx_ErrorCode init_sts = KxPWM_Init();
    if (KX_HAL_OK == init_sts)
    {
        LOG_INFO("CLI PWM initialization success");
        // create a handle and return it
        cliPWMHandle_t pwm = (cliPWMHandle_t)malloc(sizeof(cliPWMConfig_t));
        memset(pwm, 0, sizeof(cliPWMConfig_t));
        pwm->IO_pin = -1; // assuming -1 is an invalid pin
        if (pwm == NULL)
        {
            LOG_ERR("Failed to allocate memory for PWM handle");
            return KX_HAL_ERR_FAIL;
        }
        *handle = (void *)pwm;
        return KX_HAL_OK;
    }
    else
    {
        LOG_ERR("CLI PWM initialization fail");
        return KX_HAL_ERR_FAIL;
    }
}

int8_t cli_pwm_deinit(void *handle)
{
    Kx_ErrorCode deinit_sts = KxPWM_DeInit();
    if (KX_HAL_OK == deinit_sts)
    {
        LOG_INFO("CLI PWM de-initialization success");
        return KX_HAL_OK;
    }
    else
    {
        LOG_ERR("CLI PWM de-initialization fail");
        return KX_HAL_ERR_FAIL;
    }
}

void cli_pwm_help()
{
    printf("\n\r--- PWM Interface Commands ---\n\r");
    for (int i = 0; pwm_cmds[i].func != NULL; i++)
    {
        printf("  %s\n\r", pwm_cmds[i].help_str);
    }
    printf("--------------------------------\n\r");
}

int8_t cli_pwm_cmd_dispatch(void *handle, const char *cmd)
{
    if (handle == NULL)
    {
        return -1;
    }
    char key[32];
    char args[128];
    cmd_parse(cmd, key, 32, args, 128);
    int8_t cb_idx = cmd_tbl_search(key, pwm_cmds, sizeof(pwm_cmds) / sizeof(cmdEntry_t));
    if (cb_idx == -1)
    {
        LOG_ERR("\n\r Unknown PWM command \"%s\" not found", key);
        return -1;
    }
    ((pwm_cmd_func_t)pwm_cmds[cb_idx].func)(handle, args);
    return 0;
}

int8_t cli_pwm_set_pin(void *handle, char *args)
{
    CHECK_HANDLE_NULL(handle);
    // Implementation for setting PWM pin

    cliPWMHandle_t pwm_handle = (cliPWMHandle_t)handle;

    Kx_ErrorCode retVal = KxPWM_SetPin((Kx_IO)atoi(args));
    if (retVal == KX_HAL_ERR_BUSY)
    {
        LOG_ERR("Pin is already configured or invalid");
        return KX_HAL_ERR_FAIL;
    }
    else if (retVal != KX_HAL_OK)
    {
        LOG_ERR("Unable to set the PWM pin");
        return KX_HAL_ERR_FAIL;
    }

    pwm_handle->IO_pin = (Kx_IO)atoi(args);

    return KX_HAL_OK;
}

int8_t cli_pwm_set_freq_hz(void *handle, char *args)
{
    CHECK_HANDLE_NULL(handle);
    cliPWMHandle_t pwm_handle = (cliPWMHandle_t)handle;

    // Implementation for setting PWM frequency
    Kx_ErrorCode retVal = KxPWM_SetFrequency(pwm_handle->IO_pin, (uint32_t)atoi(args));
    if (retVal != KX_HAL_OK)
    {
        LOG_ERR("Unable to set the PWM frequency");
        return KX_HAL_ERR_FAIL;
    }

    return KX_HAL_OK;
}

int8_t cli_pwm_set_freq_Mhz(void *handle, char *args)
{
    CHECK_HANDLE_NULL(handle);
    cliPWMHandle_t pwm_handle = (cliPWMHandle_t)handle;

    // Implementation for setting PWM frequency
    Kx_ErrorCode retVal = KxPWM_SetFrequency(pwm_handle->IO_pin, (uint32_t)(atof(args) * 1000000));
    if (retVal != KX_HAL_OK)
    {
        LOG_ERR("Unable to set the PWM frequency");
        return KX_HAL_ERR_FAIL;
    }

    return KX_HAL_OK;
}

int8_t cli_pwm_get_freq(void *handle, char *args)
{
    return -1; // Not implemented yet
}

int8_t cli_pwm_set_duty_cycle(void *handle, char *args)
{
    CHECK_HANDLE_NULL(handle);
    cliPWMHandle_t pwm_handle = (cliPWMHandle_t)handle;

    // Implementation for setting PWM duty cycle
    Kx_ErrorCode retVal = KxPWM_SetDutyCycle(pwm_handle->IO_pin, (uint8_t)atoi(args));
    if (retVal != KX_HAL_OK)
    {
        LOG_ERR("Unable to set the PWM duty cycle");
        return KX_HAL_ERR_FAIL;
    }

    return KX_HAL_OK;
}

int8_t cli_pwm_get_duty_cycle(void *handle, char *args)
{
    return -1; // Not implemented yet
}

int8_t cli_pwm_start(void *handle, char *args)
{
    CHECK_HANDLE_NULL(handle);
    cliPWMHandle_t pwm_handle = (cliPWMHandle_t)handle;

    // Implementation for starting PWM
    Kx_ErrorCode retVal = KxPWM_Start(pwm_handle->IO_pin);
    if (retVal != KX_HAL_OK)
    {
        LOG_ERR("Unable to start the PWM");
        return KX_HAL_ERR_FAIL;
    }
    return 0;
}

int8_t cli_pwm_stop(void *handle, char *args)
{
    CHECK_HANDLE_NULL(handle);
    cliPWMHandle_t pwm_handle = (cliPWMHandle_t)handle;

    // Implementation for stopping PWM
    Kx_ErrorCode retVal = KxPWM_Stop(pwm_handle->IO_pin);
    if (retVal != KX_HAL_OK)
    {
        LOG_ERR("Unable to stop the PWM");
        return KX_HAL_ERR_FAIL;
    }
    return KX_HAL_OK;
}

int8_t cli_pwm_reset(void *handle, char *args)
{
    CHECK_HANDLE_NULL(handle);
    cliPWMHandle_t pwm_handle = (cliPWMHandle_t)handle;

    // Implementation for resetting PWM
    return 0;
}
