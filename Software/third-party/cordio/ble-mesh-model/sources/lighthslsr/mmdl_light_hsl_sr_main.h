/*************************************************************************************************/
/*!
 *  \file   mmdl_light_hsl_sr_main.h
 *
 *  \brief  Interface of the Light HSL Server model.
 *
 *  Copyright (c) 2010-2018 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019-2020 Packetcraft, Inc. 
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
 *
 */
/*************************************************************************************************/

#ifndef MMDL_LIGHT_HSL_SR_MAIN_H
#define MMDL_LIGHT_HSL_SR_MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Timeout for filtering duplicate messages from same source */
#define MSG_RCVD_TIMEOUT_MS                 6000

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void mmdlLightHslSrHandleGet(const meshModelMsgRecvEvt_t *pMsg);

void mmdlLightHslSrHandleTargetGet(const meshModelMsgRecvEvt_t *pMsg);

void mmdlLightHslSrHandleSetNoAck(const meshModelMsgRecvEvt_t *pMsg);

void mmdlLightHslSrHandleSet(const meshModelMsgRecvEvt_t *pMsg);

void mmdlLightHslSrHandleRangeGet(const meshModelMsgRecvEvt_t *pMsg);

void mmdlLightHslSrHandleDefaultGet(const meshModelMsgRecvEvt_t *pMsg);

bool_t mmdlLightHslSrProcessRangeSet(const meshModelMsgRecvEvt_t *pMsg,
                                                 uint8_t *pOutOpStatus);

void mmdlLightHslSrSendRangeStatus(meshElementId_t elementId, meshAddress_t dstAddr,
                                  uint16_t appKeyIndex, bool_t recvOnUnicast, uint8_t opStatus);

void mmdlLightHslSrSendDefaultStatus(meshElementId_t elementId, meshAddress_t dstAddr,
                                    uint16_t appKeyIndex, bool_t recvOnUnicast);

#ifdef __cplusplus
}
#endif

#endif /* MMDL_LIGHT_HSL_SR_MAIN_H */
