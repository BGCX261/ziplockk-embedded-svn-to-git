#ifndef _TC_H_
#define _TC_H_

#include <inttypes.h>

extern volatile uint16_t frontSpeed;
extern volatile uint16_t rearSpeed;
extern volatile uint16_t slip;
extern volatile uint8_t status;

void tcCheck();
void tcInit();

#endif

