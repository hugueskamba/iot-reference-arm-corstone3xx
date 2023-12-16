/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef INC_TASK_H
    #define INC_TASK_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdint.h>

    #include "fff.h"
    #include "projdefs.h"
    #include "portmacro.h"
    #include "FreeRTOS.h"

    #define tskIDLE_PRIORITY    ( ( UBaseType_t ) 0U )


    typedef enum
    {
        eNoAction = 0,            /* Notify the task without updating its notify value. */
        eSetBits,                 /* Set bits in the task's notification value. */
        eIncrement,               /* Increment the task's notification value. */
        eSetValueWithOverwrite,   /* Set the task's notification value to a specific value even if the previous value has not yet been read by the task. */
        eSetValueWithoutOverwrite /* Set the task's notification value if the previous value has been read by the task. */
    } eNotifyAction;

    struct tskTaskControlBlock
    {
        int dummy;
    };
    typedef struct tskTaskControlBlock * TaskHandle_t;


    DECLARE_FAKE_VALUE_FUNC(
        BaseType_t,
        xTaskCreate,
        TaskFunction_t,
        const char * const,
        configSTACK_DEPTH_TYPE,
        void *,
        UBaseType_t,
        TaskHandle_t *
        );
    DECLARE_FAKE_VALUE_FUNC(
        BaseType_t,
        xTaskNotifyWait,
        uint32_t,
        uint32_t,
        uint32_t *,
        TickType_t
        );
    DECLARE_FAKE_VALUE_FUNC(
        BaseType_t,
        xTaskNotify,
        TaskHandle_t,
        uint32_t,
        eNotifyAction
        );
    DECLARE_FAKE_VALUE_FUNC(
        BaseType_t,
        xTaskNotifyStateClear,
        TaskHandle_t
        );
    DECLARE_FAKE_VALUE_FUNC( TaskHandle_t, xTaskGetCurrentTaskHandle );
    DECLARE_FAKE_VOID_FUNC( vTaskDelete, TaskHandle_t );
    DECLARE_FAKE_VOID_FUNC( vTaskDelay, const TickType_t );

    #ifdef __cplusplus
    }
    #endif

#endif // INC_TASK_H
