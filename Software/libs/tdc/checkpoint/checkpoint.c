#include <stdint.h>
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "barrier.h"
#include "code-id/code_id.h"
#include "checkpoint_setup_restore.h"
#include "checkpoint.h"
#include "nvm.h"

#include "checkpoint_content.h"

#ifdef CHECKPOINT_TIMING
#include "platform.h"
#include "ext-rtc.h"
#include "bf_sync.h"
#endif

#define PRINT_LOG   (0)
#define PRINT_DEBUG (0)

#if PRINT_DEBUG
#define DEBUG_PRINT(...)    do {printf("[checkpoint-dbg] "); printf(__VA_ARGS__);} while (0)
#else
#define DEBUG_PRINT(...)
#endif

#if PRINT_LOG
#define LOG_PRINT(...)      do {printf("[checkpoint] "); printf(__VA_ARGS__);} while (0)
#else
#define LOG_PRINT(...)
#endif

int16_t adc_conv_result;
checkpoint_pwr_t checkpoint_pwr = CHECKPOINT_PWR_LOW;

bool wsf_timer_is_app;
// bool app_restore_pending; // always restore app on the fly
bool net_restore_pending;
bool app_restored CHECKPOINT_EXCLUDE_BSS;
bool net_restored CHECKPOINT_EXCLUDE_BSS;
bool conn_recovery_ckpt CHECKPOINT_EXCLUDE_BSS;

static inline void checkpoint_commit(void)
{
    barrier;

    volatile lclock_t* tmp_lclock_sub_ptr = checkpoint_sub_get_active_idx();
    lclock_t tmp_lclock_sub[LCLOCK_SUB_CHECKPOINT_SIZE];
    restore_mem(tmp_lclock_sub, (lclock_t*)tmp_lclock_sub_ptr, LCLOCK_SUB_CHECKPOINT_SIZE * sizeof(lclock_t));
    if (net_restored) {
        tmp_lclock_sub[LCLOCK_NETWORK] += 1;
    }
    if (app_restored) {
        tmp_lclock_sub[LCLOCK_APPLICATION] += 1;
    }
    tmp_lclock_sub_ptr = checkpoint_sub_get_next_idx();

    lclock_t tmp_lclock = lclock + 1;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
    write_nvm(tmp_lclock_sub_ptr, &tmp_lclock_sub, LCLOCK_SUB_CHECKPOINT_SIZE * sizeof(lclock_t));
    write_nvm(&lclock, &tmp_lclock, sizeof(lclock));
#pragma GCC diagnostic pop
    LOG_PRINT("Checkpoint committed, new lclock: %d\n", (int)lclock);
}


extern inline bool checkpoint_restore_available(void)
{
    /* Returns the potential new code-id if they are not the same */
    code_id_t new_code_id = code_id_is_new();
    if (new_code_id) {
        return false;
    }
    return true;
}

void checkpoint_restore_invalidate(void)
{
    code_id_clear();
}

void checkpoint_restore_set_availible(void)
{
    code_id_update();
}

// This function must NOT return
__attribute__((noinline)) void restore_process(void) {

    CHECKPOINT_RESTORE_CONTENT();

    /* SHOULD NEVER BE REACHED */
    while (1)
        ;
}

/*
 * Because we restore the stack we can't operate on the normal stack
 * So we need to switch the stack to a dedicated one and restore
 * the stack afterwards
 *
 * NB. This function can NOT use the stack or funny things will happen
 */

__attribute__((noinline)) static void _restore(void) {
    /* Change the restore environment so we don't corrupt the stack while
     * restoring
     */
    checkpoint_setup_restore();

    //// WORKING ON THE SAFE STACK
    restore_process();

    /* SHOULD NEVER BE REACHED */
    while (1)
        ;
}

void checkpoint_restore(void) {
    if (checkpoint_restore_available()) {
        _restore();
    }
}

__attribute__((noinline)) int checkpoint(void) {

    CHECKPOINT_CONTENT();

    // NB: restore point
    if (checkpoint_restored() == 0) {
        /* Normal operation */
        // Commit checkpoint
        checkpoint_commit();

        POST_CHECKPOINT_CONTENT();
    }

    POST_CHECKPOINT_AND_RESTORE_CONTENT();

    return checkpoint_restored();
}

int checkpoint_onetime_setup(void) {
	const lclock_t init_zero[LCLOCK_SUB_CHECKPOINT_SIZE] = {0};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
    write_nvm(&lclock, init_zero, sizeof(lclock_t));
	write_nvm(checkpoint_sub_get_active_idx(), init_zero, LCLOCK_SUB_CHECKPOINT_SIZE * sizeof(lclock_t));
    write_nvm(checkpoint_sub_get_next_idx(), init_zero, LCLOCK_SUB_CHECKPOINT_SIZE * sizeof(lclock_t));
#pragma GCC diagnostic pop

    CHECKPOINT_SETUP_CONTENT();
    net_restored = true;
    app_restored = true;

    return 0;
}

void checkpoint_init()
{
  if (!checkpoint_restore_available()) {
    // First boot
    // Init the .bss and .data (no-restore parts already init in startup)

    // Init the remaining .data
    startup_clear_data();
    startup_clear_data_network();
    startup_clear_data_application();

    // Zero the remaining .bss
    startup_clear_bss();
    startup_clear_bss_network();
    startup_clear_bss_application();

    // Init the heap variables
    startup_init_heap();
  }

//   DEBUG_PRINTF("Checkpoint setup (always)\n");
  checkpoint_setup();

//   DEBUG_PRINTF("Attempting restore\n\n");
  checkpoint_restore();
//   DEBUG_PRINTF("No restore available\n\n");

//   DEBUG_PRINTF("One-time checkpoint setup\n");
  checkpoint_onetime_setup();

  // DEBUG_PRINTF("After init checkpoint\n\n");
  // checkpoint();

  // checkpoint_restore_set_availible();
}
