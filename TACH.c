/*
 * TACH.c
 *
 * Created: 24/04/2024 22:28:50
 *  Author: david
 */ 

#include "TACH.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void TCB0_init(void) //This timer is used to calculate the frequency of the fans
{					//The CNT (counter value) is connected to the clock and each time an event is
	//detected (when the tach signal have a rising edge) the CNT is copied to the CCMP and restarted
	// Configure PORTC as an input. Port C is the event detector
	PORTC.DIRCLR = 0x00;
	
	// Pull up resistor in the Port C. This is bc the TACH signal has transistor pulled down
	PORTC.PINCONFIG = PORT_PULLUPEN_bm;
	
	// the compare value of the TCB0 is set to 0x00
	TCB0.CCMP = 0x00;
	
	// the count value of TCB0 is set to 0x00
	TCB0.CNT = 0x00;
	
	// select clock divider 1 (so no clk divider) as the clock source for the CNT counter
	// enable TCB0
	TCB0.CTRLA |= TCB_ENABLE_bm | TCB_CLKSEL_DIV2_gc;
	
	// The TCB0 compare/capture is enabled
	//	Is it set to frequency generation mode
	TCB0.CTRLB |= TCB_CCMPEN_bm | TCB_CNTMODE_FRQ_gc;
	
	// Enable input capture event for TCB0
	TCB0.EVCTRL |= TCB_CAPTEI_bm;
	
	// Assign channel 2 to the PORTC PIN0
	EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN0_gc;

	// Assign the usertcb0 to the channel 2 (so we connect the tcb0 to the events generated in the portc)
	EVSYS.USERTCB0CAPT = EVSYS_USER_CHANNEL2_gc;
}