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


#ifndef PLATFORMS_INT_BLE_V2_0_H_
#define PLATFORMS_INT_BLE_V2_0_H_

#include "nrf.h"

// LCD
#define LCD_CLK 15 // P0.15
#define LCD_MOSI 16 // P0.16
#define LCD_CS 34 // P1.02

#define LCD_FF_CLK 35 // P1.03
#define LCD_FF_PWR 37 // P1.05
#define LCD_FF_DISP 39 // P1.07

#ifndef LCD_PINS_OCCUPIED
#define DEBUG_INFO_NET_CKPT LCD_CLK
#define DEBUG_INFO_FW_CONTENT LCD_MOSI
#define DEBUG_INFO_APP_CKPT LCD_CS
#endif

// Lux
#define LUX_VCC 5 // P0.05A
#define LUX_SCL 4 // P0.04A
#define LUX_SDA 27 // P0.27

#define LUX_INT 31 // P0.31A

#define DEBUG_INFO_CKPT LUX_SCL
#define DEBUG_INFO_BLE LUX_SDA
#define DEBUG_INFO_APP LUX_INT

// Acc
#define ACC_VCC 26 // P0.26
#define ACC_SCL 8 // P0.08
#define ACC_SDA 7 // P0.07

#define ACC_INT1 40 // P1.08
#define ACC_INT2 6 // P0.06

// FRAM
#define FRAM_CS   23 // P0.23
#define FRAM_MOSI 22 // P0.22
#define FRAM_MISO 21 // P0.21
#define FRAM_CLK  19 // P0.19
#define FRAM_WP   24 // P0.24
#define FRAM_HOLD 20 // P0.20

// RTC
#define RTC_CS 13 // P0.13
#define RTC_SDO 12 // P0.12
#define RTC_SDI 41 // P1.09
#define RTC_SCL 11 // P0.11

#define RTC_SYNC_EN 3 // P0.03A
#define RTC_SYNC 17 // P0.17

#define ADC_VBAT 28 // P0.28A
#define ADC_VBAT_EN 46 // P1.14

#endif /* PLATFORMS_INT_BLE_V2_0_H_ */
