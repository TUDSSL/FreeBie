#ifndef LIBS_TDC_CHECKPOINT_TIMING_CHECKPOINT_TIMING_H_
#define LIBS_TDC_CHECKPOINT_TIMING_CHECKPOINT_TIMING_H_

#include "checkpoint_timing_config.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct TimerCP timer_cp;

extern timer_cp auto_timing_chkpt;

// hardware specific must be implemented for each architecture
extern void restore_timing_hardware(timer_cp* timer_checkpoint, bool sync);
extern void checkpoint_timing_hardware(timer_cp* timer_to_checkpoint);

size_t checkpoint_timing(void);
size_t restore_timing(bool sync);

#endif /* LIBS_TDC_CHECKPOINT_TIMING_CHECKPOINT_TIMING_H_ */
