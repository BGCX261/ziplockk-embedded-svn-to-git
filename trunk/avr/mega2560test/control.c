#include "control.h"
#include "serial0.h"
#include "serial2.h"
#include "serial.h"
#include "gps.h"
#include <stdio.h>
#include <avr/eeprom.h>
#include <util/delay.h>

enum ControlState controlState=ControlMenu;
uint8_t idleTicks=0;
uint8_t idleSeconds=0;
uint8_t autoStop=0;
uint8_t EEMEM ee_fileCounter;
uint8_t EEMEM ee_fileCounter_Check; 

uint8_t getFileCounter()
{
	uint8_t Counter;
	uint8_t Counter_Check;

	Counter=eeprom_read_byte(&ee_fileCounter);
	Counter_Check=eeprom_read_byte(&ee_fileCounter_Check);

	if(Counter!=(Counter_Check^0xff))
	{
		Counter=0;
		Counter_Check=(Counter^0xff);
		eeprom_write_byte(&ee_fileCounter,Counter);
		eeprom_write_byte(&ee_fileCounter_Check,Counter_Check);
		return 0;
	}

	Counter++;
	Counter_Check=(Counter^0xff);
	eeprom_write_byte(&ee_fileCounter,Counter);
	eeprom_write_byte(&ee_fileCounter_Check,Counter_Check);

	return Counter;
}

void menu()
{
	decDump(usart0_Tx,hwm0);
	usart0_Tx(':');
	decDump(usart0_Tx,hwm2);
	usart0_Tx('\r');
	usart0_Tx('\n');
	char buffer[100]="\n\r\n\rD : Live Data Stream\n\rL : Logger Passthrough\n\rR : Record Data\n\r<ESC> : Stop and Return\n\r";
	transmitNullTerminatedBuffer(usart0_Tx,(uint8_t *)buffer);
}

void startRecording()
{
	char buffer[80];
	uint8_t hours=workingGPSData.hours;
	uint8_t minutes=workingGPSData.minutes;
	uint8_t seconds=workingGPSData.seconds;
	uint16_t fileCounter=getFileCounter();

	sprintf(buffer,"\r\rnew %02x%02u%02u%02u.log\r",
			fileCounter,
			hours,
			minutes,
			seconds);
	transmitNullTerminatedBuffer(usart2_Tx,(uint8_t *)buffer);
	_delay_ms(200);
	sprintf(buffer,"\r\rappend %02x%02u%02u%02u.log\r",
			fileCounter,
			hours,
			minutes,
			seconds);
	transmitNullTerminatedBuffer(usart2_Tx,(uint8_t *)buffer);
	_delay_ms(200);
}

void control()
{
	char ch=0;

	if(!usart0_Rx(&ch))
	{
		ch=0;
	}

	switch(controlState)
	{
		case ControlMenu :
		{
			menu();
			controlState=ControlIdle;

			break;
		}

		case ControlIdle :
		{
			if((ch=='L')||(ch=='l'))
			{
				controlState=ControlLoggerPassthrough;
			}
			else if((ch=='D')||(ch=='d'))
			{
				controlState=ControlDataStream;
			}
//			else if(((ch=='R')||(ch=='r'))||
//				(!(PINK&0x80)) )
			else if(((ch=='R')||(ch=='r')))
			{
				startRecording();
				autoStop=0;
				idleTicks=0;
				idleSeconds=0;
				controlState=ControlLogging;
			}
			else if((ch=='F')||(ch=='f'))
			{
				char buffer[80];
				controlState=ControlRecoverFileListA;
				sprintf(buffer,"ls *.log\r");
				transmitNullTerminatedBuffer(usart2_Tx,(uint8_t *)buffer);
			}
			else if(ch!=0)
			{
				menu();
			}

			break;
		}

		case ControlLoggerPassthrough :
		{

			if(ch==27)
			{
				controlState=ControlMenu;
			}
			else if(ch!=0)
			{
				usart2_Tx(ch);
			}

			break;
		}

		case ControlDataStream :
		{
			if(ch==27)
			{
				controlState=ControlMenu;
			}

			break;
		}

		case ControlLogging :
		{
//			if((ch==27)||
//			   (PINK&0x80))
			if((ch==27))
			{
				controlState=ControlMenu;
				_delay_ms(500);
				char buffer[3]={26,26,26}; // stop logging sequence
				transmitBuffer(usart2_Tx,(uint8_t *)buffer,3);
				menu();
			}

			break;
		}
	}
}
