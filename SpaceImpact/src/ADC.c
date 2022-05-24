/*  \file  ADCh
 *  \brief functions to enable and read ADC inputs 
 */
 
#include "ADC.h"
#include "tm4c123gh6pm.h"

void ADC_Init(void)
{
  volatile uint32_t delay;
  ADC_GPIO_PE_CFG();
  SYSCTL_RCGC0_R |= 0x00030000;   // Activate ADC0/1
  delay = SYSCTL_RCGC2_R;     //allows clock to stabilize
  SYSCTL_RCGC0_R &= ~0x00000F00; //configure for 125k
  //sequencer priority
	ADC0_SSPRI_R = 0x0123; //sequencer 3 (highest prioirity)
	ADC1_SSPRI_R = 0x0123; //sequencer 3 (highest prioirity)
	// disable sample sequencer 3
  ADC0_ACTSS_R &= ~0x0008;  // disable sample sequencer 3
	ADC1_ACTSS_R &= ~0x0008;  
	//sw trigger cfg
  ADC0_EMUX_R &= ~0xF000;         // seq3 is software trigger
	ADC1_EMUX_R &= ~0xF000;
		
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+1; // channel Ain1 (PE2)
	ADC1_SSMUX3_R = (ADC1_SSMUX3_R&0xFFFFFFF0);   // channel Ain0 (PE3) (PE5)
  ADC0_SSCTL3_R = 0x0006;         // no TS0 D0, yes IE0 END0
	ADC1_SSCTL3_R = 0x0006;
		
  ADC0_ACTSS_R &= 0x0008;  // enable sample sequencer 3
	ADC1_ACTSS_R &= 0x0008;

}

void ADC_GPIO_PE_CFG(void)
{
  volatile uint32_t delay;
  SYSCTL_RCGC2_R |= 0x00000010; //Port E clock Enable
  delay = SYSCTL_RCGC2_R;     //allows clock to stabilize
  GPIO_PORTE_DIR_R &= ~0x0C; //make PE2, PE3 input 
  GPIO_PORTE_AFSEL_R |= 0x0C; //enable alternate function
  GPIO_PORTE_DEN_R &= ~0x0C; //diable digital IO
	GPIO_PORTE_AMSEL_R |= 0x0C; //enable Analog function

}

void get_ADC_val(volatile uint16_t *x_val,volatile uint16_t *y_val)
{
  ADC0_PSSI_R = 0x0008;            // initiate SS3
  while((ADC0_RIS_R&0x08)==0){};   // wait for conversion done
  *y_val = ADC0_SSFIFO3_R&0xFFF;   // read result
  ADC0_ISC_R = 0x0008;             // acknowledge completion

#ifndef SIMULATION
	ADC1_PSSI_R = 0x0008;            // initiate SS3
  while((ADC1_RIS_R&0x08)==0){};   // wait for conversion done
  *x_val = ADC1_SSFIFO3_R&0xFFF;   // read result
  ADC1_ISC_R = 0x0008;             // acknowledge completion
#endif
}
