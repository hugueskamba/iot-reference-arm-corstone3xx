/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "app_config.h"
#include "aws_clientcredential.h"
#include "dev_mode_key_provisioning.h"
#include "events.h"
#include "demo_config.h"
#include "mbedtls/platform.h"
#include "mbedtls/threading.h"
#include "ml_interface.h"
#include "mqtt_agent_task.h"
#include "ota_provision.h"
#include "tfm_ns_interface.h"
#include "bsp_serial.h"
#include "aws_mbedtls_config.h"
#include "threading_alt.h"
#include "psa/crypto.h"
#include "tfm_ns_interface.h"

#include "FreeRTOSConfig.h"

/**
 * @brief The topic to subscribe and publish to in the example.
 *
 * The topic name starts with the client identifier to ensure that each demo
 * interacts with a unique topic name.
 */
#define mqttexampleTOPIC democonfigCLIENT_IDENTIFIER "/ml/inference"

/**
 * @brief The MQTT agent manages the MQTT contexts.  This set the handle to the
 * context used by this demo.
 */
extern MQTTAgentContext_t xGlobalMqttAgentContext;

/**
 * @brief The maximum time for which application waits for an MQTT operation to be complete.
 * This involves receiving an acknowledgment for broker for SUBSCRIBE, UNSUBSCRIBE and non
 * QOS0 publishes.
 */
#define appMQTT_TIMEOUT_MS                        ( 5000U )

/**
 * @brief Used to clear bits in a task's notification value.
 */
#define appMAX_UINT32                     ( 0xffffffff )

static SemaphoreHandle_t xMqttMutex;

extern void vStartOtaTask( void );
extern int32_t network_startup( void );
extern void vStartBlinkTask( void );
extern void vStartMlTask( void );
extern void vStartMlMqttTask( void );
extern BaseType_t xStartPubSubTasks( uint32_t ulNumPubsubTasks,
                                     configSTACK_DEPTH_TYPE uxStackSize,
                                     UBaseType_t uxPriority );

psa_key_handle_t xOTACodeVerifyKeyHandle = 0;

static bool xAreAwsCredentialsValid( void )
{
    if( ( strcmp( clientcredentialMQTT_BROKER_ENDPOINT, "dummy.endpointid.amazonaws.com" ) == 0 ) ||
        ( strcmp( clientcredentialIOT_THING_NAME, "dummy_thingname" ) == 0 ) )
    {
        printf( "[ERR] INVALID BROKER ENDPOINT AND/OR THING NAME.\r\n" );
        printf( "[ERR] Set the right credentials in aws_clientcredential.h\r\n" );
        return false;
    }

    return true;
}

static bool mqtt_lock()
{
    if (xMqttMutex == NULL) {
        return false;
    }

    BaseType_t ret = xSemaphoreTake( xMqttMutex, portMAX_DELAY );
    if (ret != pdTRUE) {
        LogError( ( "xSemaphoreTake xMqttMutex failed %ld\r\n", ret ) );
        return false;
    }

    return true;
}

static bool mqtt_unlock()
{

    if (xMqttMutex == NULL) {
        return false;
    }

    BaseType_t ret = xSemaphoreGive( xMqttMutex );
    if (ret != pdPASS) {
        LogError( ( "xSemaphoreGive xMqttMutex failed %ld\r\n", ret ) );
        return false;
    }

    return true;
}

static void prvAppPublishCommandCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                          MQTTAgentReturnInfo_t * pxReturnInfo )
{
    pxCommandContext->xReturnStatus = pxReturnInfo->returnCode;

    if( pxCommandContext->xTaskToNotify != NULL )
    {
        xTaskNotify( pxCommandContext->xTaskToNotify, ( uint32_t ) ( pxReturnInfo->returnCode ), eSetValueWithOverwrite );
    }
}

void mqtt_send_message(const char* message)
{
    if ( !mqtt_lock() ) {
        return;
    }

    static MQTTPublishInfo_t publishInfo = { 0 };
    static MQTTAgentCommandInfo_t xCommandParams = { 0 };
    static MQTTAgentCommandContext_t xCommandContext = { 0 };
    MQTTStatus_t mqttStatus = MQTTBadParameter;

    publishInfo.pTopicName = mqttexampleTOPIC;
    publishInfo.topicNameLength = ( uint16_t ) strlen( mqttexampleTOPIC );
    publishInfo.qos = MQTTQoS1;
    publishInfo.pPayload = message;
    publishInfo.payloadLength = strlen( message );

    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    xTaskNotifyStateClear( NULL );

    xCommandParams.blockTimeMs = appMQTT_TIMEOUT_MS;
    xCommandParams.cmdCompleteCallback = prvAppPublishCommandCallback;
    xCommandParams.pCmdCompleteCallbackContext = ( void * ) &xCommandContext;

    LogInfo( ( "Attempting to publish (%s) to the MQTT topic %s.\r\n", message, mqttexampleTOPIC ) );
    mqttStatus = MQTTAgent_Publish( &xGlobalMqttAgentContext,
                                    &publishInfo,
                                    &xCommandParams );

    /* Wait for command to complete so MQTTSubscribeInfo_t remains in scope for the
    * duration of the command. */
    if( mqttStatus == MQTTSuccess )
    {
        BaseType_t result = xTaskNotifyWait( 0, appMAX_UINT32, NULL, pdMS_TO_TICKS( appMQTT_TIMEOUT_MS ) );

        if( result != pdTRUE )
        {
            mqttStatus = MQTTSendFailed;
        }
        else
        {
            mqttStatus = xCommandContext.xReturnStatus;
        }
    }

    if( mqttStatus != MQTTSuccess )
    {
        LogError( ( "Failed to publish result over MQTT" ) );
    }
    else
    {
        LogInfo( ( "Sent PUBLISH packet to broker %.*s to broker.\n",
                   strlen( mqttexampleTOPIC ),
                   mqttexampleTOPIC ) );
    }

    mqtt_unlock();
}


void OTA_Active_Hook(void)
{
    ml_task_inference_stop();
}

void OTA_Not_Active_Hook(void)
{
    ml_task_inference_start();
}

void vAssertCalled( const char * pcFile,
                    unsigned long ulLine )
{
    printf( "ASSERT failed! file %s:%ld, \n", pcFile, ulLine );

    taskENTER_CRITICAL();
    {
        /* Use the debugger to set ul to a non-zero value in order to step out
         *      of this function to determine why it was called. */
        volatile unsigned long looping = 0;
        while( looping == 0LU )
        {
            portNOP();
        }
    }
    taskEXIT_CRITICAL();
}

BaseType_t xApplicationGetRandomNumber( uint32_t * pulNumber )
{
    return (BaseType_t)( psa_generate_random( ( uint8_t * ) ( pulNumber ), sizeof( uint32_t ) ) == PSA_SUCCESS );
}

uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                             uint16_t usSourcePort,
                                             uint32_t ulDestinationAddress,
                                             uint16_t usDestinationPort )
{
    ( void ) ulSourceAddress;
    ( void ) usSourcePort;
    ( void ) ulDestinationAddress;
    ( void ) usDestinationPort;

    uint32_t uxRandomValue = 0U;
    psa_status_t xPsaStatus = psa_generate_random( ( uint8_t * ) ( &uxRandomValue ), sizeof( uint32_t ) );

    if( xPsaStatus != PSA_SUCCESS )
    {
        LogError( ( "psa_generate_random failed with %d.", xPsaStatus ) );
        configASSERT( 0 );
    }

    return uxRandomValue;
}

int main(void)
{
    bsp_serial_init();

    if( xAreAwsCredentialsValid() != true )
    {
        return EXIT_FAILURE;
    }

    // TODO: See if this is also needed
    xLoggingTaskInitialize( appCONFIG_LOGGING_TASK_STACK_SIZE,
                            appCONFIG_LOGGING_TASK_PRIORITY,
                            appCONFIG_LOGGING_MESSAGE_QUEUE_LENGTH );

    UBaseType_t status = tfm_ns_interface_init();
    if( status != 0 )
    {
        LogError( ( "TF-M non-secure interface init failed with [%d]. Exiting...\n", status ) );
        return EXIT_FAILURE;
    }
    LogInfo( ( "PSA Framework version is: %d\n", psa_framework_version() ) );

    if (xEventHelperInit() != 0) {
        return EXIT_FAILURE;
    }

    (void)mbedtls_platform_set_calloc_free( mbedtls_platform_calloc, mbedtls_platform_free );

    UBaseType_t xRetVal = vDevModeKeyProvisioning();
    if( xRetVal != CKR_OK )
    {
        LogError( ( "Device key provisioning failed [%d]\n", xRetVal ) );
        LogError( ( "Device cannot connect to IoT Core. Exiting...\n" ) );
        return EXIT_FAILURE;
    }
    LogInfo( ( "Device key provisioning succeeded \n" ) );

    status = ota_privision_code_signing_key( &xOTACodeVerifyKeyHandle );
    if( status != PSA_SUCCESS )
    {
        LogError( ( "OTA signing key provision failed [%d]\n", status ) );
    }
    LogInfo( ( "OTA signing key provisioning succeeded \n" ) );

    xMqttMutex = xSemaphoreCreateMutex();
    if (xMqttMutex == NULL) {
        LogError( ( "Failed to create xMqttMutex\r\n" ) );
        return EXIT_FAILURE;
    }

    if( network_startup() != 0 )
    {
        return EXIT_FAILURE;
    }

    mbedtls_threading_set_alt( mbedtls_platform_mutex_init,
                                mbedtls_platform_mutex_free,
                                mbedtls_platform_mutex_lock,
                                mbedtls_platform_mutex_unlock );

    vStartBlinkTask();

    vStartMlTask();

    vStartMlMqttTask();

    /* Start MQTT agent task */
    vStartMqttAgentTask();


    /* Start OTA task*/
    vStartOtaTask();

    /* Start demo task once agent task is started. */
    ( void ) xStartPubSubTasks( appCONFIG_MQTT_NUM_PUBSUB_TASKS,
                                appCONFIG_MQTT_PUBSUB_TASK_STACK_SIZE,
                                appCONFIG_MQTT_PUBSUB_TASK_PRIORITY );

    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
    * line will never be reached.  If the following line does execute, then
    * there was insufficient FreeRTOS heap memory available for the idle and/or
    * timer tasks	to be created.  See the memory management section on the
    * FreeRTOS web site for more details.  NOTE: This demo uses static allocation
    * for the idle and timer tasks so this line should never execute. */
    while(1)
    {
    }

    return EXIT_FAILURE;
}

/**
 * Dummy implementation of the callback function vApplicationStackOverflowHook().
 */
#if ( configCHECK_FOR_STACK_OVERFLOW > 0 )
__WEAK void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                            char * pcTaskName )
{
    ( void ) xTask;
    ( void ) pcTaskName;

    /* Assert when stack overflow is enabled but no application defined function exists */
    configASSERT( 0 );
}
#endif

/*---------------------------------------------------------------------------*/
#if ( configSUPPORT_STATIC_ALLOCATION == 1 )

/*
 * vApplicationGetIdleTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
 * equals to 1 and is required for static memory allocation support.
 */

__WEAK void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                            StackType_t ** ppxIdleTaskStackBuffer,
                                            uint32_t * pulIdleTaskStackSize )
{
    /* Idle task control block and stack */
    static StaticTask_t Idle_TCB = {0};
    static StackType_t Idle_Stack[ configMINIMAL_STACK_SIZE ] = {0};

    *ppxIdleTaskTCBBuffer = &Idle_TCB;
    *ppxIdleTaskStackBuffer = &Idle_Stack[ 0 ];
    *pulIdleTaskStackSize = ( uint32_t ) configMINIMAL_STACK_SIZE;
}

/*
 * vApplicationGetTimerTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
 * equals to 1 and is required for static memory allocation support.
 */
__WEAK void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                            StackType_t ** ppxTimerTaskStackBuffer,
                                            uint32_t * pulTimerTaskStackSize )
{
    /* Timer task control block and stack */
    static StaticTask_t Timer_TCB = {0};
    static StackType_t Timer_Stack[ configTIMER_TASK_STACK_DEPTH ] = {0};

    *ppxTimerTaskTCBBuffer = &Timer_TCB;
    *ppxTimerTaskStackBuffer = &Timer_Stack[ 0 ];
    *pulTimerTaskStackSize = ( uint32_t ) configTIMER_TASK_STACK_DEPTH;
}
#endif /* if ( configSUPPORT_STATIC_ALLOCATION == 1 ) */
