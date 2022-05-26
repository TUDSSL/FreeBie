/*************************************************************************************************/
/*!
 *  \file   wsf_timer.h
 *
 *  \brief  Timer service.
 *
 *  Copyright (c) 2009-2019 Arm Ltd. All Rights Reserved.
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
 */
/*************************************************************************************************/
#ifdef CHECKPOINT

#ifndef WSF_APP_TIMER_H
#define WSF_APP_TIMER_H

#include "wsf_os.h"
#include "wsf_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup WSF_APP_TIMER_API
 *  \{ */

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief  Initialize the timer service.  This function should only be called once
 *          upon system initialization.
 */
/*************************************************************************************************/
void WsfAppTimerInit(void);

/*************************************************************************************************/
/*!
 *  \brief  Start a timer in units of seconds.  Before this function is called parameter
 *          pTimer->handlerId must be set to the event handler for this timer and parameter
 *          pTimer->msg must be set to any application-defined timer event parameters.
 *
 *  \param  pTimer  Pointer to timer.
 *  \param  sec     Seconds until expiration.
 */
/*************************************************************************************************/
void WsfAppTimerStartSec(wsfTimer_t *pTimer, wsfTimerTicks_t sec);

/*************************************************************************************************/
/*!
 *  \brief  Start a timer in units of milliseconds.
 *
 *  \param  pTimer  Pointer to timer.
 *  \param  ms      Milliseconds until expiration.
 */
/*************************************************************************************************/
void WsfAppTimerStartMs(wsfTimer_t *pTimer, wsfTimerTicks_t ms);

/*************************************************************************************************/
/*!
 *  \brief  Stop a timer.
 *
 *  \param  pTimer  Pointer to timer.
 */
/*************************************************************************************************/
void WsfAppTimerStop(wsfTimer_t *pTimer);

/*************************************************************************************************/
/*!
 *  \brief  Update the timer service with the number of elapsed ticks.  This function is
 *          typically called only from timer porting code.
 *
 *  \param  ticks  Number of ticks since last update.
 */
/*************************************************************************************************/
void WsfAppTimerUpdate(wsfTimerTicks_t ticks);

/*************************************************************************************************/
/*!
 *  \brief  Service expired timers for the given task.  This function is typically called only
 *          WSF OS porting code.
 *
 *  \param  taskId      OS Task ID of task servicing timers.
 *
 *  \return Pointer to next expired timer or NULL if there are no expired timers.
 */
/*************************************************************************************************/
wsfTimer_t *WsfAppTimerServiceExpired(wsfTaskId_t taskId);

wsfTimerTicks_t wsfAppTimerNextExpiration(void);

void WsfAppTimerSleep(void);

void WsfAppTimerSleepUpdate(void);

/*! \} */    /* WSF_APP_TIMER_API */

#ifdef __cplusplus
};
#endif

#endif /* WSF_APP_TIMER_H */

#endif // ifdef CHECKPOINT
