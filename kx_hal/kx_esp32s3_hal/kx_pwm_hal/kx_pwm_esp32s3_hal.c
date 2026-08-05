#include "kx_pwm_hal.h"
#include "kx_hal.h"
#define LOG_TAG "KX_HAL_PWM"
#include "cli_log.h"
#include "soc/clk_tree_defs.h"
#include "driver/ledc.h"
#include "hal/ledc_types.h"
#include "esp_clk_tree.h"

#define KX_PWM_TIMER_COUNT 4
#define KX_PWM_CHANNEL_COUNT 4

typedef struct channelConfiguration
{
    uint8_t duty_resolution; // 1-16 bits
    uint8_t timer_idx;       // 0-3
    uint32_t freq_hz;        // frequency in Hz
    uint8_t duty_cycle;      // duty cycle in percentage (0-100)
    Kx_IO pin;               // associated pin for this channel
    bool is_chInit;          // indicates if the channel is currently active
} channelConfiguration_t;

// global array to hold the channel configurations (8 channels on ESP32-S3)
static channelConfiguration_t g_channel_config[KX_PWM_CHANNEL_COUNT];

static void reset_channel_config(channelConfiguration_t *cfg)
{
    cfg->pin = -1; // assuming -1 is an invalid pin
    cfg->duty_resolution = 8;
    cfg->timer_idx = 0; // real timer assignment happens on KxPWM_SetPin()
    cfg->freq_hz = 1000;
    cfg->duty_cycle = 50;
    cfg->is_chInit = false;
}

Kx_ErrorCode KxPWM_Init()
{
    for (int i = 0; i < KX_PWM_CHANNEL_COUNT; i++)
    {
        reset_channel_config(&g_channel_config[i]);
        g_channel_config[i].timer_idx = i; // distribute channels across timers
    }
    return KX_HAL_OK;
}

Kx_ErrorCode KxPWM_DeInit()
{
    for (int i = 0; i < KX_PWM_CHANNEL_COUNT; i++)
    {
        reset_channel_config(&g_channel_config[i]);
        g_channel_config[i].timer_idx = i;
    }
    return KX_HAL_OK;
}

Kx_ErrorCode KxPWM_SetPin(Kx_IO pin)
{
    // check if the pin is valid and not already configured
    for (int i = 0; i < KX_PWM_CHANNEL_COUNT; i++)
    {
        if (g_channel_config[i].pin == pin)
        {
            LOG_ERR("Pin %d is already configured for PWM", pin);
            return KX_HAL_ERR_BUSY; // pin already configured
        }
    }

    // find an available channel
    for (int i = 0; i < KX_PWM_CHANNEL_COUNT; i++)
    {
        if (g_channel_config[i].pin == -1) // assuming -1 is an invalid pin
        {
            g_channel_config[i].pin = pin;
            return KX_HAL_OK;
        }
    }

    // every channel already in use -- previously fell through and
    // returned KX_HAL_OK even though nothing was actually configured
    return KX_HAL_ERR_INVALID_ARG;
}

Kx_ErrorCode KxPWM_SetFrequency(Kx_IO pin, uint32_t freq_hz)
{
    for (int i = 0; i < KX_PWM_CHANNEL_COUNT; i++)
    {
        if (g_channel_config[i].pin == pin)
        {
            g_channel_config[i].freq_hz = freq_hz;
            if (g_channel_config[i].is_chInit)
            {
                esp_err_t err = ledc_set_freq(LEDC_LOW_SPEED_MODE,
                                              g_channel_config[i].timer_idx,
                                              freq_hz);
                if (err != ESP_OK)
                {
                    return KX_HAL_ERR_FAIL; // could not set frequency
                }
            }

            return KX_HAL_OK;
        }
    }
    return KX_HAL_ERR_INVALID_ARG;
}

Kx_ErrorCode KxPWM_SetDutyCycle(Kx_IO pin, uint8_t duty_cycle)
{
    if (duty_cycle > 100)
    {
        return KX_HAL_ERR_INVALID_ARG;
    }

    for (int i = 0; i < KX_PWM_CHANNEL_COUNT; i++)
    {
        if (g_channel_config[i].pin == pin)
        {
            g_channel_config[i].duty_cycle = duty_cycle;
            if (g_channel_config[i].is_chInit)
            {
                uint32_t max_duty = (1 << LEDC_TIMER_13_BIT) - 1; // adjust to your configured duty resolution
                uint32_t raw_duty = (max_duty * duty_cycle) / 100;

                esp_err_t err = ledc_set_duty(LEDC_LOW_SPEED_MODE,
                                              i,
                                              raw_duty);
                if (err != ESP_OK)
                {
                    return KX_HAL_ERR_FAIL; // could not set duty cycle
                }

                err = ledc_update_duty(LEDC_LOW_SPEED_MODE,
                                       i);
                if (err != ESP_OK)
                {
                    return KX_HAL_ERR_FAIL; // could not update duty cycle
                }
            }
            return KX_HAL_OK;
        }
    }
    return KX_HAL_ERR_INVALID_ARG;
}

Kx_ErrorCode KxPWM_Start(Kx_IO pin)
{
    for (int i = 0; i < KX_PWM_CHANNEL_COUNT; i++)
    {
        if (g_channel_config[i].pin != pin)
        {
            continue;
        }
        LOG_INFO("Starting PWM on pin %d", pin);
        if (ledc_set_pin(pin, LEDC_LOW_SPEED_MODE, i) != ESP_OK)
        {
            return KX_HAL_ERR_INVALID_ARG;
        }

        /*
         * esp_clk_tree_src_get_freq_hz() returns esp_err_t, NOT the
         * frequency -- the frequency comes back through the third
         * (out) parameter. The original code assigned the return
         * value directly to apb_clk_freq, which would have silently
         * given a clock frequency of 0 (ESP_OK) on success, breaking
         * every downstream resolution/timer calculation.
         */
        uint32_t apb_clk_freq = 0;
        esp_err_t err = esp_clk_tree_src_get_freq_hz(
            (soc_module_clk_t)SOC_MOD_CLK_APB,
            ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED,
            &apb_clk_freq);
        if (err != ESP_OK || apb_clk_freq == 0)
        {
            return KX_HAL_ERR_INVALID_ARG;
        }

        // note: uses this channel's own freq_hz, not a stray global
        uint32_t resolution = ledc_find_suitable_duty_resolution(apb_clk_freq, g_channel_config[i].freq_hz);
        if (resolution == 0)
        {
            // requested frequency is not achievable at any resolution
            // on this clock source
            return KX_HAL_ERR_INVALID_ARG;
        }
        LOG_INFO("Best duty resolution is %d bits for frequency %d Hz", (int)resolution, (int)g_channel_config[i].freq_hz);
        g_channel_config[i].duty_resolution = (uint8_t)resolution;

        /*
         * NOTE: ledc_timer_config() resets and reprograms the whole
         * timer. If another channel is already running on
         * g_channel_config[i].timer_idx, calling Start() here will
         * change ITS frequency/resolution too. If channels are meant
         * to share timers, timer setup belongs in a one-time
         * "configure" step, not repeated on every Start(). Left as-is
         * here since fixing it means changing the sharing model --
         * flagging it so it's a deliberate choice, not a surprise.
         */
        ledc_timer_config_t timer_config = {
            .duty_resolution = g_channel_config[i].duty_resolution,
            .freq_hz = g_channel_config[i].freq_hz,
            .timer_num = g_channel_config[i].timer_idx,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .clk_cfg = LEDC_USE_APB_CLK,
        };
        if (ledc_timer_config(&timer_config) != ESP_OK)
        {
            return KX_HAL_ERR_INVALID_ARG;
        }
        g_channel_config[i].is_chInit = true;

        // set the duty cycle in the hardware register
        uint32_t max_duty = (1u << g_channel_config[i].duty_resolution) - 1;
        uint32_t duty = (max_duty * g_channel_config[i].duty_cycle) / 100; // was the undeclared bare `duty_cycle`

        if (ledc_set_duty(LEDC_LOW_SPEED_MODE, i, duty) != ESP_OK)
        {
            return KX_HAL_ERR_INVALID_ARG;
        }
        if (ledc_update_duty(LEDC_LOW_SPEED_MODE, i) != ESP_OK)
        {
            return KX_HAL_ERR_INVALID_ARG;
        }

        return KX_HAL_OK;
    }

    // pin not found among configured channels
    return KX_HAL_ERR_INVALID_ARG;
}

Kx_ErrorCode KxPWM_Stop(Kx_IO pin)
{
    for (int i = 0; i < KX_PWM_CHANNEL_COUNT; i++)
    {
        if (g_channel_config[i].pin == pin)
        {
            if (ledc_stop(LEDC_LOW_SPEED_MODE, i, 0) != ESP_OK)
            {
                return KX_HAL_ERR_INVALID_ARG;
            }
            return KX_HAL_OK;
        }
    }
    // previously: no return here -> undefined behavior if pin not found
    return KX_HAL_ERR_INVALID_ARG;
}