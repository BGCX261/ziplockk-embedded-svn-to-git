#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include "timers.h"
#include "adc.h"
#include <util/delay.h>

#define T0RELOAD -8

uint16_t ticker=0;
uint16_t tocker=0;

SIGNAL(SIG_OVERFLOW0)
{
	TCNT0 = (uint8_t) T0RELOAD; // reload

	if(ticker==20)
	{
		// initiate ADC conversion sequence
		startAdcSequence();
		ticker=0;
	}

	ticker++;

	if(timerState)
	{
		PORTC|=0x10;
		timerState=0;
	}
	else
	{
		PORTC&=0xef;
		timerState++;
	}

//	if(tocker==1000)
	if(0)
	{
		pc6Limit++;
		if(pc6Limit>30)
		{
			pc6Limit=10;
		}
		tocker=0;
	}

	tocker++;

	if(pc6Counter==pc6Limit)
	{
		pc6Counter=0;

		if(pc6State)
		{
			PORTC|=0x40;
			pc6State=0;
		}
		else
		{
			PORTC&=0xbf;
			pc6State=1;
		}
	}

	pc6Counter++;

	if(pc7Counter==20)
	{
		pc7Counter=0;

		if(pc7State)
		{
			PORTC|=0x80;
			pc7State=0;
		}
		else
		{
			PORTC&=0x7f;
			pc7State=1;
		}
	}

	pc7Counter++;
}

void timersInit()
{	
	// set up and enable low freq timer interrupt using timer0
	TCCR0A = 0x00;
	TCCR0B = 0x05; // ck/1024

	TCNT0 = (uint8_t) T0RELOAD; // reload
	TIMSK0 |= (1<<TOIE0); // enable overflow interrupt

}
