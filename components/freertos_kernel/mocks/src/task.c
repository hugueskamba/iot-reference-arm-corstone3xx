/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "task.h"
#include "fff.h"

DEFINE_FAKE_VALUE_FUNC(
    BaseType_t,
    xTaskCreate,
    TaskFunction_t,
    const char * const,
    configSTACK_DEPTH_TYPE,
    void *,
    UBaseType_t,
    TaskHandle_t *
    );
DEFINE_FAKE_VALUE_FUNC(
    BaseType_t,
    xTaskNotifyWait,
    uint32_t,
    uint32_t,
    uint32_t *,
    TickType_t
    );
DEFINE_FAKE_VALUE_FUNC(
    BaseType_t,
    xTaskNotify,
    TaskHandle_t,
    uint32_t,
    eNotifyAction
    );
DEFINE_FAKE_VALUE_FUNC(
    BaseType_t,
    xTaskNotifyStateClear,
    TaskHandle_t
    );
DEFINE_FAKE_VALUE_FUNC( TaskHandle_t, xTaskGetCurrentTaskHandle );
DEFINE_FAKE_VOID_FUNC( vTaskDelete, TaskHandle_t );
DEFINE_FAKE_VOID_FUNC( vTaskDelay, const TickType_t );
