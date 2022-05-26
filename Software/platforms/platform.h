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

#ifndef PLATFORMS_PLATFORM_H_
#define PLATFORMS_PLATFORM_H_

#include "nrf52840_nrfx_config.h"
#include "nrf.h"

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#define CATSTR(A, B) A B

#if HWREV == 1
#include "int-ble-v1-0.h"
#elif HWREV == 2
#include "int-ble-v1-3.h"
#elif HWREV == 3
#include "int-ble-v1-4.h"
#elif HWREV == 4
#include "int-ble-v2-0.h"
#else
#error "Please define a platform the hardware revision."
#endif

#ifdef CHECKPOINT
#include "checkpoint_cfg.h"
#endif

#ifndef CHECKPOINT_EXCLUDE_DATA
#define CHECKPOINT_EXCLUDE_DATA
#endif

#ifndef CHECKPOINT_EXCLUDE_BSS
#define CHECKPOINT_EXCLUDE_BSS
#endif

#endif /* PLATFORMS_PLATFORM_H_ */
