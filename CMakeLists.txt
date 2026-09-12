# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    ${CMAKE_SOURCE_DIR}/cmd_parser
    ${CMAKE_SOURCE_DIR}/cli_app
    ${CMAKE_SOURCE_DIR}/cli_gpio
    ${CMAKE_SOURCE_DIR}/cli_pwm
    ${CMAKE_SOURCE_DIR}/cli_i2c
    ${CMAKE_SOURCE_DIR}/interface_registry
    ${CMAKE_SOURCE_DIR}/kx_hal

)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_cli)
