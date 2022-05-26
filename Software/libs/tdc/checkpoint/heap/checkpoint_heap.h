#ifndef CHECKPOINT_HEAP_H_
#define CHECKPOINT_HEAP_H_

#include <stdlib.h>
#include "checkpoint_heap_cfg.h"

extern char heap_checkpoint_nvm_0[CHECKPOINT_HEAP_SIZE];
extern char heap_checkpoint_nvm_1[CHECKPOINT_HEAP_SIZE];

size_t checkpoint_heap(void);
size_t restore_heap(void);

#endif /* CHECKPOINT_HEAP_H_ */
