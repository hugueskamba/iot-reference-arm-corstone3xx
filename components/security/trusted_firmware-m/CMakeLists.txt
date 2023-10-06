# Copyright 2023 Arm Limited and/or its affiliates
# <open-source-office@arm.com>
# SPDX-License-Identifier: MIT

set(trusted_firmware-m_SOURCE_DIR
    ${CMAKE_CURRENT_LIST_DIR}/library
    CACHE INTERNAL
    "Path to Trusted Firmware-M source code"
)

execute_process(COMMAND git am --abort
    COMMAND git am ${CMAKE_CURRENT_SOURCE_DIR}/integration/patches/0001-cs310-tfm-Fix-Ethos-U-git-link.patch
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/trusted-firmware-m"
    OUTPUT_QUIET
    ERROR_QUIET
)

add_subdirectory(integration)
