# Copyright 2021-2023 Arm Limited and/or its affiliates
# <open-source-office@arm.com>
# SPDX-License-Identifier: MIT

set(ml_embedded_evaluation_kit_SOURCE_DIR
    ${CMAKE_CURRENT_LIST_DIR}/library
    CACHE INTERNAL
    "Path to ML Embedded Evaluation Kit source code"
)

execute_process(COMMAND git am --abort
    COMMAND git apply ${CMAKE_CURRENT_LIST_DIR}/integration/patches/ml-embedded-evaluation-kit.patch
    WORKING_DIRECTORY ${ml_embedded_evaluation_kit_SOURCE_DIR}
    OUTPUT_QUIET
    ERROR_QUIET
)

add_subdirectory(integration)
