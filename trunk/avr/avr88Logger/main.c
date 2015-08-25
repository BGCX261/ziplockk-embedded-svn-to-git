#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include <util/delay.h>

#define UART_BAUD_RATE 9600

#include "adc.h"
#include "timers.h"
#include "uart.h"

uint8_t test[] __attribute__ ((section (".eeprom"))) = "Fergus Duncan";

int main(void)
{
	uint8_t flash=0;
	uint8_t flashCounter=0;

	//
	// init IO ports
	//

	DDRD=0xfc;

    	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU)); 

	adcInit(); 		// initialise ADC
	sei();			// enable interrupts
	timersInit();		// initialise Timer Interrupts

	while (1)
	{
		_delay_ms(200);

		if(flash==0)
		{
			flash=1;
		}
		else
		{
			flash=0;
		}

/*
		if(flash)
		{
			startAdcSequence();	// start ADC sequence
			PORTD=(PORTD|0x01);
		}
		else
		{
			PORTD=(PORTD&0xfe);
		}	
*/
	}
}
