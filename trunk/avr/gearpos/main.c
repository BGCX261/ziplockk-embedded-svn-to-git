#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include <util/delay.h>

#include "adc.h"
#include "timers.h"

#define A_SEG 0x01
#define B_SEG 0x02
#define C_SEG 0x04
#define D_SEG 0x08
#define E_SEG 0x10
#define F_SEG 0x20
#define G_SEG 0x40
#define DP_SEG 0x80

#define ZERO ~(A_SEG+B_SEG+C_SEG+D_SEG+E_SEG+F_SEG)
#define ONE ~(B_SEG+C_SEG)
#define TWO ~(A_SEG+B_SEG+G_SEG+D_SEG+E_SEG)
#define THREE ~(A_SEG+B_SEG+C_SEG+D_SEG+G_SEG)
#define FOUR ~(B_SEG+C_SEG+F_SEG+G_SEG)
#define FIVE ~(A_SEG+C_SEG+D_SEG+F_SEG+G_SEG) 
#define SIX ~(A_SEG+F_SEG+C_SEG+D_SEG+E_SEG+G_SEG)
#define SEVEN ~(A_SEG+B_SEG+C_SEG)
#define EIGHT ~(A_SEG+B_SEG+C_SEG+D_SEG+E_SEG+F_SEG+G_SEG) 
#define NINE ~(A_SEG+B_SEG+C_SEG+F_SEG+G_SEG)
#define OFF (0xff)
#define ALL (0x00)

#define G_1_LOW 196
#define G_1_HIGH 240
#define G_2_LOW 243
#define G_2_HIGH 328
#define G_3_LOW 379
#define G_3_HIGH 494
#define G_4_LOW 507	
#define G_4_HIGH 648
#define G_5_LOW 746
#define G_5_HIGH 872
#define G_6_LOW 880
#define G_6_HIGH 948

#define MAX 4

uint8_t test[] __attribute__ ((section (".eeprom"))) = "Fergus Duncan";

uint8_t displayValue[] = {ZERO,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE,OFF};

uint8_t calculateGearPosition(uint16_t analog_reading)
{
	uint8_t retVal=10;

	if((analog_reading>G_1_LOW)&&(analog_reading<G_1_HIGH))
	{
		retVal=1;
	}

	if((analog_reading>G_2_LOW)&&(analog_reading<G_2_HIGH))
	{
		retVal=2;
	}

	if((analog_reading>G_3_LOW)&&(analog_reading<G_3_HIGH))
	{
		retVal=3;
	}

	if((analog_reading>G_4_LOW)&&(analog_reading<G_4_HIGH))
	{
		retVal=4;
	}

	if((analog_reading>G_5_LOW)&&(analog_reading<G_5_HIGH))
	{
		retVal=5;
	}

	if((analog_reading>G_6_LOW)&&(analog_reading<G_6_HIGH))
	{
		retVal=6;
	}

	return retVal;
}

int main(void)
{
	uint8_t flash=0;
	uint8_t flashCounter=0;
	uint8_t value=0;
	uint8_t lastAdcrunning=0;
	uint8_t cyclic=0;
	uint8_t lastValid=10;
	uint8_t invalidCount=0;
	uint8_t posTable[MAX];

	//
	// init IO ports
	//
	DDRD=0xff;
	DDRC=0x0e;

	adcInit();
	startAdcSequence();

/*

	PORTD=ALL;

	_delay_ms(200);

	PORTD=OFF;

	_delay_ms(200);

	PORTD=ALL;

	_delay_ms(200);

	PORTD=OFF;

	_delay_ms(200);

	PORTD=ALL;

	_delay_ms(200);

	PORTD=OFF;

	_delay_ms(200);

*/

	while (1)
	{
		_delay_ms(25);

		if(!flashCounter)
		{
			if(!flash)
			{
				flash++;
			}
			else
			{
				flash=0;
			}

			flashCounter=10;
		}
		else
		{
			flashCounter--;
		}

/*
		if(flash)
		{
			PORTC|=0x0e;
		}
		else
		{
			PORTC&=0xf1;
		}
*/

		value=calculateGearPosition(analog_result_16);

		posTable[cyclic++]=value;
		cyclic%=MAX;

		uint8_t valid=1;
		for(uint8_t loop=0;loop<MAX;loop++)
		{
			if(posTable[loop]!=value)
			{
				valid=0;
			}
		}
		
		if(value==10)
		{
			valid=0;
		}

		if(!valid)
		{
			if(invalidCount<(MAX*12))
			{
				invalidCount++;
				value=lastValid;
			}
			else
			{
				value=10;
			}
		}
		else
		{
			lastValid=value;
			invalidCount=0;
		}

/*
		if((adcrunning!=lastAdcrunning) && flash && (value==10))
		{
			PORTD=(displayValue[value]&~DP_SEG);
			lastAdcrunning=adcrunning;
		}
		else
		{
			PORTD=displayValue[value];
		}
*/

		PORTD=displayValue[value];


		if((value>0) && (value<7))
		{
			PORTC=(value*2);
		}
		else
		{
			PORTC=0x00;
		}

	}
}
