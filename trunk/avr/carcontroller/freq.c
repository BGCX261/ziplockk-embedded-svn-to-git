#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include "freq.h"
#include <util/delay.h>

#define FREQTIMEOUT 100

volatile uint16_t freqValue[4]={0,0,0,0};
uint16_t freqPrev[4]={0,0,0,0};
uint16_t freqCurrent[4]={0,0,0,0};
uint8_t freqTimeout[4]={0,0,0,0};


// external interrupt pin 0
SIGNAL(SIG_INTERRUPT0)
{
	uint16_t temp=TCNT1;
	freqPrev[0]=freqCurrent[0];
	freqCurrent[0]=temp;
	if(freqCurrent[0]<freqPrev[0])
	{
		freqValue[0]=0xffff-freqPrev[0]+freqCurrent[0]+1;
	}
	else
	{
		freqValue[0]=freqCurrent[0]-freqPrev[0];
	}
	freqTimeout[0]=0;
}

// external interrupt pin 1
SIGNAL(SIG_INTERRUPT1)
{
	uint16_t temp=TCNT1;
	freqPrev[1]=freqCurrent[1];
	freqCurrent[1]=temp;
	if(freqCurrent[1]<freqPrev[1])
	{
		freqValue[1]=0xffff-freqPrev[1]+freqCurrent[1]+1;
	}
	else
	{
		freqValue[1]=freqCurrent[1]-freqPrev[1];
	}
	freqTimeout[1]=0;
}

// external interrupt pin 2
SIGNAL(SIG_INTERRUPT2)
{
	uint16_t temp=TCNT1;
	freqPrev[2]=freqCurrent[2];
	freqCurrent[2]=temp;
	if(freqCurrent[2]<freqPrev[2])
	{
		freqValue[2]=0xffff-freqPrev[2]+freqCurrent[2]+1;
	}
	else
	{
		freqValue[2]=freqCurrent[2]-freqPrev[2];
	}
	freqTimeout[2]=0;
}

void checkForNoFrequencyInput()
{
	uint8_t loop;

	for(loop=0;loop<4;loop++)
	{
		if(freqTimeout[loop]<FREQTIMEOUT)
		{
			freqTimeout[loop]++;
		}
		else
		{
			freqValue[loop]=0xffff;
		}
	}
}

void freqInit()
{	
	// enable external interrupts int0 and int1 rising edge triggered
	MCUCR|=0x0f; // set ISC00, ISC01, ISC10, ISC11

	// enable external interrupt int2 rising edge triggered
	MCUCSR|=0x40; // set ISC2
	
	// now things are set up setup - enable external interrupts
	GICR|=0xe0; // set INT0, INT1 and INT2
	
}
