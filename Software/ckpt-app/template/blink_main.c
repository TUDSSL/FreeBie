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

#include "platform.h"
#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_bufio.h"
#include "wsf_msg.h"
#include "wsf_assert.h"
#include "wsf_buf.h"
#include "wsf_heap.h"
#include "wsf_cs.h"
#include "wsf_timer.h"
#include "wsf_os.h"

#include "blink_api.h"

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "pal_led.h"

#ifdef CHECKPOINT
#include "wsf_app_timer.h"
#include "checkpoint.h"
#include "checkpoint_content.h"
#endif

/*! \brief Control block */
static struct {
  // // wsfTimer_t        blinkTimer;            /*! \brief periodic blink timer */
  blinkCfg_t          cfg;                  /*! \brief configurable parameters */
  uint16_t          currCount;            /*! \brief current blink count */
} blinkCb;

static void blinkTimerExp(wsfMsgHdr_t *pMsg);

void BlinkInit(wsfHandlerId_t handlerId, blinkCfg_t *pCfg) {
  blinkTimer.handlerId = handlerId;
  blinkCb.cfg = *pCfg;
}

void BlinkStart(uint8_t timerEvt) {
  /* initialize control block */
  blinkTimer.msg.event = timerEvt;
  // blinkCb.blinkTimer.msg.status = 0;
  blinkCb.currCount = 1;

  /* start timer */
  #ifdef CHECKPOINT
  WsfAppTimerStartMs(&blinkTimer, blinkCb.cfg.period);
  #else
  WsfTimerStartMs(&blinkTimer, blinkCb.cfg.period);
  #endif
}

void BlinkStop(){
  #ifdef CHECKPOINT
  WsfAppTimerStop(&blinkTimer);
  #else
  WsfTimerStop(&blinkTimer);
  #endif
}

void BlinkProcMsg(wsfMsgHdr_t *pMsg){
  #ifdef CHECKPOINT
  if (!app_restored) {
    restore_application();
    app_restored = true;
  }
  #endif
  if (pMsg->event == blinkTimer.msg.event) {
    blinkTimerExp(pMsg);
  }
}

static void blinkTimerExp(wsfMsgHdr_t *pMsg) {
  uint8_t i = 0;
  do {
    nrf_gpio_pin_toggle(DEBUG_INFO_APP);
    nrf_delay_us(200);
    nrf_gpio_pin_toggle(DEBUG_INFO_APP);
    nrf_delay_us(200);
    i++;
  } while(i < blinkCb.currCount);
  blinkCb.currCount++;
  if (blinkCb.currCount > blinkCb.cfg.maxCount) {
    blinkCb.currCount = 1;
  } 

  /* restart timer */
  #ifdef CHECKPOINT
  WsfAppTimerStartMs(&blinkTimer, blinkCb.cfg.period);
  #else
  WsfTimerStartMs(&blinkTimer, blinkCb.cfg.period);
  #endif
}
