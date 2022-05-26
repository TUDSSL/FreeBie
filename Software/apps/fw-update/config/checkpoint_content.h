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

#ifndef CHECKPOINT_CONTENT_H_
#define CHECKPOINT_CONTENT_H_

#include <stdlib.h>

#include "platform.h"

#include "checkpoint_bss.h"
#include "checkpoint_data.h"
#include "checkpoint_stack.h"
#include "checkpoint_heap.h"
#include "checkpoint_registers.h"
#include "checkpoint_network.h"
#include "checkpoint_application.h"
#include "checkpoint.h"
#ifdef CHECKPOINT_TIMING
#include "checkpoint_timing.h"
#endif
#ifdef TRACE_TIMING
#include "nrfx_gpiote.h"
#endif

extern void startup_clear_data(void);
extern void startup_clear_bss(void);
extern void startup_init_heap(void);
extern void startup_clear_data_network(void);
extern void startup_clear_bss_network(void);
extern void startup_clear_data_application(void);
extern void startup_clear_bss_application(void);

__attribute__((always_inline))
static inline void CHECKPOINT_SETUP_CONTENT(void) {
//  ..();
}

/*
 * Actions to be performed for a checkpoint
 */
__attribute__((always_inline))
static inline void CHECKPOINT_CONTENT(void) {
#ifdef CHECKPOINT_TIMING
  checkpoint_timing();
#endif
  if (app_restored) {
    nrf_gpio_pin_set(DEBUG_INFO_APP_CKPT);
    checkpoint_application();
    nrf_gpio_pin_clear(DEBUG_INFO_APP_CKPT);
  }
  if (net_restored) {
    nrf_gpio_pin_set(DEBUG_INFO_NET_CKPT);
    checkpoint_network();
    nrf_gpio_pin_clear(DEBUG_INFO_NET_CKPT);
  }
  checkpoint_data();
  checkpoint_bss();
  checkpoint_heap();
  checkpoint_stack();
  checkpoint_registers(); // MUST BE LAST
}

/*
 * Actions to be performed for a restore
 */
__attribute__((always_inline))
static inline void CHECKPOINT_RESTORE_CONTENT(void) {
  restore_data();
  restore_bss();
  restore_heap();
  restore_stack();
  if (net_restore_pending) {
    net_restore_pending = false;
    nrf_gpio_pin_set(DEBUG_INFO_NET_CKPT);
    restore_network();
    nrf_gpio_pin_clear(DEBUG_INFO_NET_CKPT);
    net_restored = true;
  }
#ifdef TRACE_TIMING
  nrf_gpio_pin_clear(DEBUG_INFO_CKPT);
#endif
#ifdef CHECKPOINT_TIMING
  restore_timing(true);
#endif
  restore_registers(); // MUST BE LAST
}

/*
 * Actions to be performed after a successful checkpoint
 */
__attribute__((always_inline))
static inline void POST_CHECKPOINT_CONTENT(void) {
  if (!checkpoint_restore_available()) {
    checkpoint_restore_set_availible();
  }
#ifdef TRACE_TIMING
  nrf_gpio_pin_clear(DEBUG_INFO_CKPT);
#endif
}

/*
 * Actions to be performed after a successful checkpoint OR restore
 */
__attribute__((always_inline))
static inline void POST_CHECKPOINT_AND_RESTORE_CONTENT(void) {
}



#endif /* CHECKPOINT_CONTENT_H_ */
