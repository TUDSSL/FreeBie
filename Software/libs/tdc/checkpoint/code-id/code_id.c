#include <stdint.h>

#include "nvm.h"
#include "checkpoint_util_mem.h"

/*
 * We require the compiler to optimize this otherwise we waist quite a lot of
 * .text (flash)
 */
#pragma GCC push_options
#pragma GCC optimize "-O2"

#include "code_id.h"
#include "compile_time.h"

code_id_t code_id_is_new(void) {
  if (code_id != UNIX_TIMESTAMP) {
    return UNIX_TIMESTAMP;
  }
  return 0;
}

code_id_t code_id_update(void) {
	uint32_t tstamp = UNIX_TIMESTAMP;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
	write_nvm(&code_id, &tstamp, sizeof(tstamp));
#pragma GCC diagnostic pop
    return code_id;
}

#pragma GCC pop_options
