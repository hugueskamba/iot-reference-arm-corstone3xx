/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "core_mqtt_agent.h"
#include "fff.h"


DEFINE_FAKE_VALUE_FUNC(
    MQTTStatus_t,
    MQTTAgent_Subscribe,
    const MQTTAgentContext_t *,
    MQTTAgentSubscribeArgs_t *,
    const MQTTAgentCommandInfo_t *
    );
DEFINE_FAKE_VALUE_FUNC(
    MQTTStatus_t,
    MQTTAgent_Unsubscribe,
    const MQTTAgentContext_t *,
    MQTTAgentSubscribeArgs_t *,
    const MQTTAgentCommandInfo_t *
    );
DEFINE_FAKE_VALUE_FUNC(
    MQTTStatus_t,
    MQTTAgent_Publish,
    const MQTTAgentContext_t *,
    MQTTPublishInfo_t *,
    const MQTTAgentCommandInfo_t *
    );
