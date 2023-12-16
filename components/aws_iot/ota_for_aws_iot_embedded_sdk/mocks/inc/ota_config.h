/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef OTA_CONFIG_H_
    #define OTA_CONFIG_H_

    #ifdef __cplusplus
    extern "C" {
    #endif

    #define otaconfigLOG2_FILE_BLOCK_SIZE        12UL
    #define otaconfigMAX_BLOCK_BITMAP_SIZE       128U

    #define otaconfigMAX_NUM_BLOCKS_REQUEST      1U

    #define otaconfigMAX_NUM_OTA_DATA_BUFFERS    otaconfigMAX_NUM_BLOCKS_REQUEST + 1

    #ifdef __cplusplus
    }
    #endif

#endif /* ifndef OTA_CONFIG_H_ */
