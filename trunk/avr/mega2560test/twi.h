#ifndef _TWI_H_
#define _TWI_H_

void twiInit();
void twiInitiate();
void twiStart();
void twiStop();
void twiWrite(uint8_t data);
uint8_t twiReadACK(void);
uint8_t twiReadNACK(void);
uint8_t twiGetStatus(void);

#endif

