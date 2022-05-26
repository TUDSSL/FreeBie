#ifndef CHECKPOINT_STACK_CFG_H_
#define CHECKPOINT_STACK_CFG_H_

#include <stdint.h>

extern uint32_t _estack;

#define stack_top (&_estack)

// The maximum size of the stack to checkpoint
#define CHECKPOINT_STACK_SIZE   4096


#endif /* CHECKPOINT_STACK_CFG_H_ */
