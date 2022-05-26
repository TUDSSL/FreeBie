#ifndef STACKPOINTER_H_
#define STACKPOINTER_H_

#include <stdint.h>
#include "checkpoint_arch.h"

/*
 * Stack pointer helper functions
 */
__attribute__((noinline,used)) static char *stackpointer_get(void) {
  volatile char sp;
  volatile char *sp_ptr = &sp;

  return (char *)sp_ptr;
}

#endif /* STACKPOINTER_H_ */
