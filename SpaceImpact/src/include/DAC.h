#ifndef __DAC_H__
#define __DAC_H__

#include <stdint.h>

/************************************************************
 *  Functions
 ************************************************************/

/** 
 *  Write external DAC voltage using PB2-0 (3.3, 2.75, 2.2, 1.65, 1.1, 0.55, 0 voltages).
 */
void write_DAC(float voltage);

/** 
 *  Set backlight from 0 to 100% using the external DAC.
 */
 void set_backlight(uint8_t intensity);

#endif
