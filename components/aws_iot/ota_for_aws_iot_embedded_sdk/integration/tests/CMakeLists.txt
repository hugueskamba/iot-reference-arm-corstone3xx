# Copyright 2023 Arm Limited and/or its affiliates
# <open-source-office@arm.com>
# SPDX-License-Identifier: MIT

add_executable(ota-agent-task-test
    test_ota_agent_task.cpp
    ../src/ota_agent_task.c
)

target_include_directories(ota-agent-task-test
    PRIVATE
        ../inc
)

target_link_libraries(ota-agent-task-test
    PRIVATE
        fff
        coremqtt-agent-mock
        freertos-kernel-mock
        freertos-ota-pal-psa-mock
        ota-for-aws-iot-embedded-sdk-mock
        tfm-ns-interface-mock
)

iot_reference_arm_corstone3xx_add_test(ota-agent-task-test)
