/*
 * PWM.h
 *
 * Created: 24/04/2024 21:48:38
 *  Author: david
 */ 


#ifndef PWM_H_
#define PWM_H_

#define MAX_COMMAND_LEN 9
#define CR 0x0D // ’\r’
#define LF 0x0A // ’\n’

#include <stdbool.h>
#include <math.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>



void TCA0_init ( void );
void TCB0_init ( void );
void configure_PWM (void);



#endif /* PWM_H_ */