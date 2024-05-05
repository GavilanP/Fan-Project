/*
 * PWM.c
 *
 * Created: 24/04/2024 21:48:57
 *  Author: david
 */ 

#include "PWM.h"

//HERE WE DEFINE THE TWO TIMERS
//TIMER A IS FOR GENERATING THE PWM WITH 2 INDEPENDENT DUTY CYCLES
//TIMER B IS FOR COUNTING THE FREQUENCY WITH THE EVENTS GENERATED

void TCA0_init(void) //This timer is used to generate the PWM signal to control both fans independently
{						// The duty cycle is set in the compare buffer (TCA0.SINGLE.CMP0), with a maximum value of TCA0.SINGLE.PER
	// Set PWM signal output on PORT D
	PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTD_gc;
	
	// Set to dual-slope WG mode. We also compare the PWM through channel 0 and 1.
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_DSBOTTOM_gc | TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm;
	
	// Set timer to count clock ticks
	TCA0.SINGLE.EVCTRL &= ~(TCA0_SINGLE_CNT);
	
	// Set PWM frequency with period value
	TCA0.SINGLE.PER = 0xFF;
	
	// Set clock source to half of system clock(sys_clk /2) and start the timer
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm;
	
	// Set pin 0 and 1 of PORT D as output. These ports will be used to output PWM signals
	PORTD.DIR |= PIN0_bm | PIN1_bm;
	
	TCA0.SINGLE.CMP0 = 0x5F;
	
	TCA0.SINGLE.CMP1 = 0x5F;
}




