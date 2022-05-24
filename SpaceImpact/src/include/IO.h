/*! \file  IO.h
 *  \brief Contains definitions used to read buttons with debounce.
 */

#ifndef __IO__
#define __IO__

#include <stdint.h>

#define FIFO_TIMER_SIZE		4

#define BIT_FLAG_SW1		(1 << 0)
#define BIT_FLAG_SW2		(1 << 1)

/************************************************************
 *  FIFO Type (Button Debounce)
 ************************************************************/

typedef struct FIFO {
	uint32_t residual[FIFO_TIMER_SIZE];
	uint8_t  button[FIFO_TIMER_SIZE];
	uint8_t  count;
	uint8_t  current;
	uint8_t  is_cfg;
} FIFO_Timer_t;


/************************************************************
 *  Button Info Type
 ************************************************************/

typedef struct btn_info {
	uint8_t  pending;
	uint8_t  state;
} Button_Info_t;


/************************************************************
 *  Functions
 ************************************************************/

/** 
 *  Configures Port B pins (2-0) to use a simple external DAC.
 */
void PortB_Init(void);

/** 
 *  Configures Port F(4,0) pins to use switches.
 */
void PortF_Init(void);

/** 
 *  Configures Port F External Interrupts.
 */
void PortF_Interrupt_Init(void);

/** 
 *  Port F External Interrupt Handler Function.
 */
void GPIOPortF_Handler(void);

/** 
 *  Port F Timer Interrupt Callback Function.
 */
void GPIOPortF_callback(void);

/** 
 *  Adds element to FIFO.
 */
void addFIFO(uint8_t button);

/** 
 *  Removes element from FIFO.
 */
void removeFIFO(void);

/** 
 *  Reads button states.
 */
uint8_t readButtons(void);

/** 
 *  Configures Port D pins.
 */
void PortD_Init(void);


#endif
