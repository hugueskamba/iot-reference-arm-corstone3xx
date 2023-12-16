/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_MQTT_SERIALIZER_H
    #define CORE_MQTT_SERIALIZER_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdbool.h>
    #include <stddef.h>
    #include <stdint.h>

    typedef enum MQTTQoS
    {
        MQTTQoS0 = 0, /**< Delivery at most once. */
        MQTTQoS1 = 1, /**< Delivery at least once. */
        MQTTQoS2 = 2  /**< Delivery exactly once. */
    } MQTTQoS_t;

    typedef enum MQTTStatus
    {
        MQTTSuccess = 0,      /**< Function completed successfully. */
        MQTTBadParameter,     /**< At least one parameter was invalid. */
        MQTTNoMemory,         /**< A provided buffer was too small. */
        MQTTSendFailed,       /**< The transport send function failed. */
        MQTTRecvFailed,       /**< The transport receive function failed. */
        MQTTBadResponse,      /**< An invalid packet was received from the server. */
        MQTTServerRefused,    /**< The server refused a CONNECT or SUBSCRIBE. */
        MQTTNoDataAvailable,  /**< No data available from the transport interface. */
        MQTTIllegalState,     /**< An illegal state in the state record. */
        MQTTStateCollision,   /**< A collision with an existing state record entry. */
        MQTTKeepAliveTimeout, /**< Timeout while waiting for PINGRESP. */
        MQTTNeedMoreBytes     /**< MQTT_ProcessLoop/MQTT_ReceiveLoop has received
                               * incomplete data; it should be called again (probably after
                               * a delay). */
    } MQTTStatus_t;

    typedef struct MQTTPublishInfo
    {
        /**
         * @brief Quality of Service for message.
         */
        MQTTQoS_t qos;

        /**
         * @brief Whether this is a retained message.
         */
        bool retain;

        /**
         * @brief Whether this is a duplicate publish message.
         */
        bool dup;

        /**
         * @brief Topic name on which the message is published.
         */
        const char * pTopicName;

        /**
         * @brief Length of topic name.
         */
        uint16_t topicNameLength;

        /**
         * @brief Message payload.
         */
        const void * pPayload;

        /**
         * @brief Message payload length.
         */
        size_t payloadLength;
    } MQTTPublishInfo_t;

    typedef struct MQTTSubscribeInfo
    {
        /**
         * @brief Quality of Service for subscription.
         */
        MQTTQoS_t qos;

        /**
         * @brief Topic filter to subscribe to.
         */
        const char * pTopicFilter;

        /**
         * @brief Length of subscription topic filter.
         */
        uint16_t topicFilterLength;
    } MQTTSubscribeInfo_t;

    #ifdef __cplusplus
    }
    #endif

#endif /* ifndef CORE_MQTT_SERIALIZER_H */
