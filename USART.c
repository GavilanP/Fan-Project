/*
 * USART.c
 *
 * Created: 24/04/2024 21:42:52
 *  Author: Pere Tio, David Merino, Pablo Gavilan
 */ #include "USART.h"

void USART3_init ( void )
{
	PORTB.DIRSET = PIN0_bm ; // Set TX pin to output
	PORTB.DIRCLR = PIN1_bm ; // Set RX pin to input
	USART3.BAUD = (uint16_t) USART3_BAUD_RATE(9600) ; // Set Baud rate 9600
	USART3.CTRLB |= USART_TXEN_bm | USART_RXEN_bm ;
	USART3.CTRLA = USART_RXCIE_bm; //Enable interrupt in the USART
}

// Function to send char
void USART3_sendChar ( char c)
{
	while (!( USART3.STATUS & USART_DREIF_bm )) // while data register not empty
	{
		;
	}
	USART3 . TXDATAL = c;
}

// Function to send string
void USART3_sendString ( char * str )
{
	for ( size_t i = 0; i < strlen ( str ); i ++)
	{
		USART3_sendChar ( str [i]);
	}
}

void USART3_printMenu(void){
	USART3_sendString("\r\n ----------------------");
	USART3_sendString("\r\nMain Menu\r\n");
	USART3_sendString("1. MAX mode\r\n");
	USART3_sendString("2. Control 1 fan\r\n");
	USART3_sendString("3. Control 2 fans\r\n");
	USART3_sendString("4. Read RPM\r\n");
	USART3_sendString("Enter command: ");
	USART3_sendString("\r\n ----------------------\r\n");
}

