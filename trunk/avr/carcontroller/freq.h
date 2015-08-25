#ifndef _FREQ_H_
#define _FREQ_H_

#include <inttypes.h>

extern volatile uint16_t freqValue[];

void checkForNoFrequencyInput();
void freqInit();

#endif

