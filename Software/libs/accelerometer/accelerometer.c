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

#include "accelerometer.h"
#include "nrfx_twim.h"
#include "nrf_twim.h"
#include "app_util_platform.h"
// #include "nrfx_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "twi_interface.h"

static const nrfx_twim_t twi_instance_acc = NRFX_TWIM_INSTANCE(TWI_ID_ACC);

void AccelerometerInit(void)
{
    if (twi_read_byte(ACC_DEV_ADDR, twi_instance_acc, 0x00) == DEVICE_ADDRESS) // check connected
    {
        // read config 0 
        uint8_t config0 = twi_read_byte(ACC_DEV_ADDR, twi_instance_acc, ACC_CONFIG0_REGISTER);
        config0 |= CONFIG0_PRESET;

        // write config 0 -> 0x02 normal mode
        twi_write_byte(ACC_DEV_ADDR, twi_instance_acc, ACC_CONFIG0_REGISTER, config0);

        // read config 1
        uint8_t config1 = twi_read_byte(ACC_DEV_ADDR, twi_instance_acc, ACC_CONFIG1_REGISTER);
        config0 |= CONFIG1_PRESET;

        // write config 1 -> 0x49 4g measurement range and 200 hz output data range
        twi_write_byte(ACC_DEV_ADDR, twi_instance_acc, ACC_CONFIG1_REGISTER, config1);

        nrf_delay_ms(10);
            // Read x,y,z axises     
        while(1)
        {
            uint8_t buf[6] = {0}; 
            twi_read_multiple(ACC_DEV_ADDR, twi_instance_acc, ACC_X_LSB_REGISTER, buf, 6);
            
            // x-axis
            uint16_t x = (buf[1] << 8) | buf[0];
            if (x > 2047)
            {
                x -= 4096;
            }
            float xf = 9.80665f * (int16_t)x * 4 / 2048; // for 4g -> from BMA400 API: https://github.com/BoschSensortec/BMA400-API

            // y-axis
            uint16_t y = (buf[3] << 8) | buf[2];
            
            if (y > 2047)
            {
                y -= 4069;
            }
            float yf = 9.80665f * (int16_t)y * 4 / 2048; 

            // z-axis
            uint16_t z = (buf[5] << 8) | buf[4];
            if (z > 2047)
            {
                z -= 4096;
            }
            float zf = 9.80665f * (int16_t)z * 4 / 2048;

            nrf_delay_ms(100);
        }

    }

}

void AccelerometerConfig(void)
{
    const nrfx_twim_config_t twi_accel_config =
    {
    .frequency          = NRF_TWIM_FREQ_100K,
    .scl                = ACC_SCL,
    .sda                = ACC_SDA,
    .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,   // TODO define in common platform BSP
    .hold_bus_uninit    = false
    };

    nrfx_twim_init(&twi_instance_acc, &twi_accel_config, NULL, NULL);
    nrfx_twim_enable(&twi_instance_acc);

    nrf_gpio_cfg_output(ACC_VCC);
	nrf_gpio_pin_set(ACC_VCC);

    nrf_delay_ms(100);
    AccelerometerInit();
}

void AccelerometerSetup(void)
{
    // nothing yet
}
