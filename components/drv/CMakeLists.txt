idf_component_register(SRCS "ILI9341/src/ILI9341.c" "PCF8574_IoExp/src/PCF8574_IoExp.c" 
                       INCLUDE_DIRS "ILI9341/includes" "PCF8574_IoExp/includes"
                       REQUIRES driver esp_hal)