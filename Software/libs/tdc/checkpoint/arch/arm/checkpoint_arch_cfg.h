#ifndef CHECKPOINT_ARCH_CFG_H_
#define CHECKPOINT_ARCH_CFG_H_

#include <stdint.h>

extern uint32_t _erestore_stack;

#define checkpoint_restore_stack_top (&_erestore_stack)

#define LCLOCK_NETWORK 0
#define LCLOCK_APPLICATION 1
#define LCLOCK_SUB_CHECKPOINT_SIZE 2

#endif /* CHECKPOINT_ARCH_CFG_H_ */
