/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef OTA_PAL_H_
    #define OTA_PAL_H_

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdint.h>

    #include "fff.h"

    #include "ota_private.h"
    #include "ota_platform_interface.h"

    DECLARE_FAKE_VALUE_FUNC( OtaPalStatus_t, otaPal_Abort, OtaFileContext_t * const );
    DECLARE_FAKE_VALUE_FUNC( OtaPalStatus_t, otaPal_CreateFileForRx, OtaFileContext_t * const );
    DECLARE_FAKE_VALUE_FUNC( OtaPalStatus_t, otaPal_CloseFile, OtaFileContext_t * const );
    DECLARE_FAKE_VALUE_FUNC( int16_t, otaPal_WriteBlock, OtaFileContext_t * const,
                             uint32_t,
                             uint8_t * const,
                             uint32_t );
    DECLARE_FAKE_VALUE_FUNC( OtaPalStatus_t, otaPal_ActivateNewImage, OtaFileContext_t * const );
    DECLARE_FAKE_VALUE_FUNC( OtaPalStatus_t, otaPal_SetPlatformImageState, OtaFileContext_t * const,
                             OtaImageState_t );
    DECLARE_FAKE_VALUE_FUNC( OtaPalImageState_t, otaPal_GetPlatformImageState, OtaFileContext_t * const );
    DECLARE_FAKE_VALUE_FUNC( OtaPalStatus_t, otaPal_ResetDevice, OtaFileContext_t * const );

    #ifdef __cplusplus
    }
    #endif

#endif // OTA_PAL_H_
