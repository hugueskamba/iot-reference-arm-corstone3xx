# Copyright 2023 Arm Limited and/or its affiliates
# <open-source-office@arm.com>
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

project(iot-reference-arm-corstone3xx LANGUAGES C CXX ASM)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/tools/cmake)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    include(AddUnitTest)
endif()


if(CMAKE_CROSSCOMPILING)
    add_subdirectory(bsp)
    add_subdirectory(components EXCLUDE_FROM_ALL)
else()
    add_subdirectory(components)
endif()
