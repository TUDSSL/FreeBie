/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Tickless timer implementation.
 *
 *  Copyright (c) 2013-2018 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019-2020 Packetcraft, Inc. 
 * 
 *  Copyright (c) 2022 Delft University of Technology.
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
 *
 * Notes:
 *
 *     This is timer driver is used by wsf_timer.c and pal_timer.c.
 *
 */
/*************************************************************************************************/

#include "nrf.h"
#include "pal_rtc.h"
#include "pal_timer.h"

#ifdef CHECKPOINT_TIMING
#include "checkpoint.h"
#include "checkpoint_timing.h"
#include "bf_sync.h"
#endif

#ifdef USE_AMBIQ_LFCLK
#include "ext-rtc.h"
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

#ifdef DEBUG

/*! \brief      Parameter and state check. */
#define PAL_RTC_CHECK(expr)         { if (!(expr)) { while(1); } }

#else

/*! \brief      Parameter and state check (disabled). */
#define PAL_RTC_CHECK(expr)

#endif

#define RTC_TIMER_TOTAL_CHANNEL         4

/**************************************************************************************************
  Variables
**************************************************************************************************/

static palRtcIrqCback_t palRtcTimerCback[RTC_TIMER_TOTAL_CHANNEL];

/*************************************************************************************************/
/*!
 *  \brief  Function for clearing rtc events.
 *
 *  \param  channelId   Channel ID Number.
 */
/*************************************************************************************************/
void PalRtcClearCompareEvents(uint8_t channelId)
{
  PAL_RTC_CHECK(channelId < RTC_TIMER_TOTAL_CHANNEL);

  NRF_RTC1->EVENTS_COMPARE[channelId] = 0;
  (void)NRF_RTC1->EVENTS_COMPARE[channelId];
}

/*************************************************************************************************/
/*!
 *  \brief  Function for starting the RTC timer.
 *
 *  \param  channelId   Channel ID Number.
 */
/*************************************************************************************************/
void PalRtcEnableCompareIrq(uint8_t channelId)
{
  PAL_RTC_CHECK(channelId < RTC_TIMER_TOTAL_CHANNEL);

  PalRtcClearCompareEvents(channelId);

  switch (channelId)
  {
    case 0:
      NRF_RTC1->INTENSET = RTC_INTENSET_COMPARE0_Msk;
      NRF_RTC1->EVTENSET = RTC_EVTENSET_COMPARE0_Msk;
      break;
    case 1:
      NRF_RTC1->INTENSET = RTC_INTENSET_COMPARE1_Msk;
      NRF_RTC1->EVTENSET = RTC_EVTENSET_COMPARE1_Msk;
      break;
    case 2:
      NRF_RTC1->INTENSET = RTC_INTENSET_COMPARE2_Msk;
      NRF_RTC1->EVTENSET = RTC_EVTENSET_COMPARE2_Msk;
      break;
    case 3:
      NRF_RTC1->INTENSET = RTC_INTENSET_COMPARE3_Msk;
      NRF_RTC1->EVTENSET = RTC_EVTENSET_COMPARE3_Msk;
      break;
    default:
      break;
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Function for stopping the RTC timer.
 *
 *  \param  channelId   Channel ID Number.
 */
/*************************************************************************************************/
void PalRtcDisableCompareIrq(uint8_t channelId)
{
  PAL_RTC_CHECK(channelId < RTC_TIMER_TOTAL_CHANNEL);

  switch (channelId)
  {
    case 0:
      NRF_RTC1->INTENCLR = RTC_INTENCLR_COMPARE0_Msk;
      NRF_RTC1->EVTENCLR = RTC_EVTENCLR_COMPARE0_Msk;
      break;
    case 1:
      NRF_RTC1->INTENCLR = RTC_INTENCLR_COMPARE1_Msk;
      NRF_RTC1->EVTENCLR = RTC_EVTENCLR_COMPARE1_Msk;
      break;
    case 2:
      NRF_RTC1->INTENCLR = RTC_INTENCLR_COMPARE2_Msk;
      NRF_RTC1->EVTENCLR = RTC_EVTENCLR_COMPARE2_Msk;
      break;
    case 3:
      NRF_RTC1->INTENCLR = RTC_INTENCLR_COMPARE3_Msk;
      NRF_RTC1->EVTENCLR = RTC_EVTENCLR_COMPARE3_Msk;
      break;
    default:
      break;
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Get the current value of the RTC counter.
 *
 *  \return Current value of the RTC counter.
 */
/*************************************************************************************************/
uint32_t PalRtcCounterGet(void)
{
  // * for Nordic errata 
  // * https://infocenter.nordicsemi.com/topic/errata_nRF52840_Rev1/ERR/nRF52840/Rev1/latest/anomaly_840_20.html
  NRF_CLOCK->EVENTS_LFCLKSTARTED  = 0;
  NRF_CLOCK->TASKS_LFCLKSTART     = 1;
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {}
  NRF_RTC1->TASKS_STOP = 0;

  #ifdef CHECKPOINT
  return PalRtcOffset(NRF_RTC1->COUNTER, 1);
  #else
  return NRF_RTC1->COUNTER;
  #endif
}

/*************************************************************************************************/
/*!
 *  \brief  Set the RTC capture compare value.
 *
 *  \param  channelId   Channel ID Number.
 *  \param  value   Set new value for compare value.
 */
/*************************************************************************************************/
void PalRtcCompareSet(uint8_t channelId, uint32_t value)
{
  PAL_RTC_CHECK(channelId < RTC_TIMER_TOTAL_CHANNEL);
  #ifdef CHECKPOINT
  NRF_RTC1->CC[channelId] = PalRtcOffset(value, 0);
  #else
  NRF_RTC1->CC[channelId] = value;
  #endif
}

/*************************************************************************************************/
/*!
 *  \brief  Get the current value of the RTC capture compare.
 *
 *  \param  channelId   Channel ID Number.
 *
 *  \return Current value of the capture compare.
 */
/*************************************************************************************************/
uint32_t PalRtcCompareGet(uint8_t channelId)
{
  PAL_RTC_CHECK(channelId < RTC_TIMER_TOTAL_CHANNEL);
  #ifdef CHECKPOINT
  return PalRtcOffset(NRF_RTC1->CC[channelId], 1);
  #else
  return NRF_RTC1->CC[channelId];
  #endif
}

/*************************************************************************************************/
/*!
 *  \brief  RTC interrupt handler.
 *
 *  This handler stores the RTC start time which is used as a reference to compute the receive
 *  packet timestamp using the HFCLK.
 *
 */
/*************************************************************************************************/
void RTC1_IRQHandler(void)
{
  for (unsigned int channelId = 0; channelId < RTC_TIMER_TOTAL_CHANNEL; channelId++)
  {
    if (NRF_RTC1->EVENTS_COMPARE[channelId])
    {
      PalRtcClearCompareEvents(channelId);

      if (palRtcTimerCback[channelId])
      {
        palRtcTimerCback[channelId]();
      }
    }
  }
}

void POWER_CLOCK_IRQHandler(void)
{
 if(NRF_CLOCK->EVENTS_LFCLKSTARTED){
   NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
 }
}

/*************************************************************************************************/
/*!
 *  \brief  Tickless timer initialization routine.
 */
/*************************************************************************************************/
void PalRtcInit(void)
{
  /* Stop RTC to prevent any running timers from expiring. */
  for (unsigned int channelId = 0; channelId < RTC_TIMER_TOTAL_CHANNEL; channelId++)
  {
    PalRtcDisableCompareIrq(channelId);
  }

  /* Configure low-frequency clock. */
  #if !defined(CHECKPOINT_TIMING)
  // datasheet 0.25s start-up time
  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
  #elif !defined(USE_AMBIQ_LFCLK)
  // HFCLK already started, almost zero start-up time
  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Synth << CLOCK_LFCLKSRC_SRC_Pos);
  #else
  // defined(CHECKPOINT_TIMING)
  extRtcSetLfclkOut(TRUE);
  NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos)
                      | (CLOCK_LFCLKSRC_BYPASS_Enabled << CLOCK_LFCLKSRC_BYPASS_Pos)
                      | (CLOCK_LFCLKSRC_EXTERNAL_Enabled << CLOCK_LFCLKSRC_EXTERNAL_Pos);
  #endif

#ifdef LFXO_NON_BLOCKING_START
  NVIC_ClearPendingIRQ(POWER_CLOCK_IRQn);
  NVIC_EnableIRQ(POWER_CLOCK_IRQn);

  NRF_CLOCK->EVENTS_LFCLKSTARTED  = 0;
  NRF_CLOCK->INTENSET = CLOCK_INTENSET_LFCLKSTARTED_Msk;
  NRF_CLOCK->TASKS_LFCLKSTART     = 1;

  __WFI();

  NRF_CLOCK->INTENCLR = CLOCK_INTENSET_LFCLKSTARTED_Msk;
  NVIC_ClearPendingIRQ(POWER_CLOCK_IRQn);
  NVIC_DisableIRQ(POWER_CLOCK_IRQn);
#else
  NRF_CLOCK->EVENTS_LFCLKSTARTED  = 0;
  NRF_CLOCK->TASKS_LFCLKSTART     = 1;
  while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) { }
#endif

  NRF_RTC1->TASKS_STOP = 0;

  NRF_RTC1->TASKS_STOP  = 1;
  NRF_RTC1->TASKS_CLEAR = 1;
  NRF_RTC1->PRESCALER   = 0;            /* clear prescaler */

  #ifndef CHECKPOINT_TIMING
  NRF_RTC1->TASKS_START = 1;
  #endif

  NVIC_SetPriority(RTC1_IRQn, 0x80);    /* medium priority */
  NVIC_ClearPendingIRQ(RTC1_IRQn);
  NVIC_EnableIRQ(RTC1_IRQn);
}

/*************************************************************************************************/
/*!
 *  \brief  Register rtc IRQ callback.
 *
 *  \param  channelId   Channel ID Number.
 *  \param  cback       Call back for ISR.
 *
 *  This callback is dedicated for scheduler timer in low power code.
 *
 */
/*************************************************************************************************/
void PalRtcIrqRegister(uint8_t channelId, palRtcIrqCback_t cback)
{
  PAL_RTC_CHECK(channelId < RTC_TIMER_TOTAL_CHANNEL);
  palRtcTimerCback[channelId] = cback;
}

/**************************************************************************************************
  RTC1 Offset and Checkpoint
**************************************************************************************************/
#ifdef CHECKPOINT
static int32_t rtc1_counter_offset = 0;

uint32_t PalRtcOffset(uint32_t val, bool_t addition){
  int32_t temp;

  if (addition) {
    temp = (int32_t) val + rtc1_counter_offset;
  } else {
    temp = (int32_t) val - rtc1_counter_offset;
  }
  
  if (temp > 0xFFFFFF) {
    // overflow
    temp -= 0x1000000;
  } else if (temp < 0) {
    // underflow
    temp += 0x1000000;
  }
  
  return (uint32_t) (temp) & 0xFFFFFF;
}

#ifndef CHECKPOINT_TIMING
static uint32_t last_rtc1_counter;
static uint32_t last_rtc1_cc[4];
static uint32_t last_rtc1_int_mask;

void PalRtcCheckpoint(){
  last_rtc1_counter = RTC1_COUNTER_VAL; // TODO: compensate checkpoint and restore time
  for (uint8_t i = 0; i < RTC_TIMER_TOTAL_CHANNEL; i++) {
    last_rtc1_cc[i] = PalRtcCompareGet(i);
  }
  last_rtc1_int_mask = NRF_RTC1->INTENSET;
}

void PalRtcRestore(){
  NRF_RTC1->INTENCLR = last_rtc1_int_mask;
  NRF_RTC1->EVTENCLR = last_rtc1_int_mask;

  rtc1_counter_offset = (int32_t) last_rtc1_counter - (int32_t) (NRF_RTC1->COUNTER);
  for (uint8_t i = 0; i < RTC_TIMER_TOTAL_CHANNEL; i++) {
    PalRtcClearCompareEvents(i);
    PalRtcCompareSet(i, last_rtc1_cc[i]);
  }

  NRF_RTC1->INTENSET = last_rtc1_int_mask;
  NRF_RTC1->EVTENSET = last_rtc1_int_mask;
}
#else
void PalRtcCheckpoint(timer_cp* timer_to_checkpoint){
  uint32_t sync_time;
  // set RTC1 counter value to bf-sync time (with underflow handled)
  if ( (NRF_RTC1->EVTEN & RTC_EVTEN_COMPARE0_Msk) && (NRF_RTC1->CC[0] < NRF_RTC1->CC[3]) ) {
    sync_time = PalRtcCompareGet(0) - (PalRtcCompareGet(0) % 8192);
    if ( (PalRtcCompareGet(0) % 8192) < 16 ) {
      sync_time -= 8192;
    }
  } else {
    sync_time = PalRtcCompareGet(3) - (PalRtcCompareGet(3) % 8192);
    if ( (PalRtcCompareGet(3) % 8192) < 16 ) {
      sync_time -= 8192;
    }
  }
  timer_to_checkpoint->IntRtcCounter = sync_time & PAL_MAX_RTC_COUNTER_VAL;
  
  for (uint8_t i = 0; i < RTC_TIMER_TOTAL_CHANNEL; i++) {
    timer_to_checkpoint->IntRtcCc[i] = PalRtcCompareGet(i);
  }
  timer_to_checkpoint->IntRtcInterruptMask = NRF_RTC1->INTENSET;
}

void PalRtcRestore(timer_cp* timer_checkpoint){
  NRF_RTC1->INTENCLR = timer_checkpoint->IntRtcInterruptMask;
  NRF_RTC1->EVTENCLR = timer_checkpoint->IntRtcInterruptMask;

  rtc1_counter_offset = (int32_t) timer_checkpoint->IntRtcCounter - (int32_t) (NRF_RTC1->COUNTER);
  for (uint8_t i = 0; i < RTC_TIMER_TOTAL_CHANNEL; i++) {
    PalRtcClearCompareEvents(i);
    PalRtcCompareSet(i, timer_checkpoint->IntRtcCc[i]);
  }

  NRF_RTC1->INTENSET = timer_checkpoint->IntRtcInterruptMask;
  NRF_RTC1->EVTENSET = timer_checkpoint->IntRtcInterruptMask;
}
#endif
#endif
