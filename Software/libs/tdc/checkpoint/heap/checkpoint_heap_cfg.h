#ifndef CHECKPOINT_HEAP_CFG_H_
#define CHECKPOINT_HEAP_CFG_H_

#include <stdint.h>

extern uint32_t WsfHeapCountUsed();


extern uint32_t _estack;
extern uint32_t  __heap_start__;

#define heap_start (&__heap_start__)
#define heap_size() WsfHeapCountUsed()



// The maximum size of the heap to checkpoint
#define CHECKPOINT_HEAP_SIZE   8192


#endif /* CHECKPOINT_HEAP_CFG_H_ */
