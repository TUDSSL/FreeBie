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

#include "ext-rtc.h"

#include <stddef.h>

#include "nrf_delay.h"
#include "nrfx_twim.h"
#include "nrfx_spim.h"
#include "app_util_platform.h"

#include "twi_interface.h"
#include "nrf_gpio.h"

// Config
#define TWI_ID 0
#define SPI_ID 0
#define RTC_COUNTDOWN_INITIAL_VALUE 1

#if HWREV <= 2
static const nrfx_twim_t twi_instance_rtc = NRFX_TWIM_INSTANCE(TWI_ID);
#else
#include <string.h>
static const nrfx_spim_t spi = NRFX_SPIM_INSTANCE(SPI_ID);
static uint8_t extRtcSpiTxBuf[10];// might overflow if large writes are executed!
#endif

void extRtcCountdown(uint8_t value) {
  // Disable countdown timer.
  extRtcWriteByte(AMX8X5_TIMER_CTRL, 0x00);
  // Read the status register to clear interrupts
  extRtcReadStatusRegister();
  // always generates TIRQ when enabled.
  uint8_t temp = extRtcReadByte(AMX8X5_INT_MASK);
  extRtcWriteByte(AMX8X5_INT_MASK, temp | 0x08);

  extRtcWriteByte(AMX8X5_TIMER, value);
  //  extRtcWriteByte(AL_VALUE_REGISTER,
  //             RTC_COUNTDOWN_INITIAL_VALUE);

  // Enable timer    0b10000000
  // TM              0b01000000
  // TFS (64Hz)      0b00000001
  extRtcWriteByte(AMX8X5_TIMER_CTRL, 0b11000001);
}

uint8_t extRtcReadStatusRegister() {
  return extRtcReadByte(AMX8X5_STATUS);
}

// instantly switches off the MCU!
void extRtcShutDownMCUCountDown() {
  // Control2
  // RS1E = 0
  // [PSW/nIRQ2] OUT2S = nTIRQ if TIE is set, else OUTB 0b10100
  // [FOUT/nIRQ] OUT1S = nIRQ if at least one interrupt is enabled, else OUT
  extRtcWriteByte(AMX8X5_CONTROL_2, 0b00010100);
}

// instantly switches off the MCU!
void extRtcShutDownMCUAlarm() {
  // Control2
  // RS1E = 0
  // [PSW/nIRQ2] OUT2S = nAIRQ if AIE is set, else OUTB 0b01100
  // [FOUT/nIRQ] OUT1S = nIRQ if at least one interrupt is enabled, else OUT
//  extRtcWriteByte(AMX8X5_CONTROL_2, 0b00001100);

  extRtcWriteByte(AMX8X5_CONTROL_2, 0b00011000);
  extRtcWriteByte(AMX8X5_SLEEP_CTRL, 0b10000000);
}

void extRtcSetAlarmInteruptPin(uint8_t enable) {
  if(enable){
    // Control2
    // RS1E = 0
    // [PSW/nIRQ2] OUT2S = nAIRQ if AIE is set, else OUTB 0b01100
    // [FOUT/nIRQ] OUT1S = nIRQ if at least one interrupt is enabled, else OUT
    extRtcWriteByte(AMX8X5_CONTROL_2, 0b00011111);
  } else {
    // Control2
    // RS1E = 0
    // [PSW/nIRQ2] OUT2S = nAIRQ if AIE is set, else OUTB 0b01100
    // [FOUT/nIRQ] OUT1S = nIRQ if at least one interrupt is enabled, else OUT
    extRtcWriteByte(AMX8X5_CONTROL_2, 0b00011100);
  }

}

void extRtcSetSquareWave(uint8_t enable) {
  if (enable) {
    // Enable square wave 0b10000000
    // Reserved???        0b00100000
    // SQW 64Hz           0b00001001
    extRtcWriteByte(AMX8X5_SQW, 0b10101001);
  } else {
    // disabled value goes to value of out bit
    extRtcWriteByte(AMX8X5_SQW, 0b00101001);
  }
}

void extRtcSetLfclkOut(uint8_t enable) {
  if (enable) {
    // Enable square wave 0b10000000
    // Reserved???        0b00100000
    // 32768 Hz           0b00000001
    extRtcWriteByte(AMX8X5_SQW, 0b10100001);
  } else {
    // disabled value goes to value of out bit
    extRtcWriteByte(AMX8X5_SQW, 0b00100001);
  }
}

// TODO deal with overflows
uint32_t extRtcReadTimeMs() {
  uint8_t rxBuffer[8];
  extRtcReadMultiple(AMX8X5_HUNDREDTHS, rxBuffer, 8);

  // implement datasheet section 5.6 Hundredths Synchronization
  if (rxBuffer[0] == 0)
  {
    // If the Hundredths Counter was 00, perform the read again.
    // The resulting value from this second read is guaranteed to be correct.
    extRtcReadMultiple(AMX8X5_HUNDREDTHS, rxBuffer, 8);
  } else if (rxBuffer[0] == 0x99)
  {
    // If the Hundredths Counter was 99, perform the read again.
    uint8_t rxBufferAgain[8];
    extRtcReadMultiple(AMX8X5_HUNDREDTHS, rxBufferAgain, 8);
    if (rxBufferAgain[0] == 0)
    {
      // If the Hundredths Counter has rolled over to 00, ..., perform the read again.
      // The resulting value from this third read is guaranteed to be correct.
      extRtcReadMultiple(AMX8X5_HUNDREDTHS, rxBuffer, 8);
    }
  }
  
  // Hundredths
  uint32_t time = BCD_TO_DECIMAL(rxBuffer[0]) * 10;
  // seconds
  time += (uint32_t)BCD_TO_DECIMAL(rxBuffer[1]) * RTC_TIME_CONV_S_TO_MS;
  // minutes
  time +=
      (uint32_t)BCD_TO_DECIMAL(rxBuffer[2]) * RTC_TIME_CONV_S_TO_MS * RTC_TIME_SCALE;
  // hours should be in 24h format, & 0x3F gets rid of the additional bits
  time += (uint32_t)BCD_TO_DECIMAL(rxBuffer[3] & 0x3F) * RTC_TIME_CONV_S_TO_MS *
          RTC_TIME_SCALE * RTC_TIME_SCALE;

  return time;  // in ms
}

void extRtcSetAlarm(AlarmConfiguration* alarmConfig) {
  // Disable Countdown timer set to match hun,sec,min,hour 0b100.
  extRtcWriteByte(AMX8X5_TIMER_CTRL, 0b00010000);
  //  Set interrupt level 0b00
  extRtcWriteByte(AMX8X5_INT_MASK, 0x00);
  // Write the alarm registers
  extRtcWriteMultiple(AMX8X5_ALARM_HUNDRS, (uint8_t*)alarmConfig, 4);

  // Read the status register to clear interrupts
  extRtcReadStatusRegister();
  // Enable alarm interrupt AIE.
  extRtcWriteByte(AMX8X5_INT_MASK, 0x04);

#ifdef DEBUG_SYSTEMOFF
  extRtcWriteByte(AMX8X5_CONTROL_2, 0b00011111);
#endif
}

void extRtcDisableAlarm() {
  // Clear alarm interrupt AIE.
  uint8_t temp = extRtcReadByte(AMX8X5_INT_MASK);
  extRtcWriteByte(AMX8X5_INT_MASK, temp & !0x04);

  // Read the status register to clear interrupts
  extRtcReadStatusRegister();
}

uint8_t extRtcCheckID() {
  // check ID
  if (extRtcReadByte(AMX8X5_PART_NUMBER_UR) != AMX8X5_PART_NUMBER_UR_VALUE) {
    return 0;
  }
  return 1;
}

void extRtcSoftwareReset() {
  // Software reset
  extRtcWriteByte(AMX8X5_CONFIG_KEY,
             AMX8X5_CONFIG_KEY_3C);

#if HWREV >= 2
  nrf_gpio_cfg_output(RTC_SYNC_EN);
  nrf_gpio_pin_set(RTC_SYNC_EN);
  nrf_gpio_cfg_input(RTC_SYNC, NRF_GPIO_PIN_NOPULL);
#endif
  do {
    nrf_delay_ms(100);
  } while(!nrf_gpio_pin_read(RTC_SYNC));


  uint8_t osc_status = extRtcReadByte(AMX8X5_OSC_STATUS);
  nrf_delay_ms(10);
  if (osc_status & 0x13) {  // if Oscillator Failure OR using internal RC
    do {  // loop until the Oscillator Failure is cleared AND using XTAL
      osc_status &= ~(0x03);
      nrf_delay_ms(2);
      extRtcWriteByte(AMX8X5_OSC_STATUS, osc_status);
      nrf_delay_ms(100);
    } while ((osc_status = extRtcReadByte(AMX8X5_OSC_STATUS)) & 0x13);
  }
}

void extRtcSetup() {
  // extRtcConfig();  // already done after application main entry

#if HWREV < 4
  extRtcSoftwareReset();
#endif

  // Write the Key register.
  extRtcWriteByte(AMX8X5_CONFIG_KEY, AMX8X5_CONFIG_KEY_9D);
  // Enable output on FOUT, CLKOUT, nRST, nTIRQ in sleep mode
  extRtcWriteByte(AMX8X5_OUTPUT_CTRL, 0x0F);

  // Control1
  // 24h mode 0b1000000
  // OUTB = 0 MCU ON
  // OUT = 1 0b10000
  // ARST = 1 Auto reset enabled
  // PWR2 Select power switch 0b10
  extRtcWriteByte(AMX8X5_CONTROL_1, 0b01010110);

  // Control2
  // RS1E = 0
  // [PSW/nIRQ2] OUT2S = OUTB 0b11100
  // [FOUT/nIRQ] OUT1S = nIRQ if at least one interrupt is enabled, else OUT
  extRtcWriteByte(AMX8X5_CONTROL_2, 0b00011100);

  // ((32768 – Fmeas)*1000000)/32768 = PAdj
  // Adj/(1000000/2^19) = PAdj/(1.90735) = Ad
  // Else if Adj < -128, set XTCAL = 2, CMDX = 0, OFFSETX = Adj +128
  // page 39  https://ambiq.com/wp-content/uploads/2020/10/Artasie-AM18X5-RTC-Datasheet.pdf

  // ((32768 – 32775)*1000000)/32768 = -213.623046875
  // -213.623046875/(1.90735) = -111.999919718 = -112
  //  -112 + 128 = 16

//  extRtcWriteReg(AMX8X5_OSC_STATUS, 0b10100000); // slow down the clock a bit -244ppm
  // extRtcWriteReg(AMX8X5_CAL_XT, 0b00010000);     // twos complement 63 through -64, chops up clock but introduces jitter

  extRtcWriteByte(AMX8X5_CONFIG_KEY, AMX8X5_CONFIG_KEY_9D);
  extRtcWriteByte(AMX8X5_BATMODE_IO, 0x00);

  extRtcWriteByte(AMX8X5_CONFIG_KEY, AMX8X5_CONFIG_KEY_VAL);
  extRtcWriteByte(AMX8X5_OSC_CONTROL, 0b00000100);
}

void extRtcInterfaceConfig(){
#if HWREV <= 2
  const nrfx_twim_config_t twi_rtc_config = {
      .frequency = NRF_TWIM_FREQ_100K,
      .scl = RTC_SCL,
      .sda = RTC_SDA,
      .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
      .hold_bus_uninit = false};

  nrfx_twim_init(&twi_instance_rtc, &twi_rtc_config, NULL, NULL);
  nrfx_twim_enable(&twi_instance_rtc);
#else
  nrfx_spim_config_t spi_config = NRFX_SPIM_DEFAULT_CONFIG;
  spi_config.ss_pin   = RTC_CS;
  spi_config.miso_pin = RTC_SDO;
  spi_config.mosi_pin = RTC_SDI;
  spi_config.sck_pin  = RTC_SCL;
  spi_config.ss_active_high = true;
  spi_config.frequency = NRF_SPIM_FREQ_500K;
  spi_config.ss_duration = 20;// in 64mhz clock cycles (Min 200ns 300/1/64mhz = 20)
  nrfx_spim_init(&spi, &spi_config, NULL, NULL);
#endif
}

void extRtcWriteByte(uint8_t address, uint8_t data){
#if HWREV <=2
  twi_write_byte(AMX805_ADDR, twi_instance_rtc, address, data);
#else
  uint8_t txbuf[2];
  txbuf[0] = address | 0x80; // set write bit
  txbuf[1] = data;

  nrfx_spim_xfer_desc_t const spim_xfer_desc =
  {
      .p_tx_buffer = txbuf,
      .tx_length   = 2,
      .p_rx_buffer = NULL,
      .rx_length   = 0,
  };
  nrfx_spim_xfer(&spi, &spim_xfer_desc, 0);
#endif
}

uint8_t extRtcReadByte(uint8_t address){
#if HWREV <=2
  return twi_read_byte(AMX805_ADDR, twi_instance_rtc, address);
#else
  uint8_t rx[2];
  nrfx_spim_xfer_desc_t const spim_xfer_desc =
  {
      .p_tx_buffer = &address,
      .tx_length   = 1,
      .p_rx_buffer = rx,
      .rx_length   = 2,
  };
  nrfx_spim_xfer(&spi, &spim_xfer_desc, 0);
  return rx[1];
#endif
}

void extRtcReadMultiple(uint8_t address, uint8_t* rxbuf, uint8_t length){
#if HWREV <=2
  twi_read_multiple(AMX805_ADDR, twi_instance_rtc, address, rxbuf, length);
#else
  uint8_t temp_rxbuf[length + 1];
  nrfx_spim_xfer_desc_t const spim_xfer_desc =
  {
      .p_tx_buffer = &address,
      .tx_length   = 1,
      .p_rx_buffer = temp_rxbuf,
      .rx_length   = length + 1,
  };
  nrfx_spim_xfer(&spi, &spim_xfer_desc, 0);
  memcpy(rxbuf, &temp_rxbuf[1], length);
#endif
}
void extRtcWriteMultiple(uint8_t address, uint8_t* txbuf, uint8_t length){
#if HWREV <=2
  twi_write_multiple(AMX805_ADDR, twi_instance_rtc, address, txbuf, length);
#else
  extRtcSpiTxBuf[0] = address | 0x80; // set write bit
  memcpy(extRtcSpiTxBuf + 1, txbuf, length);

  nrfx_spim_xfer_desc_t const spim_xfer_desc =
  {
      .p_tx_buffer = extRtcSpiTxBuf,
      .tx_length   = 1 + length,
      .p_rx_buffer = NULL,
      .rx_length   = 0,
  };
  nrfx_spim_xfer(&spi, &spim_xfer_desc, 0);
#endif

}

void extRtcConfig() {
  extRtcInterfaceConfig();

  // Set OUT2S to OUTB (Power switch to outb avoids mcu switching off
  // when interrupt register is cleared on external rtc)

  // Control2
  // RS1E = 0
  // [PSW/nIRQ2] OUT2S = OUTB 0b11100
  // [FOUT/nIRQ] OUT1S = nIRQ if at least one interrupt is enabled, else OUT
  extRtcWriteByte(AMX8X5_CONTROL_2, 0b00011100);

  extRtcDisableAlarm();
}
