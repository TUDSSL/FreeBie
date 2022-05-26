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

#include "jit_adc.h"

#include "nrfx_saadc.h"
#include "nrf_gpio.h"

// todo what is the overhead of starting and stopping every time?

CHECKPOINT_EXCLUDE_BSS
volatile uint16_t adc_sample[2];

void jitSample(){
   nrf_gpio_pin_set(LCD_EN_ADC);

   NRF_SAADC->TASKS_START = 1;
   while (NRF_SAADC->EVENTS_STARTED == 0);
   NRF_SAADC->EVENTS_STARTED = 0;

   // Sample both VDISP VDD!
   // adc_sample[0] = VDD
   // adc_sample[1] = AIN1
   NRF_SAADC->TASKS_SAMPLE = 1;
   while (NRF_SAADC->EVENTS_END == 0);
   NRF_SAADC->EVENTS_END = 0;

   NRF_SAADC->TASKS_STOP = 1;
   while (NRF_SAADC->EVENTS_STOPPED == 0);
   NRF_SAADC->EVENTS_STOPPED = 0;

   nrf_gpio_pin_clear(LCD_EN_ADC);
}

void jitConfig(uint8_t calibrate){
  nrf_gpio_cfg_output(LCD_EN_ADC);
  nrf_gpio_pin_clear(LCD_EN_ADC);

  // SAADC
  // Single channel
  // Int reference 0.6V and 1/6 gain.
  // 3us should be fine, low impedance source
  // 0.6 / (1/6) = 3.6V max range
  NRF_SAADC->CH[0].CONFIG = (SAADC_CH_CONFIG_GAIN_Gain1_6    << SAADC_CH_CONFIG_GAIN_Pos) |
                            (SAADC_CH_CONFIG_MODE_SE         << SAADC_CH_CONFIG_MODE_Pos) |
                            (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) |
                            (SAADC_CH_CONFIG_RESN_Bypass     << SAADC_CH_CONFIG_RESN_Pos) |
                            (SAADC_CH_CONFIG_RESP_Bypass     << SAADC_CH_CONFIG_RESP_Pos) |
                            (SAADC_CH_CONFIG_TACQ_3us        << SAADC_CH_CONFIG_TACQ_Pos);

  // Use VDD pin as input.
  NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_VDD << SAADC_CH_PSELP_PSELP_Pos;
  NRF_SAADC->CH[0].PSELN = SAADC_CH_PSELN_PSELN_NC << SAADC_CH_PSELN_PSELN_Pos;

  // Single channel
  // Int reference (0.6V) and 1/3 gain.
  // 3us should be fine, low impedance source
  // HW 1/3 divider 0.6/(1/3) = 1.8V, VCC max LCD = 3.3 * 1/3 = 1.1V,
  NRF_SAADC->CH[1].CONFIG = (SAADC_CH_CONFIG_GAIN_Gain1_3 << SAADC_CH_CONFIG_GAIN_Pos) |
                            (SAADC_CH_CONFIG_MODE_SE << SAADC_CH_CONFIG_MODE_Pos) |
                            (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) |
                            (SAADC_CH_CONFIG_RESN_Bypass << SAADC_CH_CONFIG_RESN_Pos) |
                            (SAADC_CH_CONFIG_RESP_Bypass << SAADC_CH_CONFIG_RESP_Pos) |
                            (SAADC_CH_CONFIG_TACQ_3us << SAADC_CH_CONFIG_TACQ_Pos);

  // Use A1 pin as input.
  NRF_SAADC->CH[1].PSELP = NRF_SAADC_INPUT_AIN1 << SAADC_CH_PSELP_PSELP_Pos;
  NRF_SAADC->CH[1].PSELN = SAADC_CH_PSELN_PSELN_NC << SAADC_CH_PSELN_PSELN_Pos;

  // Only sample with trigger TASKS_SAMPLE
  // Measure once and store result in adc_sample
  // Set resolution at 14 bits
  NRF_SAADC->SAMPLERATE = SAADC_SAMPLERATE_MODE_Task << SAADC_SAMPLERATE_MODE_Pos;
  NRF_SAADC->RESULT.MAXCNT = 2;
  NRF_SAADC->RESULT.PTR = (uint32_t) adc_sample;
  NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_14bit << SAADC_RESOLUTION_VAL_Pos;

  // Enable
  NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos;

  // Execute calibration
  if(calibrate){
    NRF_SAADC->TASKS_CALIBRATEOFFSET = 1;
    while (NRF_SAADC->EVENTS_CALIBRATEDONE == 0);
    NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
    while (NRF_SAADC->STATUS == (SAADC_STATUS_STATUS_Busy <<  SAADC_STATUS_STATUS_Pos));
  }
}
