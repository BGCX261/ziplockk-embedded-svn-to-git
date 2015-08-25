#ifndef _ADC_H_
#define _ADC_H_

#include <inttypes.h>

extern volatile uint16_t analog_result_16;
extern volatile uint8_t adcrunning;

void startAdcSequence();
void adcInit();

#endif

