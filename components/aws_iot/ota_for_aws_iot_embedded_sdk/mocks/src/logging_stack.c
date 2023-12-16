/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "logging_stack.h"

#include "fff.h"

DEFINE_FAKE_VOID_FUNC_VARARG( LogError, const char *, ... );
DEFINE_FAKE_VOID_FUNC_VARARG( LogWarn, const char *, ... );
DEFINE_FAKE_VOID_FUNC_VARARG( LogInfo, const char *, ... );
DEFINE_FAKE_VOID_FUNC_VARARG( LogDebug, const char *, ... );
