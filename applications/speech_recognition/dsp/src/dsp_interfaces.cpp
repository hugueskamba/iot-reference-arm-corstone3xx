/* Copyright 2022-2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "FreeRTOS.h"
#include "log_macros.h"
#include "dsp_interfaces.h"
#include "audio_config.h"
#include "model_config.h"
#include "task.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" {
/* Include header that defines log levels. */
#include "logging_levels.h"

/* Configure name and log level. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME     "DSP"
#endif
#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif
#include "logging_stack.h"
}

static float audio_timestamp = 0.0;

void set_audio_timestamp(float timestamp) {
    taskENTER_CRITICAL();
    audio_timestamp = timestamp;
    taskEXIT_CRITICAL();
}

float get_audio_timestamp() {
    taskENTER_CRITICAL();
    float timestamp = audio_timestamp;
    taskEXIT_CRITICAL();
    return timestamp;
}

DspAudioSource::DspAudioSource(const int16_t* audiobuffer, size_t block_count ):
        block_count{block_count},
        audiobuffer{audiobuffer}
{

}

const int16_t *DspAudioSource::getCurrentBuffer()
{
#ifndef AUDIO_VSI
    // Update block ID
    current_block = (current_block + 1) % block_count;
#endif

    return(audiobuffer + current_block*(AUDIO_BLOCK_SIZE/2));
}

#ifdef AUDIO_VSI

void DspAudioSource::waitForNewBuffer()
{
    xSemaphoreTake( semaphore, portMAX_DELAY );
}

void DspAudioSource::new_audio_block_received(void* ptr)
{
    auto* self = reinterpret_cast<DspAudioSource*>(ptr);

    // Update block ID
    self->current_block = self->block_under_write;
    self->block_under_write = ((self->block_under_write + 1) % self->block_count);

    // Wakeup task waiting
    xSemaphoreGiveFromISR(semaphore);
};

#endif

static bool dspml_lock(SemaphoreHandle_t ml_fifo_mutex)
{
    if ( ml_fifo_mutex == NULL ) {
        return false;
    }

    if ( xSemaphoreTake( ml_fifo_mutex, portMAX_DELAY ) != pdTRUE ) {
        LogError( ( "Failed to acquire ml_fifo_mutex" ) );
        return false;
    }

    return true;
}

static bool dspml_unlock(SemaphoreHandle_t ml_fifo_mutex)
{
    if ( ml_fifo_mutex == NULL ) {
        return false;
    }

    if ( xSemaphoreGive( ml_fifo_mutex ) != pdTRUE ) {
        LogError( ( "Failed to release ml_fifo_mutex" ) );
        return false;
    }

    return true;
}


DSPML::DSPML(size_t bufferLengthInSamples ):nbSamples(bufferLengthInSamples)
{
    bufferA=static_cast<int16_t*>(malloc(bufferLengthInSamples*sizeof(int16_t)));
    bufferB=static_cast<int16_t*>(malloc(bufferLengthInSamples*sizeof(int16_t)));

    dspBuffer = bufferA;
    mlBuffer = bufferB;
}

DSPML::~DSPML()
{
    free(bufferA);
    free(bufferB);
}

void DSPML::copyToDSPBufferFrom(int16_t * buf)
{
    dspml_lock(mutex);
    memcpy(dspBuffer,buf,sizeof(int16_t)*nbSamples);
    dspml_unlock(mutex);

}

void DSPML::copyFromMLBufferInto(int16_t * buf)
{
    dspml_lock(mutex);
    memcpy(buf,mlBuffer,sizeof(int16_t)*nbSamples);
    dspml_unlock(mutex);
}

void DSPML::swapBuffersAndWakeUpMLThread()
{
    int16_t* tmp;

    dspml_lock(mutex);
    tmp=dspBuffer;
    dspBuffer=mlBuffer;
    mlBuffer=tmp;
    dspml_unlock(mutex);

    BaseType_t yield = pdFALSE;
    if (xSemaphoreGiveFromISR(semaphore, &yield) == pdTRUE)
    {
        portYIELD_FROM_ISR (yield);
    }
}

void DSPML::waitForDSPData()
{
    xSemaphoreTake( semaphore, portMAX_DELAY );
}
