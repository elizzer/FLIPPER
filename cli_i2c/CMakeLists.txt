idf_component_register(SRCS "cli_i2c.c"
                    INCLUDE_DIRS "include"
                    REQUIRES cmd_parser cli_app interface_registry kx_hal)

