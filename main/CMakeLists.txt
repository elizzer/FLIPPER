idf_component_register(SRCS "flipper.c" "hal_spi_test.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_hal drv
                    )
