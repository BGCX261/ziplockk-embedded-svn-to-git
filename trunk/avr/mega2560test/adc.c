#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include <util/delay.h>
#include "adc.h"
#include "serial0.h"
#include "serial.h"

volatile uint8_t adcValueStore[NUMCHANNELS][NUMSAMPLES];
volatile uint8_t adcValues[NUMCHANNELS];
volatile int16_t gValues[3];
volatile uint8_t adcChannel=0;
volatile uint8_t adcSample=0;

uint8_t zeros[3]={72,89,83};
uint8_t oneGval[3]={42,41,42};

void calculateG()
{
	int32_t temp=0;
	int channel=0;
	
	for(channel=0;channel<3;channel++)
	{
		temp=(int32_t)(adcValues[channel]-zeros[channel]);
		temp=(int32_t)(temp*100);
		temp=(int32_t)(temp/oneGval[channel]);
		gValues[channel]=(int16_t)temp;
	}
}

ISR(ADC_vect)
{
        adcValueStore[adcChannel][adcSample]=ADCH;

	adcChannel++;

        if(adcChannel==NUMCHANNELS)
        {
                adcChannel=0;

		adcSample++;

		if(adcSample==NUMSAMPLES)
		{
			// complete all cycles
			adcSample=0;

			uint8_t channel=0;
			uint8_t sample=0;

			for(channel=0;channel<NUMCHANNELS;channel++)
			{
				uint32_t total=0;

				for(sample=0;sample<NUMSAMPLES;sample++)
				{
					total+=adcValueStore[channel][sample];
				}

				adcValues[channel]=(uint8_t)(total/NUMSAMPLES);
			}

			calculateG();
		}
		else
		{
			// configure for the next channel
       	 		ADMUX = (ADMUX&0xf0)|adcChannel;
                	ADCSRA |= (1<<ADSC); // ADC start conversion
		}
        }
        else
        {
		// configure for the next channel
        	ADMUX = (ADMUX&0xf0)|adcChannel;
                ADCSRA |= (1<<ADSC); // ADC start conversion
        }

}

void startAdcSequence()
{
        adcChannel = 0;
        adcSample = 0;
        ADMUX = (ADMUX&0xf0)|adcChannel;
	ADCSRA |= (1<<ADSC); // ADC start conversion
}

void adcInit()
{
	uint8_t channel=0;
	uint8_t sample=0;

	for(channel=0;channel<NUMCHANNELS;channel++)
	{
		for(sample=0;sample<NUMSAMPLES;sample++)
		{
			adcValueStore[channel][sample]=0;
		}
		adcValues[channel]=0;
	}

	// set prescaler cpufreq/64=125KHz (should be 50 to 200)
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX |= (1 << REFS0); // reference voltage set to AVCC
	ADMUX |= (1 << ADLAR); // ADC to 8 bit mode
	ADCSRA |= (1 << ADEN); // ADC enable
	ADCSRA |= (1 << ADIE); // Enable ADC Interrupts
}

