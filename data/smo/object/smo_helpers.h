#ifndef SMO_HELPERS_H
#define SMO_HELPERS_H

#include "types.h"

/* Flags */
#define OBJ_OVERLAP_FLAG_HITBOX					(1 << 0)
#define OBJ_OVERLAP_FLAG_HURTBOX_HITBOX_IF_ZERO	(1 << 1)
#define OBJ_OVERLAP_FLAG_HITBOX_HURTBOX_MAX		(1 << 2)
#define OBJ_OVERLAP_FLAG_IGNORE_INTANGIBLE		(1 << 3)

/* Particles */
#define OBJ_STEP_PARTICLE_NONE				0
#define OBJ_STEP_PARTICLE_MIST				(1 << 0)
#define OBJ_STEP_PARTICLE_SMOKE				(1 << 1)
#define OBJ_STEP_PARTICLE_WATER_TRAIL		(1 << 2)
#define OBJ_STEP_PARTICLE_WATER_DROPLET		(1 << 3)
#define OBJ_STEP_PARTICLE_FIRE				(1 << 4)
#define OBJ_STEP_PARTICLE_FLAME				(1 << 5)

/* Object stuff */
struct Object *obj_get_first(struct ObjectNode *list);
struct Object *obj_get_next(struct ObjectNode *list, struct Object *obj);
struct Object *obj_get_first_with_behavior(struct Object *obj, const BehaviorScript *behavior);
struct Object *obj_get_first_child_with_behavior(struct Object *obj, const BehaviorScript *behavior);
struct Object *obj_get_nearest_with_behavior(struct Object *obj, const BehaviorScript *behavior);
u8   obj_get_list_index(struct Object *obj);
s32  obj_is_surface(struct Object *obj);
s32  obj_is_on_ground(struct Object *obj);
s32  obj_is_underwater(struct Object *obj, f32 waterLevel);
s16  obj_get_object1_angle_yaw_to_object2(struct Object *obj1, struct Object *obj2);
s32  obj_is_object1_facing_object2(struct Object *obj1, struct Object *obj2, s16 angleRange);
s32  obj_is_object2_hit_from_above(struct Object *obj1, struct Object *obj2);
s32  obj_is_object2_hit_from_below(struct Object *obj1, struct Object *obj2);
s32  obj_detect_overlap(struct Object *obj1, struct Object *obj2, u32 obj1Flags, u32 obj2Flags);
s32  obj_check_if_near_animation_end(struct Object *obj);
void obj_set_forward_vel(struct Object *obj, s16 yaw, f32 mag, f32 velMax);
void obj_update_pos_and_vel(struct Object *obj, s32 moveThroughVCWalls, s32 onGround, s8 *squishTimer);
void obj_update_blink_state(struct Object *obj, s32 *timer, s16 base, s16 range, s16 length);
void obj_random_blink(struct Object *obj, s32 *timer);
void obj_make_step_sound_and_particle(struct Object *obj, f32 *dist, f32 distMin, f32 distInc, s32 soundBits, u32 particles);
void obj_set_animation_with_accel(struct Object *obj, s32 animIndex, f32 accel);
void obj_spawn_white_puff(struct Object *obj, s32 soundBits);
void obj_spawn_white_puff_at(f32 x, f32 y, f32 z, s32 soundBits);
void obj_spawn_triangle_break_particles(struct Object* obj, s32 numTris, s16 triModel, f32 triSize, s16 triAnimState);
void obj_instant_death(struct Object *obj);
void obj_explode(struct Object *obj, s32 soundBits);
void obj_update_gfx(struct Object *obj);
void vec3f_rotate_zxy(Vec3f v, s16 pitch, s16 yaw, s16 roll);

#endif // SMO_HELPERS_H
