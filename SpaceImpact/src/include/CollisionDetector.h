#ifndef __COLLISION_DETECTOR__
#define __COLLISION_DETECTOR__

#include "Objects.h"
#include <stdint.h>

enum CollisionBoundary
{
    AABB_BOUNDARY_SHAPE,    /* Treat object as a rectangle */
    CIRCLE_BOUNDARY_SHAPE   /* Treat object as a circle */
};

typedef enum CollisionBoundary CollisionBoundaryType;

//********CollisionDetector_ProcessCollisions*****************
// Description: Process collisions between enemies, bullets, and player
//
// Inputs:	collisionBoundary - Collision boundary
//
// Outputs: Number of points scored
//
uint16_t CollisionDetector_ProcessCollisions(CollisionBoundaryType collisionBoundary);

//********CollisionDetector_IsDetected*****************
// Description: Is collision detectd between object1 and object2
//
// Inputs:	object1 - Pointer to object 1
//			object2 - Pointer to object 2
//			collisionBoundary- Collision boundary
//
// Outputs: 0 - No collision detected
//			1 - Collision detected
uint8_t CollisionDetector_IsDetected (const Object_t * object1, const Object_t * object2, CollisionBoundaryType collisionBoundary);

#endif
