/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef OTA_OS_INTERFACE_H
    #define OTA_OS_INTERFACE_H

    #ifdef __cplusplus
    extern "C" {
    #endif



    typedef enum OtaOsStatus
    {
        OtaOsSuccess = 0,                    /*!< @brief OTA OS interface success. */
        OtaOsEventQueueCreateFailed = 0x80U, /*!< @brief Failed to create the event queue. */
        OtaOsEventQueueSendFailed,           /*!< @brief Posting event message to the event queue failed. */
        OtaOsEventQueueReceiveFailed,        /*!< @brief Failed to receive from the event queue. */
        OtaOsEventQueueDeleteFailed,         /*!< @brief Failed to delete the event queue. */
        OtaOsTimerCreateFailed,              /*!< @brief Failed to create the timer. */
        OtaOsTimerStartFailed,               /*!< @brief Failed to create the timer. */
        OtaOsTimerRestartFailed,             /*!< @brief Failed to restart the timer. */
        OtaOsTimerStopFailed,                /*!< @brief Failed to stop the timer. */
        OtaOsTimerDeleteFailed               /*!< @brief Failed to delete the timer. */
    } OtaOsStatus_t;

    typedef enum
    {
        OtaRequestTimer = 0,
        OtaSelfTestTimer,
        OtaNumOfTimers
    } OtaTimerId_t;

    typedef void ( * OtaTimerCallback_t )( OtaTimerId_t otaTimerId );


    struct OtaEventContext
    {
        int dummy;
    };
    typedef struct OtaEventContext OtaEventContext_t;

    typedef OtaOsStatus_t ( * OtaInitEvent_t ) ( OtaEventContext_t * pEventCtx );
    typedef OtaOsStatus_t ( * OtaSendEvent_t )( OtaEventContext_t * pEventCtx,
                                                const void * pEventMsg,
                                                unsigned int timeout );
    typedef OtaOsStatus_t ( * OtaReceiveEvent_t )( OtaEventContext_t * pEventCtx,
                                                   void * pEventMsg,
                                                   uint32_t timeout );
    typedef OtaOsStatus_t ( * OtaDeinitEvent_t )( OtaEventContext_t * pEventCtx );

    typedef OtaOsStatus_t ( * OtaStartTimer_t ) ( OtaTimerId_t otaTimerId,
                                                  const char * const pTimerName,
                                                  const uint32_t timeout,
                                                  OtaTimerCallback_t callback );
    typedef OtaOsStatus_t ( * OtaStopTimer_t ) ( OtaTimerId_t otaTimerId );
    typedef OtaOsStatus_t ( * OtaDeleteTimer_t ) ( OtaTimerId_t otaTimerId );

    typedef void * ( * OtaMalloc_t ) ( size_t size );
    typedef void ( * OtaFree_t ) ( void * ptr );
    typedef struct OtaMallocInterface
    {
        OtaMalloc_t malloc; /*!< @brief OTA memory allocate interface. */
        OtaFree_t free;     /*!< @brief OTA memory deallocate interface. */
    } OtaMallocInterface_t;

    typedef struct OtaEventInterface
    {
        OtaInitEvent_t init;               /*!< @brief Initialization event. */
        OtaSendEvent_t send;               /*!< @brief Send data. */
        OtaReceiveEvent_t recv;            /*!< @brief Receive data. */
        OtaDeinitEvent_t deinit;           /*!< @brief Deinitialize event. */
        OtaEventContext_t * pEventContext; /*!< @brief Event context to store event information. */
    } OtaEventInterface_t;


    typedef struct OtaTimerInterface
    {
        OtaStartTimer_t start;            /*!< @brief Timer start state. */
        OtaStopTimer_t stop;              /*!< @brief Timer stop state. */
        #ifndef __cplusplus
            OtaDeleteTimer_t delete;      /*!< @brief Delete timer. */
        #else
            OtaDeleteTimer_t deleteTimer; /*!< @brief Delete timer for C++ builds. */
        #endif
    } OtaTimerInterface_t;

    typedef struct OtaOSInterface
    {
        OtaEventInterface_t event; /*!< @brief OTA Event interface. */
        OtaTimerInterface_t timer; /*!< @brief OTA Timer interface. */
        OtaMallocInterface_t mem;  /*!< @brief OTA memory interface. */
    } OtaOSInterface_t;

    #ifdef __cplusplus
    }
    #endif

#endif // OTA_OS_INTERFACE_H
