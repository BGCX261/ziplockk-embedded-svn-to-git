#ifndef _SERIAL0_H_
#define _SERIAL0_H_

#include <inttypes.h>
#include "common.h"

void usart0_Init(uint16_t baudRate);
uint8_t usart0_Tx(char ch);
uint8_t usart0_Rx(char *ch);
uint8_t usart0_Rx_Overrun();
extern volatile uint8_t hwm0;

#endif

