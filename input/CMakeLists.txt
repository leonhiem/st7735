cmake_minimum_required(VERSION 3.13)

# Pull in Pico SDK (must be before project())
include(pico_sdk_import.cmake)

project(warmer_display_demo C CXX ASM)
set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(warmer_display_demo
    main.c
    st7735.c
    gfx.c
    icons.c
    icon_bitmaps.h
    baby.c
    face_bitmaps.h
    heat_indicator.c
    apgar_timer.c
    digit_bitmaps.h
    display.c
)

target_include_directories(warmer_display_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(warmer_display_demo
    pico_stdlib
    hardware_spi
    hardware_gpio
)

# UART output for printf debugging (set both to 0 if you don't want stdio)
pico_enable_stdio_uart(warmer_display_demo 1)
pico_enable_stdio_usb(warmer_display_demo 0)

pico_add_extra_outputs(warmer_display_demo)
