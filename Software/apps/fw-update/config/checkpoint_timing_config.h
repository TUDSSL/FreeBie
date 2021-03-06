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

#ifndef CONFIG_CHECKPOINT_CHECKPOINT_TIMING_CONFIG_H_
#define CONFIG_CHECKPOINT_CHECKPOINT_TIMING_CONFIG_H_

#include "ext-rtc.h"

typedef struct {
  AlarmConfiguration ExtRtcSyncConfig;
  uint32_t ExtRtcStartTimeMs;
} ExternalSyncAlarm;

struct TimerCP {
  uint32_t IntRtcCounter;
  uint32_t IntRtcCc[4];
  uint32_t IntRtcInterruptMask;
  ExternalSyncAlarm ExtRtcSyncAlarm;
};

#endif /* CONFIG_CHECKPOINT_CHECKPOINT_TIMING_CONFIG_H_ */
