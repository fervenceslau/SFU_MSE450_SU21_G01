#ifndef PWM_H
#define PWM_H

#include <stdint.h>

typedef enum DutyCycle {
	NONE_DUTY_CYCLE,
	TWENTY_FIVE_PERCENT_DUTY_CYCLE,
	FIFTY_PERCENT_DUTY_CYCLE,
	SEVENTY_FIVE_PERCENT_DUTY_CYCLE
} DutyCycle_t;

//------------PWM_Init------------
// Description: Initializes a PWM signal on PB5
// Input: 
// - duty_cycle: Duty cycle for PWM. Percentage of time the signal remains high
// - freq: Frequency (cycles per second) of PWM signal. 
//
void PWM_Init(DutyCycle_t duty_cycle,
              uint32_t freq);

//------------PWM_Disable------------
// Description: Disable PWM output
void PWM_Disable(void);

#endif
				
