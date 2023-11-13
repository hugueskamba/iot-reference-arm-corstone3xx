/* Copyright 2021-2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef ML_INTERFACE_H
    #define ML_INTERFACE_H

    #include <stddef.h>
    #include <stdint.h>

    #ifdef __cplusplus
    extern "C" {
    #endif

/**
 * @brief Start the inference task.
 */
    void ml_task_inference_start( void );

/**
 * @brief Stop the inference task.
 */
    void ml_task_inference_stop( void );

/**
 * @brief Initialises the interface for audio processing.
 * @return 0 if the interface was initialised, -1 otherwise.
 */
    int ml_interface_init( void );

/**
 * @brief
 * @param buffer
 * @param size
 */
    void ml_process_audio( const int16_t * buffer,
                           size_t size );

/**
 * @brief Task to perform ML processing results via MQTT.
 *        It is gated by the net task which lets it run
 *        if no ota job is present.
 * @param pvParameters Contextual data for the task.
 */
    void ml_task( void * pvParameters );

/**
 * @brief Task to communicate ML results via MQTT.
 * @param pvParameters Contextual data for the task.
 */
    void ml_mqtt_task( void * pvParameters );

/**
 * @brief Get the frame length.
 * @return  The frame length.
 */
    int ml_get_frame_length( void );

/**
 * @brief Get the frame stride.
 * @return  The frame stride.
 */
    int ml_get_frame_stride( void );

/**
 * @brief Hook to send the result of the inference.
 * @param message The message to send.
 */
    void mqtt_send_message( const char * message );

/**
 * @brief Create ML MQTT task.
 */
    void vStartMlMqttTask( void );

/**
 * @brief Create ML task.
 * @param pvParameters Contextual data for the task.
 */
    void vStartMlTask( void * pvParameters );

    #ifdef __cplusplus
    }
    #endif

#endif /* ! ML_INTERFACE_H */
