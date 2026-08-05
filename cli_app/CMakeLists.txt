idf_component_register(SRCS "cli_app.c"
                    INCLUDE_DIRS "include"
                    REQUIRES cmd_parser cli_gpio cli_pwm interface_registry)
