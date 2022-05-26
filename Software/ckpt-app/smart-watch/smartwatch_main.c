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
#include <time.h>

#include "wsf_types.h"
#include "util/bstream.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "wsf_bufio.h"
#include "wsf_assert.h"
#include "wsf_buf.h"
#include "wsf_heap.h"
#include "wsf_cs.h"
#include "wsf_timer.h"
#include "wsf_app_timer.h"
#include "wsf_os.h"
#include "dm_api.h"
#include "att_api.h"
#include "tipc/tipc_api.h"
#include "anpc/anpc_api.h"

#include "display.h"
#include "icons.h"
#include "text.h"
#include "smartwatch_api.h"
#include "template_api.h"

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "pal_led.h"

#ifdef CHECKPOINT
#include "checkpoint.h"
#include "checkpoint_content.h"
#endif

/*! \brief Control block */
static struct {
  smartwatchCfg_t          cfg;                  /*! \brief configurable parameters */
  struct tm current_time;
} smartwatchCb;

static void smartwatchTimerExp(wsfMsgHdr_t *pMsg) {
  /* restart timer */
#ifdef CHECKPOINT
  WsfAppTimerStartMs(&smartwatchTimer, smartwatchCb.cfg.period);
#else
  WsfTimerStartMs(&smartwatchTimer, smartwatchCb.cfg.period);
#endif

  smartwatchCb.current_time.tm_min++;   // increment minute
  mktime(&(smartwatchCb.current_time)); // adjust all time components

  displaySpiInit();
  updateTimeText((uint8_t)(smartwatchCb.current_time.tm_mon + 1),
                 (uint8_t)smartwatchCb.current_time.tm_mday,
                 (uint8_t)smartwatchCb.current_time.tm_hour,
                 (uint8_t)smartwatchCb.current_time.tm_min,
                 (uint16_t)(smartwatchCb.current_time.tm_year + 1900));

#ifdef CHECKPOINT

  if(adc_conv_result > 1934){ // full > 2.55
    writeIcon(&barIcon_5_5);
  } else if(adc_conv_result > 1858){// 2.45
    writeIcon(&barIcon_4_5);
  } else if(adc_conv_result > 1782){ //  2.35
    writeIcon(&barIcon_3_5);
  } else if(adc_conv_result > 1706){ // 2.25
    writeIcon(&barIcon_2_5);
  } else if(adc_conv_result > 1630){ // 2.15
    writeIcon(&barIcon_1_5);
  } else if(adc_conv_result > 1554){ // 2.05 and below
    writeIcon(&barIcon_0_5);
  } else {
    writeIcon(&barIcon_0_5);
  }

#endif
  displaySpiDeInit();
}

void SmartwatchInit(wsfHandlerId_t handlerId, smartwatchCfg_t *pCfg) {
  smartwatchTimer.handlerId = handlerId;
  smartwatchCb.cfg = *pCfg;
  memset(&smartwatchCb.current_time, 0, sizeof(smartwatchCb.current_time));

  displaySetup();

  displaySpiInit();
  displaySetPower(true);
  displayClear();
  writeIcon(&mobisysIcon);
  displayWriteScreen();
  displayEnable(true);
  displaySpiDeInit();
}

void SmartwatchStart(uint8_t timerEvt) {
  /* initialize control block */
  smartwatchTimer.msg.event = timerEvt;

  /* start timer */
#ifdef CHECKPOINT
  WsfAppTimerStartMs(&smartwatchTimer, smartwatchCb.cfg.period);
#else
  WsfTimerStartMs(&smartwatchTimer, smartwatchCb.cfg.period);
#endif
}

void SmartwatchStop(){
#ifdef CHECKPOINT
  WsfAppTimerStop(&smartwatchTimer);
#else
  WsfTimerStop(&smartwatchTimer);
#endif
}

static void smartwatchValueUpdate(attEvt_t *pMsg) {
  if (pMsg->hdr.status == ATT_SUCCESS) {
    uint8_t   *p;
    /* current time */
    uint16_t  year;
    uint8_t   month, day, hour, min, sec, sec256, dayOfWeek, adjustReason;
    /* Suppress unused variable compile warning */
    (void)month; (void)day; (void)hour; (void)min; (void)sec; (void)dayOfWeek; (void)adjustReason;
    (void)year; (void)sec256;
    /* unread alert status */
    uint8_t   catId;
    uint8_t   numAlert;
    /* Suppress unused variable compile warning */
    (void)catId; (void)numAlert;

    displaySpiInit();

    /* determine which profile the handle belongs to; start with most likely */
    if (pMsg->handle == pWatchCtsHdlList[TIPC_CTS_CT_HDL_IDX]) {
      /* current time */
      p = pMsg->pValue;
      BSTREAM_TO_UINT16(year, p);
      BSTREAM_TO_UINT8(month, p);
      BSTREAM_TO_UINT8(day, p);
      BSTREAM_TO_UINT8(hour, p);
      BSTREAM_TO_UINT8(min, p);
      BSTREAM_TO_UINT8(sec, p);
      BSTREAM_TO_UINT8(dayOfWeek, p);
      BSTREAM_TO_UINT8(sec256, p);
      BSTREAM_TO_UINT8(adjustReason, p);

      smartwatchCb.current_time.tm_year = year - 1900; // years since 1900
      smartwatchCb.current_time.tm_mon = month - 1; // months since January (count from 0)
      smartwatchCb.current_time.tm_mday = day;
      smartwatchCb.current_time.tm_hour = hour;
      smartwatchCb.current_time.tm_min = min;
      smartwatchCb.current_time.tm_sec = 0;

      updateTimeText(month, day, hour, min, year);

      // align timer with the notification
#ifdef CHECKPOINT
  WsfAppTimerStop(&smartwatchTimer);
  WsfAppTimerStartMs(&smartwatchTimer, smartwatchCb.cfg.period);
#else
  WsfTimerStop(&smartwatchTimer);
  WsfTimerStartMs(&smartwatchTimer, smartwatchCb.cfg.period);
#endif
    } else if (pMsg->handle == pWatchAnsHdlList[ANPC_ANS_UAS_HDL_IDX]) {
      /* unread alert status */
      p = pMsg->pValue;
      BSTREAM_TO_UINT8(catId, p);
      BSTREAM_TO_UINT8(numAlert, p);

      if(numAlert){
        writeIcon(&mailIcon20);
        updateMessagesText(numAlert);
      } else {
        clearIcon(&mailIcon20);
        updateMessagesText(numAlert);
      }
    }
    displaySpiDeInit();
  }
}

void SmartwatchProcMsg(wsfMsgHdr_t *pMsg){
  nrf_gpio_pin_set(DEBUG_INFO_APP);
#ifdef CHECKPOINT
  if (!app_restored) {
    restore_application();
    app_restored = true;
  }
#endif
  if (pMsg->event == smartwatchTimer.msg.event) {
    smartwatchTimerExp(pMsg);
  } else {
    switch (pMsg->event) {
      case ATTC_READ_RSP:
      case ATTC_HANDLE_VALUE_NTF:
      case ATTC_HANDLE_VALUE_IND:
        smartwatchValueUpdate((attEvt_t *) pMsg);
        break;

      case DM_CONN_OPEN_IND:
        displaySpiInit();
        writeIcon(&bleIcon);
        displaySpiDeInit();
        break;

      case DM_CONN_CLOSE_IND:
        displaySpiInit();
        clearIcon(&bleIcon);
        displaySpiDeInit();
        break;

      default:
        break;
    }

  }
  nrf_gpio_pin_clear(DEBUG_INFO_APP);
}
