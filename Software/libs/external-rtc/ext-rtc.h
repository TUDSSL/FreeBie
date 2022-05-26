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

#ifndef LIBS_EXTERNAL_RTC_RTC_H_
#define LIBS_EXTERNAL_RTC_RTC_H_

#include "platform.h"

// lower 4 bits are the digits 1 ... 9 + upper digits (10's so times 10)
#define BCD_TO_DECIMAL(bcdValue) \
  ((((bcdValue & 0xF0) >> 4) * 10) + (bcdValue & 0x0F))
#define DECIMAL_TO_BCD(decValue) (((decValue / 10) << 4) | (decValue % 10))
// Constants
#define RTC_TIME_SCALE 60
#define RTC_TIME_CONV_S_TO_MS 1000

typedef struct {
  uint8_t TensAndHundredths;
  uint8_t Seconds;
  uint8_t Minutes;
  uint8_t Hours;
} AlarmConfiguration;


/**
 * Register defines for AMX8X5 as per
 * am_devices_amx8x5.h from RTC SDK 2.1.0
 */
// Ambiq AMX805 addresses
#define AMX805_ADDR 0x69

#define AMX8X5_HUNDREDTHS 0x00
#define AMX8X5_YEARS 0x06
#define AMX8X5_ALARM_HUNDRS 0x08
#define AMX8X5_STATUS 0x0F
#define AMX8X5_CONTROL_1 0x10
#define AMX8X5_CONTROL_2 0x11
#define AMX8X5_INT_MASK 0x12
#define AMX8X5_SQW 0x13
#define AMX8X5_CAL_XT 0x14
#define AMX8X5_CAL_RC_HI 0x15
#define AMX8X5_CAL_RC_LOW 0x16
#define AMX8X5_SLEEP_CTRL 0x17
#define AMX8X5_TIMER_CTRL 0x18
#define AMX8X5_TIMER 0x19
#define AMX8X5_TIMER_INITIAL 0x1A
#define AMX8X5_WDT 0x1B
#define AMX8X5_OSC_CONTROL 0x1C
#define AMX8X5_OSC_STATUS 0x1D
#define AMX8X5_CONFIG_KEY 0x1F
#define AMX8X5_ACAL_FLT 0x26
#define AMX8X5_BATMODE_IO 0x27

#define AMX8X5_TRICKLE 0x20

#define AMX8X5_OUTPUT_CTRL 0x30

#define AMX8X5_EXTENDED_ADDR 0x3F
#define AMX8X5_RAM_START 0x40

// Part number registers
#define AMX8X5_PART_NUMBER_UR 0x28
#define AMX8X5_PART_NUMBER_UR_VALUE 0x18

// Keys.
#define AMX8X5_CONFIG_KEY_VAL 0xA1
#define AMX8X5_CONFIG_KEY_3C 0x3C
#define AMX8X5_CONFIG_KEY_9D 0x9D

// Enables 64Hz square wave on CLKOUT pin for the display
void extRtcSetSquareWave(uint8_t enable);
// Enables 32768Hz square wave on CLKOUT pin for MCU LFCLK
void extRtcSetLfclkOut(uint8_t enable);

void extRtcCountdown(uint8_t value);
uint8_t extRtcReadStatusRegister();

// Returns 1 if ID matches
uint8_t extRtcCheckID();

void extRtcSetup();
// Config must be ran first, even before setup
void extRtcConfig();

void extRtcSetAlarm(AlarmConfiguration* alarmConfig);
void extRtcDisableAlarm();
uint32_t extRtcReadTimeMs();

// instantly switches off the MCU!
void extRtcShutDownMCUCountDown();
void extRtcShutDownMCUAlarm();

void extRtcSetAlarmInteruptPin(uint8_t enable);

void extRtcWriteByte(uint8_t address, uint8_t data);
uint8_t extRtcReadByte(uint8_t address);

void extRtcWriteMultiple(uint8_t address, uint8_t* txbuf, uint8_t length);
void extRtcReadMultiple(uint8_t address, uint8_t* rxbuf, uint8_t length);

#endif /* LIBS_EXTERNAL_RTC_EXT_RTC_H_ */
