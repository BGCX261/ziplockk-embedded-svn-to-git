#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "serial.h"

static uint8_t rxBuf[RX_BUF_SIZE];
static volatile uint8_t rxHead;
static volatile uint8_t rxTail;
static volatile uint8_t rxCount;

SIGNAL(SIG_UART_RECV)
{
	pushRx(UDR);
}

void serialInit()
{
	UBRRL = (uint8_t)(F_CPU/(BAUD_RATE*16L)-1);
	UBRRH = (F_CPU/(BAUD_RATE*16L)-1) >> 8;
	UCSRA = 0x00;
	UCSRC = 0x86;
	UCSRB = _BV(TXEN)|_BV(RXEN)|_BV(RXCIE);
}

int putChar(char ch)
{
	while (!(UCSRA & (1 << UDRE)));
	UDR = ch;
	return ch;
}

int putString(char *s)
{
	while (*s)
		putChar(*s++);
	return 0;
}

char getChar(void)
{
	while (!(UCSRA & (1 << RXC)));
	return UDR;
}

void pushRx(char data)
{
	if(rxCount<RX_BUF_SIZE)
	{
		rxBuf[rxHead++]=data;
		rxHead%=RX_BUF_SIZE;
		rxCount++;
	}
}

char popRx()
{
	volatile int retVal=-1;

	if(rxCount) // not empty
	{
		retVal=rxBuf[rxTail++];
		rxTail%=RX_BUF_SIZE;
		rxCount--;
	}

	return retVal;
}

