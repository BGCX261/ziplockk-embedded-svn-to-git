#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "serial1.h"

#define TX_BUF_SIZE 32
#define RX_BUF_SIZE 32

static volatile uint8_t rxBuf1[RX_BUF_SIZE];
static volatile uint8_t rxWrite1=0;
static volatile uint8_t rxRead1=0;
static volatile uint8_t rxCount1=0;
static volatile uint8_t txBuf1[TX_BUF_SIZE];
static volatile uint8_t txWrite1=0;
static volatile uint8_t txRead1=0;
static volatile uint8_t txCount1=0;
static volatile uint8_t transmitting1=0;
static volatile uint8_t rxOverrun1=0;

SIGNAL(USART1_TX_vect)
{
	if(txCount1!=0)
	{
		UDR1=txBuf1[txRead1++];
		txRead1%=TX_BUF_SIZE;
		txCount1--;
	}
	else
	{
		transmitting1=0;
	}
}

SIGNAL(USART1_RX_vect)
{
	if(rxCount1<RX_BUF_SIZE)
	{
		rxBuf1[rxWrite1++]=UDR1;
		rxWrite1%=RX_BUF_SIZE;
		rxCount1++;
	}
	else
	{
		volatile uint8_t temp=UDR1; // force a read to clear the interrupt
		rxOverrun1=1;
	}
}

void usart1_Init(uint16_t baudRate)
{
	UBRR1L = (uint8_t)(F_CPU/(baudRate*16L)-1);
	UBRR1H = (F_CPU/(baudRate*16L)-1) >> 8;
	UCSR1A = 0x00;
	UCSR1C = 0x06;
	UCSR1B = _BV(TXEN1)|_BV(RXEN1)|_BV(RXCIE1)|_BV(TXCIE1);
}

uint8_t usart1_Tx(char ch)
{
	cli();

	if(!transmitting1)
	{
		UDR1=ch;
		transmitting1=1;
		sei();
		return 1;
	}
	else
	{
		if(txCount1==TX_BUF_SIZE)
		{
			sei();
			return 0;
		}
		else
		{
			txBuf1[txWrite1++]=ch;
			txWrite1%=TX_BUF_SIZE;
			txCount1++;
			sei();
			return 1;
		}
	}
}

uint8_t usart1_Rx(char *ch)
{
	uint8_t retVal=0;

	cli();

	if(rxCount1==0)
	{
		sei();
		return 0;
	}
	else
	{
		
		*ch=rxBuf1[rxRead1++];
		rxRead1%=RX_BUF_SIZE;
		retVal=rxCount1;
		rxCount1--;
		sei();
		return retVal;
	}
}

