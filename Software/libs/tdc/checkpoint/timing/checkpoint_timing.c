#include <stdlib.h>
#include "checkpoint_selector.h"

#include "checkpoint_util_mem.h"
#include "checkpoint_timing.h"

timer_cp auto_timing_chkpt;

/*
 * heap checkpoint and restore
 */
size_t checkpoint_timing(void) {
  checkpoint_timing_hardware(&auto_timing_chkpt);
  return sizeof(timer_cp);
}

size_t restore_timing(bool sync) {
  restore_timing_hardware(&auto_timing_chkpt, sync);

  return sizeof(timer_cp);
}
