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

ISR(ADC_vect)
{
	if(PORTD&0x08)
	{
		PORTD=(PORTD&0xf7);
	}
	else
	{
		PORTD=(PORTD|0x08);
	}

        adcValues[adcChan]=ADCH;

	ADMUX |= (1 << REFS0); // reference voltage set to AVCC
	ADMUX |= (1 << ADLAR); // ADC to 8 bit mode
        ADMUX |= adcChan;

	adcChan++;

        if(adcChan==8)
        {
                adcChan=0;
        }
        else
        {
                ADCSRA |= (1<<ADSC); // ADC start conversion
        }

}

void startAdcSequence()
{
	PORTD=(PORTD|0x08);

        adcChan = 0;
	ADMUX |= (1 << REFS0); // reference voltage set to AVCC
	ADMUX |= (1 << ADLAR); // ADC to 8 bit mode
        ADMUX |= adcChan;
	ADCSRA |= (1<<ADSC); // ADC start conversion
}

void adcInit()
{
	// set prescaler cpufreq/64=125KHz (should be 50 to 200)
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	ADMUX |= (1 << REFS0); // reference voltage set to AVCC
	ADMUX |= (1 << ADLAR); // ADC to 8 bit mode
	ADCSRA |= (1 << ADEN); // ADC enable
	ADCSRA |= (1 << ADIE); // Enable ADC Interrupts

/*
	// choose the ADC chanel and standard voltage source
	// use adc4 and internal 1.1v voltage.
	// ADMUX = ((1<<REFS1)|(1<<REFS0)|0x04);
	ADMUX = ((1<<REFS0)|0x00); // AVCC reference voltage, channel ADC0
  
	// adc control: prescaler, clear ADIF, interrupt enable, etc
	//  ADC prescaler: 
	// f_adc=f_osc/128=8e6/128 ~= 60KHz

	ADCSRA = ((1<<ADEN)|(1<<ADIF)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));

	// auto trigger enable
	ADCSRA |= (1<<ADATE);

	// choose trigger source:
	// continuous mode.
	// see datasheet p231(Chinese version) for details.

	ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0));
    
	sei();

	// then, ADCSRA |= (1<<ADSC) can start convention.
*/
}

