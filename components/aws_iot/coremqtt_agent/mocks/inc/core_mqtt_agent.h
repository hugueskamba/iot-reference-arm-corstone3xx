/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_MQTT_AGENT_H
    #define CORE_MQTT_AGENT_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdbool.h>
    #include <stdint.h>

    #include "fff.h"

    #include "core_mqtt_serializer.h"

    typedef struct MQTTAgentContext
    {
        void * pIncomingCallbackContext; /**< Context for incoming publish callback. */
        bool packetReceivedInLoop;       /**< Whether a MQTT_ProcessLoop() call received a packet. */
    } MQTTAgentContext_t;

    typedef struct MQTTAgentCommandContext MQTTAgentCommandContext_t;

    typedef struct MQTTAgentReturnInfo
    {
        MQTTStatus_t returnCode; /**< Return code of the MQTT command. */
        uint8_t * pSubackCodes;  /**< Array of SUBACK statuses, for a SUBSCRIBE command. */
    } MQTTAgentReturnInfo_t;


    typedef void (* MQTTAgentCommandCallback_t )( MQTTAgentCommandContext_t * pCmdCallbackContext,
                                                  MQTTAgentReturnInfo_t * pReturnInfo );

    struct MQTTAgentCommand
    {
        void * pArgs;
    };

    typedef struct MQTTAgentSubscribeArgs
    {
        MQTTSubscribeInfo_t * pSubscribeInfo; /**< @brief List of MQTT subscriptions. */
        size_t numSubscriptions;              /**< @brief Number of elements in `pSubscribeInfo`. */
    } MQTTAgentSubscribeArgs_t;

    typedef struct MQTTAgentCommandInfo
    {
        MQTTAgentCommandCallback_t cmdCompleteCallback;          /**< @brief Callback to invoke upon completion. */
        MQTTAgentCommandContext_t * pCmdCompleteCallbackContext; /**< @brief Context for completion callback. */
        uint32_t blockTimeMs;                                    /**< @brief Maximum block time for enqueueing the command. */
    } MQTTAgentCommandInfo_t;

    DECLARE_FAKE_VALUE_FUNC(
        MQTTStatus_t,
        MQTTAgent_Subscribe,
        const MQTTAgentContext_t *,
        MQTTAgentSubscribeArgs_t *,
        const MQTTAgentCommandInfo_t *
        );
    DECLARE_FAKE_VALUE_FUNC(
        MQTTStatus_t,
        MQTTAgent_Unsubscribe,
        const MQTTAgentContext_t *,
        MQTTAgentSubscribeArgs_t *,
        const MQTTAgentCommandInfo_t *
        );
    DECLARE_FAKE_VALUE_FUNC(
        MQTTStatus_t,
        MQTTAgent_Publish,
        const MQTTAgentContext_t *,
        MQTTPublishInfo_t *,
        const MQTTAgentCommandInfo_t *
        );


    #ifdef __cplusplus
    }
    #endif

#endif // CORE_MQTT_AGENT_H
