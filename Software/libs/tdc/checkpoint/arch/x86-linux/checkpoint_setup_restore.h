#ifndef CHECKPOINT_SETUP_RESTORE_H_
#define CHECKPOINT_SETUO_RESTORE_H_

#include "checkpoint_arch.h"
#include "stackpointer.h"

// Move the spackpointer to the safe stack where the restore code is executed
void checkpoint_setup_restore(void);


#endif /* CHECKPOINT_SETUP_RESTORE_H_ */
