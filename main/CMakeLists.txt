idf_component_register(SRCS "esp32_cli.c"
                    INCLUDE_DIRS "."
                    REQUIRES cmd_parser driver cli_app 
                    PRIV_REQUIRES esp_timer 
                    )
