#ifndef CHECKPOINT_REGISTERS_H_
#define CHECKPOINT_REGISTERS_H_

#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

#include "checkpoint_arch.h"
#include "checkpoint_selector.h"

typedef sigjmp_buf register_cp_t;

// NVM Checkpoint
extern register_cp_t registers_checkpoint_nvm[2];

#define checkpoint_registers() sigsetjmp(registers_checkpoint_nvm[checkpoint_get_active_idx()], SIGQUIT);

#define restore_registers() siglongjmp(registers_checkpoint_nvm[checkpoint_get_restore_idx()], 1);

#endif /* CHECKPOINT_REGISTERS_H_ */
