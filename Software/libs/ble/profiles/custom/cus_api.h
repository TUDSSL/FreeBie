/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Heart Rate profile sensor.
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
#ifndef CUS_API_H
#define CUS_API_H

#include "wsf_timer.h"
#include "att_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup CUSTOM_PROFILE
 *  \{ */

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/*! \brief Configurable parameters */
typedef struct
{
  wsfTimerTicks_t     period;     /*!< \brief Measurement timer expiration period in ms */
} customCfg_t;

typedef struct
{
  uint8_t          accel;            /*!< \brief Energy expended value */
  uint8_t           lux;            /*!< \brief Energy expended value */
  uint8_t           flags;                /*!< \brief Heart rate measurement flags */
} appCustom_t;

/*************************************************************************************************/
/*!
 *  \brief  Initialize the Heart Rate profile sensor.
 *
 *  \param  handlerId    WSF handler ID of the application using this service.
 *  \param  pCfg        Configurable parameters.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CustomInit(wsfHandlerId_t handlerId, customCfg_t *pCfg);

/*************************************************************************************************/
/*!
 *  \brief  Start periodic heart rate measurement.  This function starts a timer to perform
 *          periodic measurements.
 *
 *  \param  connId      DM connection identifier.
 *  \param  timerEvt    WSF event designated by the application for the timer.
 *  \param  hrmCccIdx   Index of heart rate CCC descriptor in CCC descriptor handle table.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CustomMeasStart(dmConnId_t connId, uint8_t timerEvt, uint8_t customCccIdx);

/*************************************************************************************************/
/*!
 *  \brief  Stop periodic heart rate measurement.
 *
 *  \param  connId      DM connection identifier.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CustomMeasStop(dmConnId_t connId);

/*************************************************************************************************/
/*!
 *  \brief  Process received WSF message.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CustomProcMsg(wsfMsgHdr_t *pMsg);

#ifdef __cplusplus
};
#endif

#endif /* CUS_API_H */
