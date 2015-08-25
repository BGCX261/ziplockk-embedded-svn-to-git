#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#define F_CPU 8000000
#define BAUD_RATE 19200

#define T0RELOAD -1

//uint8_t test[] __attribute__ ((section (".eeprom"))) = "Fergus Duncan";

volatile uint8_t state=0;
volatile uint8_t mlstate=0;
volatile uint16_t l_int0time=0xffff; // last valid pulsewidth
volatile uint16_t l_int1time=0xffff; 
volatile uint16_t c_int0time=0; // current counting pulsewidth
volatile uint16_t c_int1time=0;

// external interrupt pin 0
SIGNAL(SIG_INTERRUPT0)
{
	l_int0time=c_int0time;
	c_int0time=0;
}

// external interrupt pin 1
SIGNAL(SIG_INTERRUPT1)
{
	l_int1time=c_int1time;
	c_int1time=0;
}

//timer 0 overflow
SIGNAL(SIG_OVERFLOW0)
{
	TCNT0 = (uint8_t) T0RELOAD; // reload

	////////
	PORTC|=0x08;
	////////

	if(state)
	{
		PORTC|=0x01;
		state=0;
	}
	else
	{
		PORTC&=0xfe;
		state++;
	}

	if(c_int0time<0xffff)
	{
		c_int0time++;
	}
	else
	{
		l_int0time=0xffff;
	}

	if(c_int1time<0xffff)
	{
		c_int1time++;
	}
	else
	{
		l_int1time=0xffff;
	}

	////////
	PORTC&=0xf7;
	////////
}

int main(void)
{
	//
	// init IO ports
	//
	DDRD=0x00; // all inputs
	DDRC=0xff; // all inputs
	DDRB=0x00; // all inputs

	PORTB=0xff; // pull up all inputs
	PORTC=0x00; // all outputs off


	// set up and enable low freq timer interrupt
	TCCR0 = 4; // ck/256
	TCNT0 = (uint8_t) T0RELOAD; // reload
	TIMSK |= (1<<TOIE0); // enable overflow interrupt

	// enable external interrupts int0 and int1
	MCUCR=0x0a; // lower nibble sets int0 and int1 raised on logial change
	GICR=0xc0; // enable int0 and int1

	//
	// init global variables
	//

	//
	// enable global interrupts
	//
	sei();

	//
	// let loose
	//
	while (1)
	{
		if(l_int0time<0xffff)
		{
			PORTC|=0x20;
		}
		else
		{
			PORTC&=0xdf;
		}

		if(l_int1time<0xffff)
		{
			PORTC|=0x10;
		}
		else
		{
			PORTC&=0xef;
		}
		
/*
		if(mlstate)
		{
			PORTC|=0x08;
			mlstate=0;
		}
		else
		{
			PORTC&=0xf7;
			mlstate++;
		}
*/
	}
}


