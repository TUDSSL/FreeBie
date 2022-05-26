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


#ifndef PLATFORMS_INT_BLE_V1_0_H_
#define PLATFORMS_INT_BLE_V1_0_H_

#include "nrf.h"

// LCD
#define LCD_CLK 15 // 43 -> P0.15
#define LCD_MOSI 16 // 44 -> P0.16
#define LCD_CS 17 // 45 -> P0.17
#define LCD_DISP 25 // 49 -> P0.25
#define LCD_EXTCOMIN 13 // 15 -> P0.13

#define LCD_EN_ADC 38 // 24 -> P1.06
#define LCD_ADC 3 // 55 -> P0.03A

// Lux
#define LUX_VCC 5 // 4 -> P0.05A
#define LUX_SCL 4 // 36 -> P0.04A
#define LUX_SDA 27 // 35 -> P0.27

#define LUX_INT 31 // 3 -> P0.31A

// Acc
#define ACC_VCC 26 // 5 -> P0.26
#define ACC_SCL 8 // 38 -> P0.08
#define ACC_SDA 7 // 37 -> P0.07

#define ACC_INT1 40 // 6 -> P1.08 (32=1.00 + 8 = 40?) TODO verify
#define ACC_INT2 6 // 7 -> P0.06

// FRAM
#define FRAM_CS   23 // 48 -> P0.23
#define FRAM_MOSI 22 // 47 -> P0.22
#define FRAM_MISO 21 // 18 -> P0.21
#define FRAM_CLK  19 // 46 -> P0.19
#define FRAM_WP   24 // 19 -> P0.24
#define FRAM_HOLD 20 // 17 -> P0.20

// Harvester
#define BOOST_READY 14 // 42 -> P0.14

// RTC
#define RTC_SDA 12 // 41 -> P0.12
#define RTC_SCL 11 // 40 -> P0.11
#define RTC_SYNC LCD_CS

#endif /* PLATFORMS_INT_BLE_V1_0_H_ */
