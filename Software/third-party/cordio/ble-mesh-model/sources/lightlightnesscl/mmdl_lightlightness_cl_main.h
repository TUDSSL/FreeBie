/*************************************************************************************************/
/*!
 *  \file   mmdl_lightlightness_cl_main.h
 *
 *  \brief  Internal interface of the Light Lightness Client model.
 *
 *  Copyright (c) 2010-2019 Arm Ltd. All Rights Reserved.
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
 *
 */
/*************************************************************************************************/

#ifndef MMDL_LIGHT_LIGHTNESS_CL_MAIN_H
#define MMDL_LIGHT_LIGHTNESS_CL_MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

void mmdlLightLightnessClHandleStatus(const meshModelMsgRecvEvt_t *pMsg);
void mmdlLightLightnessLinearClHandleStatus(const meshModelMsgRecvEvt_t *pMsg);
void mmdlLightLightnessLastClHandleStatus(const meshModelMsgRecvEvt_t *pMsg);
void mmdlLightLightnessDefaultClHandleStatus(const meshModelMsgRecvEvt_t *pMsg);
void mmdlLightLightnessRangeClHandleStatus(const meshModelMsgRecvEvt_t *pMsg);

#ifdef __cplusplus
}
#endif

#endif /* MMDL_LIGHT_LIGHTNESS_CL_MAIN_H */

/*! **********************************************************************************************
 * @}
 ************************************************************************************************ */
