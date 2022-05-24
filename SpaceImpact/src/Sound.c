#include "Sound.h"
#include "PWM.h"
#include "Timer.h"
#include "tm4c123gh6pm.h"

//
// Frequency of notes in Hz
//
const uint32_t NOTE_A4 = 440;
const uint32_t NOTE_B4 = 494;
const uint32_t NOTE_C4 = 262;
const uint32_t NOTE_D4 = 294;
const uint32_t NOTE_E4 = 330;
const uint32_t NOTE_F4 = 349;
const uint32_t NOTE_G4 = 392;

uint32_t PLAY_SOUND_FOREVER = 0xFFFFFFFF;


// Assuming an 80 MHz bus-clock
const uint32_t CLOCK_FREQUENCY = 80000000;

// 0.25 second tempo
const uint32_t QUARTER_SEC_TEMPO = CLOCK_FREQUENCY / 4;

// Sound to play
volatile Sound_t Sound = NO_SOUND;

//
// Duration to play sound
//
volatile uint32_t Duration = 0xFFFFFFFF;

//
// Time elapsed so far playing sound
//
volatile uint32_t Time_Elapsed = 0;

//
// Note index
//
volatile uint8_t Note_Index = 0;

//
// Sequence of notes to play on main page
//
const char Title_Page_Notes[] = "beaf gf adcbc gc ";
const char Shoot_Notes[] = "abcdefg";

//
// Note names
//
char Note_Names[] = { 'a', 'b', 'c', 'f', 'g', 'e'};

//
// 1 = 1/4 note
// 2 = 1/2 note
// 3 = 3/4 note
// 4 = Full note
//
// Beat: How long to play each note
uint8_t Title_Page_Song_Beats[] = {1, 1, 1, 1, 1, 4, 4, 2, 1, 1, 1, 1, 1, 1, 4, 4, 3};
uint8_t Shoot_Beats[] = {1, 1, 1, 1, 1, 1};

//
//
//------------Sound_GetFrequency------------
// Description: Return frequncy of note
// Input: 
// - note to play
//
// Output: frequncy in Hz
uint32_t Sound_GetFrequency(char note) 
{
  uint8_t i;
  const uint8_t num_notes = 8;
  uint32_t frequencies[] = {NOTE_A4, NOTE_B4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4};
  
  for (i = 0; i < num_notes; i++)
  {
    if (Note_Names[i] == note)
    {
      return(frequencies[i]);
    }
  }
	
  //
  // Return 0 if couldn't find frequency
  //
  return(0); 
}


//
//
//------------Sound_GetInterruptPeriod------------
// Description: Return timer period assuming an 80 MHz bus-clock
// Input: 
// - frequency of note
//
// Output: period
uint32_t Sound_GetInterruptPeriod(uint32_t frequency) 
{
  uint8_t i;
  const uint8_t num_notes = 8;
	
  uint32_t frequencies[] = {NOTE_A4, NOTE_B4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4};

  //
  // Period = frequency / 80 MHz
  //
  uint32_t period[] = { 181818, 161943, 305344, 272109, 242424, 229226, 204082 };
  
  for (i = 0; i < num_notes; i++)  // Step through the notes
  {
    if (frequencies[i] == frequency)
    {
      return(period[i]);
    }
  }
	
  //
  // Return 0 if couldn't find period
  //
  return(0); 
}

//
// Callback function for Timer2
//
void Sound_Timer2_Callback(void)
{
	uint32_t frequency, period, beat;
	
  switch (Sound)
  {
    case TITLE_PAGE_SOUND:
    {
      frequency = Sound_GetFrequency(Title_Page_Notes[Note_Index]);
      beat = Title_Page_Song_Beats[Note_Index];
      Note_Index = (Note_Index + 1) % sizeof(Title_Page_Notes);
      break;
    }
			
    case SHOOTING_SOUND:
    {
      frequency = Sound_GetFrequency(Shoot_Notes[Note_Index]);
      beat = Title_Page_Song_Beats[Note_Index];
      Note_Index = (Note_Index + 1) % sizeof(Shoot_Notes);
      break;
    }
    default:
      break;
	}	

  period = Sound_GetInterruptPeriod(frequency);

  if (period > 0)
  {	
    //
    // Play current note
    //
    PWM_Init(FIFTY_PERCENT_DUTY_CYCLE, period);
  }
	
  Time_Elapsed += QUARTER_SEC_TEMPO * beat;

  if ((Duration != 0xFFFFFFFF) && (Duration <= Time_Elapsed))
	{
		Stop_Sound();
		return;
	}		

  //
  // Create timer to play next note
  //
  Timer2_Init(QUARTER_SEC_TEMPO * beat, 1, &Sound_Timer2_Callback);
}

//------------Play_Sound------------
// Description: Play sound
// Input: 
// - sound: Type of sound to play
// - duration: Duration to play sound in ms. 0xFFFFFFFF indicates to play sound forever in a loop.
//
// Output: 1 for success, 0 for failure
void Play_Sound(Sound_t sound,
                uint32_t duration)
{
	if ((sound > MAX_SOUND) ||
		  (Sound != NO_SOUND))
	{
		return;
	}

	Sound = sound;
	Note_Index = 0;
	if (duration == 0xFFFFFFFF)
	{
	  Duration = duration;
	}
	else
	{
		Duration = ( CLOCK_FREQUENCY / 1000 ) * duration;
	}		
	
	Time_Elapsed = 0;
	Timer2_Init(1, 1, &Sound_Timer2_Callback);
	
	return;
}

//------------Stop_Sound------------
// Description: Stop playing any sound
//
void Stop_Sound(void)
{
	Sound = NO_SOUND;
	Duration = Time_Elapsed = 0;
 	Timer2_Disable();
	PWM_Disable();
	return;
}
