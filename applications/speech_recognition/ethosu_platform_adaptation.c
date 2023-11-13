/* Copyright 2021-2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"

/****************************************************************************
* Mutex & Semaphore
* Overrides weak-linked symbols in ethosu_driver.c to implement thread handling
****************************************************************************/

void * ethosu_mutex_create( void )
{
    SemaphoreHandle_t xEthosMutex = xSemaphoreCreateMutex();

    if( xEthosMutex == NULL )
    {
        printf( "Failed to create xEthosMutex\r\n" );
    }

    return xEthosMutex;
}

void ethosu_mutex_lock( void * mutex )
{
    if( mutex != NULL )
    {
        /* SemaphoreHandle_t mutex_id = (SemaphoreHandle_t)mutex; */
        BaseType_t ret = xSemaphoreTake( ( SemaphoreHandle_t ) mutex, portMAX_DELAY );

        if( ret != pdTRUE )
        {
            printf( "xSemaphoreTake Ethos Mutex failed %ld\r\n", ret );
            return;
        }
    }
}

void ethosu_mutex_unlock( void * mutex )
{
    if( mutex != NULL )
    {
        BaseType_t status = xSemaphoreGive( ( SemaphoreHandle_t ) mutex );

        if( status != pdPASS )
        {
            printf( "xSemaphoreGive Ethos Mutex failed %ld\r\n", status );
            return;
        }
    }
}

void * ethosu_semaphore_create( void )
{
    SemaphoreHandle_t xEthosSemaphore = xSemaphoreCreateBinary();

    if( xEthosSemaphore == NULL )
    {
        printf( "Failed to create xEthosSemaphore\r\n" );
    }

    return xEthosSemaphore;
}

int ethosu_semaphore_take( void * sem )
{
    if( sem != NULL )
    {
        BaseType_t ret = xSemaphoreTake( ( SemaphoreHandle_t ) sem, portMAX_DELAY );

        if( ret != pdTRUE )
        {
            printf( "xSemaphoreTake Ethos Semaphore failed %ld\r\n", ret );
            return -1;
        }
    }

    return 0;
}

int ethosu_semaphore_give( void * sem )
{
    if( sem != NULL )
    {
        BaseType_t ret = xSemaphoreGive( ( SemaphoreHandle_t ) sem );

        if( ret != pdPASS )
        {
            printf( "xSemaphoreGive Ethos Semaphore failed %ld\r\n", ret );
            return -1;
        }
    }

    return 0;
}
