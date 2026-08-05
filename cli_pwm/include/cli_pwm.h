#ifndef CLI_PWM_H
#define CLI_PWM_H

#include <stdint.h>
#include "kx_hal_types.h"
typedef struct 
{
    Kx_IO IO_pin;
    uint32_t freq_hz;
    uint8_t duty_cycle;

}cliPWMConfig_t;

typedef cliPWMConfig_t* cliPWMHandle_t;

int8_t cli_pwm_register();

int8_t cli_pwm_init(cliPWMHandle_t* handle);
int8_t cli_pwm_deinit(cliPWMHandle_t handle);
void cli_pwm_help();
int8_t cli_pwm_cmd_dispatch(cliPWMHandle_t handle, const char * cmd);

int8_t cli_pwm_set_pin(cliPWMHandle_t,char *args);
int8_t cli_pwm_set_freq(cliPWMHandle_t,char *args);
int8_t cli_pwm_get_freq(cliPWMHandle_t,char *args);
int8_t cli_pwm_set_duty_cycle(cliPWMHandle_t,char *args);
int8_t cli_pwm_get_duty_cycle(cliPWMHandle_t,char *args);
int8_t cli_pwm_start(cliPWMHandle_t,char *args);
int8_t cli_pwm_stop(cliPWMHandle_t,char *args);
int8_t cli_pwm_reset(cliPWMHandle_t,char *args);



#endif