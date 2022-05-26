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

#include "display.h"

#include <stdbool.h>
#include <string.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrfx_spim.h"
//#include "ext-rtc.h"

typedef enum {
  WriteLine,
  WriteMultipleLines,
  Update,
  ClearAll,
  None
} CurrentTransaction;

CHECKPOINT_EXCLUDE_DATA
static const nrfx_spim_t spi = NRFX_SPIM_INSTANCE(SPI_INSTANCE);

DisplayStatus spiWrite(uint8_t* data, uint16_t size);

DisplayBufferBW displayBufferBW;
uint8_t displayStatus;

void populateAddressInBuffer() {
  for (uint8_t i = 0; i < NUM_LINES + 1; ++i) {
    displayBufferBW.line[i].address = i;
  }
}

void displaySetPower(uint8_t enable){
  if(enable){
//    extRtcSetSquareWave(true);
    nrf_gpio_pin_set(LCD_FF_PWR);
    nrf_gpio_pin_set(LCD_FF_CLK);
    nrf_delay_us(5);
    nrf_gpio_pin_clear(LCD_FF_CLK);
    nrf_gpio_pin_clear(LCD_FF_PWR);
    nrf_delay_ms(1);
  } else {
//    extRtcSetSquareWave(false);
    nrf_gpio_pin_clear(LCD_FF_PWR);
    nrf_gpio_pin_clear(LCD_FF_DISP);
    nrf_gpio_pin_set(LCD_FF_CLK);
    nrf_delay_us(5);
    nrf_gpio_pin_clear(LCD_FF_CLK);
  }
}

void displayEnable(uint8_t enable){
  if(enable){
    nrf_gpio_pin_set(LCD_FF_PWR);
    nrf_gpio_pin_set(LCD_FF_DISP);
    nrf_gpio_pin_set(LCD_FF_CLK);
    nrf_delay_us(5);
    nrf_gpio_pin_clear(LCD_FF_CLK);
    nrf_gpio_pin_clear(LCD_FF_DISP);
    nrf_gpio_pin_clear(LCD_FF_PWR);
  } else {
    nrf_gpio_pin_clear(LCD_FF_DISP);
    nrf_gpio_pin_set(LCD_FF_CLK);
    nrf_delay_us(5);
    nrf_gpio_pin_clear(LCD_FF_CLK);
  }
}


// writes a single line based on a buffer of a single line.
void displayWriteLineCpy(uint8_t line, const uint8_t* data) {
  Command command = {};
  command.mode = true;

  memcpy(displayBufferBW.line[line].data, data, LINE_SIZE_BYTES);
  displayBufferBW.line[line].cmd = command;

  spiWrite((uint8_t*)&displayBufferBW.line[line], sizeof(LineBufferBW) + 2);
}

// writes a single line based on a buffer of a single line.
void displayWriteLine(uint8_t line) {
  Command command = {};
  command.mode = true;

  displayBufferBW.line[line].cmd = command;

  spiWrite((uint8_t*)&displayBufferBW.line[line], sizeof(LineBufferBW) + 2);
}

void displayWriteScreen() {
  Command command = {};
  command.mode = true;

  displayBufferBW.line[1].cmd = command;

  spiWrite((uint8_t*)&displayBufferBW.line[1], sizeof(DisplayBufferBW));
}

// requires full screen buffer
void displayWriteMultipleLines(uint8_t startLine, uint8_t numLines) {
  Command command = {};
  command.mode = true;

  displayBufferBW.line[startLine].cmd = command;

  spiWrite((uint8_t*)&displayBufferBW.line[startLine],
           numLines * sizeof(LineBufferBW) + 2);
  // 2 byte overflow is guaranteed by DisplayBuffer::dummy
}

static void displayClearAll() {
  uint8_t spiBuffer[2];
  Command command = {};
  command.allClear = true;

  spiBuffer[0] = command.asUint8_t;

  spiWrite(spiBuffer, 2);
}

DisplayStatus spiWrite(uint8_t* data, uint16_t size) {
  nrfx_spim_xfer_desc_t xfer_desc = NRFX_SPIM_XFER_TRX(data, size, NULL, 0);
  if (nrfx_spim_xfer(&spi, &xfer_desc, 0)) {
    return DISPLAY_ERROR;
  }
  return DISPLAY_SUCCESS;
}

void spim_event_handler(nrfx_spim_evt_t const* p_event, void* p_context) {
  //    NRF_LOG_INFO("Transfer completed.");
}

static DisplayStatus spiInit(uint8_t blocking) {
  // Initialize SPIMaster.
  nrfx_spim_config_t spi_config = NRFX_SPIM_DEFAULT_CONFIG;
  spi_config.frequency = NRF_SPIM_FREQ_1M;
  spi_config.ss_pin = LCD_CS;
  spi_config.mosi_pin = LCD_MOSI;
  spi_config.sck_pin = LCD_CLK;
  spi_config.ss_active_high = true;
  spi_config.bit_order = NRF_SPIM_BIT_ORDER_LSB_FIRST;

  if (blocking) {
    nrfx_spim_init(&spi, &spi_config, NULL, NULL);
  } else {
    nrfx_spim_init(&spi, &spi_config, spim_event_handler, NULL);
  }

  return DISPLAY_SUCCESS;
}

void displaySpiDeInit() {
  nrfx_spim_uninit(&spi);
}

void displaySpiInit() {
  spiInit(DISPLAY_BLOCKING_WRITES);
}

// Can be called at anypoint but before screen writes are executed
void displaySetup() {
  populateAddressInBuffer();
  displayStatus = 0; // off
}

void displayConfig() {
  nrf_gpio_cfg_output(LCD_FF_CLK);
  nrf_gpio_cfg_output(LCD_FF_PWR);
  nrf_gpio_cfg_output(LCD_FF_DISP);

//  spiInit(DISPLAY_BLOCKING_WRITES);
//  nrf_delay_us(30);
}

void displayClear(){
  displayClearAll();
}

void displayShutdown() {
  displayEnable(false);
  displaySetPower(false);
  //	setComTimer(false);
//  displayClearAll();
  nrf_delay_us(30);
  //	setDisplayPower(false);
}
