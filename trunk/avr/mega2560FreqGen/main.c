#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include <util/delay.h>
#include "timers.h"

int main(void)
{
	sei();
	timersInit();

	// init IO ports
	DDRH=0xf0;
	DDRB=0x80;

	while (1)
	{
	}
}
