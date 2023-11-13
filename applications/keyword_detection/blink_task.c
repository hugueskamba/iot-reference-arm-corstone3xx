/* Copyright 2021-2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "FreeRTOS.h"
#include "queue.h"

#include "blink_task.h"
#include "log_macros.h"
#include "ml_interface.h"
#include "mps3_leds.h"
#include <stdbool.h>
#include <stdio.h>

/* Include header that defines log levels. */
#include "logging_levels.h"

/* Configure name and log level. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME     "Blink"
#endif
#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif
#include "logging_stack.h"

#define BLINK_TIMER_PERIOD_MS    250

enum
{
    LED1 = 1 << 0,
    LED_YES = LED1,
    LED2 = 1 << 1,
    LED_GO = LED2,
    LED3 = 1 << 2,
    LED_UP = LED3,
    LED4 = 1 << 3,
    LED_LEFT = LED4,
    LED5 = 1 << 4,
    LED_ON = LED5,
    LED6 = 1 << 5,
    LED_ALIVE = LED6,
    LED7 = 1 << 6,
    LED8 = 1 << 7,
    LED9 = 1 << 8,
    LED10 = 1 << 9,
    LED_ALL = 0xFF
};

/* Message queue */
static QueueHandle_t xUiMsgQueue = NULL;

/*
 * @brief Create blink task.
 */
void vStartBlinkTask( void )
{
    if( xTaskCreate( blink_task,
                     "BLINK_TASK ",
                     appCONFIG_BLINK_TASK_STACK_SIZE,
                     NULL,
                     appCONFIG_BLINK_TASK_PRIORITY,
                     NULL ) != pdPASS )
    {
        LogError( ( "Failed to create Blink Task\r\n" ) );
    }
}

static void ml_change_handler( void * self,
                               ml_processing_state_t new_state )
{
    ( void ) self;

    if( xUiMsgQueue == NULL )
    {
        LogError( ( "xUiMsgQueue is not initialised\r\n" ) );
        return;
    }

    if( xQueueSendToBack( xUiMsgQueue, ( void * ) &new_state, 0 ) != pdTRUE )
    {
        LogError( ( "Failed to send ml_state_change_event to ui queue\r\n" ) );
    }
}

bool process_ml_state_change( ml_processing_state_t new_state )
{
    switch( new_state )
    {
        case ML_HEARD_YES:
            LogInfo( ( "ML_HEARD_YES\n" ) );

            if( mps3_leds_turn_on( LED_YES ) != true )
            {
                LogError( ( "Failed to turn LED_YES on\r\n" ) );
                return false;
            }

            break;

        case ML_HEARD_NO:
            LogInfo( ( "ML_HEARD_NO\n" ) );

            if( mps3_leds_turn_off( LED_YES ) != true )
            {
                LogError( ( "Failed to turn LED_YES off\r\n" ) );
                return false;
            }

            break;

        case ML_HEARD_GO:
            LogInfo( ( "ML_HEARD_GO\n" ) );

            if( mps3_leds_turn_on( LED_GO ) != true )
            {
                LogError( ( "Failed to turn LED_GO on\r\n" ) );
                return false;
            }

            break;

        case ML_HEARD_STOP:
            LogInfo( ( "ML_HEARD_STOP\n" ) );

            if( mps3_leds_turn_off( LED_GO ) != true )
            {
                LogError( ( "Failed to turn LED_GO off\r\n" ) );
                return false;
            }

            break;

        case ML_HEARD_UP:
            LogInfo( ( "ML_HEARD_UP\n" ) );

            if( mps3_leds_turn_on( LED_UP ) != true )
            {
                LogError( ( "Failed to turn LED_UP on\r\n" ) );
                return false;
            }

            break;

        case ML_HEARD_DOWN:
            LogInfo( ( "ML_HEARD_DOWN\n" ) );

            if( mps3_leds_turn_off( LED_UP ) != true )
            {
                LogError( ( "Failed to turn LED_UP off\r\n" ) );
                return false;
            }

            break;

        case ML_HEARD_LEFT:
            LogInfo( ( "ML_HEARD_LEFT\n" ) );

            if( mps3_leds_turn_on( LED_LEFT ) != true )
            {
                LogError( ( "Failed to turn LED_LEFT on\r\n" ) );
                return false;
            }

            break;

        case ML_HEARD_RIGHT:
            LogInfo( ( "ML_HEARD_RIGHT\n" ) );

            if( mps3_leds_turn_off( LED_LEFT ) != true )
            {
                LogError( ( "Failed to turn LED_LEFT off\r\n" ) );
                return false;
            }

            break;

        case ML_HEARD_ON:
            LogInfo( ( "ML_HEARD_ON\n" ) );

            if( mps3_leds_turn_on( LED_ON ) != true )
            {
                LogError( ( "Failed to turn LED_ON on\r\n" ) );
                return false;
            }

            break;

        case ML_HEARD_OFF:
            LogInfo( ( "ML_HEARD_OFF\n" ) );

            if( mps3_leds_turn_off( LED_ON ) != true )
            {
                LogError( ( "Failed to turn LED_ON off\r\n" ) );
                return false;
            }

            break;

        default:
            LogInfo( ( "ML UNKNOWN\n" ) );
            break;
    }

    return true;
}

/*
 * Main task.
 *
 * Blinks LEDs according to ML processing.
 *
 * LED1 on and LED2 off       => heard YES
 * LED1 off and LED2 off      => heard NO
 * LED1 off and LED2 blinking => no/unknown input
 */
void blink_task( void * arg )
{
    ( void ) arg;

    LogInfo( ( "Blink task started\r\n" ) );

    /* Create the ui event queue */
    xUiMsgQueue = xQueueCreate( 10, sizeof( ml_processing_state_t ) );

    if( xUiMsgQueue == NULL )
    {
        LogError( ( "Failed to create xUiMsgQueue\r\n" ) );
        return;
    }

    /* Connect to the ML processing */
    on_ml_processing_change( ml_change_handler, NULL );

    if( mps3_leds_turn_off( LED_ALL ) != true )
    {
        LogError( ( "Failed to turn All LEDs off\r\n" ) );
        return;
    }

    /* Toggle is-alive LED and process messages at a fixed interval */
    const uint32_t ticks_interval = BLINK_TIMER_PERIOD_MS * configTICK_RATE_HZ / 1000;

    while( 1 )
    {
        if( ticks_interval == 0U )
        {
            return;
        }

        vTaskDelay( ticks_interval );

        if( mps3_leds_toggle( LED_ALIVE ) != true )
        {
            LogError( ( "Failed to toggle LED_ALIVE\r\n" ) );
            return;
        }

        /* Retrieve any/all existing messages with no delay */
        ml_processing_state_t msg;

        while( 1 )
        {
            if( xQueueReceive( xUiMsgQueue, &msg, 0 ) == pdPASS )
            {
                if( process_ml_state_change( msg ) != true )
                {
                    LogError( ( "Failed to process new ML state\r\n" ) );
                    return;
                }
            }
        }

        LogError( ( "xQueueReceive failed\r\n" ) );
    }
}
