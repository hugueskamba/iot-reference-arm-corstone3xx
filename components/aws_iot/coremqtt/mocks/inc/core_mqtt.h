/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_MQTT_H
    #define CORE_MQTT_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdint.h>

    #include "fff.h"

    #include "core_mqtt_serializer.h"

    DECLARE_FAKE_VALUE_FUNC(
        MQTTStatus_t,
        MQTT_MatchTopic,
        const char *,
        const uint16_t,
        const char *,
        const uint16_t,
        bool *
        );
    #ifdef __cplusplus
    }
    #endif

#endif // CORE_MQTT_H
