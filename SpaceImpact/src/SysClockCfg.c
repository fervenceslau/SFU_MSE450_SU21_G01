#include "tm4c123gh6pm.h"
#include "sysClockCfg.h"
#include "Graphics.h"


#define NVIC_ST_RELOAD_10MS  0x000C3499  // 80MHz clock is 10ms sys tick (800000 - 1)
#define RCC2_CLOCK_DIV              0x1FC00000  // clock divider field
#define RCC2_CLOCK_DIV_VAL          5           // 400MHz/5 = 80MHz clock

int32_t StartCritical(void);
void EndCritical(int32_t i);
static volatile uint32_t sysClock;
static volatile uint8_t is_paused = 1;

void SysTick_init(void)
{
  sysClockCfg();
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_10MS;  // 10ms reload value
  NVIC_ST_CURRENT_R = 0;
  NVIC_SYS_PRI3_R &= 0x00FFFFFF; //clear interrupt priority byte
  NVIC_SYS_PRI3_R |= 0x40000000; //sets sys tick priority
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC+NVIC_ST_CTRL_INTEN+NVIC_ST_CTRL_ENABLE;
  is_paused = 0;
	zeroSysClock();
}

void sysClockCfg(void)
{
  // Set retister to use RCC2 functionality and bypass during config
  SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;
  SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;
  //set oscillator source 16MHz external
  SYSCTL_RCC_R &= ~SYSCTL_RCC_XTAL_M;   // clear XTAL field
  SYSCTL_RCC_R |= SYSCTL_RCC_XTAL_16MHZ;// configure for 16 MHz crystal
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;// clear oscillator source field
  SYSCTL_RCC2_R |= SYSCTL_RCC2_OSCSRC2_MO;// configure for main oscillator source
  //activate PLL by clearing PWRDWN bit
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;
  //utlilze 400MHz PLL  
  SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;  // use 400 MHz PLL
  //Set Clock Divider
  SYSCTL_RCC2_R &= ~RCC2_CLOCK_DIV; // clear system clock divider field
  SYSCTL_RCC2_R |= (5 << 22); //80MHz Clock
  //wait for PLL to lock
  while(!(SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS)){};
  //enable PLL 
  SYSCTL_RIS_R &= ~SYSCTL_RCC2_BYPASS2;
}

void Delay_Clk(uint32_t cycles)
{
  volatile unsigned long time_elapsed = 0;
  volatile unsigned long time_start = NVIC_ST_CURRENT_R;
  while(time_elapsed <= cycles) {
		time_elapsed = (time_start - NVIC_ST_CURRENT_R) & 0x00FFFFFF;
  }
}

void Delay_ms(uint32_t time_ms)
{
  //assuming 80MHz clock 1ms = 80,000
  //Could add div scaling but not part of lab scope
  uint32_t i = 0;
  for(i = 0; i < time_ms; ++i){
		Delay_Clk(80000);
  }
}

uint32_t getSysClock(void)
{
	uint32_t ret = 0;
	int32_t i_val = 0;
  i_val = StartCritical();
  ret = sysClock;
  EndCritical(i_val);
	return ret;
}

float getSysTime(void)
{
	float time = ((float) getSysClock()) * GRAPHICS_SIMULATION_SPEED / 100;
	return time;
}

void zeroSysClock(void)
{
	int32_t i_val = 0;
	i_val = StartCritical();
	sysClock = 0;
	EndCritical( i_val );
}

void pauseSysClock(void)
{
	int32_t i_val = 0;
	if (!is_paused) {
		i_val = StartCritical();
		is_paused = 1;
		EndCritical( i_val );
	}
}

void resumeSysClock(void)
{
	int32_t i_val = 0;
	if (is_paused) {
		i_val = StartCritical();
		is_paused = 0;
		EndCritical( i_val );
	}
}

void SysTick_Handler(void)
{
	if (!is_paused) {
		sysClock++;
	}
}

void stopTickCount(void)
{
	NVIC_ST_CTRL_R &= ~(NVIC_ST_CTRL_INTEN + NVIC_ST_CTRL_ENABLE);
}

void resetTick(void)
{
	if(NVIC_ST_CTRL_R & NVIC_ST_CTRL_ENABLE) {
		stopTickCount();
	}
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_10MS;  // 10ms reload value
  NVIC_ST_CURRENT_R = 0;
  NVIC_SYS_PRI3_R &= 0x00FFFFFF; //clear interrupt priority byte
  NVIC_SYS_PRI3_R |= 0x40000000; //sets sys tick priority
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC+NVIC_ST_CTRL_INTEN+NVIC_ST_CTRL_ENABLE;
	zeroSysClock();
	is_paused = 0;
}

