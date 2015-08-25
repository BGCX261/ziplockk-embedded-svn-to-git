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
	ADCSRA |= (1<<ADSC);
}

void adcInit()
{
//	ADCSRA |= (1 << ADPS2) | (1 << ADPS1); // set prescaler cpufreq/64=125KHz (should be 50 to 200)
//	ADMUX |= (1 << REFS0); // reference voltage set to AVCC
//	//SFIOR &= 0x1f; // clear ADTS2, ADTS1 and ADTS0 to set free running mode
//	ADMUX |= (1 << ADLAR); // ADC to 8 bit mode
//	ADCSRA |= (1 << ADEN); // ADC enable
//	ADCSRA |= (1 << ADSC); // ADC start conversions
//	ADCSRA |= (1 << ADIE); // Enable ADC Interrupts
//	DDRA = 0x00; // all inputs on port A
//	PORTA = 0x00; // disable pullup
     
	/*choose the ADC chanel and standard voltage source*/
	/*use adc4 and internal 1.1v voltage.*/
	//ADMUX = ((1<<REFS1)|(1<<REFS0)|0x04);
	ADMUX = ((1<<REFS0)|0x00); // AVCC reference voltage, channel ADC0
  
	/*adc control: prescaler, clear ADIF, interrupt enable, etc*/
	/* ADC prescaler: 
	f_adc=f_osc/128=8e6/128 ~= 60KHz
	*/
	ADCSRA = ((1<<ADEN)|(1<<ADIF)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));

	/*auto trigger enable*/
	ADCSRA |= (1<<ADATE);
	/*
	choose trigger source:
	continuous mode.
	see datasheet p231(Chinese version) for details.
	*/
	ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0));
    
	/*enable interrupt*/
	sei();

	/*then, ADCSRA |= (1<<ADSC) can start convention.*/
}

