#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "serial3.h"
#include "gps.h"

#define TX_BUF_SIZE 32
#define RX_BUF_SIZE 128

static volatile uint8_t rxBuf3[RX_BUF_SIZE];
static volatile uint8_t rxWrite3=0;
static volatile uint8_t rxRead3=0;
static volatile uint8_t rxCount3=0;
static volatile uint8_t txBuf3[TX_BUF_SIZE];
static volatile uint8_t txWrite3=0;
static volatile uint8_t txRead3=0;
static volatile uint8_t txCount3=0;
static volatile uint8_t transmitting3=0;
static volatile uint8_t rxOverrun3=0;

SIGNAL(USART3_TX_vect)
{
	if(txCount3!=0)
	{
		UDR3=txBuf3[txRead3++];
		txRead3%=TX_BUF_SIZE;
		txCount3--;
	}
	else
	{
		transmitting3=0;
	}
}

SIGNAL(USART3_RX_vect)
{
	processGPSCharacter(UDR3);

//	if(rxCount3<RX_BUF_SIZE)
//	{
//		rxBuf3[rxWrite3++]=UDR3;
//		rxWrite3%=RX_BUF_SIZE;
//		rxCount3++;
//	}
//	else
//	{
//		volatile uint8_t temp=UDR3; // force a read to clear the interrupt
//		rxOverrun3=1;
//	}

}

void usart3_Init(uint16_t baudRate)
{
	UBRR3L = (uint8_t)(F_CPU/(baudRate*16L)-1);
	UBRR3H = (F_CPU/(baudRate*16L)-1) >> 8;
	UCSR3A = 0x00;
	UCSR3C = 0x06;
	UCSR3B = _BV(TXEN0)|_BV(RXEN0)|_BV(RXCIE0)|_BV(TXCIE0);
}

uint8_t usart3_Tx(char ch)
{
	cli();

	if(!transmitting3)
	{
		UDR3=ch;
		transmitting3=1;
		sei();
		return 1;
	}
	else
	{
		if(txCount3==TX_BUF_SIZE)
		{
			sei();
			return 0;
		}
		else
		{
			txBuf3[txWrite3++]=ch;
			txWrite3%=TX_BUF_SIZE;
			txCount3++;
			sei();
			return 1;
		}
	}
}

uint8_t usart3_Rx(char *ch)
{
	uint8_t retVal=0;

	cli();

	if(rxCount3==0)
	{
		sei();
		return 0;
	}
	else
	{
		
		*ch=rxBuf3[rxRead3++];
		rxRead3%=RX_BUF_SIZE;
		retVal=rxCount3;
		rxCount3--;
		sei();
		return retVal;
	}
}

