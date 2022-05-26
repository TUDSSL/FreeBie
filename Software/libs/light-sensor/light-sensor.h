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

#ifndef LIBS_LIGHT_SENSOR_LIGHT_SENSOR_H_
#define LIBS_LIGHT_SENSOR_LIGHT_SENSOR_H_

#include "platform.h"
// Non blocking mode is currently not functional. It currently reads the results
// based on the INT pin.

// Config
#define OPT_BLOCKING
#define TWI_INSTANCE_ID     0
// End of config

#ifdef OPT_BLOCKING
#define OPT_CHECK_BUSY()
#else
#define OPT_CHECK_BUSY() \
       while(optComms.isBusy){;}
#endif

// OPT3004 Device address
#define OPT_DEV_ADDR 0b1000100

// OPT3004 Registers
#define OPT_REG_RESULT    0x00
#define OPT_REG_CONFIG    0x01
#define OPT_REG_LOW_LIM   0x02
#define OPT_REG_HIGH_LIM  0x03
#define OPT_REG_MANUF_ID  0x7E
#define OPT_REG_DEVICE_ID 0x7F

// Function results
#define OPT_DEV_ERROR 1
#define OPT_SUCCESS 0

// Configuration field for the config register
struct OptConfigField {
   uint8_t FC : 2;
   uint8_t ME : 1;
   uint8_t POL : 1;
   uint8_t L : 1;
   uint8_t FL_R : 1;
   uint8_t FH_R : 1;
   uint8_t CRF_R : 1;
   uint8_t OVF_R : 1;
   uint8_t MODE : 2;
   uint8_t CT : 1;
   uint8_t RN : 4;
};

typedef union {
   struct OptConfigField OptConfig;
   uint8_t OptConfigAsBytes[2];
} OptConfig;

void lightSensorSetup();
void lightSensorConfig();

/**
 * Function to retrieve the measured lux from the sensor
 * @return lux in deci-lux
 */
uint32_t lightSensorGetLux();

#endif /* LIBS_LIGHT_SENSOR_LIGHT_SENSOR_H_ */
