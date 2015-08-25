#ifndef _SERIAL2_H_
#define _SERIAL2_H_

#include <inttypes.h>
#include "common.h"

void usart2_Init(uint16_t baudRate);
uint8_t usart2_Tx(char ch);
uint8_t usart2_Rx(char *ch);
extern volatile uint8_t hwm2;

#endif

