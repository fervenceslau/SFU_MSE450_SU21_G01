// TExaS Lab Graders Authors: Daniel Valvano, Jonathan Valvano and Ramesh Yerraballi

#include "TExaS.h"
#include "UART.h"
#include <stdio.h>
#include <string.h>

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC)) // GPIO Port F Data Register
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400)) // GPIO Port F Direction Register
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420)) // GPIO Port F Alternate Function Select
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510)) // GPIO Port F Pull-up Register
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C)) // GPIO Port F Digital Enable Register
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520)) // GPIO Port F Lock Register
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524)) // GPIO Port F Commit Register
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528)) // GPIO Port F Analog Mode Select 
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C)) // GPIO Port F Port Control 
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108)) // Run Mode Clock Gating Control Register 2
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010)) // SysTick Control and Status Register
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014)) // SysTick Reload Value Register
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018)) // SysTick Current Value Register
#define NVIC_SYS_PRI3_R         (*((volatile unsigned long *)0xE000ED20)) // System Handler Priority 3 Register
#define NVIC_ST_CTRL_COUNT      0x00010000     // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004     // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002     // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001     // Counter mode
#define NVIC_ST_RELOAD_M        8000000        // Counter load value (0.5 seconds)
#define SYSCTL_RCGC2_GPIOF      0x00000020     // Port F Clock Gating Control
#define QUARTER_SEC_DELAY       4000000        // Delay for task 4 (0.25 seconds)
#define DEFAULT_PASSWORD        "password123!" // Default password
#define ENABLE_TASK_SIX         1              // Flag to enable task 6 interrupt handler functionality

//  Global Variables
unsigned long SW1,SW2;

//   Function Prototypes
void PortF_Init(void);
void Control_LED(void); // functions you need to write
void Delay(unsigned long delay); // functions you need to write
void EnableInterrupts(void); // functions you need to write
void UART_password(void); // UART password for task 5
void SysTick_Init(void);
void TogglePF2(void);

// 3. Subroutines Section
int main(void){    
  TExaS_Init(SW_PIN_PF40,LED_PIN_PF321);   //  TExaS_Init initializes the real board grader for lab 4
  EnableInterrupts();
  PortF_Init();                            // Call initialization of port PF   
  SysTick_Init();
  UART_password();
  while(1){
    Control_LED();

/* The following code was removed for task 6 */
#if ENABLE_TASK_SIX == 0
    Delay(QUARTER_SEC_DELAY);
    TogglePF2();
#endif /* ENABLE_TASK_SIX */

  }
}

//
// Description: For task 3, control PF3 using switch attached to PF4
//
void Control_LED(void){
  SW1 = GPIO_PORTF_DATA_R&0x10;  // Get PF4

  if (!SW1)
  {
    GPIO_PORTF_DATA_R |= 0x08;  // Set PF3
  } 
  else
  {
    GPIO_PORTF_DATA_R &= ~0x08;  // Clear PF3
  }
}

//
// Description: For task 4, use SysTick timer to wait for a period of time
//
void Delay(unsigned long delay){
  volatile unsigned long timeElapsed;
  unsigned long timeStart = NVIC_ST_CURRENT_R;                  // Read current register start value
  do {
    timeElapsed = (timeStart - NVIC_ST_CURRENT_R) & 0x00FFFFFF; // Read current register and compare to start
  }
  while(timeElapsed <= delay);
}

//
// Description: For task 5, use UART to verify password
//
void UART_password(void) {
  char string[20];
  UART_OutChar(CR); // New line
  UART_OutChar(LF);
  while(1){
    UART_OutString("Please enter password (19 character limit): ");
    UART_InString(string,19);
    UART_OutChar(CR);UART_OutChar(LF);
    if (!strcmp(string, DEFAULT_PASSWORD))
    {
      UART_OutString("Correct!");
      UART_OutChar(CR);UART_OutChar(LF);
      break;
    }
  }
}

//
// Description: For task 5, use SysTick ISR to toggle PF2
//
void SysTick_Handler(void)
{
#if ENABLE_TASK_SIX == 1
    TogglePF2();
#endif /* ENABLE_TASK_SIX */
}


// 
// Description: Toggle PF2 for tasks 4 and 6
//
void TogglePF2(void)
{
  GPIO_PORTF_DATA_R ^= 0x04;  // Toggle PF2
}

//
// Description:
// Systick initialization for tasks 4 and 6.
// The SysTick timer is a simple way to create periodic interrupts.
//
// Asssumption: We're using a 16 MHz bus clock
//
void SysTick_Init(void){
  NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;  // During startup, disable SysTick
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;     // Maximum reload value is set to 0.5 seconds
  NVIC_ST_CURRENT_R = 0;                   // Any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000; // Set interrupt priority level for systick interrupt       
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE + NVIC_ST_CTRL_CLK_SRC + NVIC_ST_CTRL_INTEN; // Entable systick, set clock source and enable interrupts
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


