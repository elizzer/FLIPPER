idf_component_register(SRCS "cli_gpio.c"
                    INCLUDE_DIRS "include"
                    REQUIRES cmd_parser cli_app interface_registry kx_hal)
