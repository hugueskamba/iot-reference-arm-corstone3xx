/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef _OTA_OS_FREERTOS_H_
    #define _OTA_OS_FREERTOS_H_

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdint.h>
    #include "fff.h"
    #include "ota_os_interface.h"


    DECLARE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaInitEvent_FreeRTOS, OtaEventContext_t * );
    DECLARE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaSendEvent_FreeRTOS, OtaEventContext_t *, const void *, unsigned int );
    DECLARE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaReceiveEvent_FreeRTOS, OtaEventContext_t *, void *, uint32_t );
    DECLARE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaDeinitEvent_FreeRTOS, OtaEventContext_t * );
    DECLARE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaStartTimer_FreeRTOS, OtaTimerId_t,
                             const char * const,
                             const uint32_t,
                             OtaTimerCallback_t );
    DECLARE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaStopTimer_FreeRTOS, OtaTimerId_t );
    DECLARE_FAKE_VALUE_FUNC( OtaOsStatus_t, OtaDeleteTimer_FreeRTOS, OtaTimerId_t );
    DECLARE_FAKE_VALUE_FUNC( void *, Malloc_FreeRTOS, size_t );
    DECLARE_FAKE_VOID_FUNC( Free_FreeRTOS, void * );

    #ifdef __cplusplus
    }
    #endif

#endif // _OTA_OS_FREERTOS_H_
