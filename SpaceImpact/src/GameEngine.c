#include <stdint.h>
#include <stdlib.h>
#include "GameEngine.h"
#include "sysClockCfg.h"
#include "ADC.h"
#include "IO.h"
#include "Objects.h"
#include "Path.h"
#include "Graphics.h"
#include "Nokia5110.h"
#include "CollisionDetector.h"
#include "DAC.h"
#include "UART.h"
#include "Sound.h"


float time = 0;
float time_loop = 0;
uint16_t score = 0;
uint16_t x_adc = 0;
uint16_t y_adc = 0;
uint8_t btn_state = 0;

Game_State_t game_state = ST_TITLE;

char debug_msg[20];

void updateGame(void) {

	// Get game time, joystick (adc) and button states
	time = getSysTime();
	get_ADC_val(&x_adc, &y_adc);
	btn_state = readButtons();
	
	// Update time_loop to make enemies spawn in an infinite loop
	time_loop = time;
	while (time_loop > 25) {
		time_loop -= 25;
	}
	
	// Update game based on current game state
	switch(game_state) {
		case ST_TITLE:
			drawTitle();
			if (btn_state & BIT_FLAG_SW2) {
				score = 0;
				resetTick();
				initializeObjects();
				game_state = ST_RUNNING;
        Stop_Sound();
			} else {
				if (btn_state & BIT_FLAG_SW1) {
					game_state = ST_DEBUG;
				}
			}
			break;
		
		case ST_RUNNING:
			if (Player.lives == 0) {
				pauseSysClock();
				game_state = ST_GAME_OVER;
			} else {
				if (btn_state & BIT_FLAG_SW2) {
					pauseSysClock();
					game_state = ST_PAUSED;
				} else {
					updatePlayerPosition();
					updateProjectilePositions(Lasers);
					updateEnemyPositions();
					if (btn_state & BIT_FLAG_SW1) {
						spawnProjectile(&Player, Lasers, time, 0, 1, 4, 0);
					}
					score += CollisionDetector_ProcessCollisions(AABB_BOUNDARY_SHAPE);
					drawScoreBoard(Player.lives, (unsigned short) time, score);
				}
			}
			break;
		
		case ST_PAUSED:
			if (btn_state & BIT_FLAG_SW2) {
				resumeSysClock();
				game_state = ST_RUNNING;
			} else {
				// Display "PAUSED" in the middle of the screen
				Nokia5110_PrintString(9, 20, "Game Paused", 0);
			}
			break;
		
		case ST_GAME_OVER:
			// Display "GAME OVER", maybe count some time before resetting game...
			drawGameOver(score);
			if (btn_state & BIT_FLAG_SW2){
				game_state = ST_TITLE;
				Play_Sound(TITLE_PAGE_SOUND, PLAY_SOUND_FOREVER);
			}
			break;
			
		case ST_DEBUG:
			UART_OutString("Type in debug code: ");
			UART_InString(debug_msg, 20 - 1);
			UART_OutChar(CR);
			UART_OutChar(LF);
		
			// Set backlight based on code "B=VALUE", VALUE from 0 to 100
			if (debug_msg[0] == 'B' || debug_msg[0] == 'b') {
				uint8_t intensity = atoi(&debug_msg[2]);
				set_backlight(intensity);
			}
			break;
	}
	
}

void updatePlayerPosition(void) {
	float y_joy = (((float) y_adc) - ((1 << 11) - 1)) / 2048;
	updateObjectPosition(&Player, 0, y_joy, 1);
}

void updateEnemyPositions(void) {
	uint8_t i;
	for (i=0; i<MAX_ENEMIES; i++){
		
		// Interpolate enemies' positions according to path events
		interpolatePathEvent(path_event, &Enemies[i], time_loop - 0.5*i);
		updateObjectPosition(&Enemies[i], 0, 0, 0);
		
		// Reset enemy health when it leaves the screen
		if (!isInsideScreen(&Enemies[i])) {
			Enemies[i].lives = 1;
		}
	}
}

void initializeObjects(void) {
	uint8_t i;
	Player.x = 0;
	Player.y = 20;
	Player.lives = 3;
	Player.time_fire = 0;
	Player.time_invincible = 0;
	Player.anim_idle.time_ref = 0;
	Player.anim_death.time_ref = 0;
	for (i=0; i<MAX_ENEMIES; i++){
		Enemies[i].x = 0;
		Enemies[i].y = 0;
		Enemies[i].is_visible = 1;
		Enemies[i].lives = 1;
		Enemies[i].anim_idle.time_ref = 0;
		Enemies[i].anim_death.time_ref = 0;
	}
	for (i=0; i<MAX_PROJECTILES; i++){
		Lasers[i].x = 0;
		Lasers[i].y = 0;
		Lasers[i].is_visible = 0;
		Lasers[i].lives = 1;
	}
}
