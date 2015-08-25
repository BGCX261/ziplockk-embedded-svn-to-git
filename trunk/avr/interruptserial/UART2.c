// AVR306: Using the AVR UART in C
// Routines for interrupt controlled UART
// Last modified: 02-06-21
// Modified by: AR

/* Includes */
#include <avr/io8515.h>
#include <avr/ina90.h>

/* UART Buffer Defines */
#define UART_RX_BUFFER_SIZE 128     /* 2,4,8,16,32,64,128 or 256 bytes */
#define UART_TX_BUFFER_SIZE 128


#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1 )
#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
	#error RX buffer size is not a power of 2
#endif

#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1 )
#if ( UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK )
	#error TX buffer size is not a power of 2
#endif


/* Static Variables */
static unsigned char UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile unsigned char UART_RxHead;
static volatile unsigned char UART_RxTail;
static unsigned char UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile unsigned char UART_TxHead;
static volatile unsigned char UART_TxTail;

/* Prototypes */
void InitUART( unsigned char baudrate );
unsigned char ReceiveByte( void );
void TransmitByte( unsigned char data );

/* Main - a simple test program*/
void main( void )
{
	InitUART( 11 );   /* Set the baudrate to 19,200 bps using a 3.6864MHz crystal */

	_SEI();           /* Enable interrupts => enable UART interrupts */

	for( ; ; )        /* Forever */
	{
		TransmitByte( ReceiveByte() ); /* Echo the received character */
	}
}

/* Initialize UART */
void InitUART( unsigned char baudrate )
{
	unsigned char x;

	UBRR = baudrate; 	/* Set the baud rate */
	/* Enable UART receiver and transmitter, and receive interrupt */
	UCR = ( (1<<RXCIE) | (1<<RXEN) | (1<<TXEN) );

	x = 0; 			    /* Flush receive buffer */

	UART_RxTail = x;
	UART_RxHead = x;
	UART_TxTail = x;
	UART_TxHead = x;
}

/* Interrupt handlers */
#pragma vector=UART_RX_vect
__interrupt void UART_RX_interrupt( void )
{
	unsigned char data;
	unsigned char tmphead;

	data = UDR;                 /* Read the received data */
	/* Calculate buffer index */
	tmphead = ( UART_RxHead + 1 ) & UART_RX_BUFFER_MASK;
	UART_RxHead = tmphead;      /* Store new index */

	if ( tmphead == UART_RxTail )
	{
		/* ERROR! Receive buffer overflow */
	}
	
	UART_RxBuf[tmphead] = data; /* Store received data in buffer */
}

#pragma vector=UART_UDRE_vect
__interrupt void UART_TX_interrupt( void )
{
	unsigned char tmptail;

	/* Check if all data is transmitted */
	if ( UART_TxHead != UART_TxTail )
	{
		/* Calculate buffer index */
		tmptail = ( UART_TxTail + 1 ) & UART_TX_BUFFER_MASK;
		UART_TxTail = tmptail;      /* Store new index */
	
		UDR = UART_TxBuf[tmptail];  /* Start transmition */
	}
	else
	{
		UCR &= ~(1<<UDRIE);         /* Disable UDRE interrupt */
	}
}

/* Read and write functions */
unsigned char ReceiveByte( void )
{
	unsigned char tmptail;
	
	while ( UART_RxHead == UART_RxTail )  /* Wait for incomming data */
		;
	tmptail = ( UART_RxTail + 1 ) & UART_RX_BUFFER_MASK;/* Calculate buffer index */
	
	UART_RxTail = tmptail;                /* Store new index */
	
	return UART_RxBuf[tmptail];           /* Return data */
}

void TransmitByte( unsigned char data )
{
	unsigned char tmphead;
	/* Calculate buffer index */
	tmphead = ( UART_TxHead + 1 ) & UART_TX_BUFFER_MASK; /* Wait for free space in buffer */
	while ( tmphead == UART_TxTail );

	UART_TxBuf[tmphead] = data;           /* Store data in buffer */
	UART_TxHead = tmphead;                /* Store new index */

	UCR |= (1<<UDRIE);                    /* Enable UDRE interrupt */
}

unsigned char DataInReceiveBuffer( void )
{
	return ( UART_RxHead != UART_RxTail ); /* Return 0 (FALSE) if the receive buffer is empty */
}
