#ifndef MICROPY_INCLUDED_LIB_CHECKPOINT_CHECKPOINT_H_
#define MICROPY_INCLUDED_LIB_CHECKPOINT_CHECKPOINT_H_

#include <stdint.h>
#include <stdbool.h>

#include "checkpoint_arch.h"
#include "checkpoint_cfg.h"
#include "checkpoint_setup.h"

typedef enum {
    CHECKPOINT_PWR_LOW,
    CHECKPOINT_PWR_NORMAL
} checkpoint_pwr_t;

extern int16_t adc_conv_result;
extern checkpoint_pwr_t checkpoint_pwr;

extern bool wsf_timer_is_app;
// extern bool app_restore_pending; // always restore app on the fly
extern bool net_restore_pending;
extern bool app_restored CHECKPOINT_EXCLUDE_BSS;
extern bool net_restored CHECKPOINT_EXCLUDE_BSS;
extern bool conn_recovery_ckpt CHECKPOINT_EXCLUDE_BSS;

extern void applyConnParamNormal();
extern void applyConnParamLow();

void checkpoint_restore(void);
int checkpoint(void);
int checkpoint_onetime_setup(void);

/* Restore management */
bool checkpoint_restore_available(void);
void checkpoint_restore_invalidate(void);
void checkpoint_restore_set_availible(void);

void checkpoint_init();

#endif // MICROPY_INCLUDED_LIB_CHECKPOINT_CHECKPOINT_H_
