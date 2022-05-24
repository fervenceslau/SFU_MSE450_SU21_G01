#include <math.h>
#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "SysClockCfg.h"
#include "Timer.h"
#include "IO.h"
#include "ADC.h"
#include "UART.h"
#include "Objects.h"
#include "Graphics.h"
#include "Path.h"
#include "GameEngine.h"
#include "Sound.h"

int main(void){
	Nokia5110_Init();
	SysTick_init();
	UART_Init();
	PortB_Init();
	PortF_Init();
	Graphics_Init();
	ADC_Init();
	Play_Sound(TITLE_PAGE_SOUND, PLAY_SOUND_FOREVER);

	// Update game if graphics lock has been released by Graphics_Callback
	while(1){
		if (!lock_graphics) {
			updateGame();
			lock_graphics = 1; // resets lock state
		}
	}
}
