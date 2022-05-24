#include <stdio.h>
#include "Path.h"
#include "Graphics.h"
#include "Nokia5110.h"
#include "SysClockCfg.h"

void interpolatePathStep(const Path_t *path, Object_t *obj, float time, uint8_t mirror_y) {
	
	// Find sequence index for given time input with respect to seq_times
	uint8_t seq_idx;
	for (seq_idx = 0; (seq_idx < path->num_seq - 1) && (time > path->seq_times[seq_idx + 1]); seq_idx++);
	
	// Define object's interpolated position to be outside the screen
	float px = -obj->width;
	float py = -obj->height;
	
	// Interpolates pathing positions if current time is between two adjacent path points
	if (seq_idx < path->num_seq - 1) {
		float res_path = time - path->seq_times[seq_idx];
		float res_time = path->seq_times[seq_idx + 1] - path->seq_times[seq_idx];
		float interp_val = res_path / res_time;
		
		px = ((float) path->seq_x[seq_idx]) * (1 - interp_val) + ((float) path->seq_x[seq_idx + 1]) * interp_val;
		// Mirror path along y-axis
		if (!mirror_y) {
			py = ((float) (path->seq_y[seq_idx])) * (1 - interp_val) + ((float) path->seq_y[seq_idx + 1]) * interp_val;
		} else {
			py = ((float) (SCREENH + SCOREBOARDH - path->seq_y[seq_idx])) * (1 - interp_val) + ((float) (SCREENH + SCOREBOARDH - path->seq_y[seq_idx + 1])) * interp_val;
		}
		
	// Else reset object's lives, since current time is outside interpolation region
	} else {
		obj->lives = 1;
		obj->is_visible = 1;
	}
	
	// Update object's position directly (instead of through dx, dy)
	if (obj->lives > 0) {
		obj->x = (signed int) (px - obj->width / 2);
		obj->y = (signed int) (py - obj->height / 2);
	}
}

void interpolatePathEvent(Path_Event_t *path_event, Object_t *obj, float time){
	// Find which path event to select given global time reference
	uint8_t path_idx;
	for (path_idx = 0; (path_idx < NUM_PATH_EVENTS - 1) && (time > path_event[path_idx + 1].time_start); path_idx++);
	interpolatePathStep(path_event[path_idx].path, obj, time - path_event[path_idx].time_start, path_event[path_idx].mirror_y);
}
