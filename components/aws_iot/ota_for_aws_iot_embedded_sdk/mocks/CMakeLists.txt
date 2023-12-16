# Copyright 2023 Arm Limited and/or its affiliates
# <open-source-office@arm.com>
# SPDX-License-Identifier: MIT

add_library(ota-for-aws-iot-embedded-sdk-mock
    src/events.c
    src/logging_stack.c
    src/ota.c
    src/ota_os_freertos.c
    src/ota_pal.c
)

target_include_directories(ota-for-aws-iot-embedded-sdk-mock
    PUBLIC
        inc
)

target_link_libraries(ota-for-aws-iot-embedded-sdk-mock
    PUBLIC
        fff
)
