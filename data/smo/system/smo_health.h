#ifndef SMO_HEALTH_H
#define SMO_HEALTH_H

#include "types.h"

s32 smo_get_max_hp(s32 hp);
s32 smo_get_num_segments_per_health_gauge();
s32 smo_get_hp_per_segment();
f32 smo_get_life_up_gauge_position(s32 hp);
s32 smo_is_mario_losing_hp(struct MarioState *m);

s32 smo_update_mario_health(struct MarioState *m);
void smo_heal_mario_for_1_health(struct MarioState *m);
void smo_set_mario_to_full_health(struct MarioState *m);
void smo_set_hp_counter_to_mario_health(struct MarioState *m);
void smo_life_up_mario(struct MarioState *m);
void smo_health_reset();

#endif // SMO_HEALTH_H
