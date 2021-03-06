/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Blood Pressure profile client.
 *
 *  Copyright (c) 2012-2018 Arm Ltd. All Rights Reserved.
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
#ifndef BLPC_API_H
#define BLPC_API_H

#include "att_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup BLOOD_PRESSURE_PROFILE
 *  \{ */

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! \brief Blood Pressure service enumeration of handle indexes of characteristics to be discovered */
enum
{
  BLPC_BPS_BPM_HDL_IDX,           /*!< \brief Blood pressure measurement */
  BLPC_BPS_BPM_CCC_HDL_IDX,       /*!< \brief Blood pressure measurement CCC descriptor */
  BLPC_BPS_ICP_HDL_IDX,           /*!< \brief Intermediate cuff pressure */
  BLPC_BPS_ICP_CCC_HDL_IDX,       /*!< \brief Intermediate cuff pressure CCC descriptor */
  BLPC_BPS_BPF_HDL_IDX,           /*!< \brief Blood pressure feature */
  BLPC_BPS_HDL_LIST_LEN           /*!< \brief Handle list length */
};

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief  Perform service and characteristic discovery for Blood Pressure service.
 *          Parameter pHdlList must point to an array of length \ref BLPC_BPS_HDL_LIST_LEN.
 *          If discovery is successful the handles of discovered characteristics and
 *          descriptors will be set in pHdlList.
 *
 *  \param  connId    Connection identifier.
 *  \param  pHdlList  Characteristic handle list.
 *
 *  \return None.
 */
/*************************************************************************************************/
void BlpcBpsDiscover(dmConnId_t connId, uint16_t *pHdlList);

/*************************************************************************************************/
/*!
 *  \brief  Process a value received in an ATT read response, notification, or indication
 *          message.  Parameter pHdlList must point to an array of length \ref BLPC_BPS_HDL_LIST_LEN.
 *          If the ATT handle of the message matches a handle in the handle list the value
 *          is processed, otherwise it is ignored.
 *
 *  \param  pHdlList  Characteristic handle list.
 *  \param  pMsg      ATT callback message.
 *
 *  \return \ref ATT_SUCCESS if handle is found, \ref ATT_ERR_NOT_FOUND otherwise.
 */
/*************************************************************************************************/
uint8_t BlpcBpsValueUpdate(uint16_t *pHdlList, attEvt_t *pMsg);

/*! \} */    /* BLOOD_PRESSURE_PROFILE */

#ifdef __cplusplus
};
#endif

#endif /* BLPC_API_H */
