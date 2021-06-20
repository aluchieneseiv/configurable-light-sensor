# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/lambder/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(picoproject C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(picoproject picoproject.c )

pico_set_program_name(picoproject "picoproject")
pico_set_program_version(picoproject "0.1")

pico_enable_stdio_uart(picoproject 0)
pico_enable_stdio_usb(picoproject 1)

# Add the standard library to the build
target_link_libraries(picoproject pico_stdlib hardware_adc hardware_i2c)

pico_add_extra_outputs(picoproject)

