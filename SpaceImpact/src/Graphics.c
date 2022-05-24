#include <stdio.h>
#include "Graphics.h"
#include "Nokia5110.h"
#include "SysClockCfg.h"
#include "Timer.h"

uint8_t lock_graphics = 1;

void Graphics_Init(void) {
	Timer1_Init(8000000 / FPS_SCREEN, 1, &Graphics_Callback);
}

void Graphics_Callback(void) {
	Nokia5110_DisplayBuffer();      // draw buffer
	Nokia5110_ClearBuffer();		// clear buffer	
	lock_graphics = 0;
}

void drawScoreBoard(uint8_t lives, uint16_t time, uint16_t score) {
	uint8_t i;
	char str[5];
	
	// Print scoreboard hearts
	for (i=0; i<lives; i++) {
		Nokia5110_PrintBMP_BW(2 + 7 * i, 2, BMP_Lives, 0);
	}
	
	// Print scoreboard time
	sprintf(str, "%04d", time);
	Nokia5110_PrintString(30, 0, str, 0);
	
	// Print scoreboard score
	sprintf(str, "%04d", score);
	Nokia5110_PrintString(60, 0, str, 0);
	
	Nokia5110_DisplayBuffer();
}

void drawTitle(void) {
	Nokia5110_PrintFullImage((const char *) BMP_Title, 0, 0);
	Nokia5110_PrintString(1, 1, "Press", 1);
	Nokia5110_PrintString(54, 40, "Start", 1);
}

void drawGameOver(uint16_t score) {
	char str[12];
	sprintf(str, "Score: %04d", score);
	Nokia5110_PrintString(12, 12, "Game Over!", 0);
	Nokia5110_PrintString(9, 24, str, 0);
}

uint8_t playAnimationStep(int8_t pos_x, int8_t pos_y, Animation_t *anim, uint8_t is_loop) {
	uint8_t is_anim_done = 0; 
	float time = getSysTime();
	
	// Initialize internal timing reference for animation
	if (anim->time_ref == 0) {
		anim->time_ref = time;
	}
	
	// Checks if time difference (current vs internal ref) exceeds current frame timing
	// If true, resets internal time ref and increment frame count, etc...
	if ((time - anim->time_ref) >= anim->seq_times[anim->count_seq]) {
		anim->time_ref = 0;
		anim->count_seq += 1;
		if (anim->count_seq >= anim->num_frames) {
			if (is_loop) {
				anim->count_seq = 0;
			} else {
				is_anim_done = 1;
				anim->count_seq = anim->num_frames - 1;
			}
		}
	}
	
	// Prints current frame and returns state of animation (if its done or not)
	Nokia5110_PrintBMP_BW(pos_x, pos_y, &(anim->sprite[anim->size_sprite * anim->count_seq]), 0);
	return is_anim_done;
}
