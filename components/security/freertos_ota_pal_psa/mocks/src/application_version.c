/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "version/application_version.h"
#include "fff.h"

AppVersion32_t appFirmwareVersion;

DEFINE_FAKE_VALUE_FUNC( int, GetImageVersionPSA, psa_fwu_component_t );
