/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "subscription_manager.h"
#include "fff.h"


DEFINE_FAKE_VALUE_FUNC(
    bool,
    addSubscription,
    const char *,
    uint16_t,
    IncomingPubCallback_t,
    void *
    );
DEFINE_FAKE_VOID_FUNC(
    removeSubscription,
    const char *,
    uint16_t
    );
