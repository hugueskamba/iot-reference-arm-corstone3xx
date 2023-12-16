/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef FREERTOS_CONFIG_H
    #define FREERTOS_CONFIG_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdbool.h>

    #include "fff.h"

    DECLARE_FAKE_VOID_FUNC( configASSERT, bool );


    #ifdef __cplusplus
    }
    #endif

#endif // FREERTOS_CONFIG_H
