// AVR306: Using the AVR UART in C
// Routines for polled UART
// Last modified: 02-06-21
// Modified by: AR

/* Includes */
#include <io8515.h>

/* Prototypes */
void InitUART( unsigned char baudrate );
unsigned char ReceiveByte( void );
void TransmitByte( unsigned char data );

/* Main - a simple test program*/
void main( void )
{
	InitUART( 11 ); /* Set the baudrate to 19,200 bps using a 3.6864MHz crystal */

	for(;;) 	    /* Forever */
	{
		TransmitByte( ReceiveByte() ); /* Echo the received character */
	}
}

/* Initialize UART */
void InitUART( unsigned char baudrate )
{
	UBRR = baudrate;                  /* Set the baud rate */
	UCR = ( (1<<RXEN) | (1<<TXEN) );  /* Enable UART receiver and transmitter */
}

/* Read and write functions */
unsigned char ReceiveByte( void )
{
	while ( !(USR & (1<<RXC)) ) 	/* Wait for incomming data */
		;			                /* Return the data */
	return UDR;
}

void TransmitByte( unsigned char data )
{
	while ( !(USR & (1<<UDRE)) )
		; 			                /* Wait for empty transmit buffer */
	UDR = data; 			        /* Start transmittion */
}
