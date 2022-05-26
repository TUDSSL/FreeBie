#ifndef CHECKPOINT_LOGICAL_CLOCK_H_
#define CHECKPOINT_LOGICAL_CLOCK_H_

#include "checkpoint_arch.h"

extern volatile lclock_t lclock;

extern volatile lclock_t lclock_sub_checkpoint_0[LCLOCK_SUB_CHECKPOINT_SIZE];
extern volatile lclock_t lclock_sub_checkpoint_1[LCLOCK_SUB_CHECKPOINT_SIZE];

#endif /* CHECKPOINT_LOGICAL_CLOCK_H_ */
