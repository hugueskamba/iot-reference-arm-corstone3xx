/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "ota_os_freertos.h"
#include "fff.h"


DEFINE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaInitEvent_FreeRTOS, OtaEventContext_t * );
DEFINE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaSendEvent_FreeRTOS, OtaEventContext_t *, const void *, unsigned int );
DEFINE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaReceiveEvent_FreeRTOS, OtaEventContext_t *, void *, uint32_t );
DEFINE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaDeinitEvent_FreeRTOS, OtaEventContext_t * );
DEFINE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaStartTimer_FreeRTOS, OtaTimerId_t,
                        const char * const,
                        const uint32_t,
                        OtaTimerCallback_t );
DEFINE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaStopTimer_FreeRTOS, OtaTimerId_t );
DEFINE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaDeleteTimer_FreeRTOS, OtaTimerId_t );
DEFINE_FAKE_VALUE_FUNC( void *, Malloc_FreeRTOS, size_t );
DEFINE_FAKE_VOID_FUNC( Free_FreeRTOS, void * );
