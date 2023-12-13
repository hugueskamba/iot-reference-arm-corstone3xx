/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef PORTMACRO_H
    #define PORTMACRO_H

    #ifdef __cplusplus
    extern "C" {
    #endif


    typedef long            BaseType_t;
    typedef unsigned long   TickType_t;

    #ifndef pdMS_TO_TICKS
        #define pdMS_TO_TICKS( xTimeInMs )    ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000U ) )
    #endif

    #define pdFALSE    ( ( BaseType_t ) 0 )
    #define pdTRUE     ( ( BaseType_t ) 1 )

    #define pdPASS     ( pdTRUE )
    #define pdFAIL     ( pdFALSE )


    #ifdef __cplusplus
    }
    #endif

#endif /* ifndef PORTMACRO_H */
