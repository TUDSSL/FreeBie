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

#include "icons.h"
#include "string.h"
#include "display.h"

extern DisplayBufferBW displayBufferBW;

extern const unsigned char mail20[];

extern const unsigned char bar_0_5[];
extern const unsigned char bar_1_5[];
extern const unsigned char bar_2_5[];
extern const unsigned char bar_3_5[];
extern const unsigned char bar_4_5[];
extern const unsigned char bar_5_5[];

extern const unsigned char nsdi22_logo[];
extern const unsigned char mobisys22[];
extern const unsigned char ble_icon[];

const Icon mailIcon20 = {
    .size_x = 32,
    .size_y = 20,
    .pos_x = 48,
    .pos_y = 106,
    .data = mail20
};

const Icon barIcon_0_5 = {
    .size_x = 80,
    .size_y = 11,
    .pos_x = 24,
    .pos_y = 1,
    .data = bar_0_5
};

const Icon barIcon_1_5 = {
    .size_x = 80,
    .size_y = 11,
    .pos_x = 24,
    .pos_y = 1,
    .data = bar_1_5
};

const Icon barIcon_2_5 = {
    .size_x = 80,
    .size_y = 11,
    .pos_x = 24,
    .pos_y = 1,
    .data = bar_2_5
};

const Icon barIcon_3_5 = {
    .size_x = 80,
    .size_y = 11,
    .pos_x = 24,
    .pos_y = 1,
    .data = bar_3_5
};

const Icon barIcon_4_5 = {
    .size_x = 80,
    .size_y = 11,
    .pos_x = 24,
    .pos_y = 1,
    .data = bar_4_5
};

const Icon barIcon_5_5 = {
    .size_x = 80,
    .size_y = 11,
    .pos_x = 24,
    .pos_y = 1,
    .data = bar_5_5
};

const Icon ndsiIcon = {
    .size_x = 48,
    .size_y = 20,
    .pos_x = 40,
    .pos_y = 18,
    .data = nsdi22_logo
};

const Icon mobisysIcon = {
    .size_x = 112,
    .size_y = 20,
    .pos_x = 8,
    .pos_y = 18,
    .data = mobisys22
};

const Icon bleIcon = {
    .size_x = 16,
    .size_y = 16,
    .pos_x = 0,
    .pos_y = 1,
    .data = ble_icon
};

void clearIcon(const Icon* icon){
  if(icon->pos_x % 8 || icon->size_x % 8){
    // not x aligned. Not supported
    return;
  }

  uint8_t size_x_bytes = icon->size_x / 8;

  for(uint8_t i = 0; i < icon->size_y; i++){
    memset(&displayBufferBW.line[i + icon->pos_y].data[icon->pos_x / 8], 0, size_x_bytes);
  }

  displayWriteMultipleLines(icon->pos_y, icon->size_y);

}

void writeIcon(const Icon* icon){
  if(icon->pos_x % 8 || icon->size_x % 8){
    // not x aligned. Not supported
    return;
  }

  uint8_t size_x_bytes = icon->size_x / 8;

  for(uint8_t i = 0; i < icon->size_y; i++){
    memcpy(&displayBufferBW.line[i + icon->pos_y].data[icon->pos_x / 8], &icon->data[i * size_x_bytes], size_x_bytes);
  }

  displayWriteMultipleLines(icon->pos_y, icon->size_y);
}
