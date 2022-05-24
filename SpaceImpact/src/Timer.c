// TExaS Lab Graders Authors: Daniel Valvano, Jonathan Valvano and Ramesh Yerraballi

//#include "TExaS.h"
//#include "UART.h"
#include "tm4c123gh6pm.h"
#include <stdio.h>
#include <string.h>

void (*Timer0_Callback)(void);  // user function for timer 0
void (*Timer1_Callback)(void);  // user function for timer 1
void (*Timer2_Callback)(void);  // user function for timer 2
void (*Timer3_Callback)(void);  // user function for timer 3

void Timer0A_Handler(void);
void Timer1A_Handler(void);
void Timer2A_Handler(void);
void Timer3A_Handler(void);

//------------Timer0_Init------------
// Description: Initializes Timer 0 in 32 bit configuration and count-down mode
// Input: 
// - period: Period for timer
// - periodic_timer: 0 for one-shot, 1 for periodic
// - callback_func: Callback function to be exectured after timer expires
// Output: 1 for success, 0 for failure
unsigned int Timer0_Init(unsigned int period,
                         unsigned int periodic_timer,
                         void (*callback_func)(void))
{
    if (!callback_func ||
            !period)
    {
        return 0;
    }
    
    SYSCTL_RCGCTIMER_R |= 0x01;       // 0) activate TIMER0
    Timer0_Callback = callback_func;  // user function
    TIMER0_CTL_R = 0x00000000;        // 1) disable TIMER0A during setup
    TIMER0_CFG_R = 0x00000000;        // 2) configure for 32-bit mode
    
    if (periodic_timer)
    {
        TIMER0_TAMR_R = 0x00000002;    // 3) configure for periodic mode, default down-count settings
    }
    else
    {
        TIMER0_TAMR_R = 0x00000001;    // 3) configure for one-shot mode, default down-count settings
    }

    TIMER0_TAILR_R = period - 1;     // 4) reload value
    TIMER0_TAPR_R = 0;               // 5) bus clock resolution
    TIMER0_ICR_R = 0x00000001;       // 6) clear TIMER0A timeout flag
    TIMER0_IMR_R = 0x00000001;       // 7) arm timeout interrupt
    NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x80000000; // 8) priority 4
    NVIC_EN0_R = 1 << 19;            // 9) enable IRQ 19 in NVIC for timer 1
    TIMER0_CTL_R = 0x00000001;       // 10) enable TIMER0A
 
    return 1;
}

void Timer0_Disable(void)
{
	if (Timer0_Callback)
	{
    TIMER0_CTL_R = 0x00000000;
    TIMER0_IMR_R = 0x00000000;        // disarm timeout interrupt
    NVIC_EN0_R &= ~(1 << 19);          // disable IRQ 19 in NVIC for timer 0
	}}

//------------Timer0A_Handler------------
// Description: Timer 0 ISR
void Timer0A_Handler(void)
{
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; // acknowledge TIMER0A raw interrupt timeout
    Timer0_Callback();
}

//------------Timer1_Init------------
// Description: Initializes Timer 1 in 32 bit configuration and count-down mode
// Input: 
// - period: Period for timer
// - periodic_timer: 0 for one-shot, 1 for periodic
// - callback_func: Callback function to be exectured when timer expires
// Output: 1 for success, 0 for failure
unsigned int Timer1_Init(unsigned int period,
                         unsigned int periodic_timer,
                         void (*callback_func)(void))
{
    if (!callback_func ||
            !period)
    {
        return 0;
    }
    
    SYSCTL_RCGCTIMER_R |= 0x02;       // 0) activate TIMER1
    Timer1_Callback = callback_func;  // user function
    TIMER1_CTL_R = 0x00000000;        // 1) disable TIMER1A during setup
    TIMER1_CFG_R = 0x00000000;        // 2) configure for 32-bit mode
    
    if (periodic_timer)
    {
        TIMER1_TAMR_R = 0x00000002;     // 3) configure for periodic mode, default down-count settings
    }
    else
    {
        TIMER1_TAMR_R = 0x00000001;     // 3) configure for one-shot mode, default down-count settings
    }

    TIMER1_TAILR_R = period - 1;      // 4) reload value
    TIMER1_TAPR_R = 0;                // 5) bus clock resolution
    TIMER1_ICR_R = 0x00000001;        // 6) clear TIMER1A timeout flag
    TIMER1_IMR_R = 0x00000001;        // 7) arm timeout interrupt
    NVIC_PRI5_R = (NVIC_PRI5_R & 0xFFF00FFF) | 0x00008000; // 8) priority 4
    NVIC_EN0_R = 1 << 21;             // 9) enable IRQ 21 in NVIC for timer 1
    TIMER1_CTL_R = 0x00000001;        // 10) enable TIMER1A
 
    return 1;
}

void Timer1_Disable(void)
{
	if (Timer1_Callback)
	{
		TIMER1_CTL_R = 0x00000000;
		TIMER1_IMR_R = 0x00000000;        // disarm timeout interrupt
		NVIC_EN0_R &= ~(1 << 21);          // disable IRQ 21 in NVIC for timer 1
	}
}

//------------Timer1A_Handler------------
// Description: Timer 1 ISR
void Timer1A_Handler(void)
{
    TIMER1_ICR_R = TIMER_ICR_TATOCINT; // acknowledge TIMER1A raw interrupt timeout
    Timer1_Callback();
}

//------------Timer2_Init------------
// Description: Initializes Timer 2 in 32 bit configuration and count-down mode
// Input: 
// - period: Period for timer
// - periodic_timer: 0 for one-shot, 1 for periodic
// - callback_func: Callback function to be exectured when timer expires
// Output: 1 for success, 0 for failure
unsigned int Timer2_Init(unsigned int period,
                         unsigned int periodic_timer,
                         void (*callback_func)(void))
{
    if (!callback_func ||
            !period)
    {
        return 0;
    }
    
    SYSCTL_RCGCTIMER_R |= 0x04;       // 0) activate TIMER2
    Timer2_Callback = callback_func;  // user function
    TIMER2_CTL_R = 0x00000000;        // 1) disable TIMER2A during setup
    TIMER2_CFG_R = 0x00000000;        // 2) configure for 32-bit mode
    
    if (periodic_timer)
    {
        TIMER2_TAMR_R = 0x00000002;     // 3) configure for periodic mode, default down-count settings
    }
    else
    {
        TIMER2_TAMR_R = 0x00000001;     // 3) configure for one-shot mode, default down-count settings
    }

    TIMER2_TAILR_R = period - 1;      // 4) reload value
    TIMER2_TAPR_R = 0;                // 5) bus clock resolution
    TIMER2_ICR_R = 0x00000001;        // 6) clear TIMER2A timeout flag
    TIMER2_IMR_R = 0x00000001;        // 7) arm timeout interrupt
    NVIC_PRI5_R = (NVIC_PRI5_R & 0x00FFFFFF) | 0x80000000; // 8) priority 4
    NVIC_EN0_R = 1 << 23;             // 9) enable IRQ 23 in NVIC for timer 2
    TIMER2_CTL_R = 0x00000001;        // 10) enable TIMER2A
 
    return 1;
}

void Timer2_Disable(void)
{
	if (Timer2_Callback)
	{
		TIMER2_CTL_R = 0x00000000;
		TIMER2_IMR_R = 0x00000000;        // disarm timeout interrupt
		NVIC_EN0_R &= ~(1 << 23);          // disable IRQ 23 in NVIC for timer 2
	}
}


//------------Timer2A_Handler------------
// Description: Timer 2 ISR
void Timer2A_Handler(void)
{
    TIMER2_ICR_R = TIMER_ICR_TATOCINT; // acknowledge TIMER2A raw interrupt timeout
    Timer2_Callback();
}

//------------Timer3_Init------------
// Description: Initializes Timer 3 in 32 bit configuration and count-down mode
// Input: 
// - period: Period for timer
// - periodic_timer: 0 for one-shot, 1 for periodic
// - callback_func: Callback function to be exectured when timer expires
// Output: 1 for success, 0 for failure
unsigned int Timer3_Init(unsigned int period,
                         unsigned int periodic_timer,
                         void (*callback_func)(void))
{
    if (!callback_func ||
            !period)
    {
        return 0;
    }
    
    SYSCTL_RCGCTIMER_R |= 0x08;       // 0) activate TIMER3
    Timer3_Callback = callback_func;  // user function
    TIMER3_CTL_R = 0x00000000;        // 1) disable TIMER3A during setup
    TIMER3_CFG_R = 0x00000000;        // 2) configure for 32-bit mode
    
    if (periodic_timer)
    {
        TIMER3_TAMR_R = 0x00000002;     // 3) configure for periodic mode, default down-count settings
    }
    else
    {
        TIMER3_TAMR_R = 0x00000001;     // 3) configure for one-shot mode, default down-count settings
    }

    TIMER3_TAILR_R = period - 1;      // 4) reload value
    TIMER3_TAPR_R = 0;                // 5) bus clock resolution
    TIMER3_ICR_R = 0x00000001;        // 6) clear TIMER3A timeout flag
    TIMER3_IMR_R = 0x00000001;        // 7) arm timeout interrupt
    NVIC_PRI8_R = (NVIC_PRI8_R & 0x00FFFFFF) | 0x80000000; // 8) priority 4
    NVIC_EN1_R = 1 << 3;             // 9) enable IRQ 3 in NVIC for timer 3
    TIMER3_CTL_R = 0x00000001;        // 10) enable TIMER2A
 
    return 1;
}

void Timer3_Disable(void)
{
	if (Timer3_Callback)
	{
		TIMER3_CTL_R = 0x00000000;
		TIMER3_IMR_R = 0x00000000;        // disarm timeout interrupt
		NVIC_EN1_R &= ~(1 << 3);          // disable IRQ 3 in NVIC for timer 3
	}
}


//------------Timer3A_Handler------------
// Description: Timer 2 ISR
void Timer3A_Handler(void)
{
    TIMER3_ICR_R = TIMER_ICR_TATOCINT; // acknowledge TIMER3A raw interrupt timeout
    Timer3_Callback();
}
