/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef SEMAPHORE_H
    #define SEMAPHORE_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include "fff.h"
    #include "portmacro.h"
    #include "projdefs.h"
    #include "queue.h"

    typedef QueueHandle_t SemaphoreHandle_t;

    DECLARE_FAKE_VALUE_FUNC( BaseType_t, xSemaphoreTake, SemaphoreHandle_t, TickType_t );
    DECLARE_FAKE_VALUE_FUNC( BaseType_t, xSemaphoreGive, SemaphoreHandle_t );
    DECLARE_FAKE_VALUE_FUNC( SemaphoreHandle_t, xSemaphoreCreateMutex );
    DECLARE_FAKE_VOID_FUNC( vSemaphoreDelete, SemaphoreHandle_t );


    #ifdef __cplusplus
    }
    #endif

#endif // SEMAPHORE_H
