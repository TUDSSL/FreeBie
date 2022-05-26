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

#ifndef LIBS_FRAM_FRAM_H_
#define LIBS_FRAM_FRAM_H_

#include "platform.h"

#define JIT_RO_MEMORY_SPACE 0x12000000

#define FRAM_SUCCESS 0
#define FRAM_ERROR 1

// spi commands
#define FRAM_SET_WRITE_EN_LATCH 0x06
#define FRAM_RESET_WRITE_EN_LATCH 0x04
#define FRAM_READ_STATUS 0x05
#define FRAM_WRITE_STATUS 0x01

#define FRAM_READ_MEM_CODE 0x03
#define FRAM_WRITE_MEM_CODE 0x02
#define FRAM_READ_DEV_ID 0x9F
#define FRAM_FAST_READ 0x0B
#define FRAM_SLEEP_MODE 0xB9

#define FRAM_REF_ID 0x03497F04 // ID to be expected when reading dev id.

void framSetup();
void framConfig();

// address must be 32 bits aligned!
void framWrite(const void* address, const void* buffer, uint16_t size);
void framRead(const void* address, void* buffer, uint16_t size);
void framDeinit();

#endif /* LIBS_FRAM_FRAM_H_ */
