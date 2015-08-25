#ifndef _GPS_H_
#define _GPS_H_

#include <stdint.h>

typedef struct 
{
	uint8_t valid;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t centiseconds;
	uint8_t speed;
	char lat_sign;
	int16_t lat_degrees;
	int8_t lat_minutes;
	int32_t lat_minutes_decimal;
	char lon_sign;
	int16_t lon_degrees;
	int8_t lon_minutes;
	int32_t lon_minutes_decimal;
} GPSData;

extern GPSData workingGPSData;
extern GPSData currentGPSData;

void dumpWorkingGPSData();
void processGPSCharacter(char ch);

#endif

