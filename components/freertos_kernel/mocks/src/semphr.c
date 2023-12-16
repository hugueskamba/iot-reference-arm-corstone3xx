/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "semphr.h"
#include "fff.h"


DEFINE_FAKE_VALUE_FUNC( BaseType_t, xSemaphoreTake, SemaphoreHandle_t, TickType_t );
DEFINE_FAKE_VALUE_FUNC( BaseType_t, xSemaphoreGive, SemaphoreHandle_t );
DEFINE_FAKE_VALUE_FUNC( SemaphoreHandle_t, xSemaphoreCreateMutex );
DEFINE_FAKE_VOID_FUNC( vSemaphoreDelete, SemaphoreHandle_t );
