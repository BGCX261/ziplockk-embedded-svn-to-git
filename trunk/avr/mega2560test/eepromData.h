#ifndef _EEPROMDATA_H_
#define _EEPROMDATA_H_

#include <inttypes.h>
#include <avr/eeprom.h>

// UBX Command : Baud Rate to 38400
uint8_t EEMEM ee_setBaudRate[]= \
	{0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00, \
	 0x00,0x00,0xD0,0x08,0x00,0x00,0x00,0x96, \
	 0x00,0x00,0x07,0x00,0x03,0x00,0x00,0x00, \
	 0x00,0x00,0x93,0x90};
uint8_t EEMEM ee_setBaudRateLength=28;

// UBX Command : Rate to 5Hz
uint8_t EEMEM ee_setUpdateRate[]= \
	{0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00, \
	 0x01,0x00,0x01,0x00,0xDE,0x6A};
uint8_t EEMEM ee_setUpdateRateLength=14;

uint8_t *setBaudRate;
uint8_t setBaudRateLength;
uint8_t *setUpdateRate;
uint8_t setUpdateRateLength;

void GetEEPromData();

#endif

