# Copyright 2023 Arm Limited and/or its affiliates
# <open-source-office@arm.com>
# SPDX-License-Identifier: MIT

add_library(freertos-ota-pal-psa-mock
    src/application_version.c
)

target_include_directories(freertos-ota-pal-psa-mock
    PUBLIC
        inc
)

target_link_libraries(freertos-ota-pal-psa-mock
    PUBLIC
        fff
        tfm-ns-interface-mock
)
