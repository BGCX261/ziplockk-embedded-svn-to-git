#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "gps.h"
#include "serial0.h"
#include "serial.h"

enum SearchState
{
	searchingDollar,
	searchingG,
	searchingP,
	searchingGorV,
	searchingL1,
	searchingL2,
	searchingT,
	searchingG2,
	assembleGPGLL,
	assembleGPVTG
};

GPSData workingGPSData;
GPSData currentGPSData;
char assemblyBuffer[80];
uint8_t assemblyBufferPos=0;
uint8_t commas[10];
enum SearchState searchState=searchingDollar;

void initGPS(void (*initFunction)(uint16_t baudRate),uint8_t (*txFunction)(char ch))
{
	// UBX Command : Baud Rate to 38400
	uint8_t setBaudRate[] \
		={0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00, \
		0x00,0x00,0xD0,0x08,0x00,0x00,0x00,0x96, \
		0x00,0x00,0x07,0x00,0x03,0x00,0x00,0x00, \
		0x00,0x00,0x93,0x90};
	uint8_t	 setBaudRateLength=28;

	// UBX Command : Rate to 5Hz
	uint8_t setUpdateRate[] \
		={0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00, \
		0x01,0x00,0x01,0x00,0xDE,0x6A};
	uint8_t setUpdateRateLength=14;

	_delay_ms(500);
	initFunction(9600);
	_delay_ms(200);
	transmitBuffer(txFunction,setBaudRate,setBaudRateLength);
	_delay_ms(200);
	transmitBuffer(txFunction,setBaudRate,setBaudRateLength);
	_delay_ms(200);
	transmitBuffer(txFunction,setBaudRate,setBaudRateLength);
	_delay_ms(200);
	initFunction(38400);
	_delay_ms(200);
	transmitBuffer(txFunction,setUpdateRate,setUpdateRateLength);
	_delay_ms(200);
	transmitBuffer(txFunction,setUpdateRate,setUpdateRateLength);
	_delay_ms(200);
	transmitBuffer(txFunction,setUpdateRate,setUpdateRateLength);

	workingGPSData.lat_sign='?';
	workingGPSData.lon_sign='?';
}

void dumpWorkingGPSData(uint8_t (*txFunction)(char ch))
{
	char buffer[160];
	int pos=0;
	
	sprintf(buffer,"%02u,%02u,%02u.%02u,%03u,%c,%02d,%02u.%05lu,%c,%03d,%02u.%05lu",
							  workingGPSData.hours,
							  workingGPSData.minutes,
							  workingGPSData.seconds,
							  workingGPSData.centiseconds,
							  workingGPSData.speed,
							  workingGPSData.lat_sign,
							  workingGPSData.lat_degrees,
							  workingGPSData.lat_minutes,
							  workingGPSData.lat_minutes_decimal,
							  workingGPSData.lon_sign,
							  workingGPSData.lon_degrees,
							  workingGPSData.lon_minutes,
							  workingGPSData.lon_minutes_decimal);

	transmitNullTerminatedBuffer(txFunction,buffer);
}

uint8_t findCommas(char *buffer, uint8_t bufferLength)
{
	uint8_t pos=0;
	uint8_t commaPos=0;

	for(pos=0;pos<bufferLength;pos++)
	{
		if(buffer[pos]==',')
		{
			commas[commaPos++]=pos;
		}
	}

	return commaPos;
}

// LAST  : $GPGLL,5620.08489,N,00319.12984,W,195126.40,A,A*76

void processGPGLL(char *buffer,uint8_t bufferLength)
{
	uint8_t pos=0;
	uint8_t count=0;

	count=findCommas(buffer,bufferLength);

	if(count!=7)
	{
		// we don't understand this sentance
		return;
	}

	// data validity
	pos=(commas[5]+1); // 6th comma
	if(buffer[pos]!='A')
	{
		// this indicates invalid data
		workingGPSData.valid=0;
		return;	
	}
	else
	{
		workingGPSData.valid=1;
	}

	// time
	pos=(commas[4]+1); // 5th comma
	if(buffer[pos]!=',')
	{
		workingGPSData.hours=(buffer[pos++]-48)*10;
		workingGPSData.hours+=(buffer[pos++]-48);
		workingGPSData.minutes=(buffer[pos++]-48)*10;
		workingGPSData.minutes+=(buffer[pos++]-48);
		workingGPSData.seconds=(buffer[pos++]-48)*10;
		workingGPSData.seconds+=(buffer[pos++]-48);
		pos++; //skip the decimal point
		workingGPSData.centiseconds=(buffer[pos++]-48)*10;
		workingGPSData.centiseconds+=(buffer[pos]-48);
	}

	// latitude
	if((commas[1]-commas[0])==11)
	{
		pos=(commas[0]+1); // 5th comma
	
		workingGPSData.lat_degrees=(buffer[pos++]-'0')*10;
		workingGPSData.lat_degrees+=(buffer[pos++]-'0');
		workingGPSData.lat_minutes=(buffer[pos++]-'0')*10;
		workingGPSData.lat_minutes+=(buffer[pos++]-'0');
		pos++; // skip the decimal point
		workingGPSData.lat_minutes_decimal=(buffer[pos++]-'0')*(uint32_t)10000;
		workingGPSData.lat_minutes_decimal+=(buffer[pos++]-'0')*(uint32_t)1000;
		workingGPSData.lat_minutes_decimal+=(buffer[pos++]-'0')*(uint32_t)100;
		workingGPSData.lat_minutes_decimal+=(buffer[pos++]-'0')*(uint32_t)10;
		workingGPSData.lat_minutes_decimal+=(buffer[pos++]-'0');
	}

	// latitude (North or South)
	pos=(commas[1]+1); // 2nd comma
	workingGPSData.lat_sign=buffer[pos];

	// longitude
	if((commas[3]-commas[2])==12)
	{
		pos=(commas[2]+1); // 5th comma
	
		workingGPSData.lon_degrees=(buffer[pos++]-'0')*100;
		workingGPSData.lon_degrees+=(buffer[pos++]-'0')*10;
		workingGPSData.lon_degrees+=(buffer[pos++]-'0');
		workingGPSData.lon_minutes=(buffer[pos++]-'0')*10;
		workingGPSData.lon_minutes+=(buffer[pos++]-'0');
		pos++; // skip the decimal point
		workingGPSData.lon_minutes_decimal=(buffer[pos++]-'0')*(uint32_t)10000;
		workingGPSData.lon_minutes_decimal+=(buffer[pos++]-'0')*(uint32_t)1000;
		workingGPSData.lon_minutes_decimal+=(buffer[pos++]-'0')*(uint32_t)100;
		workingGPSData.lon_minutes_decimal+=(buffer[pos++]-'0')*(uint32_t)10;
		workingGPSData.lon_minutes_decimal+=(buffer[pos++]-'0');
	}

	// longitude (East or West)
	pos=(commas[3]+1); // 2nd comma
	workingGPSData.lon_sign=buffer[pos];

	/*
	workingGPSData.lat=(double)workingGPSData.lat_degrees;
	workingGPSData.lat+=(workingGPSData.lat_minutes/60.0);
	workingGPSData.lat+=((workingGPSData.lat_minutes_decimal)/100000.0)/60.0;
	if(workingGPSData.lat_sign=='-')
	{
		workingGPSData.lat*=-1;
	}

	workingGPSData.lon=(double)workingGPSData.lon_degrees;
	workingGPSData.lon+=(workingGPSData.lon_minutes/60.0);
	workingGPSData.lon+=((workingGPSData.lon_minutes_decimal)/100000.0)/60.0;
	if(workingGPSData.lon_sign=='-')
	{
		workingGPSData.lon*=-1;
	}
	*/
}

// FIRST : $GPVTG,,T,,M,0.288,N,0.533,K,A*24

void processGPVTG(char *buffer,uint8_t bufferLength)
{
	uint8_t pos=0;
	uint8_t count=0;
	char speedBuffer[3];

	count=findCommas(buffer,bufferLength);

	if(count!=9)
	{
		workingGPSData.speed=0;
		return;
	}

	pos=(commas[6]+1); // 7th comma

	// check to see whether we have any data, if not the next
	// character will be another comma, if so, speed=0

	if(buffer[pos]!=',')
	{
		// extract the whole part of the speed
		count=0;
		while(buffer[pos]!='.')
		{
			speedBuffer[count++]=buffer[pos++];
		}

		if(count==1)
		{
			workingGPSData.speed=speedBuffer[0]-'0';
		}
		else if(count==2)
		{
			workingGPSData.speed=(speedBuffer[0]-'0')*10;
			workingGPSData.speed+=speedBuffer[1]-'0';
		}
		else if(count==3)
		{
			workingGPSData.speed=(speedBuffer[0]-'0')*100;
			workingGPSData.speed+=(speedBuffer[1]-'0')*10;
			workingGPSData.speed+=speedBuffer[2]-'0';
		}
		else
		{
			workingGPSData.speed=0;
		}
	}
	else
	{
		workingGPSData.speed=0;
	}
}

uint8_t verifyChecksum(char *buffer, uint8_t sentanceEnd)
{
	uint8_t calcCRC=0;
	uint8_t sentanceCRC=0;

	for(uint8_t loop=1;loop<sentanceEnd;loop++)
	{
		calcCRC^=buffer[loop];
	}

	if(((uint8_t)buffer[sentanceEnd+1]>=(uint8_t)'0')&&
	   ((uint8_t)buffer[sentanceEnd+1]<=(uint8_t)'9'))
	{
		sentanceCRC=buffer[sentanceEnd+1]-'0';
	}
	else
	{
		sentanceCRC=buffer[sentanceEnd+1]-'A'+10;
	}

	sentanceCRC<<=4;

	if(((uint8_t)buffer[sentanceEnd+2]>=(uint8_t)'0')&&
	   ((uint8_t)buffer[sentanceEnd+2]<=(uint8_t)'9'))
	{
		sentanceCRC+=(buffer[sentanceEnd+2]-'0');
	}
	else
	{
		sentanceCRC+=(buffer[sentanceEnd+2]-'A'+10);
	}

	if(calcCRC==sentanceCRC)
	{
		return 1;
	}

	return 0;
}

void processGPSCharacter(char ch)
{
	switch(searchState)
	{
		case searchingDollar :
		{
			if(ch=='$')
			{
				assemblyBufferPos=0;
				assemblyBuffer[assemblyBufferPos++]=ch;
				searchState=searchingG;
			}

			break;
		}

		case searchingG :
		{
			if(ch=='G')
			{
				assemblyBuffer[assemblyBufferPos++]=ch;
				searchState=searchingP;
			}
			else
			{
				searchState=searchingDollar;
			}

			break;
		}

		case searchingP :
		{
			if(ch=='P')
			{
				assemblyBuffer[assemblyBufferPos++]=ch;
				searchState=searchingGorV;
			}
			else
			{
				searchState=searchingDollar;
			}

			break;
		}

		case searchingGorV :
		{
			if(ch=='G')
			{
				assemblyBuffer[assemblyBufferPos++]=ch;
				searchState=searchingL1;
			}
			else if(ch=='V')
			{
				assemblyBuffer[assemblyBufferPos++]=ch;
				searchState=searchingT;
			}
			else
			{
				searchState=searchingDollar;
			}

			break;
		}

		case searchingL1 :
		{
			if(ch=='L')
			{
				assemblyBuffer[assemblyBufferPos++]=ch;
				searchState=searchingL2;
			}
			else
			{
				searchState=searchingDollar;
			}

			break;
		}

		case searchingL2 :
		{
			if(ch=='L')
			{
				assemblyBuffer[assemblyBufferPos++]=ch;
				searchState=assembleGPGLL;
			}
			else
			{
				searchState=searchingDollar;
			}

			break;
		}

		case searchingT :
		{
			if(ch=='T')
			{
				assemblyBuffer[assemblyBufferPos++]=ch;
				searchState=searchingG2;
			}
			else
			{
				searchState=searchingDollar;
			}

			break;
		}

		case searchingG2 :
		{
			if(ch=='G')
			{
				assemblyBuffer[assemblyBufferPos++]=ch;
				searchState=assembleGPVTG;
			}
			else
			{
				searchState=searchingDollar;
			}

			break;
		}

		case assembleGPGLL :
		{
			if(ch!=0x0d) // look for line terminator
			{
				PORTB|=0x80;
				assemblyBuffer[assemblyBufferPos++]=ch;
			}
			else
			{
				PORTB&=0x7f;
				assemblyBuffer[assemblyBufferPos]=0;
				searchState=searchingDollar;

				if(verifyChecksum(assemblyBuffer, assemblyBufferPos-3))
				{
					processGPGLL(assemblyBuffer, assemblyBufferPos-3);
				}
			}

			break;
		}

		case assembleGPVTG :
		{
			if(ch!=0x0d) // look for line terminator
			{
				PORTB|=0x80;
				assemblyBuffer[assemblyBufferPos++]=ch;
			}
			else
			{
				PORTB&=0x7f;
				assemblyBuffer[assemblyBufferPos]=0;
				searchState=searchingDollar;

				if(verifyChecksum(assemblyBuffer, assemblyBufferPos-3))
				{
					processGPVTG(assemblyBuffer, assemblyBufferPos-3);
				}
			}

			break;
		}

		default :
		{
			exit(1);
		}
	}
}

