#include <stdint.h>
#include "Objects.h"
#include "Graphics.h"
#include "Nokia5110.h"
#include "Sound.h"

Object_t Player = {
	0, 20, 0, 0, 16, 8, 3, 1, 0, 0,
	{
		// Idle animation
		(uint8_t *) Sprite_Player,
		(float *) Times_Player, 18, 4, 0, 0
	},
	{
		// Default death animation
		(uint8_t *) Sprite_Explosion,
		(float *) Times_Explosion, 10, 6, 0, 0
	}
};

Object_t Enemies[MAX_ENEMIES] = {
	{0, 0, 0, 0, 8, 8, 1, 1, 0, 0, {(uint8_t *) Sprite_Enemy, (float *) Times_Enemy, 10, 4, 0, 0}, {(uint8_t *) Sprite_Explosion, (float *) Times_Explosion, 10, 6, 0, 0}},
	{0, 0, 0, 0, 8, 8, 1, 1, 0, 0, {(uint8_t *) Sprite_Enemy, (float *) Times_Enemy, 10, 4, 0, 0}, {(uint8_t *) Sprite_Explosion, (float *) Times_Explosion, 10, 6, 0, 0}},
	{0, 0, 0, 0, 8, 8, 1, 1, 0, 0, {(uint8_t *) Sprite_Enemy, (float *) Times_Enemy, 10, 4, 0, 0}, {(uint8_t *) Sprite_Explosion, (float *) Times_Explosion, 10, 6, 0, 0}},
	{0, 0, 0, 0, 8, 8, 1, 1, 0, 0, {(uint8_t *) Sprite_Enemy, (float *) Times_Enemy, 10, 4, 0, 0}, {(uint8_t *) Sprite_Explosion, (float *) Times_Explosion, 10, 6, 0, 0}}
};

Object_t Lasers[MAX_PROJECTILES] = {
	{0, 0, 0, 0, 8, 2, 1, 0, 0, 0, {(uint8_t *) Sprite_Laser, (float *) Times_Laser, 10, 1, 0, 0}},
	{0, 0, 0, 0, 8, 2, 1, 0, 0, 0, {(uint8_t *) Sprite_Laser, (float *) Times_Laser, 10, 1, 0, 0}},
	{0, 0, 0, 0, 8, 2, 1, 0, 0, 0, {(uint8_t *) Sprite_Laser, (float *) Times_Laser, 10, 1, 0, 0}},
	{0, 0, 0, 0, 8, 2, 1, 0, 0, 0, {(uint8_t *) Sprite_Laser, (float *) Times_Laser, 10, 1, 0, 0}}
};

Object_t Bullets[MAX_PROJECTILES] = {
	{0, 0, 0, 0, 4, 4, 1, 0, 0, 0, {(uint8_t *) Sprite_Bullet, (float *) Times_Bullet, 6, 1, 0, 0}},
	{0, 0, 0, 0, 4, 4, 1, 0, 0, 0, {(uint8_t *) Sprite_Bullet, (float *) Times_Bullet, 6, 1, 0, 0}},
	{0, 0, 0, 0, 4, 4, 1, 0, 0, 0, {(uint8_t *) Sprite_Bullet, (float *) Times_Bullet, 6, 1, 0, 0}},
	{0, 0, 0, 0, 4, 4, 1, 0, 0, 0, {(uint8_t *) Sprite_Bullet, (float *) Times_Bullet, 6, 1, 0, 0}}

};

uint8_t isInsideScreen(Object_t *obj) {
	if ((obj->x + obj->width >= 0) &&
		(obj->x < SCREENW) &&
		(obj->y + obj->height >= SCOREBOARDH) &&
		(obj->y < SCREENH)) {
		return 1;
	} else {
		return 0;
	}
}

void updateObjectPosition(Object_t *obj, float dx, float dy, uint8_t is_constrained) {
	float px = obj->x + obj->dx + dx;
	float py = obj->y + obj->dy + dy;
	
	// Restrict object to be fully inside gamaplay area
	if (is_constrained) {
		if (px >= 0 && px + obj->width < SCREENW) {
			obj->x = px;
		}
		if (py >= SCOREBOARDH && py + obj->height < SCREENH) {
			obj->y = py;
		}
	} else {
		obj->x = px;
		obj->y = py;
	}
		
	// Visibility flag is necessary in addition to lives for us to draw death animation
	if (obj->is_visible && isInsideScreen(obj)) {
		if (obj->lives > 0){
			playAnimationStep((int8_t) obj->x, (int8_t) obj->y, &obj->anim_idle, 1);
		} else {
			int8_t w = (obj->anim_idle.sprite[0] - obj->anim_death.sprite[0]) / 2;
			int8_t h = (obj->anim_idle.sprite[1] - obj->anim_death.sprite[1]) / 2;
			uint8_t is_anim_done = playAnimationStep((int8_t) obj->x + w, (int8_t) obj->y + h, &obj->anim_death, 0);
			if (is_anim_done) {
				obj->is_visible = 0;
			}
		}
	}
}

void spawnProjectile(Object_t *obj, Object_t *proj, float time, int8_t ox, int8_t oy, float dx, float dy) {
	// Get the first projectile index available
	uint8_t i;
	for (i=0; i<MAX_PROJECTILES && proj[i].is_visible; i++);
	
	// If the projectile index and the player cooldown are valid, then spawn a new projectile
	uint8_t is_cooldown  = time > Player.time_fire;
	uint8_t is_available = i < MAX_PROJECTILES;
	if (is_available && is_cooldown) {
		proj[i].is_visible = 1;
		proj[i].x = obj->x + (obj->width - proj[i].width) / 2 + ox;
		proj[i].y = obj->y + (obj->height - proj[i].height) / 2 + oy;
		proj[i].dx = dx;
		proj[i].dy = dy;
		Player.time_fire = time + ((float) COOLDOWN_PROJECTILES);
		Play_Sound(SHOOTING_SOUND, 1000);
	}
}

void updateProjectilePositions(Object_t *proj) {
	uint8_t i;
	for (i=0; i<MAX_PROJECTILES; i++) {
		if (proj[i].is_visible) {
			updateObjectPosition(&proj[i], 0, 0, 0);
			if (!isInsideScreen(&proj[i])) {
				proj[i].is_visible = 0;
			}
		}
	}
}

uint8_t isProjectileAvailable(Object_t *proj) {
	uint8_t i;
	for (i=0; i<MAX_PROJECTILES && proj[i].is_visible; i++);
	if (i < MAX_PROJECTILES) {
		return 1;
	}
	return 0;
}
