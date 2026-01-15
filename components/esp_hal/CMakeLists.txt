idf_component_register(SRCS "spi/src/spi_hal.c" 
                            "gpio/src/gpio_hal.c"
                            "i2c_hal/src/i2c_hal.c"
                            "hw_include/hw_include.c"
                       INCLUDE_DIRS "spi/includes" 
                                    "gpio/includes"
                                    "i2c_hal/includes"
                                    "hw_include"
                       REQUIRES driver)