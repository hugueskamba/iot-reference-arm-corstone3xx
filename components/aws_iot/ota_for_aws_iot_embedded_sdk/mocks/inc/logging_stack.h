/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef LOGGING_STACK_H_
    #define LOGGING_STACK_H_

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include "fff.h"

    DECLARE_FAKE_VOID_FUNC_VARARG( LogError, const char *, ... );
    DECLARE_FAKE_VOID_FUNC_VARARG( LogWarn, const char *, ... );
    DECLARE_FAKE_VOID_FUNC_VARARG( LogInfo, const char *, ... );
    DECLARE_FAKE_VOID_FUNC_VARARG( LogDebug, const char *, ... );


    #ifdef __cplusplus
    }
    #endif

#endif // LOGGING_STACK_H_
