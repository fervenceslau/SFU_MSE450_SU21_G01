/*  \file  SysClockCfg.h
 *  \brief functions to enable system clock, and the system tick such that the
 *         system tracks the elapesed time as unsigned interger 1/100 of a
 *         second resolution
 */
#ifndef SYS_CLOCK_CFG_H
#define SYS_CLOCK_CFG_H


#include <stdint.h>

/** 
 *  configures sysTick, configures the clock as result
 */
void SysTick_init(void);

/**
 *  configures system clock using PLL
 */
void sysClockCfg(void);

/**
 *  Blocking delay in clock cycles
 *  \param delay in clock cycles (12.5ns per cycle)
 */ 
void Delay_Clk(uint32_t cycles);

/**
 *  Blocking delay in ms
 *  /param delay time in ms
 */
void Delay_ms(uint32_t time_ms);

/** 
 *  returns system clock in units of 0.01s
 *  /return clock time in 1/100 seconds integers
 */
uint32_t getSysClock(void);

/** 
 *  returns system time in units of 0.01s
 *  /return clock time in 1/100 seconds float
 */
float getSysTime(void);

/* 
 * resets thes system clock back to 0
 */
void zeroSysClock(void);

/* 
 * pauses thes system clock counting
 */
void pauseSysClock(void);

/* 
 * resumes thes system clock counting
 */
void resumeSysClock(void);

/*
 * ISR handler for sysTick. Increments system clock
 */
void SysTick_Handler(void);

/*
 * Stops the sysTick by disableing tick and interrupt
 */
void stopTickCount(void);

/*
 * resets the sysTick and relaods the counter
 */
 void resetTick(void);

#endif /* SYS_CLOCK_CFG_H */
