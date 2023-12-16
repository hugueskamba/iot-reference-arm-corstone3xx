/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef EVENT_H
    #define EVENT_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdbool.h>
    #include <stdint.h>

    #include "fff.h"

    DECLARE_FAKE_VALUE_FUNC( bool, xIsMqttAgentConnected );
    DECLARE_FAKE_VOID_FUNC( vWaitUntilMQTTAgentConnected );
    DECLARE_FAKE_VOID_FUNC( vWaitUntilMQTTAgentReady );


    #ifdef __cplusplus
    }
    #endif

#endif // EVENT_H
