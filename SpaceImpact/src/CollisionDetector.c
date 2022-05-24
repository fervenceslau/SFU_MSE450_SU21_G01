#include "Objects.h"
#include "CollisionDetector.h"
#include "sysClockCfg.h"
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

/* Store enemies that are currently in collision with player in this array */
static Object_t * EnemyCollisions[MAX_ENEMIES] = {NULL, NULL, NULL, NULL};

//********CollisionDetector_IsDetected*****************
// Description: Is collision detectd between object1 and object2
//
// Inputs:	object1 - Pointer to object 1
//			object2 - Pointer to object 2
//			collisionBoundary- Collision boundary
//
// Outputs: 0 - No collision detected
//			1 - Collision detected
uint8_t CollisionDetector_IsDetected (const Object_t * object1, const Object_t * object2, CollisionBoundaryType collisionBoundary)
{
	uint32_t object1SizeX = object1->anim_idle.sprite[0];
	uint32_t object1SizeY = object1->anim_idle.sprite[1];
	uint32_t object2SizeX = object2->anim_idle.sprite[0];
	uint32_t object2SizeY = object2->anim_idle.sprite[1];
	
	switch (collisionBoundary)
	{
			case AABB_BOUNDARY_SHAPE:
			{
				if ((object1->x < object2->x + object2SizeX) &&
					(object1->x + object1SizeX > object2->x) &&
					(object1->y < object2->y + object2SizeY) &&
					(object1->y + object1SizeY > object2->y))
				{
					return 1;
				}
			}
			break;
			case CIRCLE_BOUNDARY_SHAPE:
			{
				float radiusObject1 = sqrtf(object1SizeX * object1SizeX + object1SizeY * object1SizeY)/2.0f;
				float radiusObject2 = sqrtf(object2SizeX * object2SizeX + object2SizeY * object2SizeY)/2.0f;
				
				float object1CenterX = object1->x + radiusObject1;
				float object1CenterY = object1->y - radiusObject1;
				
				float object2CenterX = object2->x + radiusObject2;
				float object2CenterY = object2->y - radiusObject2;
				
				float dx = object1CenterX - object2CenterX;
				float dy = object1CenterY - object2CenterY;
				
				float distance = sqrtf(dx * dx + dy * dy);
				
				if (distance < (radiusObject1 + radiusObject2))
				{
					 return 1;
				}
			}
			break;
			default:
			break;
	}

	return 0;
}


//********CollisionDetector_ProcessCollisions*****************
// Description: Process collisions
//
// Inputs:	collisionBoundary - Collision boundary
//
// Outputs: Number of points scored
//
uint16_t CollisionDetector_ProcessCollisions(CollisionBoundaryType collisionBoundary)
{
	uint16_t points_scored = 0;

	/*
	 * Process collisions
	 */
	for (uint8_t i = 0; i < MAX_ENEMIES; ++i)
	{
		if (!Enemies[i].lives)
		{
			continue;
		}
		
		/*
		 * Process player collisions
		 */
		if (CollisionDetector_IsDetected(&Enemies[i], &Player, AABB_BOUNDARY_SHAPE))
		{
			uint8_t r = 0;
			uint8_t first_empty_slot = 0xFF;
			
			/*
			 * Check if enemy is present in collision array
			 */
			for (; r < MAX_ENEMIES; ++r)
			{
				if (EnemyCollisions[r] == &Enemies[i])
				{
					break;
				}
				
				if ((EnemyCollisions[r] == NULL) && 
					(first_empty_slot == 0xFF))
				{
					first_empty_slot = r;
				}
			}
			
			/*
			 * Store enemy in collision array if it doesn't exist already and decrement player life
			 */
			if ((r == MAX_ENEMIES) &&
				(first_empty_slot != 0xFF))
			{
				EnemyCollisions[first_empty_slot] = &Enemies[i];

				float time = getSysTime();
				if (Player.lives > 0 && time > Player.time_invincible)
				{
					Player.time_invincible = time + ((float) COOLDOWN_INVINCIBLE);
					Player.lives--;
				}
			}
		}
		else
		{
			for (uint8_t r = 0; r < MAX_ENEMIES; ++r)
			{
				if (&Enemies[i] == EnemyCollisions[r])
				{
					EnemyCollisions[r] = NULL;
				}
			}
			
		}
	
		/*
		 * Process bullet collisions
		 */
		for (uint8_t j = 0; j < MAX_PROJECTILES; ++j)
		{
			if (Bullets[j].is_visible == 0)
			{
				continue;
			}

			if (CollisionDetector_IsDetected(&Enemies[i], &Bullets[j], AABB_BOUNDARY_SHAPE))
			{
				Enemies[i].lives = 0;
				Bullets[j].is_visible = 0;
				++points_scored;

			}
		}
		
		/*
		 * Process laser collisions
		 */
		for (uint8_t k = 0; k < MAX_PROJECTILES; ++k)
		{
			if (Lasers[k].is_visible == 0)

			{
				continue;
			}

			if (CollisionDetector_IsDetected(&Enemies[i], &Lasers[k], AABB_BOUNDARY_SHAPE))
			{
				Enemies[i].lives = 0;
				Lasers[k].is_visible = 0;
				++points_scored;
			}
		}
	}
	
	return points_scored;
}
