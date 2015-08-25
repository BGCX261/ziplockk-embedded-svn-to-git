#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "common.h"
#include <util/delay.h>
#include "display.h"
#include "serial.h"
#include "adc.h"
#include "freq.h"
#include "tc.h"

#define MODE_ANALOGUE 0
#define MODE_FREQ     1
#define MODE_TC	      2
#define MODE_DIGITAL  3

#define CLEAR "[2J"
#define HOME "[H"
#define CURSOR_OFF "[2m"
#define RESET "c"
#define LOCK_BAUD "[4c"

char temp[20];
uint8_t displayMode=MODE_ANALOGUE;

void command(char *c)
{
	putChar(27);
	putString(c);
}

void displayInit()
{
	putChar(13);
	command(CURSOR_OFF);
	command(CLEAR);
//	putString("BOOT");
//	_delay_ms(1000);
//	command(CLEAR);

	displayMode=MODE_FREQ;
}

void displayAnalogue()
{
	command(HOME);
	sprintf(temp,"0:%03d 1:%03d 2:%03d",adcValues[0],adcValues[1],adcValues[2]);
	putString(temp);
	putChar(13);
	sprintf(temp,"3:%03d 4:%03d 5:%03d",adcValues[3],adcValues[4],adcValues[5]);
	putString(temp);
	putChar(13);
	sprintf(temp,"6:%03d 7:%03d",adcValues[6],adcValues[7]);
	putString(temp);
	putChar(13);
	putString("Analogue");
}

void displayFreq()
{
	command(HOME);
	sprintf(temp,"0:%06u 1:%06u",freqValue[0],freqValue[1]);
	putString(temp);
	putChar(13);
	sprintf(temp,"2:%06u 3:%06u",freqValue[2],freqValue[3]);
	putString(temp);
	putChar(13);
	putChar(13);
	putString("Frequency");
}

void displayTC()
{
	command(HOME);
	sprintf(temp,"FS:%06u RS:%06u",frontSpeed,rearSpeed);
	putString(temp);
	putChar(13);
	sprintf(temp,"SL:%06u ST:%03u",slip,status);
	putString(temp);
	putChar(13);
	putChar(13);
	putString("Traction Control");
}

void displayRun()
{
	if(!(PINC&0x01))
	{
		displayMode=MODE_ANALOGUE;
		command(CLEAR);
	}	
	else if(!(PINC&0x02))
	{
		displayMode=MODE_FREQ;
		command(CLEAR);
	}
	else if(!(PINC&0x04))
	{
		displayMode=MODE_TC;
		command(CLEAR);
	}
	else if(!(PINC&0x08))
	{
		displayMode=MODE_DIGITAL;
		command(CLEAR);
	}

	switch(displayMode)
	{
		case MODE_ANALOGUE :
		{
			displayAnalogue();
			break;
		}
		case MODE_FREQ :
		{
			displayFreq();
			break;
		}
		case MODE_TC :
		{
			displayTC();
			break;
		}
		case MODE_DIGITAL :
		{
			break;
		}
	}
}

