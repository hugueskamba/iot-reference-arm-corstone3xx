/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef OTA_MQTT_INTERFACE_H
    #define OTA_MQTT_INTERFACE_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdint.h>

    typedef enum OtaMqttStatus
    {
        OtaMqttSuccess = 0,          /*!< @brief OTA MQTT interface success. */
        OtaMqttPublishFailed = 0xa0, /*!< @brief Attempt to publish a MQTT message failed. */
        OtaMqttSubscribeFailed,      /*!< @brief Failed to subscribe to a topic. */
        OtaMqttUnsubscribeFailed     /*!< @brief Failed to unsubscribe from a topic. */
    } OtaMqttStatus_t;

    typedef OtaMqttStatus_t ( * OtaMqttPublish_t )( const char * const pacTopic,
                                                    uint16_t usTopicLen,
                                                    const char * pcMsg,
                                                    uint32_t ulMsgSize,
                                                    uint8_t ucQoS );
    typedef OtaMqttStatus_t ( * OtaMqttUnsubscribe_t )  ( const char * pTopicFilter,
                                                          uint16_t topicFilterLength,
                                                          uint8_t ucQoS );
    typedef OtaMqttStatus_t ( * OtaMqttSubscribe_t ) ( const char * pTopicFilter,
                                                       uint16_t topicFilterLength,
                                                       uint8_t ucQoS );


    typedef struct OtaMqttInterface
    {
        OtaMqttSubscribe_t subscribe;     /*!< @brief Interface for subscribing to Mqtt topics. */
        OtaMqttUnsubscribe_t unsubscribe; /*!< @brief interface for unsubscribing to MQTT topics. */
        OtaMqttPublish_t publish;         /*!< @brief Interface for publishing MQTT messages. */
    } OtaMqttInterface_t;

    #ifdef __cplusplus
    }
    #endif

#endif // OTA_MQTT_INTERFACE_H
