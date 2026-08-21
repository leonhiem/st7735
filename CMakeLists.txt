cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(st7735 C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)
pico_sdk_init()

add_executable(st7735
emojis.h  font.h  main.c  st7735.c  st7735.h

)


target_include_directories(st7735 PUBLIC)
target_link_directories(st7735 PUBLIC)
target_link_libraries(st7735 pico_stdlib hardware_spi hardware_gpio hardware_pwm)

# enable usb output, disable uart output
pico_enable_stdio_usb(st7735 1)
pico_enable_stdio_uart(st7735 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(st7735)

# example_auto_set_url(st7735)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)
