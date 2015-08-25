#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include <util/delay.h>
#include "adc.h"

volatile uint8_t adcValues[8]={0,0,0,0,0,0,0,0};
volatile uint8_t adcChan=0;

SIGNAL(SIG_ADC)
{
	adcValues[ADMUX&0x1f]=ADCH;
	ADMUX=(ADMUX&0xe0)|adcChan;
	if(adcChan==8)
	{
		adcChan=0;
//		PORTC&=0xdf;
	}
	else
	{
		adcChan++;
		ADCSRA |= (1 << ADSC); // ADC start conversion
	}
}

void startAdcSequence()
{
//	PORTC|=0x20;
	ADMUX=(ADMUX&0xe0)|adcChan;
	adcChan++;
	ADCSRA |= (1 << ADSC); // ADC start conversion
}

void adcInit()
{
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1); // set prescaler cpufreq/64=125KHz (should be 50 to 200)
	ADMUX |= (1 << REFS0); // reference voltage set to AVCC
	//SFIOR &= 0x1f; // clear ADTS2, ADTS1 and ADTS0 to set free running mode
	ADMUX |= (1 << ADLAR); // ADC to 8 bit mode
	ADCSRA |= (1 << ADEN); // ADC enable
	ADCSRA |= (1 << ADSC); // ADC start conversions
	ADCSRA |= (1 << ADIE); // Enable ADC Interrupts
	DDRA = 0x00; // all inputs on port A
	PORTA = 0x00; // disable pullup
}

