cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

project(dengo2switch)

# rest of your project

add_executable(dengo2switch
    dengo2switch.c
    ps2.c
    usb_descriptors.c
)

target_include_directories(dengo2switch PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

pico_enable_stdio_usb(dengo2switch 1)
pico_enable_stdio_uart(dengo2switch 0)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(dengo2switch pico_stdlib tinyusb_device tinyusb_board)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(dengo2switch)
