#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include "timers.h"
#include <util/delay.h>

#define T3RELOAD -99 // generates a 25Hz interrupt

static uint16_t HIGHCOUNT=1;
static uint16_t LOWCOUNT=199;
static uint8_t flipper=0;
static uint8_t high=0;
static uint8_t inc=1;
static uint16_t counter=0;

SIGNAL(SIG_OVERFLOW3)
{
	TCNT3 = (uint16_t) T3RELOAD; // reload

	/*
	if(flipper==1)
	{
		flipper=0;
		PORTH=0xf0;
	}
	else
	{
		flipper=1;
		PORTH=0x00;
	}
	*/

	if(high)
	{
		if(counter<(HIGHCOUNT-1))
		{
			PORTH=0xf0;
			PORTB=0x80;
			counter++;
		}
		else
		{
			PORTH=0x00;
			PORTB=0x00;
			high=0;
			counter=0;
		}
	}
	else
	{
		if(counter<(LOWCOUNT-1))
		{
			PORTH=0x00;
			PORTB=0x00;
			counter++;
		}
		else
		{
			PORTH=0xf0;
			PORTB=0x80;
			high=1;
			counter=0;

			if(inc)
			{
				if(HIGHCOUNT==199)
				{
					inc=0;
				}
				else
				{
					HIGHCOUNT++;
					LOWCOUNT--;
				}
			}
			else
			{
				if(HIGHCOUNT==1)
				{
					inc=1;
				}
				else
				{
					HIGHCOUNT--;
					LOWCOUNT++;
				}
			}
		}
	}
}

void timersInit()
{	
	// set up and enable low freq timer interrupt using timer1
	TCCR3B = _BV(CS31); // div 8 prescale
	TCNT3 = (uint16_t) T3RELOAD; // reload
	TIMSK3 |= (1<<TOIE3); // enable overflow interrupt
}
