/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Custom service implementation.
 *
 *  Copyright (c) 2011-2018 Arm Ltd.
 *
 *  Copyright (c) 2019 Packetcraft, Inc. 
 * 
 *  Copyright (c) 2022 Delft University of Technology.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/

#ifndef SVC_CUSTOM_H
#define SVC_CUSTOM_H

#include "att_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup CUSTOM_SERVICE
 *  \{ */
/**************************************************************************************************
  Macros
**************************************************************************************************/
// Custom service UUID
// 3ccfd068-3c90-4447-8867-1a7ef147e251 - created from https://www.uuidgenerator.net/version4
// * Note from Nordic softdevice
// * @note Bytes 12 and 13 of the provided UUID will not be used internally, since those are always replaced by
// * the 16-bit uuid field in @ref ble_uuid_t.
/*! \brief Base UUID:  3ccfXXXX-3c90-4447-8867-1a7ef147e251 */
#define CUSTOM_UUID_PART1                0x51, 0xe2, 0x47, 0xf1, 0x7e, 0x1a, 0x67, 0x88, \
                                      0x47, 0x44, 0x90, 0x3c
/*! \brief Base UUID Part 2 */
#define CUSTOM_UUID_PART2                0xcf, 0x3c

/*! \brief Macro for building UUIDs */
#define CUSTOM_UUID_BUILD(part)          CUSTOM_UUID_PART1, UINT16_TO_BYTES(part), CUSTOM_UUID_PART2

#define CUSTOM_UUID_SERVICE              CUSTOM_UUID_BUILD(0x0001)
#define CUSTOM_UUID_CP                   CUSTOM_UUID_BUILD(0x0002)
#define CUSTOM_UUID_FL                   CUSTOM_UUID_BUILD(0x0003)
#define CUSTOM_UUID_RI                   CUSTOM_UUID_BUILD(0x0004)
#define CUSTOM_UUID_FS                   CUSTOM_UUID_BUILD(0x0005)

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/** \name Heart Rate Service Handles
 *
 */
/**@{*/
#define CUSTOM_START_HDL               0x240                  /*!< \brief Start handle. */
#define CUSTOM_END_HDL                 (CUSTOM_MAX_HDL - 1)   /*!< \brief End handle. */
/**************************************************************************************************
 Handles
**************************************************************************************************/

enum
{
  CUSTOM_SVC_HDL = CUSTOM_START_HDL,   /*!< \brief CUSTOM service declaration */
  CUSTOM_CP_CH_HDL,                    /*!< \brief Control Point characteristic */
  CUSTOM_CP_HDL,                       /*!< \brief Control Point */
  CUSTOM_CP_CH_CCC_HDL,                /*!< \brief Control Point client characteristic configuration */
  CUSTOM_FL_CH_HDL,                    /*!< \brief Firmware Length characteristic */
  CUSTOM_FL_HDL,                       /*!< \brief Firmware Length */
  CUSTOM_RI_CH_HDL,                    /*!< \brief Request Index characteristic */
  CUSTOM_RI_HDL,                       /*!< \brief Request Index */
  CUSTOM_RI_CH_CCC_HDL,                /*!< \brief Request Index client characteristic configuration */
  CUSTOM_FS_CH_HDL,                    /*!< \brief Firmware Section characteristic */
  CUSTOM_FS_HDL,                       /*!< \brief Firmware Section */
  CUSTOM_MAX_HDL                       /*!< \brief Maximum handle. */
};
/**@}*/

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCustomAddGroup(void);

/*************************************************************************************************/
/*!
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCustomRemoveGroup(void);

/*************************************************************************************************/
/*!
 *  \brief  Register callbacks for the service.
 *
 *  \param  readCback   Read callback function.
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCustomCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback);

/*! \} */    /* HEART_RATE_SERVICE */

#ifdef __cplusplus
};
#endif

#endif /* SVC_HRS_H */
