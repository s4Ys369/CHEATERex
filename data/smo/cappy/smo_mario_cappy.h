#ifndef SMO_MARIO_CAPPY_H
#define SMO_MARIO_CAPPY_H

#include "types.h"

enum {
	CAPPY_ACT_FLAG_GROUND		= 0,
	CAPPY_ACT_FLAG_AIRBORNE		= 1,
	CAPPY_ACT_FLAG_WATER		= 2,
	CAPPY_ACT_FLAG_METAL_WATER	= 3,
	CAPPY_ACT_FLAG_FLYING		= 4,
};

/* Cappy */
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

#endif // SMO_MARIO_CAPPY_H
