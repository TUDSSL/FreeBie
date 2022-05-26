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

#ifndef LIBS_BF_SYNC_BF_SYNC_H_
#define LIBS_BF_SYNC_BF_SYNC_H_

#include <stdint.h>
#include <stdbool.h>
#include "platform.h"

// We need to align to 250ms intervals.
// This is the first clean division of 32.768 Hz crystals.

// Hence if we want to resume at T_x this is always a multiple of 250ms.
// We need to wake up accounting for the restore time T_rs
// To sync the clocks at moment T_x we introduce some margin T_m
// Resulting in
//
// T_wakeup = T_x - T_rs - T_m
// T_sync = T_x

#define T_Restore 10  // in ms / 10ms intervals
#define T_Margin 10   // in ms / 10ms intervals
#define T_Sync_Interval_Ms 250 // in ms, 250 * 2^x, i.e. multiples of 250

extern uint32_t extRtcTimeMs;

void bfSyncConfig();
void bfSyncPowerDown();
bool bfSyncWakeUpScheduleAbsolute(uint32_t nextEventRtcTick, uint32_t currentRtcTick, uint8_t turnOff);
void bfSyncConfigPowerUpSync();
void bfSyncSetupSync();
#ifdef DEBUG_SYSTEMOFF
void bfSyncDebugSystemoff();
#endif
#endif /* LIBS_BF_SYNC_BF_SYNC_H_ */
