/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "events.h"

#include "fff.h"

DEFINE_FAKE_VALUE_FUNC( bool, xIsMqttAgentConnected );
DEFINE_FAKE_VOID_FUNC( vWaitUntilMQTTAgentConnected );
DEFINE_FAKE_VOID_FUNC( vWaitUntilMQTTAgentReady );
