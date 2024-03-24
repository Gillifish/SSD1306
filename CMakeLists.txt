cmake_minimum_required(VERSION 3.27)

set(PICO_SDK_FETCH_FROM_GIT on)
include(pico_sdk_import.cmake)

project(SSD1306 C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

pico_sdk_init()

add_executable(SSD1306
    src/main.cpp
    src/SSD1306.cpp
    #src/desperate.c
)

target_link_libraries(SSD1306
    pico_stdlib
    hardware_pwm
    hardware_clocks
    hardware_i2c
)

pico_add_extra_outputs(SSD1306)
pico_enable_stdio_usb(SSD1306 1)
pico_enable_stdio_uart(SSD1306 0)
