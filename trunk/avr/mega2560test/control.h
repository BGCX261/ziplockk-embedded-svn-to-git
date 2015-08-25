#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <inttypes.h>

enum ControlState
{
	ControlMenu,
	ControlIdle,
	ControlLoggerPassthrough,
	ControlDataStream,
	ControlLogging,
	ControlRecoverFileListA,
	ControlRecoverFileListB
};

extern enum ControlState controlState;
extern uint8_t idleTicks;
extern uint8_t idleSeconds;
extern uint8_t autoStop;

void control();

#endif

