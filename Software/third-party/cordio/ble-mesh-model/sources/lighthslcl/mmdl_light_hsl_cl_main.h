/*************************************************************************************************/
/*!
 *  \file   mmdl_light_hsl_cl_main.h
 *
 *  \brief  Internal interface of the HSL Client model.
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

#ifndef MMDL_LIGHT_HSL_CL_MAIN_H
#define MMDL_LIGHT_HSL_CL_MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*!  Timeout for filtering duplicate messages from same source */
#define MSG_RCVD_TIMEOUT_MS                 6000

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void mmdlLightHslClHandleStatus(const meshModelMsgRecvEvt_t *pMsg);

void mmdlLightHslClHandleTargetStatus(const meshModelMsgRecvEvt_t *pMsg);

void mmdlLightHslClHandleHueStatus(const meshModelMsgRecvEvt_t *pMsg);

void mmdlLightHslClHandleSatStatus(const meshModelMsgRecvEvt_t *pMsg);

void mmdlLightHslClHandleDefStatus(const meshModelMsgRecvEvt_t *pMsg);

void mmdlLightHslClHandleRangeStatus(const meshModelMsgRecvEvt_t *pMsg);

#ifdef __cplusplus
}
#endif

#endif /* MMDL_LIGHT_HSL_CL_MAIN_H */
