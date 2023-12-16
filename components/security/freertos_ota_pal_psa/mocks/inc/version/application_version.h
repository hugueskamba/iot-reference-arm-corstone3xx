/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef APPLICATION_VERSION_H_
    #define APPLICATION_VERSION_H_

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdint.h>

    #include "fff.h"

    #include "psa/update.h"

    typedef struct
    {
        /* MISRA Ref 19.2.1 [Unions] */
        /* More details at: https://github.com/aws/ota-for-aws-iot-embedded-sdk/blob/main/MISRA.md#rule-192 */
        /* coverity[misra_c_2012_rule_19_2_violation] */
        union
        {
            #if ( defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && ( __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) ) || ( __little_endian__ == 1 ) || WIN32 || ( __BYTE_ORDER == __LITTLE_ENDIAN )
                struct version
                {
                    uint16_t build; /*!< @brief Build of the firmware (Z in firmware version Z.Y.X). */
                    uint8_t minor;  /*!< @brief Minor version number of the firmware (Y in firmware version Z.Y.X). */

                    uint8_t major;  /*!< @brief Major version number of the firmware (X in firmware version Z.Y.X). */
                } x;                /*!< @brief Version number of the firmware. */
            #elif ( defined( __BYTE_ORDER__ ) && defined( __ORDER_BIG_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ ) || ( __big_endian__ == 1 ) || ( __BYTE_ORDER == __BIG_ENDIAN )
                struct version
                {
                    uint8_t major;  /*!< @brief Major version number of the firmware (X in firmware version X.Y.Z). */
                    uint8_t minor;  /*!< @brief Minor version number of the firmware (Y in firmware version X.Y.Z). */

                    uint16_t build; /*!< @brief Build of the firmware (Z in firmware version X.Y.Z). */
                } x;                /*!< @brief Version number of the firmware. */
            #else /* if ( defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) || ( __little_endian__ == 1 ) || WIN32 || ( __BYTE_ORDER == __LITTLE_ENDIAN ) */
            #error "Unable to determine byte order!"
            #endif /* if ( defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) || ( __little_endian__ == 1 ) || WIN32 || ( __BYTE_ORDER == __LITTLE_ENDIAN ) */
            uint32_t unsignedVersion32;
            int32_t signedVersion32;
        } u; /*!< @brief Version based on configuration in big endian or little endian. */
    } AppVersion32_t;

    extern AppVersion32_t appFirmwareVersion;

    DECLARE_FAKE_VALUE_FUNC( int, GetImageVersionPSA, psa_fwu_component_t );

    #ifdef __cplusplus
    }
    #endif

#endif // APPLICATION_VERSION_H_
