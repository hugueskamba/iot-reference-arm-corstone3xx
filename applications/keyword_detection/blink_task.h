/* Copyright 2021-2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef BLINK_TASK_H
    #define BLINK_TASK_H

    #include <stdint.h>

    #ifdef __cplusplus
    extern "C" {
    #endif

/* Blinks LEDs according the ML model state.
 */
    void blink_task( void * arg );

    #ifdef __cplusplus
    }
    #endif

#endif /* ! BLINK_TASK_H*/
