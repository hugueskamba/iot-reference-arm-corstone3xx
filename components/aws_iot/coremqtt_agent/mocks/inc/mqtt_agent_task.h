/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef MQTT_AGENT_H
    #define MQTT_AGENT_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include "task.h"
    #include "core_mqtt_agent.h"
    #include "core_mqtt_serializer.h"

    MQTTAgentContext_t xGlobalMqttAgentContext;

    struct MQTTAgentCommandContext
    {
        MQTTStatus_t xReturnStatus;
        TaskHandle_t xTaskToNotify;
        void * pArgs;
    };

    #ifdef __cplusplus
    }
    #endif

#endif // MQTT_AGENT_H
