
#include "clock_gen_hal.h"
#include "gpio_clock_gen/gpio_clock_gen.h"
#include "string.h"

static CLOCK_SRC_t clk_src(uint32_t frequency_hz){
    // if(frequency_hz <= 1000000){
        return CLK_SRC_GPIO;
    // }
    // else if(frequency_hz <= 10000000){
    //     return CLK_SRC_LEDC_PWM;
    // }else{
    //     return CLK_SRC_I2S;
    // }
}

static int8_t set_clock_ops(CLOCK_SRC_t clk_src, ClockHalOps_t *ops){
    switch (clk_src)
    {
    case CLK_SRC_GPIO:
        ops->clock_config = gpio_clock_config;
        ops->clock_pin_config = NULL;
        ops->start_clk = gpio_clock_start;
        ops->stop_clk = gpio_clock_stop;
        ops->deinit_clk = gpio_clock_deinit;
        break;
    
    case CLK_SRC_LEDC_PWM:
        // ops->clock_config = ledc_pwm_clock_config;
        // ops->clock_pin_config = ledc_pwm_clock_pin_config;
        // ops->start_clk = ledc_pwm_start_clock;
        // ops->stop_clk = ledc_pwm_stop_clock;
        break;

    case CLK_SRC_I2S:
        // ops->clock_config = i2s_clock_config;
        // ops->clock_pin_config = i2s_clock_pin_config;
        // ops->start_clk = i2s_start_clock;
        // ops->stop_clk = i2s_stop_clock;
        break;

    default:
        printf("Invalid clock source");
        return -1;
    }
    return 0;
}

int8_t clock_gen_hal_init(const ClockGenHalConfig_t *config, ClockGenHalHandle_t *handle){
    //validate the config parameters
    if(config == NULL ){
        printf("Invalid parameters to clock_gen_hal_init\n");
        return -1;
    }
    //allocate dynamic memory for the handle
    // handle = (ClockGenHalHandle_t*)malloc(sizeof(ClockGenHalHandle_t));
    // if(handle == NULL){
    //     printf("Failed to allocate memory for clock_gen_hal handle\n");
    //     return -1;
    // }
    //copy the config to the handle
    memcpy(&handle->config, config, sizeof(ClockGenHalConfig_t));
    
    handle->clk_src = clk_src(config->frequency_hz);
    set_clock_ops(handle->clk_src, &handle->ops);
    //initialize the clock source
    if(handle->ops.clock_config(handle) != 0){
        printf("Failed to configure the clock source\n");
        free(handle);
        return -1;
    }
    //call the congigure clock pin function if available
    if(handle->ops.clock_config != NULL){
        if(handle->ops.clock_config(handle) != 0){
            printf("Failed to configure the clock output pin\n");
            free(handle);
            return -1;
        }
    }else{
        printf("Clock pin configuration function not implemented for the selected clock source\n");
        return -1;
    }

    return 0;
}

int8_t clock_gen_hal_start_clk(ClockGenHalHandle_t *handle){
    if(handle == NULL){
        printf("Invalid parameters to clock_gen_start\n");
        return -1;
    }
    if (handle->ops.start_clk == NULL){
        printf("Start clock function not implemented for the selected clock source\n");
        return -1;
    }
    printf("Starting clock generator\n");
    handle->ops.start_clk(handle);
    return 0; 
}

int8_t clock_gen_hal_stop_clk(ClockGenHalHandle_t *handle){
    if(handle == NULL){
        printf("Invalid parameters to clock_gen_stop\n");
        return -1;
    }
    return handle->ops.stop_clk(handle);
}

int8_t clock_gen_hal_deinit(ClockGenHalHandle_t *handle){
    if(handle == NULL){
        printf("Invalid parameters to clock_gen_deinit\n");
        return -1;
    }
    int8_t status = 0;
    if(handle->ops.deinit_clk != NULL){
        status = handle->ops.deinit_clk(handle);
    }
    free(handle);
    return status;
}

int8_t clock_gen_hal_update_frequency(ClockGenHalHandle_t *handle, uint32_t frequency_hz){
    if(handle == NULL){
        printf("Invalid parameters to clock_gen_update_frequency\n");
        return -1;
    }
    //stop the clock before updating the frequency
    if(handle->ops.stop_clk(handle) != 0){
        printf("Failed to stop the clock before updating frequency\n");
        return -1;
    }
    //deinity the clock source before reconfiguring with new frequency
    if(handle->ops.deinit_clk != NULL){
        if(handle->ops.deinit_clk(handle) != 0){
            printf("Failed to deinit the clock source before updating frequency\n");
            return -1;
        }
    }
    //update the frequency in the handle config
    handle->config.frequency_hz = frequency_hz;
    //reconfigure the clock source with the new frequency
    if(handle->ops.clock_config(handle) != 0){
        printf("Failed to reconfigure the clock source with new frequency\n");
        return -1;
    }
    //restart the clock after updating the frequency
    if(handle->ops.start_clk(handle) != 0){
        printf("Failed to start the clock after updating frequency\n");
        return -1;
    }
    return 0;
}