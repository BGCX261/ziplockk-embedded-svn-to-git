#include <avr/io.h>
#include <avr/interrupt.h>
#include "twi.h"
#include "serial0.h"

#define  DS1307_W 0xD0
#define  DS1307_R 0xD1

enum TwiState
{
	txStart,
	txAddress,
	txRegister
};

volatile enum TwiState state;
volatile uint8_t buffer[6];

SIGNAL(SIG_2WIRE_SERIAL)
{
	switch(state)
	{
		case txStart :
		{
			usart0_Tx('S');
			state=txAddress;
    			TWDR = DS1307_W;
    			TWCR = (1<<TWEN)|(1<<TWIE)|
			       (1<<TWINT)|(1<<TWEN);
			break;
		}
		case txAddress :
		{
			usart0_Tx('A');
			state=txRegister;
    			TWDR = 0x00;
    			TWCR = (1<<TWEN)|(1<<TWIE)|
			       (1<<TWINT)|(1<<TWEN);
			break;
		}
		case txRegister :
		{
			usart0_Tx('R');
    			TWCR = (1<<TWEN)|(0<<TWIE)|
			       (1<<TWINT)|(1<<TWSTO);
			break;
		}
		default :
		{
			TWCR = (0<<TWIE);
			usart0_Tx('@');
			break;
		}
	}
}

void twiInit()
{
    //set SCL to 400kHz
    TWSR = 0x00;
    TWBR = 72;
}

void twiInitiate()
{
	usart0_Tx('I');
	state=txStart;
	TWCR = (1<<TWEN)|                        // TWI Interface enabled.
	       (1<<TWIE)|(1<<TWINT)|            // Enable TWI Interupt and clear the flag.
	       (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)| // Initiate a START condition.
	       (0<<TWWC);                             
}

void twiStop()
{
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void twiWrite(uint8_t data)
{
    TWDR = data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}

uint8_t twiReadACK(void)
{
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}

uint8_t twiReadNACK(void)
{
    TWCR = (1<<TWINT)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}

uint8_t twiGetStatus(void)
{
    uint8_t status;
    //mask status
    status = TWSR & 0xF8;
    return status;
}

