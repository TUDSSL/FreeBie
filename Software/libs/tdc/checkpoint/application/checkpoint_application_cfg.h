#ifndef CHECKPOINT_APPLICATION_CFG_H_
#define CHECKPOINT_APPLICATION_CFG_H_

#include <stdint.h>

/* .data section to checkpoint the linkerscript */
extern uint32_t _sdata_application;
extern uint32_t _edata_application;

#define checkpoint_application_data_start    (&_sdata_application)
#define checkpoint_application_data_end      (&_edata_application)


/* .data NVM checkpoint section in the linkerscript */
extern uint32_t _data_application_checkpoint_0_start;
extern uint32_t _data_application_checkpoint_1_start;
extern uint32_t _data_application_checkpoint_1_end;

#define checkpoint_application_data_0_start  (&_data_application_checkpoint_0_start)
#define checkpoint_application_data_0_end    (&_data_application_checkpoint_1_start)

#define checkpoint_application_data_1_start  (&_data_application_checkpoint_1_start)
#define checkpoint_application_data_1_end    (&_data_application_checkpoint_1_end)

/* .bss section to checkpoint the linkerscript */
extern uint32_t _sbss_application;
extern uint32_t _ebss_application;

#define checkpoint_application_bss_start    (&_sbss_application)
#define checkpoint_application_bss_end      (&_ebss_application)


/* .bss NVM checkpoint section in the linkerscript */
extern uint32_t _bss_application_checkpoint_0_start;
extern uint32_t _bss_application_checkpoint_1_start;
extern uint32_t _bss_application_checkpoint_1_end;

#define checkpoint_application_bss_0_start  (&_bss_application_checkpoint_0_start)
#define checkpoint_application_bss_0_end    (&_bss_application_checkpoint_1_start)

#define checkpoint_application_bss_1_start  (&_bss_application_checkpoint_1_start)
#define checkpoint_application_bss_1_end    (&_bss_application_checkpoint_1_end)


#endif /* CHECKPOINT_APPLICATION_CFG_H_ */
