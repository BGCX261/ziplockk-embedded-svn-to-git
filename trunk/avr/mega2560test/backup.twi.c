#include <avr/io.h>

enum
{
    writeAddressSelect,
    setRegisterAddress,
    readAddressSelect,
    readDataByte
} rtcState;

SIGNAL(SIG_2WIRE_SERIAL)
{
}

void twiInit()
{
    //set SCL to 400kHz
    TWSR = 0x00;
    TWBR = 0x0C;
    //enable TWI
    TWCR = (1<<TWEN);
}

void twiStart()
{
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
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

void readDS1307(void)
{
}

