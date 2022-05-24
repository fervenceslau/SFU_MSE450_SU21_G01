/*! \file  Path.h
 *  \brief Contains definitions used to update enemy positions
 *	following a simple structure using interpolation.
 */
 
#ifndef __PATH___
#define __PATH___

#include <stdio.h>
#include "Objects.h"

#define	NUM_PATH_EVENTS		2

/************************************************************
 *  Enemy Path Type
 ************************************************************/

typedef struct Path {
	const signed char   *seq_x;
	const signed char   *seq_y;
	const float 		*seq_times;
	const unsigned char num_seq;
} Path_t;


/************************************************************
 *  Path Event Type
 ************************************************************/

typedef struct Path_Event {
	const Path_t	*path;
	const float 	time_start;
	const uint8_t	mirror_y;
	
} Path_Event_t;


/************************************************************
 *  Path and Path Event initialization
 ************************************************************/

static const signed char path_zig_zag_x[] 	  = { 91,  33, 73, -8};
static const signed char path_zig_zag_y[] 	  = { 17,  17, 39, 39};
static const float 		 path_zig_zag_times[] = {  0,   2,  3,  5};

static Path_t path = {
	path_zig_zag_x,
	path_zig_zag_y,
	path_zig_zag_times,
	4
};

static Path_Event_t path_event[NUM_PATH_EVENTS] = {
	{&path, 5, 0},
	{&path, 15, 1}
};


/************************************************************
 *  Functions
 ************************************************************/

/** 
 *  Interpolates object's position given a path.
 */
void interpolatePathStep(const Path_t *path, Object_t *obj, float time, uint8_t mirror_y);

/** 
 *  Interpolates object's position given a list of timed path events.
 */
void interpolatePathEvent(Path_Event_t *path_event, Object_t *obj, float time);

#endif
