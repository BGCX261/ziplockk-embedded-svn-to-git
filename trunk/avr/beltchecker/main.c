#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#define F_CPU			1000000
#define BAUD_RATE		19200

#define T0RELOAD		-25
#define FAILTIME		20	

#define CMDTIMEOUT		100
#define CMDSCAN			0
#define CMDBUILD		1
#define CMDMAXLEN		10

volatile uint8_t state=0;
volatile uint8_t failcount=0;
volatile uint8_t fail=1;

// analogue comparator interrupt
//SIGNAL(SIG_ANA_COMP)
//{
//	failcount=0;
//}

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

	if(ACSR&0x10)
	{
		// ana comparator tripped
		// reset (write 1 to ACI)
		ACSR=0x10;
		if(failcount>0)
		{
			failcount-=1;
		}
		else
		{
			fail=0;
		}
	}
	else
	{
		if(failcount<FAILTIME)
		{
			failcount+=1;
		}
		if(failcount<FAILTIME)
		{
			failcount+=1;
		}
		if(failcount<FAILTIME)
		{
			failcount+=1;
		}
		else
		{
			fail=1;
		}
	}
	
	if(fail)
	{
//		if(state)
//		{
//			PORTA&=0xfd;
//		}
//		else
//		{
//			PORTA|=0x02;
//		}

		PORTA|=0x02;
	}
	else
	{
		PORTA&=0xfd;
	}
}

int main(void)
{
	//
	// init IO ports
	//
	DDRA=0x03;	// bits 0 and 1 outputs
	PORTA=0x00;	// all off

	// timer setup
	TCCR0 = 5; // ck/1024
	TCNT0 = (uint8_t) T0RELOAD;	// reload
	TIMSK |= (1<<TOIE0); // enable overflow interrupt

	// analogue comparator setup
	ACSR=0x02; // ACIS0 and ACIS1 = 1 rising edge detect

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


