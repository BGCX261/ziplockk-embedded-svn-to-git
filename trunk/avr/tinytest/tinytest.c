#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#define F_CPU			1000000
#define BAUD_RATE		19200

#define T0RELOAD		-100
#define FAILTIME		20	

#define CMDTIMEOUT		100
#define CMDSCAN			0
#define CMDBUILD		1
#define CMDMAXLEN		10

volatile uint8_t state=0;
volatile uint8_t failcount=0;

// external interrupt pin
SIGNAL(SIG_INTERRUPT0)
{
	failcount=0;
}

//timer 0 overflow
SIGNAL(SIG_OVERFLOW0)
{
	TCNT0 = (uint8_t) T0RELOAD;	// reload

	if(state)
	{
		PORTA|=0x01;
		state=0;
	}	
	else
	{
		PORTA&=0xfe;
		state=1;
	}

	if(failcount<FAILTIME)
	{
		failcount++;
		PORTA&=0xfd;
	}
	else
	{
		if(state)
		{
			PORTA|=0x02;
		}
		else
		{
			PORTA&=0xfd;
		}
	}
}

int main(void)
{
	//
	// init IO ports
	//
	DDRA=0xFF;	// all outputs
	PORTA=0x01;	// all off

	TCCR0 = 5; // ck/1024
	TCNT0 = (uint8_t) T0RELOAD;	// reload
	TIMSK |= (1<<TOIE0); // enable overflow interrupt


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
	}
}


