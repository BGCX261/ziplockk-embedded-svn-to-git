#include <inttypes.h>
#include <avr/interrupt.h>
#include "dio.h"
#include "serial0.h"

// interrupts for PCINT7:0
SIGNAL(PCINT0_vect)
{
}

// interrupts for pins PCINT15:8
SIGNAL(PCINT1_vect)
{
}

// interrupts for pins PCINT23:16
SIGNAL(PCINT2_vect)
{
}


void digitalIOInit()
{
}

void digitalIOPoll()
{
}

