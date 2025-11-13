# This is the standard pico_sdk_import.cmake from Raspberry Pi examples with minimal changes.
# If you already have one in your repo, you can keep using it.
include(FetchContent)

if (NOT DEFINED PICO_SDK_PATH)
    if (DEFINED ENV{PICO_SDK_PATH})
        set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    else()
        message(FATAL_ERROR "PICO_SDK_PATH not set. Please set environment or cache var to your pico-sdk checkout")
    endif()
endif()

set(PICO_SDK_FETCH_FROM_GIT on)
set(PICO_SDK_PATH ${PICO_SDK_PATH} CACHE PATH "Path to the Raspberry Pi Pico SDK")

# Initialize the SDK (this defines pico_sdk_init())
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
