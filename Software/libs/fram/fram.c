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

#include "fram.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "debug.h"

#include "nrfx_qspi.h"

void framSetup(){

}

void qspi_handler(nrfx_qspi_evt_t event, void * p_context){
//    UNUSED_PARAMETER(event);
//    UNUSED_PARAMETER(p_context);
//    m_finished = true;
}

uint8_t framReadDeviceId(){
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = FRAM_READ_DEV_ID,
        .length    = NRF_QSPI_CINSTR_LEN_5B,
        .io2_level = false,
        .io3_level = false,
        .wipwait   = false,
        .wren      = false
    };

    uint8_t rxbuf[4];
    nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, rxbuf); // by default synchronous

    uint32_t* deviceID = (uint32_t*)rxbuf;
    if(*deviceID != FRAM_REF_ID){
    	return FRAM_ERROR;
    }
    return FRAM_SUCCESS;
}

void framInitializeMemory(){
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = FRAM_SET_WRITE_EN_LATCH,
        .length    = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = false,
        .io3_level = false,
        .wipwait   = false,
        .wren      = false
    };

    // Set write enable
    nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
}

void framWrite(const void* address, const void* buffer, uint16_t size){
  if(!size) return;
	if(
	(nrfx_qspi_write(buffer, size, ((uint32_t)address) - JIT_RO_MEMORY_SPACE) != NRFX_SUCCESS) ||
	(size % 4 != 0)){
		  DEBUG_PRINTF("FRAM WRITE FAILED, size %d\n", size);
	}
}

void framRead(const void* address, void* buffer, uint16_t size){
	nrfx_qspi_read(buffer, size, ((uint32_t)address) - JIT_RO_MEMORY_SPACE);
}


void framConfig(){
    nrfx_qspi_config_t config = NRFX_QSPI_DEFAULT_CONFIG;
    config.pins.csn_pin = FRAM_CS;
    config.pins.io0_pin = FRAM_MOSI;
    config.pins.io1_pin = FRAM_MISO;
    config.pins.sck_pin = FRAM_CLK;

    config.prot_if.readoc = QSPI_IFCONFIG0_READOC_FASTREAD; // (opcode 0x0b)
    config.prot_if.writeoc = QSPI_IFCONFIG0_WRITEOC_PP; // (opcode 0x02)
    config.prot_if.addrmode = QSPI_IFCONFIG0_ADDRMODE_24BIT;

    config.phy_if.sck_freq = NRF_QSPI_FREQ_32MDIV1;
    config.phy_if.sck_delay = 1;
    config.phy_if.spi_mode = QSPI_IFCONFIG1_SPIMODE_MODE0;

	nrf_gpio_cfg_output(FRAM_WP);
	nrf_gpio_pin_set(FRAM_WP);
	nrf_gpio_cfg_output(FRAM_HOLD);
	nrf_gpio_pin_set(FRAM_HOLD);

    nrfx_qspi_init(&config, NULL, NULL);

    framInitializeMemory();
    framReadDeviceId();
}

void framDeinit(){
    nrfx_qspi_uninit();
}
