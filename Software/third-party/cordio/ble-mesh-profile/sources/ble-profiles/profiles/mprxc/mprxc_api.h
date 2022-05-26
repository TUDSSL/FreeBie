/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Mesh Proxy Service client.
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
#ifndef MPRXC_API_H
#define MPRXC_API_H

#include "att_api.h"
#include "mesh_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup MESH_PROXY_PROFILE Mesh Proxy Profile API
 *  \{ */

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Mesh Proxy service enumeration of handle indexes of characteristics to be discovered */
enum
{
  MPRXC_MPRXS_DIN_HDL_IDX,        /*!< Data In */
  MPRXC_MPRXS_DOUT_HDL_IDX,       /*!< Data Out  */
  MPRXC_MPRXS_DOUT_CCC_HDL_IDX,   /*!< Data Out CCC descriptor */
  MPRXC_MPRXS_HDL_LIST_LEN        /*!< Handle list length */
};

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief  Perform service and characteristic discovery for Mesh Proxy service.
 *          Parameter pHdlList must point to an array of length MPRXC_MPRXS_HDL_LIST_LEN.
 *          If discovery is successful the handles of discovered characteristics and
 *          descriptors will be set in pHdlList.
 *
 *  \param  connId    Connection identifier.
 *  \param  pHdlList  Characteristic handle list.
 *
 *  \return None.
 */
/*************************************************************************************************/
void MprxcMprxsDiscover(dmConnId_t connId, uint16_t *pHdlList);


/*************************************************************************************************/
/*!
 *  \brief  Set the handle used by the application for interacting with the Mesh
 *          Proxy service Data In characteristic.
 *
 *  \param  connId         Connection ID.
 *  \param  dataInHandle   Data In handled on the server discovered by the client.
 *  \param  dataOutHandle  Data Out handled on the server discovered by the client.
 *
 *  \return None.
 */
/*************************************************************************************************/
void MprxcSetHandles(dmConnId_t connId, uint16_t dataInHandle, uint16_t dataOutHandle);

/*************************************************************************************************/
/*!
 *  \brief  Send data to the Mesh Proxy Server.
 *
 *  \param[in]  pEvt  GATT Proxy PDU send event.
 *
 *  \return None.
 */
/*************************************************************************************************/
void MprxcSendDataIn(meshGattProxyPduSendEvt_t *pEvt);

/*************************************************************************************************/
/*!
 *  \brief  This function is called by the application when a message that requires
 *          processing by the Mesh Proxy client is received.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void MprxcProcMsg(wsfMsgHdr_t *pMsg);

/*! \} */    /* MESH_PROXY_PROFILE */

#ifdef __cplusplus
};
#endif

#endif /* MPRXC_API_H */
