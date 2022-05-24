/*  \file  ADCh
 *  \brief functions to enable and read ADC inputs 
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void ADC_Init(void);

void ADC_GPIO_PE_CFG(void);

void get_ADC_val(volatile uint16_t *x_val,volatile uint16_t *y_val);


#endif /* ADC_H */
