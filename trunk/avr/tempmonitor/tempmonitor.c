#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#define F_CPU 8000000
#define BAUD_RATE 9600
#define TX_BUF_SIZE		32
#define RX_BUF_SIZE		32

#define NUMAVE			100

static uint8_t rxBuf[RX_BUF_SIZE];
static volatile uint8_t rxHead;
static volatile uint8_t rxTail;
static volatile uint8_t rxCount;

static volatile uint16_t ticker=0;
static volatile uint16_t adc0val=0;
static volatile uint16_t adc0ave=0;
static volatile uint8_t aveCount=0;
static volatile uint8_t temp[20];

#define T0RELOAD -195

//uint8_t test[] __attribute__ ((section (".eeprom"))) = "Fergus Duncan";

volatile uint8_t state=0;

int putchar(int ch)
{
	while (!(UCSRA & (1 << UDRE)));
	UDR = ch;
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
	while (!(UCSRA & (1 << RXC)));
	return UDR;
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

// external interrupt pin 0
SIGNAL(SIG_INTERRUPT0)
{
}

// external interrupt pin 1
SIGNAL(SIG_INTERRUPT1)
{
}

//timer 0 overflow
SIGNAL(SIG_OVERFLOW0)
{
	TCNT0 = (uint8_t) T0RELOAD; // reload

	if(state)
	{
		PORTC|=0x10;
		state=0;
	}
	else
	{
		PORTC&=0xef;
		state++;
	}

	ticker++;
}

SIGNAL(SIG_UART_RECV)
{
	pushRx(UDR);
}

/*
SIGNAL(SIG_ADC)
{
	adc0ave+=(uint8_t)ADC;
	aveCount++;

	if(aveCount==NUMAVE)
	{
		adc0val=adc0ave/NUMAVE;
	}
	else
	{
		ADCSR|=(1<<ADSC);
	}
}
*/

int main(void)
{
	//
	// setup USART
	//
	UBRRL = (uint8_t)(F_CPU/(BAUD_RATE*16L)-1);
	UBRRH = (F_CPU/(BAUD_RATE*16L)-1) >> 8;
	UCSRA = 0x00;
	UCSRC = 0x86;
	UCSRB = _BV(TXEN)|_BV(RXEN)|_BV(RXCIE);

	//
	// init IO ports
	//
	DDRD=0x00; // all inputs
	DDRC=0x30; // all inputs except PC4, PC5
	DDRB=0x00; // all inputs

	PORTB=0xff; // pull up all inputs
	PORTC=0x00; // all outputs off


	// set up and enable low freq timer interrupt
	TCCR0 = 5; // ck/256
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

	puts("BOOT\r\n");

	/*
	// initilise ADC - channel 0 only - single shot mode
	ADMUX=0;
	ADCSR|=(1<<ADPS2);  // prescaler
	ADCSR|=(1<<ADIE);   // interrupt on sample complete enable
	ADCSR|=(1<<ADEN);   // enable

	// start ADC conversion	
	ADCSR|=(1<<ADSC);
	*/

	//
	// let loose
	//
	while (1)
	{
		if(ticker>=400)
		{
			ticker=0;
			sprintf(temp,"%d\n",adc0val);
			puts(temp);

			// start ADC conversion	
			adc0ave=0;
			aveCount=0;
			//ADCSR|=(1<<ADSC);
		}
	}
}


