#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "serial2.h"
#include "serial0.h"
#include "control.h"

#define TX_BUF_SIZE 128
#define RX_BUF_SIZE 128 

static volatile uint8_t rxBuf2[RX_BUF_SIZE];
static volatile uint8_t rxWrite2=0;
static volatile uint8_t rxRead2=0;
static volatile uint8_t rxCount2=0;
static volatile uint8_t txBuf2[TX_BUF_SIZE];
static volatile uint8_t txWrite2=0;
static volatile uint8_t txRead2=0;
static volatile uint8_t txCount2=0;
static volatile uint8_t transmitting2=0;
static volatile uint8_t rxOverrun2=0;
volatile uint8_t hwm2=0;

SIGNAL(USART2_TX_vect)
{
	if(txCount2!=0)
	{
		UDR2=txBuf2[txRead2++];
		txRead2%=TX_BUF_SIZE;
		txCount2--;
	}
	else
	{
		transmitting2=0;
	}
}

SIGNAL(USART2_RX_vect)
{
	/*
	if(rxCount2<RX_BUF_SIZE)
	{
		rxBuf2[rxWrite2++]=UDR2;
		rxWrite2%=RX_BUF_SIZE;
		rxCount2++;
	}
	else
	{
		volatile uint8_t temp=UDR2; // force a read to clear the interrupt
		rxOverrun2=1;
	}
	*/

	if(controlState==ControlLoggerPassthrough)
	{
		usart0_Tx(UDR2);
	}
	else if(controlState==ControlRecoverFileListA)
	{
		// strip off the echo of the ls command
		char ch=UDR2;

		if(ch==0x0a)
		{
			controlState=ControlRecoverFileListB;
		}
	}
	else if(controlState==ControlRecoverFileListB)
	{
		// pass back the response to the ls command and
		// return to idle when the command prompt is seen
		char ch=UDR2;

		if(ch=='>')
		{
			// send an easy to find terminator
			usart0_Tx('#');
			usart0_Tx(0x0a);
			usart0_Tx(0x0d);
			controlState=ControlIdle;
		}
		else
		{
			usart0_Tx(ch);
		}
	}
	else
	{
		uint8_t temp=UDR2;
	}
}

void usart2_Init(uint16_t baudRate)
{
	UBRR2L = (uint8_t)(F_CPU/(baudRate*16L)-1);
	UBRR2H = (F_CPU/(baudRate*16L)-1) >> 8;
	UCSR2A = 0x00;
	UCSR2C = 0x06;
	UCSR2B = _BV(TXEN0)|_BV(RXEN0)|_BV(RXCIE0)|_BV(TXCIE0);
	hwm2=0;
}

uint8_t usart2_Tx(char ch)
{
	cli();

	if(!transmitting2)
	{
		UDR2=ch;
		transmitting2=1;
		sei();
		return 1;
	}
	else
	{
		if(txCount2==TX_BUF_SIZE)
		{
			sei();
			return 0;
		}
		else
		{
			txBuf2[txWrite2++]=ch;
			txWrite2%=TX_BUF_SIZE;
			txCount2++;
			if(txCount2>hwm2)
			{
				hwm2=txCount2;
			}
			sei();
			return 1;
		}
	}
}

uint8_t usart2_Rx(char *ch)
{
	uint8_t retVal=0;

	cli();

	if(rxCount2==0)
	{
		sei();
		return 0;
	}
	else
	{
		
		*ch=rxBuf2[rxRead2++];
		rxRead2%=RX_BUF_SIZE;
		retVal=rxCount2;
		rxCount2--;
		sei();
		return retVal;
	}
}

