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

	//
	// init IO ports
	//
	DDRB=0xff;

	adcInit();
	startAdcSequence();

	PORTB=ALL;

	_delay_ms(1000);

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
		}

//		if(flashCounter++==2)
//		{
//			flashCounter=0;
//
//			value=analog_result_16/102;
//
//			if(value>9)
//			{
//				value=9;
//			}
//		}

		value=calculateGearPosition(analog_result_16);

		if((adcrunning!=lastAdcrunning) && flash && (value==10))
		{
			PORTB=(displayValue[value]&~DP_SEG);
			lastAdcrunning=adcrunning;
		}
		else
		{
			PORTB=displayValue[value];
		}

	}
}
