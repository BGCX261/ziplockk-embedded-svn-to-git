#include "serial.h"
#include "serial1.h"
#include <stdio.h>

uint8_t transmitBuffer(uint8_t (*txFunction)(char ch),uint8_t *buffer,uint8_t bufferLen)
{
	uint8_t pos=0;

	for(pos=0;pos<bufferLen;pos++)
	{
		if(!txFunction(buffer[pos]))
		{
			return 0;
		}
	}

	return 1;
}

uint8_t transmitNullTerminatedBuffer(uint8_t (*txFunction)(char ch),uint8_t *buffer)
{
	uint8_t pos=0;

	while(buffer[pos]!=0)
	{
		if(!txFunction(buffer[pos++]))
		{
			return 0;
		}
	}

	return 1;
}

void hexDump(uint8_t (*txFunction)(char ch),uint8_t data)
{
	uint8_t temp=(data&0xf0)>>4;

	if(temp<10)
	{
		txFunction(temp+'0');
	}
	else
	{
		txFunction(temp+'A'-10);
	}

	temp=(data&0x0f);

	if(temp<10)
	{
		txFunction(temp+'0');
	}
	else
	{
		txFunction(temp+'A'-10);
	}
}

void decDump(uint8_t (*txFunction)(char ch),uint8_t data)
{
	char buffer[4];
	
	sprintf(buffer,"%03u",data);
	txFunction(buffer[0]);
	txFunction(buffer[1]);
	txFunction(buffer[2]);
}

void int8Dump(uint8_t (*txFunction)(char ch),int8_t data)
{
	char buffer[5];
	
	sprintf(buffer,"%+04d",data);
	txFunction(buffer[0]);
	txFunction(buffer[1]);
	txFunction(buffer[2]);
	txFunction(buffer[3]);
}

void int16Dump(uint8_t (*txFunction)(char ch),int16_t data)
{
	char buffer[6];
	
	sprintf(buffer,"%+04d",data);
	txFunction(buffer[0]);
	txFunction(buffer[1]);
	txFunction(buffer[2]);
	txFunction(buffer[3]);
}

void hexDumpBuffer(uint8_t (*txFunction)(char ch),uint8_t *buffer,uint8_t bufferLength)
{
	uint8_t pos=0;

	for(pos=0;pos<bufferLength;pos++)
	{
		hexDump(txFunction,buffer[pos]);
		if(pos<(bufferLength-1))
		{
			txFunction(',');
		}
	}
}

void decDumpBuffer(uint8_t (*txFunction)(char ch),uint8_t *buffer,uint8_t bufferLength)
{
	uint8_t pos=0;

	for(pos=0;pos<bufferLength;pos++)
	{
		decDump(txFunction,buffer[pos]);
		if(pos<(bufferLength-1))
		{
			txFunction(',');
		}
	}
}

void int8DumpBuffer(uint8_t (*txFunction)(char ch),int8_t *buffer,uint8_t bufferLength)
{
	uint8_t pos=0;

	for(pos=0;pos<bufferLength;pos++)
	{
		int8Dump(txFunction,buffer[pos]);
		if(pos<(bufferLength-1))
		{
			txFunction(',');
		}
	}
}

void int16DumpBuffer(uint8_t (*txFunction)(char ch),int16_t *buffer,uint8_t bufferLength)
{
	uint8_t pos=0;

	for(pos=0;pos<bufferLength;pos++)
	{
		int16Dump(txFunction,buffer[pos]);
		if(pos<(bufferLength-1))
		{
			txFunction(',');
		}
	}
}

void doubleDumpBuffer(uint8_t (*txFunction)(char ch),double *buffer,uint8_t bufferLength)
{
	uint8_t pos=0;
	char sprintfBuffer[5];

	for(pos=0;pos<bufferLength;pos++)
	{
		sprintf(sprintfBuffer,"%1.1f",buffer[pos]);
		txFunction(sprintfBuffer[0]);
		txFunction(sprintfBuffer[1]);
		txFunction(sprintfBuffer[2]);
		if(buffer[pos]<0)
		{
			txFunction(sprintfBuffer[3]);
		}

		if(pos<(bufferLength-1))
		{
			txFunction(',');
		}
	}
}

