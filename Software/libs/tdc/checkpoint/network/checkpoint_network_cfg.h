#ifndef CHECKPOINT_NETWORK_CFG_H_
#define CHECKPOINT_NETWORK_CFG_H_

#include <stdint.h>

/* .data section to checkpoint the linkerscript */
extern uint32_t _sdata_network;
extern uint32_t _edata_network;

#define checkpoint_network_data_start    (&_sdata_network)
#define checkpoint_network_data_end      (&_edata_network)


/* .data NVM checkpoint section in the linkerscript */
extern uint32_t _data_network_checkpoint_0_start;
extern uint32_t _data_network_checkpoint_1_start;
extern uint32_t _data_network_checkpoint_1_end;

#define checkpoint_network_data_0_start  (&_data_network_checkpoint_0_start)
#define checkpoint_network_data_0_end    (&_data_network_checkpoint_1_start)

#define checkpoint_network_data_1_start  (&_data_network_checkpoint_1_start)
#define checkpoint_network_data_1_end    (&_data_network_checkpoint_1_end)

/* .bss section to checkpoint the linkerscript */
extern uint32_t _sbss_network;
extern uint32_t _ebss_network;

#define checkpoint_network_bss_start    (&_sbss_network)
#define checkpoint_network_bss_end      (&_ebss_network)


/* .bss NVM checkpoint section in the linkerscript */
extern uint32_t _bss_network_checkpoint_0_start;
extern uint32_t _bss_network_checkpoint_1_start;
extern uint32_t _bss_network_checkpoint_1_end;

#define checkpoint_network_bss_0_start  (&_bss_network_checkpoint_0_start)
#define checkpoint_network_bss_0_end    (&_bss_network_checkpoint_1_start)

#define checkpoint_network_bss_1_start  (&_bss_network_checkpoint_1_start)
#define checkpoint_network_bss_1_end    (&_bss_network_checkpoint_1_end)


#endif /* CHECKPOINT_NETWORK_CFG_H_ */
