idf_component_register(SRCS "kx_esp32s3_hal/kx_gpio_hal/kx_gpio_esp32s3_hal.c" 
                            "kx_esp32s3_hal/kx_pwm_hal/kx_pwm_esp32s3_hal.c"
                            "kx_esp32s3_hal/kx_i2c_hal/kx_i2c_esp32s3_hal.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver soc hal cmd_parser esp_rom esp_timer
                    PRIV_REQUIRES esp_hw_support
                    )
                   
