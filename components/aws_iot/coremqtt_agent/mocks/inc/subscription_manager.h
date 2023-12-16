/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef SUBSCRIPTION_MANAGER_H
    #define SUBSCRIPTION_MANAGER_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include "fff.h"

    #include <stdbool.h>
    #include <stdint.h>
    #include "core_mqtt_serializer.h"

    typedef void (* IncomingPubCallback_t)( void * pvIncomingPublishCallbackContext,
                                            MQTTPublishInfo_t * pxPublishInfo );

    DECLARE_FAKE_VALUE_FUNC(
        bool,
        addSubscription,
        const char *,
        uint16_t,
        IncomingPubCallback_t,
        void *
        );
    DECLARE_FAKE_VOID_FUNC(
        removeSubscription,
        const char *,
        uint16_t
        );

    #ifdef __cplusplus
    }
    #endif

#endif // SUBSCRIPTION_MANAGER_H
