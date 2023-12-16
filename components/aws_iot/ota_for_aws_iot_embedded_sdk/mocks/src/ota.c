/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "ota.h"
#include "fff.h"


DEFINE_FAKE_VALUE_FUNC(
    OtaErr_t,
    OTA_Init,
    const OtaAppBuffer_t *,
    const OtaInterfaces_t *,
    const uint8_t *,
    OtaAppCallback_t
    );
DEFINE_FAKE_VALUE_FUNC( OtaErr_t, OTA_Suspend );
DEFINE_FAKE_VALUE_FUNC( OtaErr_t, OTA_Resume );
DEFINE_FAKE_VOID_FUNC( OTA_EventProcessingTask, const void * );
DEFINE_FAKE_VALUE_FUNC( OtaErr_t, OTA_ActivateNewImage );
DEFINE_FAKE_VALUE_FUNC( OtaState_t, OTA_Shutdown, uint32_t, uint8_t );
DEFINE_FAKE_VALUE_FUNC( OtaErr_t, OTA_SetImageState, OtaImageState_t );
DEFINE_FAKE_VALUE_FUNC( bool, OTA_SignalEvent, const OtaEventMsg_t * const );
DEFINE_FAKE_VALUE_FUNC( OtaState_t, OTA_GetState );
DEFINE_FAKE_VALUE_FUNC( OtaErr_t, OTA_GetStatistics, OtaAgentStatistics_t * );
