#ifndef _SERIAL3_H_
#define _SERIAL3_H_

#include <inttypes.h>
#include "common.h"

void usart3_Init(uint16_t baudRate);
uint8_t usart3_Tx(char ch);
uint8_t usart3_Rx(char *ch);

#endif

