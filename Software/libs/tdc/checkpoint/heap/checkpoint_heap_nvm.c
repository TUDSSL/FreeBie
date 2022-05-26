/******************************************************************************
 * This file contains non-volatile memory data structures                     *
 ******************************************************************************/

#include "nvm.h"
#include "checkpoint_heap_cfg.h"

//__attribute__((aligned(__alignof__(char *))))
nvm char heap_checkpoint_nvm_0[CHECKPOINT_HEAP_SIZE];

//__attribute__((aligned(__alignof__(char *))))
nvm char heap_checkpoint_nvm_1[CHECKPOINT_HEAP_SIZE];
