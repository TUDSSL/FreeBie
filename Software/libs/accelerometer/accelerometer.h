/*************************************************************************************************/
/*!
 *  Copyright (c) 2022 Delft University of Technology.
 *  Jasper de Winkel, John Hendriks, Haozhe Tang and Przemyslaw Pawelczak
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

#ifndef LIBS_ACCELEROMETER_ACCELEROMETER_H_
#define LIBS_ACCELEROMETER_ACCELEROMETER_H_

#include "platform.h"

#define TWI_ID_ACC                      0

#define ACC_DEV_ADDR                    0x14  

// Registers
#define ACC_DEVICE_ADDRESS_REGISTER     0x00

#define ACC_X_LSB_REGISTER              0x04

#define ACC_CONFIG0_REGISTER            0x19
#define ACC_CONFIG1_REGISTER            0x1A

// Values
/* Device address (0x00) */
#define DEVICE_ADDRESS                  0x90

/* Config 0 (0x19) */
#define NORMAL_MODE                     0x02            // bit 1 0
#define CONFIG0_PRESET                  NORMAL_MODE     // 0x02

/* Config 1 (0x1A) */ 
#define OUTPUT_DATA_RATE_100HZ          0x08            // bit 3 2 1 0
#define OUTPUT_DATA_RATE_200HZ          0x09            // bit 3 2 1 0
#define ACC_MEAS_RANGE_2G               0x00            // bit 7 6
#define ACC_MEAS_RANGE_4G               0x01            // bit 7 6
#define CONFIG1_PRESET                  ((ACC_MEAS_RANGE_4G << 6) | OUTPUT_DATA_RATE_200HZ) // 0x49

void AccelerometerInit(void);
void AccelerometerConfig(void); // Function is called on reboot
void AccelerometerSetup(void);  // Function is called once

#endif /* LIBS_ACCELEROMETER_ACCELEROMETER_H_ */
