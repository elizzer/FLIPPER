#ifndef CLOCK_GEN_HAL_H
#define CLOCK_GEN_HAL_H

#include <stdint.h>


typedef enum
{
    CLK_SRC_GPIO = 0,
    CLK_SRC_LEDC_PWM,
    CLK_SRC_I2S,
} CLOCK_SRC_t;

typedef struct
{
    uint32_t frequency_hz;
    uint8_t output_pin;
} ClockGenHalConfig_t;

typedef struct ClockHalOps_s
{
    int8_t (*clock_config)(void *handle);     // Configure the clock source dependency
    int8_t (*clock_pin_config)(void *handle); // Configure the clock output pin based on the clock source
    int8_t (*start_clk)(void *handle);
    int8_t (*stop_clk)(void *handle);
    int8_t (*deinit_clk)(void *handle);
} ClockHalOps_t;

typedef struct
{
    ClockGenHalConfig_t config;
    CLOCK_SRC_t clk_src;
    struct ClockHalOps_s ops;
    void *clk_src_handle; // Handle for the specific clock source, if needed
} ClockGenHalHandle_t;


int8_t clock_gen_hal_init(const ClockGenHalConfig_t *config, ClockGenHalHandle_t *handle);
int8_t clock_gen_hal_update_frequency(ClockGenHalHandle_t *handle, uint32_t frequency_hz);
int8_t clock_gen_hal_start_clk(ClockGenHalHandle_t *handle);
int8_t clock_gen_hal_stop_clk(ClockGenHalHandle_t *handle);
int8_t clock_gen_hal_deinit(ClockGenHalHandle_t *handle);

#endif /* CLOCK_GEN_HAL_H */