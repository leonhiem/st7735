cmake_minimum_required(VERSION 3.13)

# Pull in Pico SDK (must be before project())
include(pico_sdk_import.cmake)

project(warmer_display_demo C CXX ASM)
set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# ON by default in this bench-prototype build so the APGAR checkpoint
# flash (normally at 1/5/10 real minutes) can be previewed in under a
# minute. Turn OFF (-DAPGAR_FAST_DEMO=OFF) for anything resembling a
# real deployment.
option(APGAR_FAST_DEMO "Use seconds instead of minutes for APGAR checkpoints, to preview the flash quickly" ON)

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
    text_console.c
    font_bitmaps.h
    display.c
)

target_include_directories(warmer_display_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(APGAR_FAST_DEMO)
    target_compile_definitions(warmer_display_demo PRIVATE APGAR_FAST_DEMO=1)
endif()

target_link_libraries(warmer_display_demo
    pico_stdlib
    hardware_spi
    hardware_gpio
)

# UART output for printf debugging (set both to 0 if you don't want stdio)
pico_enable_stdio_uart(warmer_display_demo 1)
pico_enable_stdio_usb(warmer_display_demo 0)

pico_add_extra_outputs(warmer_display_demo)
