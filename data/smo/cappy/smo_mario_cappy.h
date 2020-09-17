#ifndef SMO_MARIO_CAPPY_H
#define SMO_MARIO_CAPPY_H

#include "types.h"

<<<<<<< HEAD
=======
/* Constants */

#define CAPPY_WALL_RADIUS		48.f
#define CAPPY_HITBOX_RADIUS		80.f
#define CAPPY_HITBOX_HEIGHT		112.f
#define CAPPY_HITBOX_DOWNOFFSET 32.f
#define CAPPY_MAX_LIFETIME		120		// 4 seconds
#define CAPPY_DELTA_VEL			80.f	// max number of units cappy moves per frame

>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
enum {
	CAPPY_ACT_FLAG_GROUND		= 0,
	CAPPY_ACT_FLAG_AIRBORNE		= 1,
	CAPPY_ACT_FLAG_WATER		= 2,
	CAPPY_ACT_FLAG_METAL_WATER	= 3,
	CAPPY_ACT_FLAG_FLYING		= 4,
};

/* Cappy */
<<<<<<< HEAD
u32  mario_spawn_cappy(struct MarioState *m, u8 actFlag);
void mario_update_cappy(struct MarioState *m);
void mario_unload_cappy(struct MarioState *m);

/* Behaviors */
void cappy_bhv1_init(struct Object *cappy, struct MarioState *m);
void cappy_bhv1_update(struct Object *cappy, struct MarioState *m);
void cappy_bhv2_init(struct Object *cappy, struct MarioState *m);
void cappy_bhv2_update(struct Object *cappy, struct MarioState *m);
void cappy_bhv3_init(struct Object *cappy, struct MarioState *m);
void cappy_bhv3_update(struct Object *cappy, struct MarioState *m);
=======
struct Object *get_cappy_object(struct MarioState *m);
u32 mario_spawn_cappy(struct MarioState *m, u8 actFlag);
void cappy_register_wall_object(struct Object *cappy, struct Object *obj);
void cappy_register_floor_object(struct Object *cappy, struct Object *obj);
void cappy_register_ceiling_object(struct Object *cappy, struct Object *obj);
void cappy_process_object_collision(struct Object *cappy, struct MarioState *m);
void cappy_start_return_to_mario(struct Object *cappy);
s32 cappy_return_to_mario(struct Object *cappy, struct MarioState *m);

/* Behaviors */
extern const BehaviorScript bhvCappy1[]; // Straight line
extern const BehaviorScript bhvCappy2[]; // Orbits around Mario
extern const BehaviorScript bhvCappy3[]; // Flies around Mario while Wing-cap flying
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

#endif // SMO_MARIO_CAPPY_H
