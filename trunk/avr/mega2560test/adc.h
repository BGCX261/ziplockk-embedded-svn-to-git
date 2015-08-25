#ifndef _ADC_H_
#define _ADC_H_

#include <inttypes.h>

#define NUMSAMPLES  16
#define NUMCHANNELS 8

extern volatile uint8_t adcValueStore[NUMCHANNELS][NUMSAMPLES];
extern volatile uint8_t adcValues[NUMCHANNELS];
extern volatile int16_t gValues[3];
void startAdcSequence();
void adcInit();

#endif

