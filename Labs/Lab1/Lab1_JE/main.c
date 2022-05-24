// TExaS Lab Graders Authors: Daniel Valvano, Jonathan Valvano and Ramesh Yerraballi

#include "TExaS.h"
#include "UART.h"
#include <stdio.h>
#include <string.h>

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))


//	Global Variables
unsigned long SW1,SW2;  

//   Function Prototypes
void PortF_Init(void);
void Control_LED(void); // functions you need to write
void Delay(void); // functions you need to write
void EnableInterrupts(void); // functions you need to write <-incorrect
void UART_Password_Check(void);
void UART_example(void); // An example to help you with Task 5 :)

// 3. Subroutines Section
int main(void){    
  TExaS_Init(SW_PIN_PF40,LED_PIN_PF321); 	//	TExaS_Init initializes the real board grader for lab 4
  PortF_Init();        										// Call initialization of port PF
	//EnableInterrupts();
	UART_Password_Check();
  while(1){
    Control_LED();
		Delay();
		GPIO_PORTF_DATA_R ^= 0x04; //toggle PF2 
	}
}

// Task 3
void Control_LED(void){
	SW1 = GPIO_PORTF_DATA_R&0x10;	//Get PF4
	if(SW1)
	{
		GPIO_PORTF_DATA_R |= 0x08; //set PF3
	}
	else
	{
		GPIO_PORTF_DATA_R &= 0xF7; //Reset PF3 
	}
}

// Task 4-6 functions go here
void Delay(void)
{
	unsigned long int big_number = 1e6; //make a big number
	while(big_number)
	{
		big_number--;
	}
}

void UART_Password_Check(void)
{
	char string[20];
	char password[20] = {"password"};
  UART_OutChar(CR); // New line
  UART_OutChar(LF);
  while(strcmp(string, password)){
    UART_OutString("Enter Password: ");
    UART_InString(string,19);
		UART_OutChar(CR);UART_OutChar(LF);
		UART_OutString("Have a good semester!");
		UART_OutChar(CR);UART_OutChar(LF);
	}
}

// Example to help you with Task 5
void UART_example(void) {
	char string[20];
  UART_OutChar(CR); // New line
  UART_OutChar(LF);
  while(1){
    UART_OutString("Welcome to MSE450/750! Enter student ID: ");
    UART_InString(string,19);
		UART_OutChar(CR);UART_OutChar(LF);
		UART_OutString("Have a good semester!");
		UART_OutChar(CR);UART_OutChar(LF);
  }
}

// Initializes Port F
void PortF_Init(void){ 
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}


