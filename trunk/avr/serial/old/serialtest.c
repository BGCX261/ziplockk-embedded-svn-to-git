#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#define F_CPU			8000000
#define BAUD_RATE		19200
#define T2RELOADVAL		-20
#define TX_BUF_SIZE		32
#define RX_BUF_SIZE		32

volatile uint16_t tocker;
volatile uint16_t pwmCount;
volatile uint16_t pwmPos;
volatile uint16_t clock;
static uint8_t rxBuf[RX_BUF_SIZE];
static volatile uint8_t rxHead;
static volatile uint8_t rxTail;
static volatile uint8_t rxCount;

#define CMDTIMEOUT		100
#define CMDSCAN			0
#define CMDBUILD		1
#define CMDMAXLEN		10

static uint8_t motorPos[4];

#define MIN_PWM			50	
#define MAX_PWM			180

static uint8_t cmd[CMDMAXLEN];
static volatile uint8_t cmdPos;
static volatile uint8_t cmdState;

int putchar(int ch)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = ch;
	return ch;
}

int puts(char *s)
{
	while (*s)
		putchar(*s++);
	return 0;
}

int getch(void)
{
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void pushRx(unsigned char data)
{
	if(rxCount<RX_BUF_SIZE)
	{
		rxBuf[rxHead++]=data;
		rxHead%=RX_BUF_SIZE;
		rxCount++;
	}
}

int popRx()
{
	volatile int retVal=-1;

	if(rxCount) // not empty
	{
		retVal=rxBuf[rxTail++];
		rxTail%=RX_BUF_SIZE;
		rxCount--;
	}

	return retVal;
}

//
// timer 2 interrupt
// triggers every 10uS
//
SIGNAL(SIG_OVERFLOW2)
{
	TCNT2 = (uint8_t) T2RELOADVAL;	// reload

	tocker++;

	if(pwmCount==0)
	{
		PORTA|=0x0c;
	}
	else
	{
		if(pwmCount==motorPos[0])
		{
			PORTA&=0xfb;
		}

		if(pwmCount==motorPos[1])
		{
			PORTA&=0xf7;
		}
	}

	if(pwmCount==2000)
	{
		pwmCount=0;
	}
	else
	{
		pwmCount++;
	}

}

SIGNAL(SIG_USART0_RECV)
{
	pushRx(UDR0);
}

void processSet()
{
	if(cmd[1]=='M')
	{
		// set motor if valid motor number
		if((cmd[2]>='1')&&(cmd[2]<='4'))
		{
			volatile uint8_t pos=0;

			if((cmd[3]>='0')&&(cmd[3]<='9'))
			{
				pos+=((cmd[3]-'0')*16);
			}
			else if((cmd[3]>='A')&&(cmd[3]<='F'))
			{
				pos+=((cmd[3]-'A'+10)*16);
			}
			else
			{
				return;
			}

			if((cmd[4]>='0')&&(cmd[4]<='9'))
			{
				pos+=((cmd[4]-'0'));
			}
			else if((cmd[4]>='A')&&(cmd[4]<='F'))
			{
				pos+=((cmd[4]-'A'+10));
			}
			else
			{
				return;
			}
			
			motorPos[(cmd[2]-'1')]=pos;
		}
	}
	else if(cmd[1]=='P')
	{
		// set port
	}
}

void processRead()
{
}

int main(void)
{
	//
	// setup USART
	//
	UBRR0L = (uint8_t)(F_CPU/(BAUD_RATE*16L)-1);
	UBRR0H = (F_CPU/(BAUD_RATE*16L)-1) >> 8;
	UCSR0A = 0x00;
	UCSR0C = 0x86;
	UCSR0B = _BV(TXEN0)|_BV(RXEN0)|_BV(RXCIE0);

	//
	// init IO ports
	//
	DDRA=0xFF;	// all outputs
	PORTA=0x00;	// all off
	DDRB=0x00;	// all inputs
	PORTB=0xff;	// active pullups

	//
	// init Timer2 for 1 mS interrupts
	//
	
	/*
	 * 8000000Hz crystal
	 * /8 (2) gives 1uS per count
	 */
	TCCR2 = 2;
	TCNT2 = (uint8_t) T2RELOADVAL;
	TIMSK |= (1<<TOIE2);


	//
	// init global variables
	//
	tocker=0;
	clock=0;
	rxHead=0;
	rxTail=0;
	rxCount=0;
	cmdState=CMDSCAN;
	pwmCount=0;
	pwmPos=MIN_PWM;

	motorPos[0]=25;
	motorPos[1]=25;

	//
	// enable global interrupts
	//
	sei();

	//
	// let loose
	//
	while (1)
	{
		// process periodics

		if(tocker>=10000)
		{
			tocker=0;

			// flash health LED

			if(PORTA&0x01)
			{
				PORTA&=0xfe;
			}
			else
			{
				PORTA|=0x01;
			}

			// increment command timeout clock
			clock++;

			// check command timeout clock for expiry if not in scan mode
			if((clock>CMDTIMEOUT)&&(cmdState!=CMDSCAN))
			{
				cmdState=CMDSCAN;
			}

		}
		
		// process serial data
		
		volatile int data=popRx();

		if(data>=0)
		{
			// serial data to process
			if(cmdState==CMDSCAN)
			{
				if(data=='@')
				{
					// command start found
					cmdState=CMDBUILD;
					clock=0;
					cmdPos=0;
				}

				putchar(data);
			}
			else if(cmdState==CMDBUILD)
			{
				if(data=='#')
				{
					// command terminator found
					// process the command

					if(cmd[0]=='S')
					{
						// set command
						processSet();
					}
					else if(cmd[0]=='R')
					{
						// read command
						processRead();
					}

					// no matter what - we're done - back into scan mode
					cmdState=CMDSCAN;
				}
				else
				{
					if(cmdPos<CMDMAXLEN)
					{
						// add last received byte to command block
						cmd[cmdPos++]=data;
					}
					else
					{
						// abort command scan - command too long
						cmdState=CMDSCAN;
					}
				}
			}		
		}
	}
}


