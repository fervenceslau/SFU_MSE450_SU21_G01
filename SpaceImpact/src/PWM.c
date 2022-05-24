#include "PWM.h"
#include "Timer.h"
#include "tm4c123gh6pm.h"
#include "IO.h"

uint32_t H, L;

DutyCycle_t Duty_Cycle = NONE_DUTY_CYCLE;

void PWM_Handler(void)
{
  //
  // Toggle PD5
  //
  if (GPIO_PORTD_DATA_R & 0x20)
  { 
    GPIO_PORTD_DATA_R &= ~0x20; // make PD5 low
    Timer3_Init(L, 1, &PWM_Handler);     // reload value for low phase
  }
  else
  {
    GPIO_PORTD_DATA_R |= 0x20;  // make PD5 high
    Timer3_Init(H, 1, &PWM_Handler);     // reload value for high phase
  }
}

//------------PWM_Init------------
// Description: Initializes a PWM signal on PB5
// Input: 
// - duty_cycle: Duty cycle for PWM. Percentage of time the signal remains high
// - freq: Frequency (cycles per second) of PWM signal. 
//
void PWM_Init(DutyCycle_t duty_cycle,
              uint32_t period)
{
  switch (duty_cycle)
  {
    case FIFTY_PERCENT_DUTY_CYCLE:
      H = L = period / 2;
    break;

    case SEVENTY_FIVE_PERCENT_DUTY_CYCLE:
      H = (period * 3) / 4;
      L = period - H;		
    break;

    case TWENTY_FIVE_PERCENT_DUTY_CYCLE:
      H = period / 4;
      L = period - H;
    break;

    default:
      H = period / 2;
      break;
  }
  Duty_Cycle = duty_cycle;
  PortD_Init();
  Timer3_Init(L, 1, &PWM_Handler);
}

void PWM_Disable (void)
{
  if (NONE_DUTY_CYCLE != Duty_Cycle)
  {
    Timer3_Disable();
    GPIO_PORTD_DATA_R &= ~0x20;   // make PD5 low
  }
}


