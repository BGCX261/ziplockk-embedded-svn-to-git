/*-----------------------------------------------------------------------------
 * File: MR162.C
 * Description: Turns on and off all the ports every 0.5 sec.
 * X-tal frequency = 8 MHz
 *
 * MICROROBOT NA Inc.(www.microrobotna.com) 
 * Free Open Source. Free as in 'Free beer'.
 * You can do whatever you want with this stuff. 
 * Don't even worry about buying a beer. ~ha ha
 * - James Jeong.
 *---------------------------------------------------------------------------*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#define LED1	PB5

typedef unsigned char byte;
typedef unsigned int word;

// 1msec UNIT delay function
void delay_1ms(unsigned int i)
{
	word j;
	while(i--)
	{
		j=11415;   // 8Mhz Exteranl Crystal(CKSEL3..0 = 1,1,1,1)
		while(j--);
	}
}

void ports_init(void)
{
	DDRA = 0xff;	//Configures PORTA as an output port.
	DDRB = 0xff;	//Configures PORTB as an output port.
	DDRC = 0xff;    //Configures PORTC as an output port.
	DDRD = 0xff;	//Configures PORTD as an output port.
	DDRE = 0xff;	//Configures PORTE as an output port.
}

void ports_set(void)
{
	PORTA = 0xff;	//Outputs 0xff to PORTA. 
	PORTB = 0xff;	//Outputs 0xff to PORTB.
	PORTC = 0xff;	//Outputs 0xff to PORTC.
	PORTD = 0xff;	//Outputs 0xff to PORTD.
	PORTE = 0xff;	//Outputs 0xff to PORTE.
}

void ports_clear(void)
{
	PORTA = 0;	//Outputs 0 to PORTA. 
	PORTB = 0;	//Outputs 0 to PORTB.
	PORTC = 0;	//Outputs 0 to PORTC.
	PORTD = 0;	//Outputs 0 to PORTD.
	PORTE = 0;	//Outputs 0 to PORTE.
}

void start_signal(void)
{
	byte c;
	for(c=5; c>0; --c)
	{
		PORTB &= ~_BV(LED1);  // Bit clear.= Turn On LED1.		
		delay_1ms(20);        // 0.2 sec delay.
		PORTB |= _BV(LED1);   // Bit set.= Turn Off LED1.
		delay_1ms(20);        // 0.2 sec delay.
	}	
}

int main(void)
{
	ports_init();	//Ports Initialization
	start_signal(); // Toggle LED1 five times.

	while(1)      // Keeps toggling all ports every 0.5 sec.
	{
		ports_set();
		delay_1ms(200);
		ports_clear();
		delay_1ms(200);
	}
}
