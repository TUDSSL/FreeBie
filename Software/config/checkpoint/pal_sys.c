/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  System hooks.
 *
 *  Copyright (c) 2013-2019 Arm Ltd. All Rights Reserved.
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
 */
/*************************************************************************************************/

#include "nrf.h"
#include "pal_rtc.h"
#include "pal_sys.h"
#include "pal_led.h"
#include "pal_timer.h"
#include "pal_bb.h"
#include "pal_uart.h"

#include <string.h>

#ifdef CHECKPOINT
  #include "checkpoint.h"
  #include "app_main.h"
  #include "platform.h"
  #include "nrf_saadc.h"
  #include "nrf_gpio.h"
  #include "pal_bb_ble.h"
  #ifdef CHECKPOINT_TIMING
    #include "bb_api.h"
    #include "sch_api.h"
    #include "ext-rtc.h"
    #include "bf_sync.h"
    #ifdef DEBUG_SYSTEMOFF
      #include "nrf_power.h"
    #endif
  #endif
#endif

/**************************************************************************************************
  Macros
**************************************************************************************************/

#ifdef __GNUC__

/*! \brief      Stack initial values. */
#define INIT_STACK_VAL      0xAFAFAFAF

/*! \brief      Starting memory location of free memory. */
#define FREE_MEM_START      ((uint8_t *)&__heap_start__)

/*! \brief      Total size in bytes of free memory. */
#define FREE_MEM_SIZE       ((uint32_t)&__heap_end__ - (uint32_t)&__heap_start__)

extern uint8_t *SystemHeapStart;
extern uint32_t SystemHeapSize;

extern unsigned long __text_end__;
extern unsigned long __data_start__;
extern unsigned long __data_end__;
extern unsigned long __bss_start__;
extern unsigned long __bss_end__;
extern unsigned long __stack_top__;
extern unsigned long __stack_limit__;
extern unsigned long __heap_end__;
extern unsigned long __heap_start__;

#else

/*! \brief      Starting memory location of free memory. */
#define FREE_MEM_START      ((uint8_t *)palSysFreeMem)

/*! \brief      Total size in bytes of free memory. */
#define FREE_MEM_SIZE       (1024 * 196)

#endif

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

#ifdef __GNUC__

uint8_t *SystemHeapStart;
uint32_t SystemHeapSize;

#else

/*! \brief      Free memory for pool buffers (align to word boundary). */
uint32_t palSysFreeMem[FREE_MEM_SIZE/sizeof(uint32_t)];

uint8_t *SystemHeapStart = (uint8_t *) palSysFreeMem;
uint32_t SystemHeapSize = FREE_MEM_SIZE;

#endif

/*! \brief      Number of assertions. */
static uint32_t palSysAssertCount;

/*! \brief      Trap enabled flag. */
static volatile bool_t PalSysAssertTrapEnable;

static uint32_t palSysBusyCount;

/**************************************************************************************************
  Functions
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief  Enter a critical section.
 */
/*************************************************************************************************/
void PalEnterCs(void)
{
  #ifdef __IAR_SYSTEMS_ICC__
      __disable_interrupt();
  #endif
  #ifdef __GNUC__
      __asm volatile ("cpsid i");
  #endif
  #ifdef __CC_ARM
      __disable_irq();
  #endif
}

/*************************************************************************************************/
/*!
 *  \brief  Exit a critical section.
 */
/*************************************************************************************************/
void PalExitCs(void)
{
  #ifdef __IAR_SYSTEMS_ICC__
      __enable_interrupt();
  #endif
  #ifdef __GNUC__
      __asm volatile ("cpsie i");
  #endif
  #ifdef __CC_ARM
      __enable_irq();
  #endif
}

/*************************************************************************************************/
/*!
 *  \brief      Common platform initialization.
 */
/*************************************************************************************************/
void PalSysInit(void)
{
  NRF_POWER->DCDCEN0 = 1;
  NRF_POWER->DCDCEN = 1;

  /* Enable Flash cache */
  NRF_NVMC->ICACHECNF |= (NVMC_ICACHECNF_CACHEEN_Enabled << NVMC_ICACHECNF_CACHEEN_Pos);

  /* Use 16 MHz crystal oscillator (system starts up using 16MHz RC oscillator). */
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART    = 1;
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) { }

  palSysAssertCount = 0;
  PalSysAssertTrapEnable = TRUE;
  palSysBusyCount = 0;

  // Button pin
  nrf_gpio_cfg_input(18, NRF_GPIO_PIN_PULLUP);

  PalRtcInit();
  PalLedInit();

  // PalLedOff(PAL_LED_ID_ERROR);
  // PalLedOn(PAL_LED_ID_CPU_ACTIVE);

  if (nrf_gpio_pin_read(18) == 0) {
    int i = 5;
    while(1){
      // for debugger connection
      i++;
    }
  }

#if defined(DEBUG) && !defined(CHECKPOINT)
  /* Reset free memory. */
  memset(SystemHeapStart, 0, SystemHeapSize);
#endif
}

/*************************************************************************************************/
/*!
 *  \brief      System fault trap.
 */
/*************************************************************************************************/
void PalSysAssertTrap(void)
{
  PalLedOn(PAL_LED_ID_ERROR);

  palSysAssertCount++;

  while (PalSysAssertTrapEnable);
}

/*************************************************************************************************/
/*!
 *  \brief      Set system trap.
 *
 *  \param      enable    Enable assert trap or not.
 */
/*************************************************************************************************/
void PalSysSetTrap(bool_t enable)
{
  PalSysAssertTrapEnable = enable;
}

/*************************************************************************************************/
/*!
 *  \brief      Get assert count.
 */
/*************************************************************************************************/
uint32_t PalSysGetAssertCount(void)
{
  return palSysAssertCount;
}

/*************************************************************************************************/
/*!
 *  \brief      Count stack usage.
 *
 *  \return     Number of bytes used by the stack.
 */
/*************************************************************************************************/
uint32_t PalSysGetStackUsage(void)
{
#ifdef __GNUC__
  unsigned long *pUnused = &__stack_limit__;

  while (pUnused < &__stack_top__)
  {
    if (*pUnused != INIT_STACK_VAL)
    {
      break;
    }

    pUnused++;
  }

  return (uint32_t)(&__stack_top__ - pUnused) * sizeof(*pUnused);
#else
  /* Not available; stub routine. */
  return 0;
#endif
}

/*************************************************************************************************/
/*!
 *  \brief      System sleep.
 */
/*************************************************************************************************/
#ifdef CHECKPOINT
void adcConvertVdd() {
  // peripheral config
  NRF_SAADC->RESOLUTION = NRF_SAADC_RESOLUTION_12BIT;
  NRF_SAADC->OVERSAMPLE = NRF_SAADC_OVERSAMPLE_DISABLED;
  NRF_SAADC->INTENCLR = NRF_SAADC_INT_ALL;
  NRF_SAADC->EVENTS_END = 0;
  NRF_SAADC->EVENTS_STARTED = 0;
  NRF_SAADC->EVENTS_STOPPED = 0;
  // NRF_SAADC->INTENSET = NRF_SAADC_INT_STARTED || NRF_SAADC_INT_END;
  NRF_SAADC->ENABLE = NRFX_SAADC_ENABLED;

  // channel config
  NRF_SAADC->CH[0].CONFIG = (SAADC_CH_CONFIG_TACQ_40us << SAADC_CH_CONFIG_TACQ_Pos) & SAADC_CH_CONFIG_TACQ_Msk;
  NRF_SAADC->CH[0].PSELN = SAADC_CH_PSELN_PSELN_NC;
  #if HWREV < 4
  NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_VDD;
  #else
  NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_AnalogInput4; // resistor divided 2/3 VBAT
  nrf_gpio_cfg_output(ADC_VBAT_EN);
  nrf_gpio_pin_set(ADC_VBAT_EN);
  #endif
  // prepare and start
  NRF_SAADC->RESULT.PTR = (uint32_t)&adc_conv_result;
  NRF_SAADC->RESULT.MAXCNT = 1;
  NRF_SAADC->TASKS_START = 1;
  while (NRF_SAADC_EVENT_STARTED == 0);
  NRF_SAADC->EVENTS_STARTED = 0;
  NRF_SAADC->TASKS_SAMPLE = 1;

  // wait for sample finish, could be optimized
  while (NRF_SAADC->EVENTS_END == 0);
  NRF_SAADC->EVENTS_END = 0;
}

void adaptConnParamToVdd() {
  static const int VDD_AVG_POW = 1;
  static const int VDD_DIFF_AVG_POW = 5;
  static const int VDD_FORECAST_DIFF_POW = 3;
  static int32_t vdd_avg, vdd_diff_avg;
  #if HWREV < 4
  static int32_t vdd_last = 0x0800;  // initial value 1.8 V
  #else
  static int32_t vdd_last = 0x07C3;  // initial value 1.74 V (actual VBAT = 1.74 * 3 / 2 = 2.61 V)
  #endif
  int32_t vdd_cur, vdd_diff, vdd_forecast;

  vdd_cur = adc_conv_result;
  vdd_diff = vdd_cur - vdd_last;
  vdd_avg = ((vdd_avg << VDD_AVG_POW) - vdd_avg + vdd_cur) >> VDD_AVG_POW;
  vdd_diff_avg = ((vdd_diff_avg << VDD_DIFF_AVG_POW) - vdd_diff_avg + vdd_diff) >> VDD_DIFF_AVG_POW;
  vdd_forecast = vdd_avg + (vdd_diff_avg << VDD_FORECAST_DIFF_POW);

  #if HWREV < 4
  if (vdd_forecast > 0x0E00) {  // 3.15 V
  #else
  if (vdd_forecast > 0x07C3) {  // 1.74 V (actual VBAT = 1.74 * 3 / 2 = 2.61 V)
  #endif
    if (checkpoint_pwr == CHECKPOINT_PWR_LOW) {
      dmConnId_t myConnId = AppConnIsOpen();
      if (myConnId > 0) {
        applyConnParamNormal();
        appConnUpdateTimerStart(myConnId);
      }
      checkpoint_pwr = CHECKPOINT_PWR_NORMAL;
    }
  #if HWREV < 4
  } else if (vdd_forecast < 0x0C00) {  // 2.7 V
  #else
  } else if (vdd_forecast < 0x071C) { // actual VBAT 2.4 V
  #endif
    if (checkpoint_pwr == CHECKPOINT_PWR_NORMAL) {
      dmConnId_t myConnId = AppConnIsOpen();
      if (myConnId > 0) {
        applyConnParamLow();
        appConnUpdateTimerStart(myConnId);
      }
      checkpoint_pwr = CHECKPOINT_PWR_LOW;
    }
  }

  vdd_last = vdd_cur;
}
#endif

void PalSysSleep(void)
{
#ifdef CHECKPOINT
#ifdef CHECKPOINT_TIMING
  const uint32_t supervisionTimeoutTicks = 32 * 32768; // max allowed supervision timeout 32 s
  uint32_t wsfToPalTicks;
  // uint32_t palToWsfTicks;
  uint32_t wsfEventTicks = PalRtcCompareGet(0);
  // uint32_t wsfSyncTicks = wsfEventTicks - (wsfEventTicks % 8192);
  // uint32_t wsfTurnOnTicks = wsfSyncTicks - BB_US_TO_BB_TICKS((T_Restore + T_Margin) * 1000);
  uint32_t palEventTicks = PalRtcCompareGet(3);
  uint32_t palSyncTicks = palEventTicks - (palEventTicks % 8192);
  uint32_t palTurnOnTicks = palSyncTicks - BB_US_TO_BB_TICKS((T_Restore + T_Margin) * 1000);
  uint32_t relativeSleepTicks = 0;
  uint32_t nextEventRtcTicks;
  uint32_t currentRtcTicks = PalRtcCounterGet();

  if ( net_restored && (NRF_RTC1->CC[3] < NRF_RTC1->COUNTER)   ) {
  // if ( net_restored && (palEventTicks < currentRtcTicks) ) {
    // low level scheduler still executing
    // __WFI();
    return;
  }

  if (wsf_timer_is_app) {
    if ( (NRF_RTC1->EVTEN & RTC_EVTEN_COMPARE0_Msk) && (NRF_RTC1->EVTEN & RTC_EVTEN_COMPARE3_Msk) ) {
      wsfToPalTicks = (palTurnOnTicks - wsfEventTicks) & PAL_MAX_RTC_COUNTER_VAL;
      if (wsfToPalTicks > supervisionTimeoutTicks) {
        wsfToPalTicks = 0;
      }
      net_restore_pending = wsfToPalTicks > 8192 ? false : true;
    } else if ( !(NRF_RTC1->EVTEN & RTC_EVTEN_COMPARE0_Msk) && (NRF_RTC1->EVTEN & RTC_EVTEN_COMPARE3_Msk) ) {
      net_restore_pending = true;
    } else if ( (NRF_RTC1->EVTEN & RTC_EVTEN_COMPARE0_Msk) && !(NRF_RTC1->EVTEN & RTC_EVTEN_COMPARE3_Msk) ) {
      net_restore_pending = false;
    } else {
      // just to make code complete, but this should not happen
      while(1); // debug trap
    }
  } else {
    net_restore_pending = true;
  }

  if ( (NRF_RTC1->EVTEN & RTC_EVTEN_COMPARE0_Msk) && (NRF_RTC1->CC[0] < NRF_RTC1->CC[3]) ) {
  // if ( (NRF_RTC1->EVTEN & RTC_EVTEN_COMPARE0_Msk) && (wsfEventTicks < palEventTicks) ) {
    relativeSleepTicks = NRF_RTC1->CC[0] - NRF_RTC1->COUNTER;
    // relativeSleepTicks = wsfEventTicks - currentRtcTicks;
    nextEventRtcTicks = wsfEventTicks;
  } else {
    relativeSleepTicks = NRF_RTC1->CC[3] - NRF_RTC1->COUNTER;
    // relativeSleepTicks = palEventTicks - currentRtcTicks;
    nextEventRtcTicks = palEventTicks;
  }

  if (relativeSleepTicks >= supervisionTimeoutTicks) {
    // ! should not reach here
    // ? somehow we will reach here at the 2nd conn event after recovery
    // for debug purpose
    volatile uint32_t rtc1_cnt_with_offset = PalRtcCounterGet();
    volatile uint32_t rtc1_cnt_raw = NRF_RTC1->COUNTER;
    volatile uint32_t rtc1_cc0_raw = NRF_RTC1->CC[0];
    volatile uint32_t rtc1_cc3_raw = NRF_RTC1->CC[3];
    (void)rtc1_cnt_with_offset;
    (void)rtc1_cnt_raw;
    (void)rtc1_cc0_raw;
    (void)rtc1_cc3_raw;
    // nrf_gpio_pin_set(DEBUG_INFO_APP);
    // while(1);
    relativeSleepTicks = 0;
    nextEventRtcTicks = currentRtcTicks;
  }
  PalExitCs();

  // bool checkpoint_pending = bfSyncWakeUpScheduleDiff( BB_TICKS_TO_US(relativeSleepTicks) / 1000, false);
  bool checkpoint_pending = bfSyncWakeUpScheduleAbsolute(nextEventRtcTicks, currentRtcTicks, false);

  if (checkpoint_pending) {
    do {
      conn_recovery_ckpt = false;

    #ifdef TRACE_TIMING
      nrf_gpio_pin_set(DEBUG_INFO_CKPT);
    #endif
      if (checkpoint()) // return true if restoring
      {
        // post-restore actions: reinit Pal Bb
        // adcConvertVdd();
        // adaptConnParamToVdd();
      }
      else
      {
        // post-checkpoint actions
      #ifdef USE_AMBIQ_LFCLK
        NRF_CLOCK->TASKS_LFCLKSTOP = 1;
        NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos);
        extRtcSetLfclkOut(FALSE);
      #endif

      #ifndef DEBUG_SYSTEMOFF
        extRtcShutDownMCUAlarm();
        while(1);
      #else
        bfSyncDebugSystemoff();  // infinite loop inside
      #endif
        // should NOT continue after this line
      }

    } while (conn_recovery_ckpt);
  }

#endif
#endif

  /* Clock management for low power mode. */
  #if BB_CLK_RATE_HZ == 32768 && !defined(CHECKPOINT_TIMING)
  #ifdef CHECKPOINT
    uint32_t rtcNow = RTC1_COUNTER_VAL;
  #else
    uint32_t rtcNow = NRF_RTC1->COUNTER;
  #endif

    if ((BbGetCurrentBod() == NULL) && PalUartGetState(PAL_UART_ID_CHCI) == PAL_UART_STATE_UNINIT)
    {
      if ((PalTimerGetState() == PAL_TIMER_STATE_BUSY &&
          ((PalRtcCompareGet(3) - rtcNow) & PAL_MAX_RTC_COUNTER_VAL) > PAL_HFCLK_OSC_SETTLE_TICKS) ||
          (PalTimerGetState() == PAL_TIMER_STATE_READY))
      {
        /* disable HFCLK */
        NRF_CLOCK->TASKS_HFCLKSTOP = 1;
        NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
        (void)NRF_CLOCK->EVENTS_HFCLKSTARTED;
      }
    }
  #endif

  /* CPU sleep. */
  #ifdef __IAR_SYSTEMS_ICC__
    __wait_for_interrupt();
  #endif
  #ifdef __GNUC__
          __asm volatile ("wfi");
  #endif
  #ifdef __CC_ARM
    __wfi();
  #endif
}

/*************************************************************************************************/
/*!
 *  \brief      Check if system is busy.
 *
 *  \return     TRUE if system is busy.
 */
/*************************************************************************************************/
bool_t PalSysIsBusy(void)
{
  bool_t sysIsBusy = FALSE;
  PalEnterCs();
  sysIsBusy = ((palSysBusyCount == 0) ? FALSE : TRUE);
  PalExitCs();
  return sysIsBusy;
}

/*************************************************************************************************/
/*!
 *  \brief      Set system busy.
 *
 *  \return     none.
 */
/*************************************************************************************************/
void PalSysSetBusy(void)
{
  PalEnterCs();
  palSysBusyCount++;
  PalExitCs();
}

/*************************************************************************************************/
/*!
 *  \brief      Set system idle.
 */
/*************************************************************************************************/
void PalSysSetIdle(void)
{
  PalEnterCs();
  if (palSysBusyCount)
  {
    palSysBusyCount--;
  }
  PalExitCs();
}
