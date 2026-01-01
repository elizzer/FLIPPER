idf_component_register(SRCS "spi/src/spi_hal.c" 
                            "gpio/src/gpio_hal.c"
                       INCLUDE_DIRS "spi/includes" 
                                    "gpio/includes"
                       REQUIRES driver)