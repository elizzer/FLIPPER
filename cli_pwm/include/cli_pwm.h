#ifndef CLI_PWM_H
#define CLI_PWM_H

#include <stdint.h>
#include "kx_hal_types.h"
typedef struct
{
    Kx_IO IO_pin;
    uint32_t freq_hz;
    uint8_t duty_cycle;

} cliPWMConfig_t;

typedef cliPWMConfig_t *cliPWMHandle_t;

int8_t cli_pwm_register();

int8_t cli_pwm_init(void **handle);
int8_t cli_pwm_deinit(void *handle);
void cli_pwm_help();
int8_t cli_pwm_cmd_dispatch(void *handle, const char *cmd);

int8_t cli_pwm_set_pin(void *handle, char *args);
int8_t cli_pwm_set_freq_hz(void *handle, char *args);
int8_t cli_pwm_set_freq_Mhz(void *handle, char *args);
int8_t cli_pwm_get_freq(void *handle, char *args);
int8_t cli_pwm_set_duty_cycle(void *handle, char *args);
int8_t cli_pwm_get_duty_cycle(void *, char *args);
int8_t cli_pwm_start(void *handle, char *args);
int8_t cli_pwm_stop(void *handle, char *args);
int8_t cli_pwm_reset(void *handle, char *args);

#endif