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

#include <stdint.h>
#include <stdbool.h>
#include "bf_sync.h"

#include "debug.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_rtc.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

#include "ext-rtc.h"
#include "checkpoint.h"
#include "checkpoint_timing.h"
#include "checkpoint_network.h"
#include "pal_rtc.h"
#include "pal_bb_ble.h"
#include "bb_api.h"
#include "sch_api.h"
#include "dm_api.h"
#include "app_api.h"
#include "lctr_api_conn.h"

#ifdef DEBUG_SYSTEMOFF
#include "nrf_power.h"
#endif

#define BF_SYNC_PPI_CHANNEL 0
#define BF_SYNC_GPIOTE_CHANNEL 0
#define BF_SYNC_SYNC_PIN RTC_SYNC

// packetcraft pal_timer defines
// copied for now from pal_timer.c
#if BB_CLK_RATE_HZ == 32768
  #define PAL_TIMER_US_TO_TICKS(us)         ((uint32_t)(((uint64_t)(us) * (uint64_t)(70368745)) >> 31))  /* calculated value may be one tick low */
  #define PAL_TIMER_TICKS_TO_US(ticks)      ((uint32_t)(((uint64_t)(ticks) * 15625) >> 9))
#else
  #define PAL_TIMER_US_TO_TICKS(us)         (us)
  #define PAL_TIMER_TICKS_TO_US(ticks)      (ticks)
#endif

uint32_t extRtcTimeMs;
uint32_t lastNetExtRtcTimeMs;
// Should be NOT checkpointed, timing is restored from the timing checkpoint.
CHECKPOINT_EXCLUDE_DATA ExternalSyncAlarm syncAlarmConfig;

void bfSyncConvertToBcdAlarm(uint32_t time, AlarmConfiguration* alarmOut);

bool bfSyncWakeUpScheduleAbsolute(uint32_t nextEventRtcTick, uint32_t currentRtcTick, uint8_t turnOff){
  uint32_t lastSyncRtcTick = auto_timing_chkpt.IntRtcCounter;
  uint32_t lastSyncExtRtcTimeMs = extRtcTimeMs;
  uint32_t nextEventms = lastSyncExtRtcTimeMs + (PAL_TIMER_TICKS_TO_US( (nextEventRtcTick - lastSyncRtcTick) & PAL_MAX_RTC_COUNTER_VAL ) / 1000);
  AlarmConfiguration wakeUpAlarmConfig;
  uint32_t nextWakeupTime;

  if (net_restored) {
    // used to check timeout during conn recovery
    lastNetExtRtcTimeMs = extRtcTimeMs;
  }

  if ( (nextEventRtcTick % 8192) < 16 ) {
    nextEventms -= T_Sync_Interval_Ms;
  }

  syncAlarmConfig.ExtRtcStartTimeMs = nextEventms - (nextEventms % T_Sync_Interval_Ms);
  nextWakeupTime = syncAlarmConfig.ExtRtcStartTimeMs - T_Restore - T_Margin;

  uint32_t elapsedRtcTicks = (currentRtcTick - lastSyncRtcTick) & PAL_MAX_RTC_COUNTER_VAL;
  uint32_t currentTimeMs = lastSyncExtRtcTimeMs + (PAL_TIMER_TICKS_TO_US(elapsedRtcTicks) / 1000);
  if (nextWakeupTime <= currentTimeMs + 2 * (T_Restore + T_Margin)) {
    return false;
  }

  extRtcTimeMs = syncAlarmConfig.ExtRtcStartTimeMs;
  // convert to bcd to set alarm
  bfSyncConvertToBcdAlarm(nextWakeupTime, &wakeUpAlarmConfig);
  bfSyncConvertToBcdAlarm(syncAlarmConfig.ExtRtcStartTimeMs, &syncAlarmConfig.ExtRtcSyncConfig);

  // set the alarm.
  extRtcSetAlarm(&wakeUpAlarmConfig);

  if (turnOff) {
  #ifndef DEBUG_SYSTEMOFF
    extRtcShutDownMCUAlarm();
  #else
    bfSyncDebugSystemoff();
  #endif
  }

  return true;
}

static void bfSyncConfigSyncPinIntHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  // Must have a handler to enable interrupt so empty function...
  __NOP();
}

static void bfSyncConfigPPI(){
  nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
  // must have handler otherwise no interrupt is enabled.
  nrfx_gpiote_in_init(BF_SYNC_SYNC_PIN, &config, bfSyncConfigSyncPinIntHandler);

  NRF_PPI->CH[BF_SYNC_PPI_CHANNEL].EEP = (uint32_t) &NRF_GPIOTE->EVENTS_IN[BF_SYNC_GPIOTE_CHANNEL];        /* configure event */
  NRF_PPI->CH[BF_SYNC_PPI_CHANNEL].TEP = (uint32_t) &NRF_RTC1->TASKS_START;     /* configure task */
}

static void bfSyncEnablePPI(){
  nrfx_gpiote_in_event_enable(BF_SYNC_SYNC_PIN, true);
  NRF_PPI->CHENSET = PPI_CHENSET_CH0_Msk;                            /* enable channel */
}

static void bfSyncDisablePPI() {
  NRF_PPI->CHENCLR = PPI_CHENSET_CH0_Msk;
  nrfx_gpiote_in_event_disable(BF_SYNC_SYNC_PIN);
}

void bfSyncSetupSync() {
  uint32_t currentTimeMs = extRtcReadTimeMs();
  timer_cp syncPoint = {};

  //select next interval, apply T_margin to ensure we can meet the sync point
  currentTimeMs += T_Sync_Interval_Ms + T_Margin;
  syncPoint.ExtRtcSyncAlarm.ExtRtcStartTimeMs = currentTimeMs - (currentTimeMs % T_Sync_Interval_Ms);
  extRtcTimeMs = syncPoint.ExtRtcSyncAlarm.ExtRtcStartTimeMs;
  lastNetExtRtcTimeMs = extRtcTimeMs;

  bfSyncConvertToBcdAlarm(syncPoint.ExtRtcSyncAlarm.ExtRtcStartTimeMs, &syncPoint.ExtRtcSyncAlarm.ExtRtcSyncConfig);
  bfSyncConfigPowerUpSync(&syncPoint);
}

void bfSyncPowerDown() {
  extRtcShutDownMCUAlarm();
}

void bfSyncConvertToBcdAlarm(uint32_t time, AlarmConfiguration* alarmOut) {
  uint8_t hours = time / (RTC_TIME_CONV_S_TO_MS * RTC_TIME_SCALE * RTC_TIME_SCALE);
  time -= hours * (RTC_TIME_CONV_S_TO_MS * RTC_TIME_SCALE * RTC_TIME_SCALE);
  alarmOut->Hours = DECIMAL_TO_BCD(hours);
  uint8_t minutes = time / (RTC_TIME_CONV_S_TO_MS * RTC_TIME_SCALE);
  time -= minutes * (RTC_TIME_CONV_S_TO_MS * RTC_TIME_SCALE);
  alarmOut->Minutes = DECIMAL_TO_BCD(minutes);
  uint8_t seconds = time / (RTC_TIME_CONV_S_TO_MS);
  time -= seconds * (RTC_TIME_CONV_S_TO_MS);
  alarmOut->Seconds = DECIMAL_TO_BCD(seconds);
  uint8_t hundreths = time / 10;
  alarmOut->TensAndHundredths = DECIMAL_TO_BCD(hundreths);
}

void RTC2_IRQHandler(void) {
  if (NRF_RTC2->EVENTS_COMPARE[0]) {
    // if RTC2 as a watchdog has expired
    NRF_RTC2->EVENTS_COMPARE[0] = 0;
    checkpoint_restore_invalidate();
    NVIC_SystemReset();
  }
}

void bfSyncConfig(){
  syncAlarmConfig.ExtRtcStartTimeMs = 0;

  bfSyncConfigPPI();

  // using RTC2 as a watchdog
  NRF_RTC2->INTENCLR = 0x000F0003;
  NRF_RTC2->EVTENCLR = 0x000F0003;
  NRF_RTC2->TASKS_STOP  = 1;
  NRF_RTC2->TASKS_CLEAR = 1;
  // use max prescaler / 4096
  // now 1 RTC2 tick = 125 ms
  NRF_RTC2->PRESCALER   = 0xFFF;
  NVIC_SetPriority(RTC2_IRQn, 0x80);    /* medium priority */
  NVIC_ClearPendingIRQ(RTC2_IRQn);
  NVIC_EnableIRQ(RTC2_IRQn);
  // NRF_RTC2->CC[0] = 32; // 4 seconds
  NRF_RTC2->CC[0] = 4; // 500 ms
  NRF_RTC2->EVENTS_COMPARE[0] = 0;
  NRF_RTC2->EVTENSET = 0x00010000;
  NRF_RTC2->INTENSET = 0x00010000;
  NRF_RTC2->TASKS_START = 1;

#ifdef DEBUG_SYSTEMOFF
  nrf_gpio_cfg_sense_set(BF_SYNC_SYNC_PIN, GPIO_PIN_CNF_SENSE_Disabled);
#if HWREV >= 2
  nrf_gpio_cfg_output(RTC_SYNC_EN);
  nrf_gpio_pin_set(RTC_SYNC_EN);
#endif
#endif
}

void bfSyncConfigPowerUpSync(timer_cp* timer_checkpoint) {
#if HWREV >= 2
  nrf_gpio_cfg_output(RTC_SYNC_EN);
  nrf_gpio_pin_set(RTC_SYNC_EN);
#endif

  // Set the sync alarm.
  extRtcSetAlarm(&timer_checkpoint->ExtRtcSyncAlarm.ExtRtcSyncConfig);
  extRtcSetAlarmInteruptPin(true);

  // Enable ppi linking pin interrupt to enable rtc clk
  bfSyncEnablePPI();

  __DSB();
  __WFI();

  // if we can run to here then RTC2 must have not expired
  // disable RTC2
  NRF_RTC2->TASKS_STOP  = 1;
  NRF_RTC2->EVTENCLR = 0x00010000;
  NRF_RTC2->INTENCLR = 0x00010000;
  NVIC_ClearPendingIRQ(RTC2_IRQn);
  NVIC_DisableIRQ(RTC2_IRQn);

  bfSyncDisablePPI();
  extRtcDisableAlarm();
}

#ifdef DEBUG_SYSTEMOFF
void bfSyncDebugSystemoff() {
  // extRtcSetAlarmInteruptPin(true);  // already set in power-up sync
  while (!nrf_gpio_pin_read(BF_SYNC_SYNC_PIN));  // wait till sync pin is high
  nrf_gpio_cfg_sense_set(BF_SYNC_SYNC_PIN, GPIO_PIN_CNF_SENSE_Low);
  nrf_power_system_off(); // infinite loop inside
}
#endif

void restore_timing_hardware(timer_cp* timer_checkpoint, bool sync){
  /* Cycle radio peripheral power to guarantee known radio state. */
  NRF_RADIO->POWER = 0;
  NRF_RADIO->POWER = 1;

  PalBbBleInit();
  PalBbBleEnable();
  PalBbBleEnableDataWhitening(TRUE);

  PalRtcRestore(timer_checkpoint);

#if RECOVER_CONNECTIONS
  uint32_t extRtcCurrentTimeMs = extRtcReadTimeMs();
  uint32_t extRtcSyncTimeMs = timer_checkpoint->ExtRtcSyncAlarm.ExtRtcStartTimeMs;
  uint32_t rtcSyncTick = timer_checkpoint->IntRtcCounter;
  uint32_t currentRtcTick;
  uint32_t currentTimeUsec;
  BbOpDesc_t *pSchHead = schGetPtrToHead();

  if (extRtcCurrentTimeMs > extRtcSyncTimeMs) {
    // we have already passed the scheduled sync time
    // this sync could be network or application, we may need to recover connection
    if(!net_restored) {
      // restore network to check its status
      restore_network();
      net_restored = true;
    }

    if (AppConnIsOpen() != DM_CONN_ID_NONE) {
      // if there is a connection
      currentRtcTick = (PAL_TIMER_US_TO_TICKS((extRtcCurrentTimeMs - extRtcSyncTimeMs) * 1000) + rtcSyncTick) & PAL_MAX_RTC_COUNTER_VAL;    
      currentTimeUsec = BB_TICKS_TO_US(currentRtcTick);
      if (BbGetTargetTimeDelta(pSchHead->dueUsec, currentTimeUsec) == 0) {
        // we have missed a scheduled connection event
        uint32_t timeFromMissedEventUsec = BbGetTargetTimeDelta(currentTimeUsec, pSchHead->dueUsec);
        // if (timeFromMissedEventUsec < 24000000) {
          // recover
          extRtcTimeMs = timer_checkpoint->ExtRtcSyncAlarm.ExtRtcStartTimeMs;
          lctrSlvConnRecover(pSchHead, timeFromMissedEventUsec);
          conn_recovery_ckpt = bfSyncWakeUpScheduleAbsolute(PalRtcCompareGet(3), currentRtcTick, false);
          net_restore_pending = conn_recovery_ckpt;
          return;
        // } else {
        //   // already timeout, reset
        //   checkpoint_restore_invalidate();
        //   NVIC_SystemReset();
        // }
      }
    }
  }
#endif

  if (sync) {
    bfSyncConfigPowerUpSync(timer_checkpoint);
  }
}

void checkpoint_timing_hardware(timer_cp* timer_to_checkpoint){
  NRF_RTC1->TASKS_STOP  = 1;
  if(syncAlarmConfig.ExtRtcStartTimeMs != 0){
    timer_to_checkpoint->ExtRtcSyncAlarm = syncAlarmConfig;
    PalRtcCheckpoint(timer_to_checkpoint);
  }
}
