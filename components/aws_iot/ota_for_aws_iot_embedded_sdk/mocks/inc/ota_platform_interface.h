/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef OTA_PLATFORM_INTERFACE
    #define OTA_PLATFORM_INTERFACE

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include "ota_private.h"

    typedef uint32_t OtaPalStatus_t;

    typedef OtaPalStatus_t ( * OtaPalAbort_t )( OtaFileContext_t * const pFileContext );
    typedef OtaPalStatus_t (* OtaPalCreateFileForRx_t)( OtaFileContext_t * const pFileContext );
    typedef OtaPalStatus_t ( * OtaPalCloseFile_t )( OtaFileContext_t * const pFileContext );
    typedef int16_t ( * OtaPalWriteBlock_t ) ( OtaFileContext_t * const pFileContext,
                                               uint32_t offset,
                                               uint8_t * const pData,
                                               uint32_t blockSize );
    typedef OtaPalStatus_t ( * OtaPalActivateNewImage_t )( OtaFileContext_t * const pFileContext );
    typedef OtaPalStatus_t ( * OtaPalResetDevice_t ) ( OtaFileContext_t * const pFileContext );
    typedef OtaPalStatus_t ( * OtaPalSetPlatformImageState_t )( OtaFileContext_t * const pFileContext,
                                                                OtaImageState_t eState );
    typedef OtaPalImageState_t ( * OtaPalGetPlatformImageState_t ) ( OtaFileContext_t * const pFileContext );



    typedef struct OtaPalInterface
    {
        OtaPalAbort_t abort;                                 /*!< @brief Abort an OTA transfer. */
        OtaPalCreateFileForRx_t createFile;                  /*!< @brief Create a new receive file. */
        OtaPalCloseFile_t closeFile;                         /*!< @brief Authenticate and close the receive file. */
        OtaPalWriteBlock_t writeBlock;                       /*!< @brief Write a block of data to the specified file at the given offset. */
        OtaPalActivateNewImage_t activate;                   /*!< @brief Activate the file received over-the-air. */
        OtaPalResetDevice_t reset;                           /*!< @brief Reset the device. */
        OtaPalSetPlatformImageState_t setPlatformImageState; /*!< @brief Set the state of the OTA update image. */
        OtaPalGetPlatformImageState_t getPlatformImageState; /*!< @brief Get the state of the OTA update image. */
    } OtaPalInterface_t;

    #ifdef __cplusplus
    }
    #endif

#endif // OTA_PLATFORM_INTERFACE
