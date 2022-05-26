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

#include "light-sensor.h"
#include "nrfx_twim.h"
#include "nrfx_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

static const nrfx_twim_t m_twi = NRFX_TWIM_INSTANCE(TWI_INSTANCE_ID);

typedef struct {
	uint8_t Transmit[3];
	uint8_t Receive[2];
	volatile uint8_t isBusy;
} OptTwoWire;

static OptTwoWire optComms;
static OptConfig optConfig = {    //set default config
		.OptConfig.L = 1, // latch until user controlled event
		.OptConfig.MODE = 0b01, // single shot
		.OptConfig.CT = 0, // 100ms measurement time
		.OptConfig.RN = 0b1100 // autoscale
		};

static uint32_t measuredLux = 0;

void twi_handler(nrfx_twim_evt_t const * p_event, void * p_context){
    switch (p_event->type){
        case NRFX_TWIM_EVT_DONE:
            if (p_event->xfer_desc.type == NRFX_TWIM_XFER_TXRX) {
//                data_handler(m_sample);
            }
            optComms.isBusy = false;
            break;
        default:
            break;
    }
}

void lightSensorSetup(){
//
}

uint8_t lightSensorCheckDeviceID(){
	OPT_CHECK_BUSY();

	optComms.Transmit[0] = OPT_REG_DEVICE_ID;
    optComms.isBusy = true;
    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TXRX(OPT_DEV_ADDR, optComms.Transmit, 1, optComms.Receive, 2);
    nrfx_twim_xfer(&m_twi,&xfer,0);

	OPT_CHECK_BUSY();

    if(optComms.Receive[0] != 0x30 || optComms.Receive[1] != 0x1){
    	//error
    	return OPT_DEV_ERROR;
    }
    return OPT_SUCCESS;
}

uint8_t lightSensorCheckManufID(){
	OPT_CHECK_BUSY();

	optComms.Transmit[0] = OPT_REG_MANUF_ID;
    optComms.isBusy = true;
    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TXRX(OPT_DEV_ADDR, optComms.Transmit, 1, optComms.Receive, 2);
    nrfx_twim_xfer(&m_twi,&xfer,0);

	OPT_CHECK_BUSY();

    if(optComms.Receive[0] != 0x54 || optComms.Receive[1] != 0x49){
    	//error
    	return OPT_DEV_ERROR;
    }
    return OPT_SUCCESS;
}

void lightSensorStartMeasurement(){
	optConfig.OptConfig.MODE = 0b01; // single shot

	OPT_CHECK_BUSY();

	optComms.Transmit[0] = OPT_REG_CONFIG;
	optComms.Transmit[1] = optConfig.OptConfigAsBytes[1];
	optComms.Transmit[2] = optConfig.OptConfigAsBytes[0];

    optComms.isBusy = true;
    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TX(OPT_DEV_ADDR, optComms.Transmit, 3);
    nrfx_twim_xfer(&m_twi,&xfer,0);
}

void lightSensorDisable(){
	optConfig.OptConfig.MODE = 0b00; // shutdown

	OPT_CHECK_BUSY();

	optComms.Transmit[0] = OPT_REG_CONFIG;
	optComms.Transmit[1] = optConfig.OptConfigAsBytes[1];
	optComms.Transmit[2] = optConfig.OptConfigAsBytes[0];

    optComms.isBusy = true;
    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TX(OPT_DEV_ADDR, optComms.Transmit, 3);
    nrfx_twim_xfer(&m_twi,&xfer,0);
}

void lightSensorSetLowLimit(){
	OPT_CHECK_BUSY();

	optComms.Transmit[0] = OPT_REG_LOW_LIM;
	optComms.Transmit[1] = 0b11000000;
	optComms.Transmit[2] = 0;

    optComms.isBusy = true;
    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TX(OPT_DEV_ADDR, optComms.Transmit, 3);
    nrfx_twim_xfer(&m_twi,&xfer,0);
}

void lightSensorReadLux(){
	uint8_t results[2];

	OPT_CHECK_BUSY();

	optComms.Transmit[0] = OPT_REG_RESULT;
	optComms.isBusy = true;
    nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TXRX(OPT_DEV_ADDR, optComms.Transmit, 1, optComms.Receive, 2);
    nrfx_twim_xfer(&m_twi, &xfer,0);

    results[0] = optComms.Receive[0];
	results[1] = optComms.Receive[1];

    lightSensorDisable();

    uint8_t E =  results[0] >> 4;
    uint16_t R = ((uint16_t)(results[0] & 0x0F) << 8) | results[1];
    // start conversion
    measuredLux = R * ((uint32_t)1 << E);
}

void lightSensorInterruptHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
	lightSensorReadLux(); // does not work for blocking (Nested Interrupts)
}

void lightSensorConfig(){
    const nrfx_twim_config_t twi_opt3004_config = {
       .scl                = LUX_SCL,
       .sda                = LUX_SDA,
       .frequency          = TWIM_FREQUENCY_FREQUENCY_K100,
       .interrupt_priority = NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY,
       .hold_bus_uninit    = false
    };

#ifdef OPT_BLOCKING
    nrfx_twim_init(&m_twi, &twi_opt3004_config, NULL, NULL);
#else
    nrfx_twim_init(&m_twi, &twi_opt3004_config, twi_handler, NULL);
#endif

	nrf_gpio_cfg_output(LUX_VCC);
	nrf_gpio_pin_set(LUX_VCC);

	nrfx_twim_enable(&m_twi);

	// Int pin: get result upon int pin going low

	nrfx_gpiote_init();

	nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;
    nrfx_gpiote_in_init(LUX_INT, &in_config, lightSensorInterruptHandler);

    nrfx_gpiote_in_event_enable(LUX_INT, true);

    // end of int pin

	nrf_delay_ms(100);
	lightSensorCheckDeviceID();
	lightSensorCheckManufID();

	lightSensorSetLowLimit();
    lightSensorStartMeasurement();
}

uint32_t lightSensorGetLux(){
	return measuredLux;
}
