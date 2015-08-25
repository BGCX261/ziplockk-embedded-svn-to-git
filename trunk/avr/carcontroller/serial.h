#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <inttypes.h>
#include "common.h"

#define BAUD_RATE 9600
#define TX_BUF_SIZE 32
#define RX_BUF_SIZE 32

void serialInit();
int putChar(char ch);
int putString(char *s);
char getChar(void);
void pushRx(char data);
char popRx();

#endif

