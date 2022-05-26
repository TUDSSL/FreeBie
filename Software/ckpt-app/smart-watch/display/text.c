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

#include "text.h"

#include "mcufont.h"

#include <stdio.h>

#include "display.h"
#include "string.h"

extern const struct mf_rlefont_s mf_rlefont_DejaVuSans16bw;
extern const struct mf_rlefont_s mf_rlefont_DejaVuSans40bw;

#define FONT_BIG mf_rlefont_DejaVuSans40bw
#define FONT_NORMAL mf_rlefont_DejaVuSans16bw

static void pixel_callback(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state)
    {
    while (count--)
        {

         displayBufferBW.line[y].data[x / 8] |= 1 << x % 8;
            /* your code goes here, ex: drawPixel(x, y, alpha, color::black); */
            x++;
        }
    }

static uint8_t char_big_callback(int16_t x0, int16_t y0, mf_char character, void *state)
    {
        return mf_render_character(&FONT_BIG.font, x0, y0, character, &pixel_callback, state);
    }

static uint8_t char_callback(int16_t x0, int16_t y0, mf_char character, void *state)
    {
        return mf_render_character(&FONT_NORMAL.font, x0, y0, character, &pixel_callback, state);
    }


void updateTimeText(uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint16_t year){

  char timeBuffer[10];
  sprintf(timeBuffer, "%02d:%02d", hour, min);
  char timeBufferDate[15];
  sprintf(timeBufferDate, "%02d/%02d/%02d", month, day, year);

  // clear the buffer, clears full lines!
  for(uint8_t i = 40; i < 35+45; i++){
    memset(&displayBufferBW.line[i].data[0], 0, LINE_SIZE_BYTES);
  }

  for(uint8_t i = 77; i < 77+16; i++){
    memset(&displayBufferBW.line[i].data[0], 0, LINE_SIZE_BYTES);
  }

  mf_render_aligned(
      &FONT_BIG.font,
      64, 35,
      MF_ALIGN_CENTER,
      timeBuffer, 13,
      &char_big_callback, NULL);

  mf_render_aligned(
      &FONT_NORMAL.font,
      64, 77,
      MF_ALIGN_CENTER,
      timeBufferDate, 13,
      &char_callback, NULL);

//  displayWriteMultipleLines(35, 45);
//  displayWriteMultipleLines(77, 16);
  displayWriteScreen();
}

void updateMessagesText(uint8_t messages){
  if(messages > 99){
    messages = 99;
  }

  for(uint8_t i = 106; i < 106+16; i++){
    memset(&displayBufferBW.line[i].data[10], 0, 4);
  }

  if(messages != 0){
    char messagesBuffer[5];
    sprintf(messagesBuffer, "%2d", messages);
    mf_render_aligned(
        &FONT_NORMAL.font,
        82, 106,
        MF_ALIGN_LEFT,
        messagesBuffer, 13,
        &char_callback, NULL);
  }
  displayWriteMultipleLines(106, 16);
}
