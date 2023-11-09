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

    typedef enum ml_processing_state_t
    {
        ML_SILENCE,
        ML_UNKNOWN,
        ML_HEARD_YES,
        ML_HEARD_NO,
        ML_HEARD_UP,
        ML_HEARD_DOWN,
        ML_HEARD_LEFT,
        ML_HEARD_RIGHT,
        ML_HEARD_ON,
        ML_HEARD_OFF,
        ML_HEARD_GO,
        ML_HEARD_STOP
    } ml_processing_state_t;

/* return pointer to string version of ml_processing_state_t */
    const char * get_inference_result_string( ml_processing_state_t ref_state );

/**
 * @brief Start the inference task.
 */
    void ml_task_inference_start();

/**
 * @brief Stop the inference task.
 */
    void ml_task_inference_stop();

/**
 * @brief Initialises the interface for audio processing.
 * @return 0 if the interface was initialised, -1 otherwise.
 */
    int ml_interface_init( void );

/* Type of the handler called when the processing state changes.
 */
    typedef void (* ml_processing_change_handler_t)( void * self,
                                                     ml_processing_state_t new_state );

/**
 * @brief Register an handler called when the processing state change.
 * @param handler Function to register
 * @param ctx Contextual data for the task.
 */
    void register_ml_processing_change_cb( ml_processing_change_handler_t handler,
                                           void * ctx );

/**
 * @brief Task to perform ML processing.
 *        It is gated by the net task which lets it run
 *        if no ota job is present.
 * @param arg Contextual data for the task.
 */
    void ml_task( void * arg );

/**
 * @brief Task to communicate ML results via MQTT.
 * @param arg Contextual data for the task.
 */
    void ml_mqtt_task( void * arg );

/**
 * @brief Create ML task.
 */
    void vStartMlTask( void );

/**
 * @brief Create ML MQTT task.
 */
    void vStartMlMqttTask( void );

    #ifdef __cplusplus
    }
    #endif

#endif /* ! ML_INTERFACE_H */
