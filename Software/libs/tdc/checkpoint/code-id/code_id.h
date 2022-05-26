#ifndef CODE_ID_H_
#define CODE_ID_H_

#include <stdint.h>
#include "nvm.h"
#include "checkpoint_util_mem.h"

typedef uint32_t code_id_t;

extern volatile code_id_t code_id;
code_id_t code_id_is_new(void);
code_id_t code_id_update(void);

static inline void code_id_clear(void) {
    // code_id = 0;
	uint32_t temp_zero = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
	write_nvm(&code_id, &temp_zero, sizeof(temp_zero));
#pragma GCC diagnostic pop
}

#endif /* CODE_ID_H_ */
