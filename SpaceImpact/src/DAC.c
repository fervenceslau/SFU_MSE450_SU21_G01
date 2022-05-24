#include <stdint.h>
#include "DAC.h"
#include "tm4c123gh6pm.h"

// Decodr LUT used to map digital to analog voltage
uint8_t PB_LUT[]  = {0, 4, 2, 1, 6, 5, 3, 7};

void write_DAC(float voltage) {
	
	// Calculate number of divisions of 0.55 volts and its remainder
	uint8_t divisions = (uint8_t) (voltage / 0.55);
	float   remainder  = voltage - divisions * 0.55;
	
	// Add offset caused by the 1.65 symmetry
	if (voltage >= 1.65) {
		divisions++;
	}
	
	// Round to nearest DAC value
	if (remainder > 0.55 / 2) {
		divisions++;
	}
	
	// Saturate number of divisions to maximum range
	if (divisions > 7) {
		divisions = 7;
	}
	
	// Output to PORT B based on number of divisions and lookup table
	GPIO_PORTB_DATA_R = PB_LUT[divisions];
}

void set_backlight(uint8_t intensity) {
	float voltage = ((float) intensity) * (3.3 / 100);
	if (voltage > 3.3) {
		voltage = 3.3;
	}
	write_DAC(voltage);
}
