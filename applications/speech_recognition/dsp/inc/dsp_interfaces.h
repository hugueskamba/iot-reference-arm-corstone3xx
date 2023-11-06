/* Copyright 2022-2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef _DSP_INTERFACE_H_
#define _DSP_INTERFACE_H_

#include "semphr.h"

extern void set_audio_timestamp( float timestamp );
extern float get_audio_timestamp();

/* Communication between DspAudioSource and ISR */
struct DspAudioSource
{
public:
    DspAudioSource( const int16_t * audiobuffer,
                    size_t block_count );

    const int16_t * getCurrentBuffer();

    #ifdef AUDIO_VSI
        void waitForNewBuffer();

        static void new_audio_block_received( void * ptr );
    #endif

private:
    size_t block_count;
    #ifdef AUDIO_VSI
        size_t block_under_write = 0;
    #endif
    size_t current_block = 0;
    const int16_t * audiobuffer;
    SemaphoreHandle_t semaphore = xSemaphoreCreateBinary();
};

class DSPML {
public:
    DSPML( size_t bufferLengthInSamples );
    ~DSPML();

    void copyToDSPBufferFrom( int16_t * buf );
    void copyFromMLBufferInto( int16_t * buf );

    void swapBuffersAndWakeUpMLThread();
    void waitForDSPData();
    size_t getNbSamples()
    {
        return nbSamples;
    }

private:
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    SemaphoreHandle_t semaphore = xSemaphoreCreateBinary();
    int16_t * bufferA, * bufferB, * dspBuffer, * mlBuffer;
    size_t nbSamples;
};

#endif /* ifndef _DSP_INTERFACE_H_ */
