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

#ifndef LIBS_TWI_INTERFACE_TWI_INTERFACE_H_
#define LIBS_TWI_INTERFACE_TWI_INTERFACE_H_

#include "platform.h"
#include "nrfx_twim.h"

uint8_t twi_read_byte(uint8_t addr, nrfx_twim_t instance, uint8_t reg);
uint8_t* twi_read_multiple(uint8_t addr, nrfx_twim_t instance, uint8_t reg, uint8_t * buf, uint8_t length);
void twi_write_byte(uint8_t addr, nrfx_twim_t instance, uint8_t reg, uint8_t value);
void twi_write_multiple(uint8_t addr, nrfx_twim_t instance, uint8_t reg, uint8_t *value, uint8_t length);
void twi_clear_bits(uint8_t addr, nrfx_twim_t instance, uint8_t reg, uint8_t mask);
void twi_set_bits(uint8_t addr, nrfx_twim_t instance, uint8_t reg, uint8_t mask);

#endif /* LIBS_TWI_INTERFACE_TWI_INTERFACE_H_ */
