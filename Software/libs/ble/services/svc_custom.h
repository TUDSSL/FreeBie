/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Example Heart Rate service implementation.
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

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup CUSTOM_SERVICE
 *  \{ */

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/** \name Heart Rate Service Handles
 *
 */
/**@{*/
#define CUSTOM_START_HDL               600                    /*!< \brief Start handle. */
#define CUSTOM_END_HDL                 (CUSTOM_MAX_HDL - 1)   /*!< \brief End handle. */
/**************************************************************************************************
 Handles
**************************************************************************************************/

enum
{
  CUSTOM_SVC_HDL = CUSTOM_START_HDL,    /*!< \brief CUSTOM service declaration */
  CUSTOM_DAT_ACCEL_CH_HDL,                    /*!< \brief CUSTOM data characteristic */
  CUSTOM_DAT_ACCEL_HDL,                       /*!< \brief CUSTOM data */
  CUSTOM_DAT_ACCEL_CH_CCC_HDL,                /*!< \brief CUSTOM data client characteristic configuration */
  CUSTOM_DAT_LUX_CH_HDL,                    /*!< \brief CUSTOM data characteristic */
  CUSTOM_DAT_LUX_HDL,                       /*!< \brief CUSTOM data */
  CUSTOM_DAT_LUX_CH_CCC_HDL,                /*!< \brief CUSTOM data client characteristic configuration */
  CUSTOM_MAX_HDL                        /*!< \brief Maximum handle. */
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
