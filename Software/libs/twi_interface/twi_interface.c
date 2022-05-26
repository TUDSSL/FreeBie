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

#include "twi_interface.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

uint8_t twi_read_byte(uint8_t addr, nrfx_twim_t instance, uint8_t reg)
{
    uint8_t rx;
    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TXRX(addr, &reg, 1, &rx, 1);
    nrfx_twim_xfer(&instance, &xfer, 0);
    return rx; 
}

void twi_write_byte(uint8_t addr, nrfx_twim_t instance, uint8_t reg, uint8_t value)
{
    uint8_t tx[2] = {reg, value};
    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TX(addr, tx, sizeof(tx));
    nrfx_twim_xfer(&instance, &xfer, 0);
}

void twi_clear_bits(uint8_t addr, nrfx_twim_t instance, uint8_t reg, uint8_t mask){
    uint8_t temp;

    temp = twi_read_byte(addr, instance, reg);
    if (temp & mask) {  // skip if bits are not set
        temp &= ~mask;
        twi_write_byte(addr, instance, reg, temp);
    }
}

void twi_set_bits(uint8_t addr, nrfx_twim_t instance, uint8_t reg, uint8_t mask){
    uint8_t temp;

    temp = twi_read_byte(addr, instance, reg);
    temp |= mask;
    twi_write_byte(addr, instance, reg, temp);
}




uint8_t* twi_read_multiple(uint8_t addr, nrfx_twim_t instance, uint8_t reg, uint8_t * buf, uint8_t length)
{
    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TXRX(addr, &reg, 1, buf, length);
    nrfx_twim_xfer(&instance, &xfer, 0);
    return buf;
}

void twi_write_multiple(uint8_t addr, nrfx_twim_t instance, uint8_t reg, uint8_t * value, uint8_t length)
{
    uint8_t tx[length+1];
    tx[0] = reg;
    for (int i = 0; i < length; i++)
    {
        tx[i+1] = value[i];
    }

    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TX(addr, tx, sizeof(tx));
    nrfx_twim_xfer(&instance, &xfer, 0);
}
