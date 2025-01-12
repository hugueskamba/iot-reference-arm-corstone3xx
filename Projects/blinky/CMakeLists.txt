# Copyright 2023 Arm Limited and/or its affiliates
# <open-source-office@arm.com>
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.21)

# Start of the project
project(blinky-example LANGUAGES C CXX)

# Declare the target of the total solution
set(ARM_CORSTONE_BSP_TARGET_PLATFORM "" CACHE STRING "Featured Reference Integration target")

# Configure target
if (${ARM_CORSTONE_BSP_TARGET_PLATFORM} STREQUAL "corstone300")
    set(TFM_PLATFORM_LOCAL_PATH "arm/mps3/corstone300/fvp")
    set( FREERTOS_PORT "GCC_ARM_CM55_TFM" CACHE STRING "" FORCE)
elseif(${ARM_CORSTONE_BSP_TARGET_PLATFORM} STREQUAL "corstone310")
    set(TFM_PLATFORM_LOCAL_PATH "arm/mps3/corstone310/fvp")
    set( FREERTOS_PORT "GCC_ARM_CM85_TFM" CACHE STRING "" FORCE)
else()
    message(FATAL_ERROR "Invalid ARM_CORSTONE_BSP_TARGET_PLATFORM (${ARM_CORSTONE_BSP_TARGET_PLATFORM}) set. Supported are corstone300/corstone310")
endif()

# Extra arguments for TF-M and ML
set(TFM_CMAKE_ARGS
    -DPROJECT_CONFIG_HEADER_FILE=${PRJ_DIR}/Config/tfm-config/project_config.h
    -DPLATFORM_DEFAULT_PROVISIONING=OFF
    -DTFM_DUMMY_PROVISIONING=OFF
    -DCONFIG_TFM_ENABLE_CP10CP11=ON
    -DTFM_EXCEPTION_INFO_DUMP=ON
    -DNS=ON;
    -DPLATFORM_DEFAULT_UART_STDOUT=ON;
    -DMCUBOOT_SIGNATURE_KEY_LEN=3072
    -DMCUBOOT_LOG_LEVEL=INFO
    -DTFM_SPM_LOG_LEVEL=TFM_SPM_LOG_LEVEL_INFO
    -DTFM_PARTITION_CRYPTO=ON
    -DTFM_PARTITION_INITIAL_ATTESTATION=ON
    -DTFM_PARTITION_INTERNAL_TRUSTED_STORAGE=ON
    -DTFM_PARTITION_PLATFORM=ON
    -DTFM_PARTITION_PROTECTED_STORAGE=ON
    -DTFM_PLATFORM=${TFM_PLATFORM_LOCAL_PATH}
)

# Configuration of executable
set(EXE_SUFFIX ".axf")
set(CMAKE_EXECUTABLE_SUFFIX ${EXE_SUFFIX})

# Set global optimization level to reduce code size while keeping the debug experience.
# ARMClang does not have -Og but officially recommends -O1 for debug.
if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_compile_options(-Og)
else()
    add_compile_options(-O1)
endif()

add_subdirectory(../../Middleware/ARM ${CMAKE_BINARY_DIR}/Middleware/ARM)
add_subdirectory(../../Bsp ${CMAKE_BINARY_DIR}/Bsp)
add_subdirectory(../../Config ${CMAKE_BINARY_DIR}/Config)

target_compile_definitions(arm-corstone-platform-bsp
    INTERFACE
        __DOMAIN_NS=1
)

# Compilation Database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM
    INTERFACE
        ${PRJ_DIR}/Config/freertos-config
)

target_compile_definitions(freertos_config
  INTERFACE
    projCOVERAGE_TEST=0
)

target_link_libraries(freertos_config
    INTERFACE
        tfm-ns-interface
        app-config
        fri-bsp
)

set( FREERTOS_HEAP "3" CACHE STRING "" FORCE)

add_subdirectory(../../Middleware/FreeRTOS ${CMAKE_BINARY_DIR}/Middleware/FreeRTOS)

# Declare the blinky executable
add_executable(blinky main.c)

target_link_libraries(blinky
    freertos_kernel
    tfm-ns-interface
    fri-bsp
)

include(${PRJ_DIR}/Bsp/cmake/BspUtilities.cmake)
set_linker_script(blinky)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Middleware/ARM/TF-M/cmake)
include(SignTfmImage)
include(MergeTfmImages)

# The non-secure application image should be padded while being signed
# Hence, passing "TRUE" as the input parameter to the pad option of sign function.
iot_reference_arm_corstone3xx_tf_m_sign_image(blinky blinky_signed 0.0.1 TRUE)

# A user project that consumes the ARM FRI needs to explicitly provide
# addresses in order to merge images for TF-M. The addresses cannot
# be easily programmatically extracted as they are defined in the linker
# scripts.
iot_reference_arm_corstone3xx_tf_m_merge_images(blinky ${BL2_IMAGE_LOAD_ADDRESS} ${S_IMAGE_LOAD_ADDRESS} ${NS_IMAGE_LOAD_ADDRESS} ${S_PROVISIONING_BUNDLE_LOAD_ADDRESS})
