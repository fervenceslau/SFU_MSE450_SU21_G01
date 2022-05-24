#include "SysClockCfg.h"
#include "Timer.h"
#include "ADC.h"

void getADCvals(void);

volatile uint16_t x_val = 0;
volatile uint16_t y_val = 0;

int main(void)
{
	SysTick_init();
	ADC_Init();
	Timer0_Init(8000000, 1, &getADCvals);
	while(1)
	{
	}
}

void getADCvals(void)
{
	get_ADC_val(&x_val, &y_val);
}

