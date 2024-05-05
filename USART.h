/*
 * USART.h
 *
 * Created: 24/04/2024 21:42:31
 *  Author: david
 */ 


#ifndef USART_H_
#define USART_H_

#define F_CPU 4000000UL
#define USART3_BAUD_RATE(BAUD_RATE) ((float)(F_CPU*64 / (16*(float)BAUD_RATE )) + 0.5)

#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>


void USART3_init(void);
void USART3_sendChar(char c);
void USART3_sendString(char * str );
void USART3_printMenu(void);
//Comentario que se tiene que eliminar: las ISR() nunca se incluyen en los .h, solo van el los .c y se pueden hacer todas las que quieras (cambiando el vector de interrupcion)

/*
AQUÍ ESTARÍA BIEN PONER TODAS LAS FUNCIONES DEL MENÚ PARA CONTROLAR LOS VENTILADORES
*/



#endif /* USART_H_ */