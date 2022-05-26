/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Stack initialization for smartwatch.
 *
 *  Copyright (c) 2016-2018 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019 Packetcraft, Inc.
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

#include "sec_api.h"
#include "hci_handler.h"
#include "dm_handler.h"
#include "l2c_handler.h"
#include "att_handler.h"
#include "smp_handler.h"
#include "l2c_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_api.h"
#include "hci_core.h"
#include "sec_api.h"
#include "app_terminal.h"

#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
#include "ll_init_api.h"
#endif

#include "pal_bb.h"
#include "pal_cfg.h"

#include "template_api.h"
#include "app_ui.h"

#include "fram.h"
#include "display.h"
#ifdef CHECKPOINT
  #include "checkpoint.h"
  #ifdef CHECKPOINT_TIMING
    #include "platform.h"
    #include "ext-rtc.h"
    #include "bf_sync.h"
  #endif
#endif

#ifdef TRACE_TIMING
  #include "nrf.h"
  #include "nrf_gpio.h"
  #include "nrfx_gpiote.h"
  #include "pal_led.h"
#endif

#include <string.h>

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! \brief UART TX buffer size */
#define PLATFORM_UART_TERMINAL_BUFFER_SIZE      2048U

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/*! \brief  Pool runtime configuration. */
static wsfBufPoolDesc_t mainPoolDesc[] =
{
  { 16,              8 },
  { 32,              4 },
  { 192,             8 },
  { 256,             8 }
};

#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
  static LlRtCfg_t mainLlRtCfg;
#endif

/**************************************************************************************************
  Functions
**************************************************************************************************/

/*! \brief  Stack initialization for app. */
extern void StackInitTemplate(void);

/*************************************************************************************************/
/*!
 *  \brief  Initialize WSF.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void mainWsfInit(void)
{
#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
  /* +12 for message headroom, + 2 event header, +255 maximum parameter length. */
  const uint16_t maxRptBufSize = 12 + 2 + 255;

  /* +12 for message headroom, +4 for header. */
  const uint16_t aclBufSize = 12 + mainLlRtCfg.maxAclLen + 4 + BB_DATA_PDU_TAILROOM;

  /* Adjust buffer allocation based on platform configuration. */
  mainPoolDesc[2].len = maxRptBufSize;
  mainPoolDesc[2].num = mainLlRtCfg.maxAdvReports;
  mainPoolDesc[3].len = aclBufSize;
  mainPoolDesc[3].num = mainLlRtCfg.numTxBufs + mainLlRtCfg.numRxBufs;
#endif

  const uint8_t numPools = sizeof(mainPoolDesc) / sizeof(mainPoolDesc[0]);

  uint16_t memUsed;
  memUsed = WsfBufInit(numPools, mainPoolDesc);
  WsfHeapAlloc(memUsed);
  WsfOsInit();
  WsfTimerInit();
#if (WSF_TOKEN_ENABLED == TRUE) || (WSF_TRACE_ENABLED == TRUE)
  WsfTraceRegisterHandler(WsfBufIoWrite);
  WsfTraceEnable(TRUE);
#endif
}

#ifdef TRACE_TIMING
void SWI0_EGU0_IRQHandler(void) {
  if (NRF_EGU0->EVENTS_TRIGGERED[0]) {
    /* Clear event register */
    NRF_EGU0->EVENTS_TRIGGERED[0] = 0;
    nrf_gpio_pin_set(DEBUG_INFO_BLE);
  } else { 
    /* Clear event register */
    NRF_EGU0->EVENTS_TRIGGERED[1] = 0;
    NRF_EGU0->EVENTS_TRIGGERED[2] = 0;
    nrf_gpio_pin_clear(DEBUG_INFO_BLE);
  }
}

void TraceTimingConfig() {
  NVIC_SetPriority(SWI0_EGU0_IRQn, 2);
  NVIC_EnableIRQ(SWI0_EGU0_IRQn);

  NRF_EGU0->INTENSET = 1 << 0;
  NRF_PPI->CH[1].EEP = (uint32_t) &NRF_RADIO->EVENTS_READY;  /* configure event */
  NRF_PPI->CH[1].TEP = (uint32_t) &NRF_EGU0->TASKS_TRIGGER[0];    /* configure task */
  NRF_PPI->CHENSET = 1 << 1;                            /* enable channel */

  NRF_EGU0->INTENSET = 1 << 1;
  NRF_PPI->CH[2].EEP = (uint32_t) &NRF_RADIO->EVENTS_END;  /* configure event */
  NRF_PPI->CH[2].TEP = (uint32_t) &NRF_EGU0->TASKS_TRIGGER[1];    /* configure task */
  NRF_PPI->CHENSET = 1 << 2;                            /* enable channel */

  NRF_EGU0->INTENSET = 1 << 2;
  NRF_PPI->CH[3].EEP = (uint32_t) &NRF_RADIO->EVENTS_DISABLED;  /* configure event */
  NRF_PPI->CH[3].TEP = (uint32_t) &NRF_EGU0->TASKS_TRIGGER[2];    /* configure task */
  NRF_PPI->CHENSET = 1 << 3;                            /* enable channel */
}
#endif

int main(void) {
#ifdef CHECKPOINT_TIMING
  extRtcConfig();
  bfSyncConfig();
#endif
#ifdef TRACE_TIMING
  TraceTimingConfig();
#endif
  displayConfig();
#ifdef CHECKPOINT
  framConfig();
#ifdef TRACE_TIMING
  nrf_gpio_pin_set(DEBUG_INFO_CKPT);
#endif
  checkpoint_init();
#endif

#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
  /* Configurations must be persistent. */
  static BbRtCfg_t mainBbRtCfg;

  PalBbLoadCfg((PalBbCfg_t *)&mainBbRtCfg);
  LlGetDefaultRunTimeCfg(&mainLlRtCfg);
#if (BT_VER >= LL_VER_BT_CORE_SPEC_5_0)
  /* Set 5.0 requirements. */
  mainLlRtCfg.btVer = LL_VER_BT_CORE_SPEC_5_0;
#endif
  PalCfgLoadData(PAL_CFG_ID_LL_PARAM, &mainLlRtCfg.maxAdvSets, sizeof(LlRtCfg_t) - 9);
#if (BT_VER >= LL_VER_BT_CORE_SPEC_5_0)
  PalCfgLoadData(PAL_CFG_ID_BLE_PHY, &mainLlRtCfg.phy2mSup, 4);
#endif
#endif

  uint32_t memUsed;
//  memUsed = WsfBufIoUartInit(WsfHeapGetFreeStartAddress(), PLATFORM_UART_TERMINAL_BUFFER_SIZE);
//  WsfHeapAlloc(memUsed);

  mainWsfInit();
  AppTerminalInit();

#if defined(HCI_TR_EXACTLE) && (HCI_TR_EXACTLE == 1)
  LlInitRtCfg_t llCfg =
  {
    .pBbRtCfg     = &mainBbRtCfg,
    .wlSizeCfg    = 4,
    .rlSizeCfg    = 4,
    .plSizeCfg    = 4,
    .pLlRtCfg     = &mainLlRtCfg,
    .pFreeMem     = WsfHeapGetFreeStartAddress(),
    .freeMemAvail = WsfHeapCountAvailable()
  };

  memUsed = LlInit(&llCfg);
  WsfHeapAlloc(memUsed);

  bdAddr_t bdAddr;
  PalCfgLoadData(PAL_CFG_ID_BD_ADDR, bdAddr, sizeof(bdAddr_t));
  LlSetBdAddr((uint8_t *)&bdAddr);
  LlMathSetSeed((uint32_t *)&bdAddr);
#endif

  StackInitTemplate();
  TemplateStart();

  WsfOsEnterMainLoop();

  /* Does not return. */
  return 0;
}

/**
 *@}
 **/
