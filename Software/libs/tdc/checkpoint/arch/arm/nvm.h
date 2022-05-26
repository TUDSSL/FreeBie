#ifndef NVM_H_
#define NVM_H_

#include "fram.h"

#define nvm __attribute__((section(".nvm"),aligned (4)))

#endif /* NVM_H_ */
