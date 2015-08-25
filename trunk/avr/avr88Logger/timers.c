#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include "timers.h"
#include "adc.h"
#include "uart.h"
#include <util/delay.h>

#define T0RELOAD -180

uint16_t ticker=0;
uint16_t tocker=0;

ISR(TIMER0_OVF_vect)
{
	TCNT0 = (uint8_t) T0RELOAD; // reload

	if(PORTD&0x04)
	{
		PORTD=(PORTD&0xfb);
	}
	else
	{
		PORTD=(PORTD|0x04);
	}

	startAdcSequence();

	uart_putc('A');

	tocker++;
}

void timersInit()
{	
	// set up and enable low freq timer interrupt using timer0
	TCCR0A = 0x00;
//	TCCR0B = 0x05; // ck/1024

	TCCR0B = (1 << CS02); // ck/256

	TCNT0 = (uint8_t) T0RELOAD; // reload
	TIMSK0 |= (1<<TOIE0); // enable overflow interrupt

}
