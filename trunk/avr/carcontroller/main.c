#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include <util/delay.h>

#include "serial.h"
#include "display.h"
#include "adc.h"
#include "timers.h"
#include "freq.h"
#include "tc.h"

uint8_t test[] __attribute__ ((section (".eeprom"))) = "Fergus Duncan";

int main(void)
{
	serialInit();

	//
	// init IO ports
	//
	DDRD=0x00; // all inputs
	DDRC=0xf0; // all inputs except PC4, PC5, PC6, PC7
	DDRB=0x00; // all inputs

	PORTB=0xff; // pull up all inputs
	PORTC=0x0f; // all outputs off

	timersInit();

	freqInit();

	adcInit();

	// enable global interrupts
	sei();

	_delay_ms(1000);

	displayInit();

	// initialise display

	while (1)
	{
		displayRun();
		_delay_ms(500);
	}
}
