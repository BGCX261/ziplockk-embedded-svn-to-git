#ifndef _SERIAL1_H_
#define _SERIAL1_H_

#include <inttypes.h>
#include "common.h"

void usart1_Init(uint16_t baudRate);
uint8_t usart1_Tx(char ch);
uint8_t usart1_Rx(char *ch);

#endif

