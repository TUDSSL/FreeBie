#include <stdlib.h>
#include "checkpoint_selector.h"

#include "stackpointer.h"

#include "checkpoint_util_mem.h"
#include "checkpoint_heap.h"

struct heap_cp {
    size_t heap_size;
    char data[];
};

//size_t heap_size(char *heap_ptr) {
//  size_t heap_size = heap_get_size() - heap_start
//  size_t heap_size = (uintptr_t)stack_top - (uintptr_t)heap_ptr;
//  return heap_size;
//}

static inline char *heap_get_active_checkpoint(void) {
    char *cp;

    if (checkpoint_get_active_idx() == 0)
        cp = heap_checkpoint_nvm_0;
    else
        cp = heap_checkpoint_nvm_1;

    return cp;
}


static inline char *heap_get_restore_checkpoint(void) {
    char *cp;

    if (checkpoint_get_restore_idx() == 0)
        cp = heap_checkpoint_nvm_0;
    else
        cp = heap_checkpoint_nvm_1;

    return cp;
}

/*
 * heap checkpoint and restore
 */
size_t checkpoint_heap(void) {
  struct heap_cp *cp = (struct heap_cp *)heap_get_active_checkpoint();

  char* heap_ptr = (char *)heap_start;
  size_t size = heap_size();

  write_nvm(&cp->heap_size, &size, sizeof(size)); // todo might be trouble with the errata
  if(size){
    checkpoint_mem(cp->data, heap_ptr, size);
  }
  return size;
}

size_t restore_heap(void) {
  struct heap_cp *cp = (struct heap_cp *)heap_get_restore_checkpoint();

  char* heap_ptr = (char *)heap_start;
  size_t heap_size = cp->heap_size; // todo might be trouble with the errata
  if(heap_size){
    restore_mem(heap_ptr, cp->data, heap_size);
  }
  return heap_size;
}
