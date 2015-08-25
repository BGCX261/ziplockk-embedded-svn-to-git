#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include <util/delay.h>
#include "adc.h"

#define AVECOUNT 50

volatile uint8_t adcValues[8]={0,0,0,0,0,0,0,0};
volatile uint8_t adcChan=0;
volatile uint16_t analog_result_16=0;
volatile uint8_t adcrunning=0;
volatile uint16_t average_chan0[AVECOUNT];
volatile uint16_t average_chan0_tot=0;
volatile uint8_t average_chan0_pos=0;
volatile uint8_t flip=0;

ISR(ADC_vect)
{
	uint8_t adlow;
	uint8_t adhigh;

	adlow = ADCL;
	adhigh = ADCH;
	analog_result_16 = ((adhigh<<8)|adlow);
	adcrunning++;

	// freq check

/*
	if(!flip)
	{
		PORTB=0x00;
		flip++;
	}
	else
	{
		PORTB=0xff;
		flip=0;
	}
*/

	// calc average
	average_chan0_tot+=analog_result_16;
	average_chan0_tot-=average_chan0[average_chan0_pos];
	average_chan0[average_chan0_pos]=analog_result_16;
	average_chan0_pos++;
	if(average_chan0_pos==AVECOUNT)
	{
		average_chan0_pos=0;
	}

	analog_result_16=average_chan0_tot/AVECOUNT;
}

void startAdcSequence()
{
	/* start first convertion*/
	ADCSR |= (1<<ADSC);
}

void adcInit()
{
	/*choose the ADC chanel and standard voltage source*/
	/*use adc4 and internal 1.1v voltage.*/
	ADMUX = ((1<<REFS0)|0x00); // AVCC reference voltage, channel ADC0
  
	/*adc control: prescaler, clear ADIF, interrupt enable, etc*/
	/* ADC prescaler: 
	f_adc=f_osc/128=8e6/128 ~= 60KHz
	*/
	ADCSR = ((1<<ADEN)|(1<<ADIF)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));

	/*auto trigger enable*/
	ADCSR |= (1<<ADFR);
	/*
	choose trigger source:
	continuous mode.
	see datasheet p231(Chinese version) for details.
	*/
	///////// ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0));
    
	/*enable interrupt*/
	sei();

	/*then, ADCSRA |= (1<<ADSC) can start convention.*/
}

