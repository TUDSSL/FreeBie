/*************************************************************************************************/
/*!
 *  Copyright (c) 2022 Delft University of Technology.
 *  Jasper de Winkel, Haozhe Tang and Przemyslaw Pawelczak
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

#include "wsf_timer.h"

#ifdef CHECKPOINT
#include "checkpoint.h"
#include "checkpoint_content.h"
#endif

typedef struct
{
  wsfTimerTicks_t     period;     /*!< \brief Smartwatch timer expiration period in ms */
} smartwatchCfg_t;

extern wsfTimer_t smartwatchTimer; /*! \brief periodic smartwatch timer */

void SmartwatchInit(wsfHandlerId_t handlerId, smartwatchCfg_t *pCfg);
void SmartwatchStart(uint8_t timerEvt);
void SmartwatchStop();
void SmartwatchProcMsg(wsfMsgHdr_t *pMsg);
