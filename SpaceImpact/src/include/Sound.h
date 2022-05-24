#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

extern uint32_t PLAY_SOUND_FOREVER;

typedef enum Sound {
	NO_SOUND,
	TITLE_PAGE_SOUND,
	SHOOTING_SOUND,
	MAX_SOUND
} Sound_t;

//------------Play_Sound------------
// Description: Play sound
// Input: 
// - sound: Type of sound to play
// - duration: Duration to play sound in ms. 0xFFFFFFFF indicates to play it in a loop
//
void Play_Sound(Sound_t sound,
                uint32_t duration);

//------------Stop_Sound------------
// Description: Stop playing any sound
//
void Stop_Sound(void);

#endif
				
