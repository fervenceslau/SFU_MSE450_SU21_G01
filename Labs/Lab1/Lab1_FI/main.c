// TExaS Lab Graders Authors: Daniel Valvano, Jonathan Valvano and Ramesh Yerraballi

#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include <stdio.h>

// Preprocessing directives
#define	NUM_CHAR_MESSAGE	32
#define STR_PASSWORD		"123456"

// Global Variables
unsigned long 	SW1;
char 			message[NUM_CHAR_MESSAGE];
int 			is_logged_in = 0;

// Function Prototypes
void System_Init(void);
void PortF_Init(void);
void PortF_Interrupt_Init(void);
void UART1_Init(void);
void UART1_Transmitter(char data);
void Control_LED(void);
void Delay(void);
int  CheckPassword(char *inStr, char *pwdStr);

// Subroutines Section
int main(void){    
	TExaS_Init(SW_PIN_PF40, LED_PIN_PF321); // TExaS_Init initializes the real board grader for lab 4
	System_Init();
	PortF_Init();        					// Call initialization of port PF   
	PortF_Interrupt_Init();					// Enable port F interrupts
	UART_Init();
	
	while(1){
		// Task 5 - UART password
		if (!is_logged_in) {
			UART_OutString("Type in the password to unlock the program: ");
			UART_InString(message, NUM_CHAR_MESSAGE - 1);
			UART_OutChar(CR);
			UART_OutChar(LF);
			is_logged_in = CheckPassword(message, STR_PASSWORD);
			if (!is_logged_in) {
				UART_OutString("Wrong password, try again\n");
			} else {
				UART_OutString("Logged in!\n");
			}
		} else {
			Control_LED();
		}
	}
}

// Task 3
void Control_LED(void){
	SW1 = GPIO_PORTF_DATA_R & (1 << 4);	// Get PF4
	
	// Your code goes here
	
	// Task 3 - LED at PF3 controlled by SW1
	if (!SW1) { 									// SW1 pressed
		GPIO_PORTF_DATA_R |= (1 << 3); 				// Turn on Red LED (PF3)
	} else {
		GPIO_PORTF_DATA_R &= ~(1 << 3); 			// Turn off Red LED (PF3)
	}
	
	// Task 4 - Blinking LED at PF2
	Delay();
	GPIO_PORTF_DATA_R ^= (1 << 2);					// Toggle Blue LED (PF2)
}

// Task 4 - Delay function
void Delay(void) {
	volatile int count;								// Volatile is necessary or else the compiler will remove the for loop
	for (count = 0; count < 615384; count++) {} 	// Delay = 0.1s (count = (80 mhz / 13) * delay = 615384)
}

// Task 6 - External GPIO Interrupt Handler
void GPIOPortF_Handler(void) {
	if (GPIO_PORTF_MIS_R & (1 << 4)) {				// Check interrupt on SW1
		GPIO_PORTF_ICR_R |= (1 << 4); 				// Clear interrupt
		GPIO_PORTF_DATA_R ^= (1 << 1);				// Toggle Green LED (PF1)
	}
}

// Initializes System Clock to 80 Mhz from 400 Mhz Pll and 16 Mhz Xtal
void System_Init(void){
	SYSCTL_RCC_R		&=	~(1 << 0);				// Enable Main Oscilator
	SYSCTL_RCC_R		&=	~((1 << 5) | (1 << 4));	// Use Main Oscilator as Clock Source
	SYSCTL_RCC_R		|=	(0x15 << 6);			// Set Xtal Osc to 16 Mhz
	SYSCTL_RCC_R		&=	~(1 << 11);				// Select PLL / SYSDIV as System Clock
	SYSCTL_RCC_R		&=	~(1 << 13);				// PLL Power Down
	SYSCTL_RCC_R		|=	(1 << 22);				// Use SYSDIV
	SYSCTL_RCC_R		|=	(0x02 << 23);			// SYSDIV = /5
	SYSCTL_RCC_R		&=	~(1 << 27);				// Auto Clock Gating set to RCGCn
}

// Initializes Port F
void PortF_Init(void){ 
	SYSCTL_RCGC2_R 		|=	0x00000020; 	    	// 1) F clock
	GPIO_PORTF_LOCK_R 	=	0x4C4F434B; 		  	// 2) unlock PortF PF0  
	GPIO_PORTF_CR_R 	=	0x1F;       		    // allow changes to PF4-0       
	GPIO_PORTF_AMSEL_R 	=	0x00;       		 	// 3) disable analog function
	GPIO_PORTF_PCTL_R	=	0x00000000; 		  	// 4) GPIO clear bit PCTL  
	GPIO_PORTF_DIR_R 	=	0x0E;       		   	// 5) PF4,PF0 input, PF3,PF2,PF1 output   
	GPIO_PORTF_AFSEL_R 	=	0x00;       		 	// 6) no alternate function
	GPIO_PORTF_PUR_R 	=	0x11;       		   	// enable pullup resistors on PF4,PF0       
	GPIO_PORTF_DEN_R 	=	0x1F;					// 7) enable digital pins PF4-PF0        
}

// Enable Port F Interrupt (falling edge)
void PortF_Interrupt_Init(void) {
	GPIO_PORTF_IS_R		&=	~(1 << 4);				// PF4 0-edge sense
	GPIO_PORTF_IBE_R	&=	~(1 << 4);				// Trigger controlled by IEV
	GPIO_PORTF_IEV_R	&=	~(1 << 4);				// Falling edge trigger
	GPIO_PORTF_ICR_R	|=	(1 << 4);				// Clear interrupt
	GPIO_PORTF_IM_R		|=	(1 << 4);				// Unmask interrupt
	NVIC_PRI7_R			|=	5 << 21; 				// Set interrupt priority to 5. PRI7 (INT 28-31), INT 30 = Bits 23:21
	NVIC_EN0_R			|=	(1 << 30);				// Enable interrupt 30 (GPIOF)
}

// Check if two strings are equal
int CheckPassword(char *inStr, char *pwdStr) {
	int i=0;
	for (i=0; i<NUM_CHAR_MESSAGE && (*(inStr + i) != 0 || *(pwdStr + i) != 0); i++){
		if (*(inStr + i) != *(pwdStr + i)) {
			return 0;
		}
	}
	return 1;
}
