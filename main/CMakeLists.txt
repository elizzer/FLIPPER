idf_component_register(SRCS "flipper.c" "hal_spi_test.c" "ili9341_lcd_disp_test.c" "IoExp_test.c" 
                    INCLUDE_DIRS "."
                    REQUIRES esp_hal drv gui_framework event_manager esp_timer 
                    )
