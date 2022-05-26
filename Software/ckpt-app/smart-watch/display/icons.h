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

#ifndef LIBS_DISPLAY_ICONS_H_
#define LIBS_DISPLAY_ICONS_H_

#include "platform.h"

typedef struct {
  uint8_t size_x; // must be multiples of 8
  uint8_t size_y;
  uint8_t pos_x; // must be multiples of 8
  uint8_t pos_y;
  const unsigned char* data;
} Icon;

extern const Icon mailIcon20;

extern const Icon barIcon_0_5;
extern const Icon barIcon_1_5;
extern const Icon barIcon_2_5;
extern const Icon barIcon_3_5;
extern const Icon barIcon_4_5;
extern const Icon barIcon_5_5;

extern const Icon ndsiIcon;
extern const Icon bleIcon;
extern const Icon mobisysIcon;

void writeIcon(const Icon* icon);
void clearIcon(const Icon* icon);

#endif /* LIBS_DISPLAY_ICONS_H_ */
