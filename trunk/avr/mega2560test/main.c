#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "common.h"
#include <util/delay.h>
#include "serial0.h"
#include "serial1.h"
#include "serial2.h"
#include "serial3.h"
#include "timers.h"
#include "gps.h"
#include "adc.h"
#include "control.h"

int main(void)
{
	//
	// init IO ports
	//
	DDRB=0xff; // all outputs
	DDRE=0x02; // all inputs TX0 output
	DDRJ=0x02; // all inputs TX3 output
	DDRF=0x00; // all inputs

	// input pins and input interrupts
	DDRK=0x00; // all inputs
	PCMSK2=_BV(PCINT23)|_BV(PCINT22); // enable interrupts on PCINT23:22
	PCICR=_BV(PCIE2); // enable PCINT23:16

	sei();
	usart0_Init(57600);
	//usart1_Init();
	usart2_Init(57600);
	adcInit();
	timersInit();

	initGPS(usart3_Init,usart3_Tx);

	while (1)
	{
		control();
	}
}
