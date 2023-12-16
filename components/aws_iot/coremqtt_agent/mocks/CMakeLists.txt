# Copyright 2023 Arm Limited and/or its affiliates
# <open-source-office@arm.com>
# SPDX-License-Identifier: MIT

add_library(coremqtt-agent-mock
    src/core_mqtt_agent.c
    src/subscription_manager.c
)

target_include_directories(coremqtt-agent-mock
    PUBLIC
        inc
)

target_link_libraries(coremqtt-agent-mock
    PUBLIC
        fff
        coremqtt-mock
)
