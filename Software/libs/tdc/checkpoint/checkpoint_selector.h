#ifndef CHECKPOINT_SELECTOR_H_
#define CHECKPOINT_SELECTOR_H_

#include "checkpoint_logical_clock.h"

#define checkpoint_get_active_idx()     (lclock%2)
#define checkpoint_get_restore_idx()    ((lclock+1)%2)

//__attribute__((always_inline))
static inline volatile lclock_t* checkpoint_sub_get_active_idx(void) {
    volatile lclock_t *cp;
    if (checkpoint_get_active_idx() == 0)
        cp = lclock_sub_checkpoint_0;
    else
        cp = lclock_sub_checkpoint_1;

    return cp;
}

//__attribute__((always_inline))
static inline volatile lclock_t* checkpoint_sub_get_next_idx(void) {
    volatile lclock_t *cp;
    if (checkpoint_get_restore_idx() == 0)
        cp = lclock_sub_checkpoint_0;
    else
        cp = lclock_sub_checkpoint_1;

    return cp;
}

#endif /* CHECKPOINT_SELECTOR_H_ */
