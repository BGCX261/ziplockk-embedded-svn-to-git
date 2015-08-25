#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <inttypes.h>

uint8_t transmitBuffer(uint8_t (*txFunction)(char ch),uint8_t *buffer,uint8_t bufferLen);
uint8_t transmitNullTerminatedBuffer(uint8_t (*txFunction)(char ch),uint8_t *buffer);
void hexDump(uint8_t (*txFunction)(char ch),uint8_t data);
void decDump(uint8_t (*txFunction)(char ch),uint8_t data);
void hexDumpBuffer(uint8_t (*txFunction)(char ch),uint8_t *buffer,uint8_t bufferLength);
void decDumpBuffer(uint8_t (*txFunction)(char ch),uint8_t *buffer,uint8_t bufferLength);
void doubleDumpBuffer(uint8_t (*txFunction)(char ch),double *buffer,uint8_t bufferLength);

#endif

