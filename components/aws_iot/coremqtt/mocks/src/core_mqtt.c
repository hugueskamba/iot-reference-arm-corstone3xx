/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "core_mqtt.h"
#include "fff.h"


DEFINE_FAKE_VALUE_FUNC(
    MQTTStatus_t,
    MQTT_MatchTopic,
    const char *,
    const uint16_t,
    const char *,
    const uint16_t,
    bool *
    );
