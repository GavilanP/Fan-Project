/* FINAL PROJECT MICROCONTROLER SYSTEMS - IELET2111
 * main.c
 *
 * Created: 24/04/2024 22:28:50
 *  Author: Pere Tio, David Merino, Pablo Gavilan
 */ 


#define F_CPU 4000000UL
//We set the baud rate to communicate with the interface. We have used Termite
//Note both Baud rates (in Termite and here) should coincide
#define USART3_BAUD_RATE(BAUD_RATE) ((float)(F_CPU*64 / (16*(float)BAUD_RATE )) + 0.5)
//We define the length for the command buffer. The buffer is where the USART3 DATA inputs are stored
#define MAX_COMMAND_LEN 9
#define CR 0x0D // ’\r’
//\n is used in text files and streams to move the cursor to the next line
#define LF 0x0A // ’\n’

#include <avr/io.h> //We include the different libraries
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>


volatile uint8_t input_index = 0;
//The menu option variable selects between the different modes of operation.
//It's 
volatile uint8_t menu_option = 0;

char buffer[256];
char param0;
char param1;
char command [ MAX_COMMAND_LEN ];
char rpm_vect[5];

uint8_t index = 0;
char c;
uint16_t f;
uint16_t RPM;
uint16_t v_expected;
uint16_t v_max;
uint16_t v_min;



// Wrapping USART3_sendChar in a function computable with FDEV_SETUP_STREAM
static int USART3_printChar ( char c, FILE * stream )
{
	USART3_sendChar (c);
	return 0;
}



// Function to start the fans
void start_fans() {
	USART3_sendString("\r\nFans started. Enter values between 0 and 255 to control the speed.\r\n");
	USART3_sendString("Type 'e' to return to the main menu.\r\n");
}

// Function to handle option 2
void handle_option_2() {
	USART3_sendString("\r\nWaiting for two numbers between 0 and 255 separated by one space\r\n to control the speed of both fans\r\n");
}

// Interrupt Service Routine for USART3 reception

ISR(USART3_RXC_vect) {
    char c = USART3.RXDATAL;
    
    // Ignore LF and CR characters
    if (c != LF && c != CR) {
        // Store character in command buffer
        command[input_index++] = c;
        
        // Check if command buffer is full
        if (input_index > MAX_COMMAND_LEN) {
            USART3_sendString("\r\n");
            input_index = 0;
        }
    }
    
    // Check if CR character received
    if (c == CR) {
        // Null-terminate the command string
        command[input_index] = '\0';
        input_index = 0;
        
        // Process the command
		if (menu_option == 1) {
            // Handling option 1: MAX speed fans
            if (strcmp(command, "e") == 0) {
                // Return to main menu
                USART3_sendString("\r\nReturning to main menu...\r\n");
                menu_option = 0; // Reset menu option
                USART3_printMenu();
            } else {
				// Set fan speed
				USART3_sendString("\r\nFan speed set to MAX");
				USART3_sendString("\r\n");
				TCA0.SINGLE.CMP0BUF = 255;
                }
            }
        		
		
        else if (menu_option == 2) {
            // Handling option 2: Start fans
            if (strcmp(command, "e") == 0) {
                // Return to main menu
                USART3_sendString("\r\nReturning to main menu...\r\n");
                menu_option = 0; // Reset menu option
                USART3_printMenu();
            } else {
                int value = atoi(command);
                if (value >= 0 && value <= 255) {
                    // Set fan speed
                    USART3_sendString("\r\nFan speed set to ");
                    USART3_sendString(command);
                    USART3_sendString("\r\n");
                    TCA0.SINGLE.CMP0BUF = atoi(command);
                } else {
                    // Invalid input
                    USART3_sendString("\r\nInvalid input! Please enter a value between 0 and 255.\r\n");
                    USART3_sendString("\r\nType e to go back\r\n");
                }
            }
        } else if (menu_option == 3) {
            // Handling option 3: Check for two numbers separated by space or 'e' to go back
            if (strcmp(command, "e") == 0) {
                // Return to main menu
                USART3_sendString("\r\nReturning to main menu...\r\n");
                menu_option = 0; // Reset menu option
                USART3_printMenu();
            } else {
                // Split the command into two arguments
                char* param0 = strtok(command, " ");
                char* param1 = strtok(NULL, " ");
                
                // Check if both arguments are valid numbers
                if (param0 != NULL && param1 != NULL) {
                    int value0 = atoi(param0);
                    int value1 = atoi(param1);
                    if ((value0 >= 0 && value0 <= 255) && (value1 >= 0 && value1 <= 255)) {
                        // Configure TCA0.SINGLE.CMP0 and TCA0.SINGLE.CMP1
                        TCA0.SINGLE.CMP0 = value0;
                        TCA0.SINGLE.CMP1 = value1;
                        USART3_sendString("\r\nFan 1 set to ");
                        USART3_sendString(param0);
                        USART3_sendString(" and Fan 2 set to ");
                        USART3_sendString(param1);
                        USART3_sendString("\r\n");
                        // Reset command buffer after setting speeds
                        //memset(command, 0, sizeof(command));
                        // Prompt for more input
                        USART3_sendString("Enter two numbers between 0 and 255 separated by a space or 'e' to exit: ");
                    } else {
                        // Invalid input
                        USART3_sendString("\r\nInvalid input! Please enter two numbers between 0 and 255 separated by a space.\r\n");
                        USART3_sendString("\r\nType e to go back\r\n");
                    }
                } else {
                    // Invalid input
                    USART3_sendString("\r\nInvalid input! Please enter two numbers between 0 and 255 separated by a space.\r\n");
                    USART3_sendString("\r\nType e to go back\r\n");
                }
            }
        } else if (menu_option == 4) {
            // Handling option 4: Read RPM
            if (strcmp(command, "0") == 0 || strcmp(command, "1") == 0) {
                // Set the appropriate EVSYS channel based on the input
                if (strcmp(command, "0") == 0) {
                    EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN0_gc;
					v_expected = TCA0.SINGLE.CMP0;

                } else if (strcmp(command, "1") == 0) {
                    EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN1_gc;
					v_expected = TCA0.SINGLE.CMP1;
					}
				_delay_ms(50); //Delay to let the timer update the value of the new pin - channel connection
				rpm_vect[5] = 0;
                // Calculate RPM
				f = TCB0.CCMP;
				RPM = (4000000*15)/f;	//4MHz is the clock freq. We have to go from sec to min and we divide by two the frequency
										//A turn of the fan is equal to two periods (we have to divide by two again)
				itoa(RPM, rpm_vect, 10);
				// Send RPM value
				USART3_sendString("\r\nRPM: ");
				USART3_sendString(rpm_vect);
				USART3_sendString("\r\n\r\n");
				//v_min = v_expected*54*0.9;
				//v_max = v_expected*54*1.1;
				if ((RPM < v_expected*54*0.9) || (RPM > v_expected*54*1.1)){
					PORTE.OUT = 0xFF;
				}
				else{
					PORTE.OUT = 0x00;
				}

                // Prompt for more input
                USART3_sendString("Enter 0 or 1 to read RPM from Fan 0 or Fan 1 respectively, or 'e' to exit: ");
            } else if (strcmp(command, "e") == 0) {
                // Return to main menu
                USART3_sendString("\r\nReturning to main menu...\r\n");
                menu_option = 0; // Reset menu option
                USART3_printMenu();
            } else {
                // Invalid input
                USART3_sendString("\r\nInvalid input! Please enter '1', '2', '3', '4'.\r\n");
                USART3_sendString("Enter 0 or 1 to read RPM from Fan 0 or Fan 1 respectively, or 'e' to exit: ");
            }
        } else {
            // Main menu
			if (strcmp(command, "1") == 0) {
                // Option 1 selected
				USART3_sendString("Type anything but e to start MAX speed");
                menu_option = 1;
            } else if (strcmp(command, "2") == 0) {
                // Option 1 selected
                start_fans();
                menu_option = 2;
            } else if (strcmp(command, "3") == 0) {
                // Option 2 selected
                handle_option_2();
                menu_option = 3;
                // Prompt for input for Option 2
                USART3_sendString("\r\nEnter two numbers between 0 and 255 separated by a space or 'e' to exit: ");
            } else if (strcmp(command, "4") == 0) {
                // Option 3 selected
                USART3_sendString("\r\nEnter 0 or 1 to read RPM from Fan 0 or Fan 1 respectively, or 'e' to exit: ");
                menu_option = 4;
            } else {
                // Invalid input
                USART3_sendString("\r\nInvalid input! Please enter '1', '2', '3', '4'.\r\n");
                USART3_sendString("Enter command: ");
            }
        }
    }
}



int main ( void )
{
	PORTE.DIR = 0xff; //set all port E as output
	PORTD.DIR = 0xff; //set portD pin 0 (PWM) as output and the others as input
	PORTB.DIR = 0xff;
	PORTB.OUT = 0xff;
	
	USART3_init(); 
	TCA0_init ();
	TCB0_init();
	sei();
	_delay_ms (10) ;
	USART3_printMenu();

	while (1)
	{
	
	}
}
