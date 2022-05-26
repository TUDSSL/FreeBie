/*************************************************************************************************/
/*!
 *  \file   mmdl_light_hsl_hue_sr.h
 *
 *  \brief  Interface of the Light HSL Hue Server model.
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

#ifndef MMDL_LIGHT_HSL_HUE_SR_H
#define MMDL_LIGHT_HSL_HUE_SR_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief     Set the local state. The set is instantaneous.
 *
 *  \param[in] elementId  Identifier of the Element implementing the model.
 *  \param[in] hue        New hue value.
 *
 *  \return    None.
 */
/*************************************************************************************************/
void MmdlLightHslHueSrSetHue(meshElementId_t elementId, uint16_t hue);

/*************************************************************************************************/
/*!
 *  \brief     Set the local hue state. The set is instantaneous.
 *
 *  \param[in] elementId   Identifier of the Element implementing the model.
 *  \param[in] presentHue  New present hue value.
 *  \param[in] targetHue   New target hue value.
 *
 *  \return    None.
 */
/*************************************************************************************************/
void MmdlLightHslHueSrSetBoundState(meshElementId_t elementId, uint16_t presentHue,
                                    uint16_t targetHue);

#ifdef __cplusplus
}
#endif

#endif /* MMDL_LIGHT_HSL_HUE_SR_H */
