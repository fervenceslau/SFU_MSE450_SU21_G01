#ifndef TIMER_H
#define TIMER_H

//------------Timer0_Init------------
// Description: Initializes Timer 0 in 32 bit configuration and count-down mode
// Input: 
// - period: Period for timer
// - periodic_timer: 0 for one-shot, 1 for periodic
// - callback_func: Callback function to be exectured after timer expires
// Output: 1 for success, 0 for failure
unsigned int Timer0_Init(unsigned int period,
                         unsigned int periodic_timer,
                         void (*callback_func)(void));

void Timer0_Disable(void);

//------------Timer1_Init------------
// Description: Initializes Timer 1 in 32 bit configuration and count-down mode
// Input: 
// - period: Period for timer
// - periodic_timer: 0 for one-shot, 1 for periodic
// - callback_func: Callback function to be exectured when timer expires
// Output: 1 for success, 0 for failure
unsigned int Timer1_Init(unsigned int period,
                         unsigned int periodic_timer,
                         void (*callback_func)(void));

void Timer1_Disable(void);
												 
//------------Timer2_Init------------
// Description: Initializes Timer 2 in 32 bit configuration and count-down mode
// Input: 
// - period: Period for timer
// - periodic_timer: 0 for one-shot, 1 for periodic
// - callback_func: Callback function to be exectured when timer expires
// Output: 1 for success, 0 for failure
unsigned int Timer2_Init(unsigned int period,
                         unsigned int periodic_timer,
                         void (*callback_func)(void));

void Timer2_Disable(void);
												 
//------------Timer3_Init------------
// Description: Initializes Timer 3 in 32 bit configuration and count-down mode
// Input: 
// - period: Period for timer
// - periodic_timer: 0 for one-shot, 1 for periodic
// - callback_func: Callback function to be exectured when timer expires
// Output: 1 for success, 0 for failure
unsigned int Timer3_Init(unsigned int period,
                         unsigned int periodic_timer,
                         void (*callback_func)(void));

void Timer3_Disable(void);

#endif
				
