#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "IO.h"
#include "Timer.h"

Button_Info_t btn_info   = {.pending=0, .state=0};
FIFO_Timer_t fifo_timer0 = {.count=0, .current=0, .is_cfg=0};

// Initializes Port B (DAC)
void PortB_Init(void) {
	SYSCTL_RCGC2_R 		|=	(1 << 1);	 	    	// 1) B clock
	GPIO_PORTB_LOCK_R 	=	0x4C4F434B; 		  	// 2) unlock PortB
	GPIO_PORTB_CR_R 	=	0x07;       		    // 	  allow changes to PB2-0       
	GPIO_PORTB_AMSEL_R 	=	0x00;       		 	// 3) disable analog function
	GPIO_PORTB_PCTL_R	=	0x00000000; 		  	// 4) GPIO clear bit PCTL  
	GPIO_PORTB_DIR_R 	=	0x07;       		   	// 5) PB2, PB1, PB0 as outputs
	GPIO_PORTB_AFSEL_R 	=	0x00;       		 	// 6) no alternate function
	GPIO_PORTB_DEN_R 	=	0x07;					// 7) enable digital pins PB2-0
}

// Initializes Port F (SW)
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
	PortF_Interrupt_Init();
}

// Enable Port F Interrupt (falling edge)
void PortF_Interrupt_Init(void) {
	
	// Configure External Interrupt for SW1 (PF4)
	GPIO_PORTF_IS_R		&=	~(1 << 4);				// PF4 0-edge sense
	GPIO_PORTF_IBE_R	&=	~(1 << 4);				// Trigger controlled by IEV
	GPIO_PORTF_IEV_R	&=	~(1 << 4);				// Falling edge trigger
	GPIO_PORTF_ICR_R	|=	(1 << 4);				// Clear interrupt
	GPIO_PORTF_IM_R		|=	(1 << 4);				// Unmask interrupt
	
	// Configure External Interrupt for SW2 (PF0)
	GPIO_PORTF_IS_R		&=	~(1 << 0);				// PF0 0-edge sense
	GPIO_PORTF_IBE_R	&=	~(1 << 0);				// Trigger controlled by IEV
	GPIO_PORTF_IEV_R	&=	~(1 << 0);				// Falling edge trigger
	GPIO_PORTF_ICR_R	|=	(1 << 0);				// Clear interrupt
	GPIO_PORTF_IM_R		|=	(1 << 0);				// Unmask interrupt
	
	// Configure PortF Interrupt
	NVIC_PRI7_R			|=	5 << 21; 				// Set interrupt priority to 5. PRI7 (INT 28-31), INT 30 = Bits 23:21
	NVIC_EN0_R			|=	(1 << 30);				// Enable interrupt 30 (GPIOF)
}


// External GPIO Interrupt Handler
void GPIOPortF_Handler(void) {
	uint8_t count_old = fifo_timer0.count;	// Variable used to start Timer0 for the first fifo element
	
	// Check interrupt on SW1
	if (GPIO_PORTF_MIS_R & (1 << 4)) {				
		GPIO_PORTF_ICR_R |= (1 << 4);	// Clear interrupt
		addFIFO(BIT_FLAG_SW1);				// Add button 0 event to FIFO (if not pending)
	}
	
	// Check interrupt on SW2
	if (GPIO_PORTF_MIS_R & (1 << 0)) {				
		GPIO_PORTF_ICR_R |= (1 << 0); 	// Clear interrupt
		addFIFO(BIT_FLAG_SW2);				// Add button 1 event to FIFO (if not pending)
	}
	
	// Start Timer0 for the first fifo element
	if (count_old == 0) {
		uint32_t test = fifo_timer0.residual[fifo_timer0.current];
		Timer0_Init(test, 1, &GPIOPortF_callback);
		fifo_timer0.is_cfg = 1;
	}
}

void GPIOPortF_callback(void) {
	TIMER0_CTL_R = 0x00000000; 	// disable countdowns
	TIMER0_IMR_R = 0x00000000; 	// disable timer interrupt to make it one-shot
	TIMER0_ICR_R = 0x00000001;	// clear interrupt flag
	//TIMER0_ICR_R = TIMER_ICR_TATOCINT;
	
	// Check which button enabled the timer, to sample it and avoid switch bouncing...
	switch (fifo_timer0.button[fifo_timer0.current]) {
		case BIT_FLAG_SW1: // bit 0
			btn_info.state   |= (~GPIO_PORTF_DATA_R & (1 << 4)) >> 4; // bit 0
			btn_info.pending &= ~(1 << 0);
			break;
		case BIT_FLAG_SW2: // bit 1
			btn_info.state   |= (~GPIO_PORTF_DATA_R & (1 << 0)) << 1; // bit 1
			btn_info.pending &= ~(1 << 1);
			break;
	}
	
	// Remove current entry from FIFO
	removeFIFO();
	
	// If there are elements in FIFO, then reset the timer with their residual counts
	if (fifo_timer0.count) {
		Timer0_Init(fifo_timer0.residual[fifo_timer0.current], 1, &GPIOPortF_callback);
	}
}

// Populates new fifo entry: residual, button, and then increments fifo counter
void addFIFO(uint8_t button){
	uint8_t pos_next = (fifo_timer0.current + fifo_timer0.count) % FIFO_TIMER_SIZE;
	if (!(btn_info.pending & button)) {
		btn_info.pending |= button;
		fifo_timer0.button[pos_next] = button;
		if (fifo_timer0.count == 0) {
			fifo_timer0.residual[pos_next] = 1600000;
		} else {
			// Verify if timer0 module has been configured
			// to avoid bug with two simulataneous interrupts (first time)
			if (fifo_timer0.is_cfg) {
				fifo_timer0.residual[pos_next] = 1600000 - TIMER0_TAR_R + 50;
			} else {
				fifo_timer0.residual[pos_next] = 50; // offset workaround to avoid timer interrupt glitches
			}
		}
		fifo_timer0.count++;
	}
}

// Decrement fifo counter and change current index
void removeFIFO(void){
	fifo_timer0.count--;
	fifo_timer0.current = (fifo_timer0.current + 1) % FIFO_TIMER_SIZE;
}

uint8_t readButtons(void) {
	uint8_t state = btn_info.state;
	btn_info.state = 0;
	return state;
}

// Initializes Port D
void PortD_Init(void){ 
  SYSCTL_RCGC2_R |= 0x00000008; // activate clock for port D
  (void)SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTD_LOCK_R 	=	0x4C4F434B; 		  	// 2) unlock PortF PF0  
  GPIO_PORTD_CR_R 	|=	0x20;       		    // allow changes to PF4-0       
  GPIO_PORTD_AMSEL_R &= ~0x20;      // disable analog functionality on PD5
  GPIO_PORTD_PCTL_R &= ~0x00F00000; // configure PD5 as GPIO
  GPIO_PORTD_DIR_R |= 0x20;     // make PD5 out
  GPIO_PORTD_DR8R_R |= 0x20;    // enable 8 mA drive on PD5
  GPIO_PORTD_AFSEL_R &= ~0x20;  // disable alt funct on PD5
  GPIO_PORTD_DEN_R |= 0x20;     // enable digital I/O on PD5
  GPIO_PORTD_DATA_R &= ~0x20;   // make PD5 low
}
