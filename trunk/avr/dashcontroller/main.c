#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include <util/delay.h>

#include "adc.h"
#include "timers.h"

uint8_t test[] __attribute__ ((section (".eeprom"))) = "Fergus Duncan";

int main(void)
{
	uint8_t flash=0;
	uint8_t flashCounter=0;
	uint8_t input_fault=0;
	uint8_t input_neutral=0;
	uint8_t input_shift=0;
	uint8_t output_portd;

	//
	// init IO ports
	//
	DDRD=0x0f;

	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz 
	ADMUX |= (1 << REFS0); // Set ADC reference to AVCC 
	ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading 
//	ADCSRA |= (1 << ADFR);  // Set ADC to Free-Running Mode 
	ADCSRB = 0;  // Set ADC to Free-Running Mode 
	ADCSRA |= (1 << ADEN);  // Enable ADC 
	ADCSRA |= (1 << ADSC);  // Start A2D Conversions 

	while (1)
	{
		_delay_ms(10);

		PORTD|=0x0f; // activate pullup resistors for inputs - remember to set lower port bits later

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

		if(flashCounter++==20)
		{
			flashCounter=0;
		}

		input_fault=PIND&0x01;
		input_neutral=PIND&0x02;
		input_shift=PIND&0x04;
		output_portd=0x0f;

		if(!input_fault)
		{
			if(flash)
			{
				output_portd|=0x20;
			}
			else
			{
				if(!input_neutral)
				{
					output_portd|=0x40;
				}
			}
		}
		else
		{
			if(!input_neutral)
			{
				output_portd|=0x40;
			}
			else
			{
				if(!input_shift)
				{
					output_portd|=0x80;
				}
			}
		}

		PORTD=output_portd;

	}
}
