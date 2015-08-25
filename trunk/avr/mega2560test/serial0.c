#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial0.h"

#define TX_BUF_SIZE 256
#define RX_BUF_SIZE 64

static volatile uint8_t rxBuf0[RX_BUF_SIZE];
static volatile uint8_t rxWrite0=0;
static volatile uint8_t rxRead0=0;
static volatile uint8_t rxCount0=0;
static volatile uint8_t txBuf0[TX_BUF_SIZE];
static volatile uint8_t txWrite0=0;
static volatile uint8_t txRead0=0;
static volatile uint8_t txCount0=0;
static volatile uint8_t transmitting0=0;
static volatile uint8_t rxOverrun0=0;
volatile uint8_t hwm0=0;

SIGNAL(USART0_TX_vect)
{
	if(txCount0!=0)
	{
		UDR0=txBuf0[txRead0++];
		txRead0%=TX_BUF_SIZE;
		txCount0--;
	}
	else
	{
		transmitting0=0;
	}
}

SIGNAL(USART0_RX_vect)
{
	if(rxCount0<RX_BUF_SIZE)
	{
		rxBuf0[rxWrite0++]=UDR0;
		rxWrite0%=RX_BUF_SIZE;
		rxCount0++;
	}
	else
	{
		volatile uint8_t temp=UDR0; // force a read to clear the interrupt
		rxOverrun0=1;
	}
}

void usart0_Init(uint16_t baudRate)
{
	UBRR0L = (uint8_t)(F_CPU/(baudRate*16L)-1);
	UBRR0H = (F_CPU/(baudRate*16L)-1) >> 8;
	UCSR0A = 0x00;
	UCSR0C = 0x06;
	UCSR0B = _BV(TXEN0)|_BV(RXEN0)|_BV(RXCIE0)|_BV(TXCIE0);
	hwm0=0;
}

uint8_t usart0_Tx(char ch)
{
	cli();

	if(!transmitting0)
	{
		UDR0=ch;
		transmitting0=1;
		sei();
		return 1;
	}
	else
	{
		if(txCount0==TX_BUF_SIZE)
		{
			sei();
			return 0;
		}
		else
		{
			txBuf0[txWrite0++]=ch;
			txWrite0%=TX_BUF_SIZE;
			txCount0++;
			if(txCount0>hwm0)
			{
				hwm0=txCount0;
			}
			sei();
			return 1;
		}
	}
}

uint8_t usart0_Rx(char *ch)
{
	uint8_t retVal=0;

	cli();

	if(rxCount0==0)
	{
		sei();
		return 0;
	}
	else
	{
		
		*ch=rxBuf0[rxRead0++];
		rxRead0%=RX_BUF_SIZE;
		retVal=rxCount0;
		rxCount0--;
		sei();
		return retVal;
	}
}

uint8_t usart0_Rx_Overrun()
{
	return rxOverrun0;
}
