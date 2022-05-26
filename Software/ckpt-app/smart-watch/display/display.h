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
#ifndef LIBS_DISPLAY_DISPLAY_H_
#define LIBS_DISPLAY_DISPLAY_H_

#include "platform.h"

// config
#define SPI_INSTANCE 3
#define BLOCKING_WRITES
// end config

#define NUM_PIXELS 1

#define NUM_LINES 128
#define LINE_SIZE 128
#define LINE_SIZE_BYTES LINE_SIZE* NUM_PIXELS / 8

#ifdef BLOCKING_WRITES
#define DISPLAY_BLOCKING_WRITES 1
#else
#define DISPLAY_BLOCKING_WRITES 0
#endif

typedef enum { DISPLAY_SUCCESS, DISPLAY_ERROR } DisplayStatus;

typedef union {
  struct {
    uint8_t mode : 1;
    uint8_t frameInv : 1;
    uint8_t allClear : 1;
    uint8_t FourBitsData : 1;  // R/G/B/D or when both one and four are low then
                               // 3 bit R/G/B, only for LPM013M126
    uint8_t OneBitData : 1;  // B/W or when both one and four are low then 3 bit
                             // R/G/B, only for LPM013M126
    uint8_t dummy : 3;
  };
  uint8_t asUint8_t;
} Command;

typedef struct {
  Command cmd;
  uint8_t address;
  uint8_t data[LINE_SIZE_BYTES];
} LineBufferBW;

typedef struct {
  LineBufferBW line[NUM_LINES + 1];
  uint8_t dummy[2];
} DisplayBufferBW;

extern DisplayBufferBW displayBufferBW;

void displaySpiInit();
void displaySpiDeInit();

void displaySetup();
void displayConfig();
void displayShutdown();
void displaySetPower(uint8_t enable);
void displayEnable(uint8_t enable);
void displayClear();

void displayWriteLine(uint8_t line);
void displayWriteScreen();

void displayWriteMultipleLines(uint8_t startLine, uint8_t numLines);
void displayWriteLineCpy(uint8_t line, const uint8_t* data);

#endif /* LIBS_DISPLAY_DISPLAY_H_ */
