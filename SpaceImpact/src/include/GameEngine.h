/*! \file  GameEngine.h
 *  \brief Contains definitions used to control how the game operates.
 */

#ifndef __GAME_ENGINE__
#define __GAME_ENGINE__

/************************************************************
 *  Game State Enumeration Type
 ************************************************************/

typedef enum gs {
	ST_TITLE,
	ST_RUNNING,
	ST_PAUSED,
	ST_GAME_OVER,
	ST_DEBUG
} Game_State_t;


/************************************************************
 *  Functions
 ************************************************************/

/** 
 *  Updates game data such as player position, spawns lasers, tests collisions, etc.
 *  This function takes into account game state to perform specific actions.
 */
void updateGame(void);

/** 
 *  Updates player position after reading joystick (adc) values.
 */
void updatePlayerPosition(void);

/** 
 *  Updates enemies' position after following a timed path event list.
 */
void updateEnemyPositions(void);

/** 
 *  Initialize all objects to their default values.
 */
void initializeObjects(void);

#endif
