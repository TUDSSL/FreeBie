#include "nvm.h"
#include "checkpoint_arch.h"

nvm volatile lclock_t lclock;

// work around for nrf errata with pointers in external memory
nvm volatile lclock_t lclock_sub_checkpoint_0[LCLOCK_SUB_CHECKPOINT_SIZE];
nvm volatile lclock_t lclock_sub_checkpoint_1[LCLOCK_SUB_CHECKPOINT_SIZE];
