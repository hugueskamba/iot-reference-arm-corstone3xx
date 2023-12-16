/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef OTA_H
    #define OTA_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdint.h>

    #include "fff.h"

    #include "ota_http_interface.h"
    #include "ota_mqtt_interface.h"
    #include "ota_os_interface.h"
    #include "ota_platform_interface.h"
    #include "ota_private.h"

    typedef enum OtaJobEvent
    {
        OtaJobEventActivate = 0,       /*!< @brief OTA receive is authenticated and ready to activate. */
        OtaJobEventFail = 1,           /*!< @brief OTA receive failed. Unable to use this update. */
        OtaJobEventStartTest = 2,      /*!< @brief OTA job is now in self test, perform user tests. */
        OtaJobEventProcessed = 3,      /*!< @brief OTA event queued by OTA_SignalEvent is processed. */
        OtaJobEventSelfTestFailed = 4, /*!< @brief OTA self-test failed for current job. */
        OtaJobEventParseCustomJob = 5, /*!< @brief OTA event for parsing custom job document. */
        OtaJobEventReceivedJob = 6,    /*!< @brief OTA event when a new valid AFT-OTA job is received. */
        OtaJobEventUpdateComplete = 7, /*!< @brief OTA event when the update is completed. */
        OtaJobEventNoActiveJob = 8,    /*!< @brief OTA event when no active job is pending. */
        OtaLastJobEvent = OtaJobEventStartTest
    } OtaJobEvent_t;

    typedef void (* OtaAppCallback_t)( OtaJobEvent_t eEvent,
                                       void * pData );

    typedef enum OtaState
    {
        OtaAgentStateNoTransition = -1,
        OtaAgentStateInit = 0,
        OtaAgentStateReady,
        OtaAgentStateRequestingJob,
        OtaAgentStateWaitingForJob,
        OtaAgentStateCreatingFile,
        OtaAgentStateRequestingFileBlock,
        OtaAgentStateWaitingForFileBlock,
        OtaAgentStateClosingFile,
        OtaAgentStateSuspended,
        OtaAgentStateShuttingDown,
        OtaAgentStateStopped,
        OtaAgentStateAll
    } OtaState_t;

    typedef enum OtaErr
    {
        OtaErrNone = 0,               /*!< @brief No error occurred during the operation. */
        OtaErrUninitialized,          /*!< @brief The error code has not yet been set by a logic path. */
        OtaErrPanic,                  /*!< @brief Unrecoverable Firmware error. Probably should log error and reboot. */
        OtaErrInvalidArg,             /*!< @brief API called with invalid argument. */
        OtaErrAgentStopped,           /*!< @brief Returned when operations are performed that requires OTA Agent running & its stopped. */
        OtaErrSignalEventFailed,      /*!< @brief Failed to send event to OTA state machine. */
        OtaErrRequestJobFailed,       /*!< @brief Failed to request the job document. */
        OtaErrInitFileTransferFailed, /*!< @brief Failed to update the OTA job status. */
        OtaErrRequestFileBlockFailed, /*!< @brief Failed to request file block. */
        OtaErrCleanupControlFailed,   /*!< @brief Failed to clean up the control plane. */
        OtaErrCleanupDataFailed,      /*!< @brief Failed to clean up the data plane. */
        OtaErrUpdateJobStatusFailed,  /*!< @brief Failed to update the OTA job status. */
        OtaErrJobParserError,         /*!< @brief An error occurred during job document parsing. See reason sub-code. */
        OtaErrInvalidDataProtocol,    /*!< @brief Job does not have a valid protocol for data transfer. */
        OtaErrMomentumAbort,          /*!< @brief Too many OTA stream requests without any response. */
        OtaErrDowngradeNotAllowed,    /*!< @brief Firmware version is older than the previous version. */
        OtaErrSameFirmwareVersion,    /*!< @brief Firmware version is the same as previous. New firmware could have failed to commit. */
        OtaErrImageStateMismatch,     /*!< @brief The OTA job was in Self Test but the platform image state was not. Possible tampering. */
        OtaErrNoActiveJob,            /*!< @brief Attempt to set final image state without an active job. */
        OtaErrUserAbort,              /*!< @brief User aborted the active OTA. */
        OtaErrFailedToEncodeCbor,     /*!< @brief Failed to encode CBOR object for requesting data block from streaming service. */
        OtaErrFailedToDecodeCbor,     /*!< @brief Failed to decode CBOR object from streaming service response. */
        OtaErrActivateFailed,         /*!< @brief Failed to activate the new image. */
        OtaErrFileSizeOverflow,       /*!< @brief Firmware file size greater than the max allowed size. */
        OtaErrEmptyJobDocument        /*!< @brief Empty job document found - this happens on start up. */
    } OtaErr_t;

    typedef struct OtaAppBuffer
    {
        uint8_t * pUpdateFilePath;   /*!< @brief Path to store the files. */
        uint16_t updateFilePathsize; /*!< @brief Maximum size of the file path. */
        uint8_t * pCertFilePath;     /*!< @brief Path to certificate file. */
        uint16_t certFilePathSize;   /*!< @brief Maximum size of the certificate file path. */
        uint8_t * pStreamName;       /*!< @brief Name of stream to download the files. */
        uint16_t streamNameSize;     /*!< @brief Maximum size of the stream name. */
        uint8_t * pDecodeMemory;     /*!< @brief Place to store the decoded files. */
        uint32_t decodeMemorySize;   /*!< @brief Maximum size of the decoded files buffer. */
        uint8_t * pFileBitmap;       /*!< @brief Bitmap of the parameters received. */
        uint16_t fileBitmapSize;     /*!< @brief Maximum size of the bitmap. */
        uint8_t * pUrl;              /*!< @brief Presigned url to download files from S3. */
        uint16_t urlSize;            /*!< @brief Maximum size of the URL. */
        uint8_t * pAuthScheme;       /*!< @brief Authentication scheme used to validate download. */
        uint16_t authSchemeSize;     /*!< @brief Maximum size of the auth scheme. */
    } OtaAppBuffer_t;

    typedef struct OtaInterface
    {
        OtaOSInterface_t os;     /*!< @brief OS interface to store event, timers and memory operations. */
        OtaMqttInterface_t mqtt; /*!< @brief MQTT interface that references the publish subscribe methods and callbacks. */
        OtaHttpInterface_t http; /*!< @brief HTTP interface to request data. */
        OtaPalInterface_t pal;   /*!< @brief OTA PAL callback structure. */
    } OtaInterfaces_t;

    DECLARE_FAKE_VALUE_FUNC(
        OtaErr_t,
        OTA_Init,
        const OtaAppBuffer_t *,
        const OtaInterfaces_t *,
        const uint8_t *,
        OtaAppCallback_t
        );
    DECLARE_FAKE_VALUE_FUNC( OtaErr_t, OTA_Suspend );
    DECLARE_FAKE_VALUE_FUNC( OtaErr_t, OTA_Resume );
    DECLARE_FAKE_VALUE_FUNC( OtaErr_t, OTA_GetStatistics, OtaAgentStatistics_t * );
    DECLARE_FAKE_VOID_FUNC( OTA_EventProcessingTask, const void * );
    DECLARE_FAKE_VALUE_FUNC( OtaErr_t, OTA_ActivateNewImage );
    DECLARE_FAKE_VALUE_FUNC( OtaState_t, OTA_Shutdown, uint32_t, uint8_t );
    DECLARE_FAKE_VALUE_FUNC( OtaErr_t, OTA_SetImageState, OtaImageState_t );
    DECLARE_FAKE_VALUE_FUNC( bool, OTA_SignalEvent, const OtaEventMsg_t * const );
    DECLARE_FAKE_VALUE_FUNC( OtaState_t, OTA_GetState );

    #ifdef __cplusplus
    }
    #endif

#endif // OTA_H
