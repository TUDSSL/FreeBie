#ifndef CHECKPOINT_MEM_H_
#define CHECKPOINT_MEM_H_

#include <string.h>

#include "fram.h"

#define checkpoint_mem      framWrite
#define restore_mem         memcpy
#define checkpoint_memcpy   memcpy
#define write_nvm           checkpoint_mem

#endif /* CHECKPOINT_MEM_H_ */
