/*! \file  Graphics.h
 *  \brief Contains object type definition, initialization and functions
 *         to update objects' states (position, lives, etc).
 */

#ifndef __OBJECT___
#define __OBJECT___

#include <stdint.h>
#include "Graphics.h"

#define		MAX_ENEMIES				4
#define		MAX_PROJECTILES			4
#define		COOLDOWN_PROJECTILES	0.5
#define		COOLDOWN_INVINCIBLE		2

/************************************************************
 *  Object Type
 ************************************************************/

typedef struct Obj {
	float x;
	float y;
	float dx;
	float dy;
	uint8_t width;
	uint8_t height;
	uint8_t lives;
	uint8_t is_visible;
	float	time_fire;
	float	time_invincible;
	Animation_t anim_idle;
	Animation_t anim_death;
} Object_t;


/************************************************************
 *  External Variables
 ************************************************************/

extern Object_t Player;
extern Object_t Enemies[MAX_ENEMIES];
extern Object_t Lasers[MAX_PROJECTILES];
extern Object_t Bullets[MAX_PROJECTILES];


/************************************************************
 *  Functions
 ************************************************************/

/** 
 *  Tests if object is inside the gameplay area or not.
 */
uint8_t isInsideScreen(Object_t *obj);

/** 
 *  Updates object's position given a displacement and its internal velocity.
 *	The position update can be constrained to the playable area.
 */
void updateObjectPosition(Object_t *obj, float dx, float dy, uint8_t is_constrained);

/** 
 *  Creates a new projectile at object's position offsetted by a given value with given velocity.
 *	This projectile will only be created if the parent object is not on cooldown and the maximum number of
 *	projectiles on screen has not been reached.
 */
void spawnProjectile(Object_t *obj, Object_t *proj, float time, int8_t ox, int8_t oy, float dx, float dy);

/** 
 *  Updates all spawned projectiles' positions.
 */
void updateProjectilePositions(Object_t *proj);

/** 
 *  Tests if there are available projectiles to be spawned in given list.
 */
uint8_t isProjectileAvailable(Object_t *proj);

#endif
