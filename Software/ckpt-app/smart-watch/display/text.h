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
#ifndef LIBS_DISPLAY_TEXT_H_
#define LIBS_DISPLAY_TEXT_H_

#include "platform.h"
#include <time.h>

void updateTimeText(uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint16_t year);
void updateMessagesText(uint8_t messages);

#endif /* LIBS_DISPLAY_TEXT_H_ */
