#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

#include <setjmp.h>

#include <ucontext.h>

#include "checkpoint_setup_restore.h"
#include "checkpoint_cfg.h"

CHECKPOINT_EXCLUDE_BSS
ucontext_t restore_ctx;


CHECKPOINT_EXCLUDE_BSS
__attribute__ ((aligned (16)))
char _restore_stack[RESTORE_STACK_SIZE];


extern void restore_process(void);

void checkpoint_setup_restore(void)
{
    getcontext(&restore_ctx);
    restore_ctx.uc_stack.ss_sp = _restore_stack;
    restore_ctx.uc_stack.ss_size = sizeof(_restore_stack);
    restore_ctx.uc_link = NULL;
    makecontext(&restore_ctx, restore_process, 0);
    setcontext(&restore_ctx);
}

