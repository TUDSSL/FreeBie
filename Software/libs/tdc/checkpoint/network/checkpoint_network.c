#include <stdlib.h>
#include "checkpoint_selector.h"
#include "checkpoint_logical_clock.h"
#include "barrier.h"

#include "checkpoint_util_mem.h"
#include "checkpoint_network.h"

__attribute__((section(".allocate_checkpoint_flags")))
char _checkpoint_network_allocate_checkpoint_ld;

static inline size_t network_size_data(void) {
  size_t data_size = (size_t)checkpoint_network_data_0_end - (size_t)checkpoint_network_data_0_start;
  return data_size;
}

static inline size_t network_size_bss(void) {
  size_t bss_size = (size_t)checkpoint_network_bss_0_end - (size_t)checkpoint_network_bss_0_start;
  return bss_size;
}

static inline char *data_get_active_checkpoint(void) {
    char *cp;
    // * implement workaround for errata 216 QSPI race condition in XIP
    volatile lclock_t *lclock_sub_ptr = checkpoint_sub_get_active_idx();
    __NOP();
    volatile lclock_t lclock_sub = lclock_sub_ptr[LCLOCK_NETWORK];
    if (lclock_sub % 2 == 0)
        cp = (char *)checkpoint_network_data_0_start;
    else
        cp = (char *)checkpoint_network_data_1_start;

    return cp;
}

static inline char *data_get_restore_checkpoint(void) {
    char *cp;
    // * implement workaround for errata 216 QSPI race condition in XIP
    volatile lclock_t *lclock_sub_ptr = checkpoint_sub_get_active_idx();
    __NOP();
    volatile lclock_t lclock_sub = lclock_sub_ptr[LCLOCK_NETWORK] - 1;
    if (lclock_sub % 2 == 0)
        cp = (char *)checkpoint_network_data_0_start;
    else
        cp = (char *)checkpoint_network_data_1_start;

    return cp;
}

static inline char *bss_get_active_checkpoint(void) {
    char *cp;
    // * implement workaround for errata 216 QSPI race condition in XIP
    volatile lclock_t *lclock_sub_ptr = checkpoint_sub_get_active_idx();
    __NOP();
    volatile lclock_t lclock_sub = lclock_sub_ptr[LCLOCK_NETWORK];
    if (lclock_sub % 2 == 0)
        cp = (char *)checkpoint_network_bss_0_start;
    else
        cp = (char *)checkpoint_network_bss_1_start;

    return cp;
}

static inline char *bss_get_restore_checkpoint(void) {
    char *cp;
    // * implement workaround for errata 216 QSPI race condition in XIP
    volatile lclock_t *lclock_sub_ptr = checkpoint_sub_get_active_idx();
    __NOP();
    volatile lclock_t lclock_sub = lclock_sub_ptr[LCLOCK_NETWORK] - 1;
    if (lclock_sub % 2 == 0)
        cp = (char *)checkpoint_network_bss_0_start;
    else
        cp = (char *)checkpoint_network_bss_1_start;

    return cp;
}

/*
 * Network checkpoint and restore
 */
size_t checkpoint_network(void) {
  char* cp = data_get_active_checkpoint();
  char* data_ptr = (char*)checkpoint_network_data_start;
  size_t size_data = network_size_data();

  checkpoint_mem(cp, data_ptr, size_data);

  cp = bss_get_active_checkpoint();
  char* bss_ptr = (char*)checkpoint_network_bss_start;
  size_t size_bss = network_size_bss();

  checkpoint_mem(cp, bss_ptr, size_bss);
  return size_data + size_bss;
}

size_t restore_network(void) {
  char* cp = data_get_restore_checkpoint();
  char* data_ptr = (char*)checkpoint_network_data_start;
  size_t size_data = network_size_data();

  restore_mem(data_ptr, cp, size_data);

  cp = bss_get_restore_checkpoint();
  char* bss_ptr = (char*)checkpoint_network_bss_start;
  size_t size_bss = network_size_bss();

  restore_mem(bss_ptr, cp, size_bss);
  return size_data + size_bss;
}
