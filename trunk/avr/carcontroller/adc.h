#ifndef _ADC_H_
#define _ADC_H_

#include <inttypes.h>

extern volatile uint8_t adcValues[];
extern volatile uint8_t adcChan;

void startAdcSequence();
void adcInit();

#endif

