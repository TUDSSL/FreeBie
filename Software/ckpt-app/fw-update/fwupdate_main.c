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

#include <string.h>

#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_bufio.h"
#include "wsf_msg.h"
#include "wsf_assert.h"
#include "wsf_buf.h"
#include "wsf_heap.h"
#include "wsf_cs.h"
#include "wsf_timer.h"
#include "wsf_app_timer.h"
#include "wsf_os.h"

#include "cus_api.h"
#include "fwupdate_api.h"

#include "platform.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "pal_led.h"
#include "fram.h"

#ifdef CHECKPOINT
#include "checkpoint.h"
#include "checkpoint_content.h"
#endif

extern const uint32_t  _sfirmware_0; // start firmware 0 in fram
extern const uint32_t  _efirmware_0; // end of firmware 0 in fram
extern const uint32_t  _sfirmware_1;
extern const uint32_t  _efirmware_1;
extern const uint32_t  _sfirmware_rom; // start firmware in flash 4k aligned
extern const uint32_t  _efirmware_rom; //

// defines the linker script to include the flash section, comment out/remove for non firmware update applications
__attribute__((section(".allocate_firmware_flags"))) char _allocate_firmware_ld; 


// __attribute__((section(".firmware_rom"))) __attribute__((target("thumb"))) void firmware_a() {
//   for (volatile uint8_t j = 32; j > 0; j--);
// }
__attribute__((section(".firmware_rom"))) static const uint8_t firmware_rom[] = {
    0x82, 0xB0, 0x20, 0x23, 0x8D, 0xF8, 0x07, 0x30,
    0x06, 0xE0, 0x9D, 0xF8, 0x07, 0x30, 0xDB, 0xB2,
    0x01, 0x3B, 0xDB, 0xB2, 0x8D, 0xF8, 0x07, 0x30,
    0x9D, 0xF8, 0x07, 0x30, 0xDB, 0xB2, 0x00, 0x2B,
    0xF3, 0xD1, 0x00, 0xBF, 0x00, 0xBF, 0x02, 0xB0,
    0x70, 0x47, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// __attribute__((section(".firmware_rom"))) __attribute__((target("thumb"))) void firmware_b() {
//   const uint32_t pos = 1UL << 16;
//   uint32_t pins_state;

//   for (volatile uint8_t j = 32; j > 0; j--);

//   pins_state = NRF_P0->OUT;
//   NRF_P0->OUTSET = (~pins_state & pos);
//   NRF_P0->OUTCLR = (pins_state & pos);
// }
// __attribute__((section(".firmware_rom"))) static const uint8_t firmware_rom[] = {
//     0x84, 0xB0, 0x4F, 0xF4, 0x80, 0x33, 0x03, 0x93,
//     0x20, 0x23, 0x8D, 0xF8, 0x07, 0x30, 0x06, 0xE0,
//     0x9D, 0xF8, 0x07, 0x30, 0xDB, 0xB2, 0x01, 0x3B,
//     0xDB, 0xB2, 0x8D, 0xF8, 0x07, 0x30, 0x9D, 0xF8,
//     0x07, 0x30, 0xDB, 0xB2, 0x00, 0x2B, 0xF3, 0xD1,
//     0x4F, 0xF0, 0xA0, 0x43, 0xD3, 0xF8, 0x04, 0x35,
//     0x02, 0x93, 0x02, 0x9B, 0xDA, 0x43, 0x4F, 0xF0,
//     0xA0, 0x41, 0x03, 0x9B, 0x13, 0x40, 0xC1, 0xF8,
//     0x08, 0x35, 0x4F, 0xF0, 0xA0, 0x41, 0x02, 0x9A,
//     0x03, 0x9B, 0x13, 0x40, 0xC1, 0xF8, 0x0C, 0x35,
//     0x00, 0xBF, 0x04, 0xB0, 0x70, 0x47, 0xff, 0xff,
//   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
// };

firmware_ptr_t firmware_ptr;

static struct {
  fwUpdateCfg_t cfg;                  /*! \brief configurable parameters */
  uint8_t firmware_lclock;
  uint8_t firmware_len;
} fwUpdateCb;

static void startup_populate_firmware(void){
    // firmware_a();  // keep it not optimized out
    // firmware_b();  // keep it not optimized out

    char *firmware_fram = (char *)&_sfirmware_0;
    // char *firmware_init = (char *)&_sfirmware_rom;
    char *firmware_init = (char *)firmware_rom;  // keep firmware_rom not optimized out

    uint8_t tempBuffer[sizeof(firmware_rom)]; // as long as the firmware is small this should be fine.
    memcpy(tempBuffer,firmware_init, sizeof(firmware_rom)); // workaroud since QSPI can only write from SRAM...
    framWrite(firmware_fram, tempBuffer, sizeof(firmware_rom));
}

static void FwUpdateSetFirmwarePtr() {
  uint32_t firmware_ptr_val;
  if (fwUpdateCb.firmware_lclock % 2 == 0) {
    firmware_ptr_val = (uint32_t)&_sfirmware_0;
  } else {
    firmware_ptr_val = (uint32_t)&_sfirmware_1;
  }
  // firmware pointer will be compiled to a `BLX` instruction
  // setting bit 0 to force the thumb state
  firmware_ptr_val |= 1UL;
  firmware_ptr = (firmware_ptr_t)firmware_ptr_val;
  fwUpdateCb.firmware_lclock++;
}

static void FwUpdateFinish() {
  FwUpdateSetFirmwarePtr();
  custom_control_point = 0;
  customSetupToSendCp();
  customSendCpNtf();
  /* restart timer */
#ifdef CHECKPOINT
  WsfAppTimerStartMs(&fwExeTimer, fwUpdateCb.cfg.period);
#else
  WsfTimerStartMs(&fwExeTimer, fwUpdateCb.cfg.period);
#endif
}

static void fwExeTimerExp(wsfMsgHdr_t *pMsg) {
  if (custom_control_point == 0x00) {
    /* restart timer */
#ifdef CHECKPOINT
  WsfAppTimerStartMs(&fwExeTimer, fwUpdateCb.cfg.period);
#else
  WsfTimerStartMs(&fwExeTimer, fwUpdateCb.cfg.period);
#endif
    firmware_ptr();
  }
}

void FwUpdateInit(wsfHandlerId_t handlerId, fwUpdateCfg_t *pCfg) {
  fwExeTimer.handlerId = handlerId;
  fwUpdateCb.cfg = *pCfg;
  fwUpdateCb.firmware_lclock = 0;
  startup_populate_firmware();
  FwUpdateSetFirmwarePtr();
}

void FwUpdateStart(uint8_t timerEvt) {
  /* initialize control block */
  fwExeTimer.msg.event = timerEvt;

  /* start timer */
#ifdef CHECKPOINT
  WsfAppTimerStartMs(&fwExeTimer, fwUpdateCb.cfg.period);
#else
  WsfTimerStartMs(&fwExeTimer, fwUpdateCb.cfg.period);
#endif
}

void FwUpdateStop(){
#ifdef CHECKPOINT
  WsfAppTimerStop(&fwExeTimer);
#else
  WsfTimerStop(&fwExeTimer);
#endif
}

void FwUpdateProcMsg(wsfMsgHdr_t *pMsg){
  nrf_gpio_pin_set(DEBUG_INFO_APP);
#ifdef CHECKPOINT
  if (!app_restored) {
    nrf_gpio_pin_set(DEBUG_INFO_APP_CKPT);
    restore_application();
    app_restored = true;
    nrf_gpio_pin_clear(DEBUG_INFO_APP_CKPT);
  }
#endif
  if (pMsg->event == fwExeTimer.msg.event) {
    fwExeTimerExp(pMsg);
  } else {
    switch (pMsg->event) {
      case CUS_FW_CP_UPDATED:
        if (custom_control_point > 0) {
          if (custom_firmware_len > 0) {
            // stop the exe timer
#ifdef CHECKPOINT
  WsfAppTimerStop(&fwExeTimer);
#else
  WsfTimerStop(&fwExeTimer);
#endif
            // start requesting fw sections
            custom_request_index = 0;
            customSetupToSendRi();
            customSendRiNtf();
          } else {
            // if firmware length is 0
            // terminate fw update right away
            custom_control_point = 0;
            customSetupToSendCp();
            customSendCpNtf();
          }
        }
        break;
      
      case CUS_FW_LEN_RECEIVED:
        fwUpdateCb.firmware_len = custom_firmware_len;
        break;

      case CUS_FW_SECTION_RECEIVED:
        {
          uint8_t *firmware_fram_ptr;
          if (fwUpdateCb.firmware_lclock % 2 == 0) {
            firmware_fram_ptr = (uint8_t *)&_sfirmware_0;
          } else {
            firmware_fram_ptr = (uint8_t *)&_sfirmware_1;
          }
          firmware_fram_ptr += custom_request_index * 8;
          framWrite(firmware_fram_ptr, custom_firmware_section_buffer, 8);

          // increment the request index
          if (++custom_request_index < fwUpdateCb.firmware_len) {
            // request the next firmware section
            customSetupToSendRi();
            customSendRiNtf();
          } else {
            // all firmware sections obtained, finish up FW update
            FwUpdateFinish();
          }
        }
        break;

      default:
        break;
    }
  }
  nrf_gpio_pin_clear(DEBUG_INFO_APP);
}
