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

#ifndef LIBS_DEBUG_DEBUG_H_
#define LIBS_DEBUG_DEBUG_H_

#if (DEBUG_PRINTF == 1)
#include "SEGGER_RTT.h"
#define DEBUG_PRINTF(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

#endif /* LIBS_DEBUG_DEBUG_H_ */
