#include "checkpoint_setup.h"

#include "nrf.h"

void checkpoint_setup(void)
{
    NVIC_SetPriority(SVCall_IRQn, 0xff); /* Lowest possible priority */
}
