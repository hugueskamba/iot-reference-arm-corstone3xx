/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef OTA_PRIVATE_H
    #define OTA_PRIVATE_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdbool.h>
    #include <stdint.h>

    #include "ota_config.h"

    #define OTA_MAX_BLOCK_BITMAP_SIZE    otaconfigMAX_BLOCK_BITMAP_SIZE                                              /*!< @brief Max allowed number of bytes to track all blocks of an OTA file. Adjust block size if more range is needed. */
    #define OTA_REQUEST_URL_MAX_SIZE     ( 1500 )                                                                    /*!< @brief Maximum size of the S3 presigned URL */

    #define OTA_DATA_BLOCK_SIZE          ( ( 1U << otaconfigLOG2_FILE_BLOCK_SIZE ) + OTA_REQUEST_URL_MAX_SIZE + 30 ) /*!< @brief Header is 19 bytes.*/

    typedef enum OtaEvent
    {
        OtaAgentEventStart = 0,           /*!< @brief Start the OTA state machine */
        OtaAgentEventStartSelfTest,       /*!< @brief Event to trigger self test. */
        OtaAgentEventRequestJobDocument,  /*!< @brief Event for requesting job document. */
        OtaAgentEventReceivedJobDocument, /*!< @brief Event when job document is received. */
        OtaAgentEventCreateFile,          /*!< @brief Event to create a file. */
        OtaAgentEventRequestFileBlock,    /*!< @brief Event to request file blocks. */
        OtaAgentEventReceivedFileBlock,   /*!< @brief Event to trigger when file block is received. */
        OtaAgentEventRequestTimer,        /*!< @brief Event to request event timer. */
        OtaAgentEventCloseFile,           /*!< @brief Event to trigger closing file. */
        OtaAgentEventSuspend,             /*!< @brief Event to suspend ota task */
        OtaAgentEventResume,              /*!< @brief Event to resume suspended task */
        OtaAgentEventUserAbort,           /*!< @brief Event triggered by user to stop agent. */
        OtaAgentEventShutdown,            /*!< @brief Event to trigger ota shutdown */
        OtaAgentEventMax                  /*!< @brief Last event specifier */
    } OtaEvent_t;

    typedef enum OtaImageState
    {
        OtaImageStateUnknown = 0,  /*!< @brief The initial state of the OTA MCU Image. */
        OtaImageStateTesting = 1,  /*!< @brief The state of the OTA MCU Image post successful download and reboot. */
        OtaImageStateAccepted = 2, /*!< @brief The state of the OTA MCU Image post successful download and successful self_test. */
        OtaImageStateRejected = 3, /*!< @brief The state of the OTA MCU Image when the job has been rejected. */
        OtaImageStateAborted = 4,  /*!< @brief The state of the OTA MCU Image after a timeout publish to the stream request fails.
                                    *   Also if the OTA MCU image is aborted in the middle of a stream. */
        OtaLastImageState = OtaImageStateAborted
    } OtaImageState_t;

    typedef enum OtaPalImageState
    {
        OtaPalImageStateUnknown = 0,   /*!< @brief The initial state of the OTA PAL Image. */
        OtaPalImageStatePendingCommit, /*!< @brief OTA PAL Image awaiting update. */
        OtaPalImageStateValid,         /*!< @brief OTA PAL Image is valid. */
        OtaPalImageStateInvalid        /*!< @brief OTA PAL Image is invalid. */
    } OtaPalImageState_t;

    typedef struct OtaFileContext
    {
        uint8_t * pFilePath;          /*!< @brief Update file pathname. */
        uint16_t filePathMaxSize;     /*!< @brief Maximum size of the update file path */
        uint32_t fileSize;            /*!< @brief The size of the file in bytes. */
        uint32_t blocksRemaining;     /*!< @brief How many blocks remain to be received (a code optimization). */
        uint32_t fileAttributes;      /*!< @brief Flags specific to the file being received (e.g. secure, bundle, archive). */
        uint32_t serverFileID;        /*!< @brief The file is referenced by this numeric ID in the OTA job. */
        uint8_t * pJobName;           /*!< @brief The job name associated with this file from the job service. */
        uint16_t jobNameMaxSize;      /*!< @brief Maximum size of the job name. */
        uint8_t * pStreamName;        /*!< @brief The stream associated with this file from the OTA service. */
        uint16_t streamNameMaxSize;   /*!< @brief Maximum size of the stream name. */
        uint8_t * pRxBlockBitmap;     /*!< @brief Bitmap of blocks received (for deduplicating and missing block request). */
        uint16_t blockBitmapMaxSize;  /*!< @brief Maximum size of the block bitmap. */
        uint8_t * pCertFilepath;      /*!< @brief Pathname of the certificate file used to validate the receive file. */
        uint16_t certFilePathMaxSize; /*!< @brief Maximum certificate path size. */
        uint8_t * pUpdateUrlPath;     /*!< @brief Url for the file. */
        uint16_t updateUrlMaxSize;    /*!< @brief Maximum size of the url. */
        uint8_t * pAuthScheme;        /*!< @brief Authorization scheme. */
        uint16_t authSchemeMaxSize;   /*!< @brief Maximum size of the auth scheme. */
        uint32_t updaterVersion;      /*!< @brief Used by OTA self-test detection, the version of Firmware that did the update. */
        bool isInSelfTest;            /*!< @brief True if the job is in self test mode. */
        uint8_t * pProtocols;         /*!< @brief Authorization scheme. */
        uint16_t protocolMaxSize;     /*!< @brief Maximum size of the  supported protocols string. */
        uint8_t * pDecodeMem;         /*!< @brief Decode memory. */
        uint32_t decodeMemMaxSize;    /*!< @brief Maximum size of the decode memory. */
        uint32_t fileType;            /*!< @brief The file type id set when creating the OTA job. */
    } OtaFileContext_t;

    typedef struct OtaEventData
    {
        uint8_t data[ OTA_DATA_BLOCK_SIZE ]; /*!< Buffer for storing event information. */
        uint32_t dataLength;                 /*!< Total space required for the event. */
        bool bufferUsed;                     /*!< Flag set when buffer is used otherwise cleared. */
    } OtaEventData_t;

    typedef struct OtaEventMsg
    {
        OtaEventData_t * pEventData; /*!< Event status message. */
        OtaEvent_t eventId;          /*!< Identifier for the event. */
    } OtaEventMsg_t;

    typedef struct OtaAgentStatistics
    {
        uint32_t otaPacketsReceived;  /*!< Number of OTA packets received by the MQTT callback. */
        uint32_t otaPacketsQueued;    /*!< Number of OTA packets queued by the MQTT callback. */
        uint32_t otaPacketsProcessed; /*!< Number of OTA packets processed by the OTA task. */
        uint32_t otaPacketsDropped;   /*!< Number of OTA packets dropped due to congestion. */
    } OtaAgentStatistics_t;

    #ifdef __cplusplus
    }
    #endif

#endif /* ifndef OTA_PRIVATE_H */
