
/*

 * TEST-USART.c

 *

 * Created: 8/30/2017 12:14:36 PM

 * Author : madiv

 */ 


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "AT-SMS.h"


void USART0_Init( unsigned int ubrr)

{
	/* Set the baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable UART receiver and transmitter */
	UCSR0B = ((1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0));
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (3<<UCSZ00);
	//Double speed mode
	UCSR0A = (1<<U2X0);
}

void USART1_Init( unsigned int ubrr)
{
	/* Set the baud rate */
	UBRR1H = (unsigned char)(ubrr>>8);
	UBRR1L = (unsigned char)ubrr;
	/* Enable UART receiver and transmitter */
	UCSR1B = ((1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1));
	/* Set frame format: 8data, 2stop bit */
	UCSR1C = (3<<UCSZ10);
	//Double speed mode
	UCSR1A = (1<<U2X1);
}

int USART0_Transmit( char data0, FILE *stream)
{
	if (data0 == '\n') {
		USART0_Transmit('\r', stream);
	}
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );	;
	/* Put data into buffer, sends the data */
	UDR0 = data0;
}

int USART0_Receive( FILE *stream )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)));
	/* Get and return received data from buffer */
	return UDR0;
}

int USART1_Transmit( char data1, FILE *stream )
{
	if (data1 == '\n') {
		USART1_Transmit('\r', stream);
	}
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)) );	;
	/* Put data into buffer, sends the data */
	UDR1 = data1;
}

int USART1_Receive( FILE *stream )
{
	/* Wait for data to be received */
	while ( !(UCSR1A & (1<<RXC1)));
	/* Get and return received data from buffer */
	return UDR1;
}


