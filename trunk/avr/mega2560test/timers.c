#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include "timers.h"
#include <util/delay.h>
#include "serial.h"
#include "serial0.h"
#include "serial2.h"
#include "adc.h"
#include "gps.h"
#include "control.h"

#define T0RELOAD -162
//define T1RELOAD -49999 // generates a 5Hz interrupt
//define T1RELOAD -24999 // generates a 10Hz interrupt
//define T1RELOAD -12499 // generates a 20Hz interrupt
#define T1RELOAD -9999 // generates a 25Hz interrupt

SIGNAL(TIMER0_OVF_vect)
{
	static uint8_t divider=0;

	TCNT0 = (uint8_t) T0RELOAD; // reload

	if(divider==3)
	{
		divider=0;
		usart0_Tx('*');
	}
	else
	{
		divider++;
	}
}

SIGNAL(TIMER1_OVF_vect)
{
	TCNT1 = (uint16_t) T1RELOAD; // reload

	if(controlState==ControlDataStream)
	{
		// send current data set to the control channel
		decDumpBuffer(usart0_Tx,adcValues,8);
		usart0_Tx(',');
		dumpWorkingGPSData(usart0_Tx);
		usart0_Tx(',');
		int16DumpBuffer(usart0_Tx,gValues,3);
		usart0_Tx('\n');
		usart0_Tx('\r');
	}

	if(controlState==ControlLogging)
	{
		if(!autoStop)
		{
			// send current data set to the logger
			decDumpBuffer(usart2_Tx,adcValues,8);
			usart2_Tx(',');
			dumpWorkingGPSData(usart2_Tx);
			usart2_Tx(',');
			int16DumpBuffer(usart2_Tx,gValues,3);
			usart2_Tx('\n');

			// detect zero speed and trigger auto stop
			if(workingGPSData.speed==0)
			{
				idleTicks++;
				if(idleTicks==25)
				{
					idleTicks=0;
					idleSeconds++;
					if(idleSeconds==120)
					{
						autoStop=1;
					}
				}
			}
			else
			{
				autoStop=0;
				idleTicks=0;
				idleSeconds=0;
			}
		}
	}

	startAdcSequence();
}

void timersInit()
{	
	// set up and enable low freq timer interrupt using timer0
//	TCCR0B = _BV(CS02)|_BV(CS00);
//	TCNT0 = (uint8_t) T0RELOAD; // reload
//	TIMSK0 |= (1<<TOIE0); // enable overflow interrupt

	// set up and enable low freq timer interrupt using timer1
	TCCR1B = _BV(CS11)|_BV(CS10); // div 32 prescale
	TCNT1 = (uint16_t) T1RELOAD; // reload
	TIMSK1 |= (1<<TOIE1); // enable overflow interrupt
}
