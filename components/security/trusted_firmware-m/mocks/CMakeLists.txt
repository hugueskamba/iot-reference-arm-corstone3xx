# Copyright 2023 Arm Limited and/or its affiliates
# <open-source-office@arm.com>
# SPDX-License-Identifier: MIT

add_library(tfm-ns-interface-mock INTERFACE)

target_include_directories(tfm-ns-interface-mock
    INTERFACE
        inc
)

target_link_libraries(tfm-ns-interface-mock
    INTERFACE
        fff
)
