cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(pico_max31855_examples C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -Wno-maybe-uninitialized
)

include_directories(..)

add_executable(example-pico-max31855
    example-pico-max31855.cpp
	../max31855.cpp
)

target_link_libraries(example-pico-max31855
    pico_stdlib
    hardware_spi
)

# enable stdio_usb
pico_enable_stdio_usb(example-pico-max31855 1) 
pico_enable_stdio_uart(example-pico-max31855 0) 

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(example-pico-max31855)
