cmake_minimum_required(VERSION 3.13)

# Import Raspberry Pi Pico SDK (make sure PICO_SDK_PATH is set)
include(pico_sdk_import.cmake)

project(rp2350_usb_can C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Select your board: pico2 or pico2_w (Wi‑Fi). You can override at configure time:
#   cmake -DPICO_BOARD=pico2 -S . -B build
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Init SDK
set(PICO_EXTRAS_PATH "/home/andrei/pico/pico/pico-extras-master")
pico_sdk_init()

add_executable(rp2350_usb_can
    src/main.cpp
    src/usb_descriptors.cpp
    src/usb_callbacks.cpp
    src/xl2515.c
    src/slcan.cpp
    src/xl2515ext.c
    src/rb.c
    )
add_subdirectory(/home/andrei/pico/pico/pico-extras-master ${CMAKE_BINARY_DIR}/pico-extras)
target_include_directories(rp2350_usb_can PRIVATE include)

# Link with Pico SDK and TinyUSB (device stack)
target_link_libraries(rp2350_usb_can
    pico_stdlib
    hardware_uart
    tinyusb_device
    hardware_spi
    tinyusb_board
)

# Use USB stdio and disable UART stdio by default (you can flip if needed)
pico_enable_stdio_usb(rp2350_usb_can 1)
pico_enable_stdio_uart(rp2350_usb_can 0)

# Produce UF2, bin, map, disassembly, etc.
pico_add_extra_outputs(rp2350_usb_can)
