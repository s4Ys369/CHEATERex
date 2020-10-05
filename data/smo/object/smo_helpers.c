#include "../smo_c_includes.h"
#include <stdarg.h>

//
// Object stuff
//

struct Object *obj_get_first(struct ObjectNode *list) {
    struct Object *obj = (struct Object *) list->next;
    if (obj == (struct Object *) list) {
        return NULL;
    }
    return obj;
}

struct Object *obj_get_next(struct ObjectNode *list, struct Object *obj) {
    struct Object *next = (struct Object *) obj->header.next;
    if (next == (struct Object *) list) {
        return NULL;
    }
    return next;
}

struct Object *obj_get_first_with_behavior(struct Object *obj, const BehaviorScript *behavior) {
    struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behavior)];
    struct Object *next = (struct Object *) listHead->next;
    while (next != (struct Object *) listHead) {
        if (next->behavior == behavior && next->activeFlags != ACTIVE_FLAG_DEACTIVATED && next != obj) {
            return next;
        }
        next = (struct Object *) next->header.next;
    }
    return NULL;
}

struct Object *obj_get_first_child_with_behavior(struct Object *obj, const BehaviorScript *behavior) {
    struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behavior)];
    struct Object *next = (struct Object *) listHead->next;
    while (next != (struct Object *) listHead) {
        if (next->behavior == behavior && next->activeFlags != ACTIVE_FLAG_DEACTIVATED && next != obj && next->parentObj == obj) {
            return next;
        }
        next = (struct Object *) next->header.next;
    }
    return NULL;
}

struct Object *obj_get_nearest_with_behavior(struct Object *obj, const BehaviorScript *behavior) {
    f32 nearDist = 15000.f;
    struct Object *nearObj = NULL;
    struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behavior)];
    struct Object *next = (struct Object *) listHead->next;
    while (next != (struct Object *) listHead) {
        if (next->behavior == behavior) {
            if (next->activeFlags != ACTIVE_FLAG_DEACTIVATED && next != obj) {
                f32 dist = sqrtf(sqr(obj->oPosX - next->oPosX) + sqr(obj->oPosY - next->oPosY) + sqr(obj->oPosZ - next->oPosZ));
                if (dist < nearDist) {
                    nearObj = next;
                    nearDist = dist;
                }
            }
        }
        next = (struct Object *) next->header.next;
    }
    return nearObj;
}

u8 obj_get_list_index(struct Object *obj) {
    u8 cmd = ((obj->behavior[0] >> 24) & 0xFF);
    return (cmd == 0x00 ? (u8)((obj->behavior[0] >> 16) & 0xFF) : OBJ_LIST_DEFAULT);
}

s32 obj_is_surface(struct Object *obj) {
    u8 type = obj_get_list_index(obj);
    return
        type == OBJ_LIST_SURFACE ||
        type == OBJ_LIST_POLELIKE;
}

s32 obj_is_on_ground(struct Object *obj) {
    return (obj->oFloor != NULL) && (obj->oFloorHeight == 0);
}

s32 obj_is_underwater(struct Object *obj, f32 waterLevel) {
    return (obj->oPosY + (obj->hitboxHeight / 2) - obj->hitboxDownOffset) < waterLevel;
}

s16 obj_get_object1_angle_yaw_to_object2(struct Object *obj1, struct Object *obj2) {
    return atan2s(obj2->oPosZ - obj1->oPosZ, obj2->oPosX - obj1->oPosX);
}

s32 obj_is_object1_facing_object2(struct Object *obj1, struct Object *obj2, s16 angleRange) {
    s16 yawObj1ToObj2 = obj_get_object1_angle_yaw_to_object2(obj1, obj2);
    s32 yawDiff = (s32)((s16)((s32)(yawObj1ToObj2) - (s32)(obj1->oMoveAngleYaw)));
    return (-angleRange <= yawDiff) && (yawDiff <= angleRange);
}

s32 obj_is_object2_hit_from_above(struct Object *obj1, struct Object *obj2) {
    return (obj1->oVelY < 0.f) && (obj1->oPosY >= (obj2->oPosY + (obj2->hitboxHeight / 2) - obj2->hitboxDownOffset));
}

s32 obj_is_object2_hit_from_below(struct Object *obj1, struct Object *obj2) {
    return (obj1->oVelY > 0.f) && (obj2->oPosY >= (obj1->oPosY + (obj1->hitboxHeight / 2) - obj1->hitboxDownOffset));
}

struct Box { f32 radius; f32 height; };
static struct Box obj_get_box_for_overlap_check(struct Object *obj, u32 flags) {
    struct Box box;
    f32 hitboxRadius = obj->hitboxRadius;
    f32 hitboxHeight = obj->hitboxHeight;
    f32 hurtboxRadius = obj->hurtboxRadius;
    f32 hurtboxHeight = obj->hurtboxHeight;

    if (flags & OBJ_OVERLAP_FLAG_HITBOX_HURTBOX_MAX) {
        box.radius = MAX(hitboxRadius, hurtboxRadius);
        box.height = MAX(hitboxHeight, hurtboxHeight);
    } else if (flags & OBJ_OVERLAP_FLAG_HURTBOX_HITBOX_IF_ZERO) {
        box.radius = hurtboxRadius;
        box.height = hurtboxHeight;
        if (box.radius <= 0 || box.height <= 0) {
            box.radius = hitboxRadius;
            box.height = hitboxHeight;
        }
    } else if (flags & OBJ_OVERLAP_FLAG_HITBOX) {
        box.radius = hitboxRadius;
        box.height = hitboxHeight;
    } else { // No box by default
        box.radius = 0;
        box.height = 0;
    }
    return box;
}

s32 obj_detect_overlap(struct Object *obj1, struct Object *obj2, u32 obj1Flags, u32 obj2Flags) {
    // Tangibility check
    if ((obj1->oIntangibleTimer != 0 && !(obj1Flags & OBJ_OVERLAP_FLAG_IGNORE_INTANGIBLE)) ||
        (obj2->oIntangibleTimer != 0 && !(obj2Flags & OBJ_OVERLAP_FLAG_IGNORE_INTANGIBLE))) {
        return FALSE;
    }

    // Box dimensions check
    struct Box box1 = obj_get_box_for_overlap_check(obj1, obj1Flags);
    struct Box box2 = obj_get_box_for_overlap_check(obj2, obj2Flags);
    if (box1.radius == 0 || box1.height == 0 ||
        box2.radius == 0 || box2.height == 0) {
        return FALSE;
    }

    // Radius check
    f32 sumOfRadius2 = sqr(box1.radius + box2.radius);
    f32 distance2 = sqr(obj1->oPosX - obj2->oPosX) + sqr(obj1->oPosZ - obj2->oPosZ);
    if (distance2 > sumOfRadius2) {
        return FALSE;
    }

    // Height check
    f32 obj1LowerBound = obj1->oPosY - obj1->hitboxDownOffset;
    f32 obj1UpperBound = box1.height + obj1LowerBound;
    f32 obj2LowerBound = obj2->oPosY - obj2->hitboxDownOffset;
    f32 obj2UpperBound = box2.height + obj2LowerBound;
    f32 sumOfHeights   = box1.height + box2.height;
    if (((obj2UpperBound - obj1LowerBound) > sumOfHeights) ||
        ((obj1UpperBound - obj2LowerBound) > sumOfHeights)) {
        return FALSE;
    }

    return TRUE;
}

s32 obj_check_if_near_animation_end(struct Object *obj) {
    u32 spC = (s32) obj->header.gfx.unk38.curAnim->flags;
    s32 sp8 = obj->header.gfx.unk38.animFrame;
    s32 sp4 = obj->header.gfx.unk38.curAnim->unk08 - 2;
    s32 sp0 = FALSE;

    if (spC & 0x01) {
        if (sp4 + 1 == sp8) {
            sp0 = TRUE;
        }
    }

    if (sp8 == sp4) {
        sp0 = TRUE;
    }

    return sp0;
}

void obj_set_forward_vel(struct Object *obj, s16 yaw, f32 mag, f32 velMax) {
    obj->oVelX = sins(yaw) * mag * velMax;
    obj->oVelZ = coss(yaw) * mag * velMax;
    obj->oForwardVel = mag * velMax;
}

#define STEPS 32
#define Y_START_OFFSET 30.f
static s32 obj_process_surface_collisions(struct Object *obj, s32 moveThroughVCWalls, s32 onGround) {

    if (moveThroughVCWalls) {
        obj->activeFlags |= ACTIVE_FLAG_MOVE_THROUGH_GRATE;
    } else {
        obj->activeFlags &= ~ACTIVE_FLAG_MOVE_THROUGH_GRATE;
    }

    // Wall collision
    struct Surface *wall = NULL;
    f32 dy = MAX(0.f, obj->hitboxHeight - Y_START_OFFSET) / STEPS;
    for (s32 i = 0; i != STEPS; ++i) {
        struct WallCollisionData hitbox;
        hitbox.x = obj->oPosX;
        hitbox.y = obj->oPosY;
        hitbox.z = obj->oPosZ;
        hitbox.offsetY = Y_START_OFFSET + (dy * i) - obj->hitboxDownOffset;
        hitbox.radius = obj->oWallHitboxRadius;
        if (find_wall_collisions(&hitbox) != 0) {
            obj->oPosX = hitbox.x;
            obj->oPosY = hitbox.y;
            obj->oPosZ = hitbox.z;
            wall = hitbox.walls[0];
        }
    }

    // Floor collision
    struct Surface *floor = NULL;
    f32 floorY = find_floor(obj->oPosX, obj->oPosY - obj->hitboxDownOffset, obj->oPosZ, &floor) + obj->hitboxDownOffset;
    if (floor != NULL) {
        if (obj->oPosY <= floorY + (onGround ? 40.0f : 0.0f)) {
            obj->oPosY = floorY;
            obj->oVelY = MAX(obj->oVelY, 0.f);
        }
        obj->oFloor = floor;
        obj->oFloorHeight = obj->oPosY - floorY;
    } else {
        // Out of bounds
        obj->oVelX = 0;
        obj->oVelZ = 0;
        return FALSE;
    }
    
    // Ceiling collision
    struct Surface *ceil = NULL;
    f32 ceilY = find_ceil(obj->oPosX, obj->oPosY, obj->oPosZ, &ceil);
    if (ceil != NULL) {
        f32 h = (obj->hitboxHeight - obj->hitboxDownOffset);
        if (obj->oPosY <= ceilY && obj->oPosY + h > ceilY) {
            obj->oPosY = MAX(ceilY - h, floorY);
            obj->oVelY = MIN(obj->oVelY, 0.f);
            if (obj->oPosY == floorY) {
                // Treat floor + ceiling collision
                // as OOB to cancel out step
                return FALSE;
            }
            // Counts as a wall
            obj->oWall = ceil;
        }
    }

    // A solid wall is hit
    if (wall) {
        obj->oWall = wall;
    }
    return TRUE;
}
#undef STEPS
#undef Y_START_OFFSET

#define STEPS 8
void obj_update_pos_and_vel(struct Object *obj, s32 moveThroughVCWalls, s32 onGround, s8 *squishTimer) {
    obj->oFloor = NULL;
    obj->oWall = NULL;

    // Squish test
    if (squishTimer != NULL) {
        s32 squished = 0;
        struct Surface *floor = NULL;
        f32 floorY = find_floor(obj->oPosX, obj->oPosY - obj->hitboxDownOffset, obj->oPosZ, &floor) + obj->hitboxDownOffset;
        if (floor != NULL) {
            struct Surface *ceil = NULL;
            f32 ceilY = find_ceil(obj->oPosX, obj->oPosY, obj->oPosZ, &ceil);
            if (ceil != NULL) {
                f32 h = (obj->hitboxHeight - obj->hitboxDownOffset);
                if ((obj->oPosY <= ceilY) && (obj->oPosY + h > ceilY) && (ceilY - floorY < h)) {
                    squished = 1;
                }
            }
        }
        *squishTimer = squished * (*squishTimer + 1);
    }

    // Movement
    for (s32 i = 0; i != STEPS; ++i) {
        f32 x = obj->oPosX;
        f32 y = obj->oPosY;
        f32 z = obj->oPosZ;
        obj->oPosX += (obj->oVelX / STEPS);
        obj->oPosY += (obj->oVelY / STEPS);
        obj->oPosZ += (obj->oVelZ / STEPS);

        if (!obj_process_surface_collisions(obj, moveThroughVCWalls, onGround)) {
            // Out of bounds, cancel out step
            obj->oPosX = x;
            obj->oPosY = y;
            obj->oPosZ = z;
        } else {
            onGround = (obj_is_on_ground(obj) && obj->oVelY <= 0.f);
        }
    }

    obj->oHomeX = obj->oPosX;
    obj->oHomeY = obj->oPosY;
    obj->oHomeZ = obj->oPosZ;
    obj->oForwardVel = sqrtf(sqr(obj->oVelX) + sqr(obj->oVelZ));
}
#undef STEPS

void obj_update_blink_state(struct Object *obj, s32 *timer, s16 base, s16 range, s16 length) {
    if (*timer != 0) {
        (*timer)--;
    } else {
        *timer = base + (s16)(range * random_float());
    }

    if (*timer > length) {
        obj->oAnimState = 0;
    } else {
        obj->oAnimState = 1;
    }
}

void obj_random_blink(struct Object *obj, s32 *timer) {
    if (*timer == 0) {
        if ((s16)(random_float() * 100.0f) == 0) {
            obj->oAnimState = 1;
            *timer = 1;
        }
    } else {
        (*timer)++;
        if (*timer >= 6) {
            obj->oAnimState = 0;
        }
        if (*timer >= 11) {
            obj->oAnimState = 1;
        }
        if (*timer >= 16) {
            obj->oAnimState = 0;
            *timer = 0;
        }
    }
}

static void bhv_fire_smoke_init(void) {
    cur_obj_scale(random_float() * 2 + 4.0);
}

static const BehaviorScript bhvFireSmoke[] = {
    0x00060000,
    0x11010001,
    0x21000000,
    0x0C000000, (uintptr_t) bhv_fire_smoke_init,
    0x101AFFFF,
    0x0500000A,
    0x0F1A0001,
    0x06000000,
    0x1D000000,
};

void obj_make_step_sound_and_particle(struct Object *obj, f32 *dist, f32 distMin, f32 distInc, s32 soundBits, u32 particles) {
    *dist += distInc;
    if (*dist >= distMin) {
        *dist -= distMin;

        // Particles
        if (particles & OBJ_STEP_PARTICLE_MIST) {
            spawn_object(obj, MODEL_NONE, bhvMistParticleSpawner);
        }
        if (particles & OBJ_STEP_PARTICLE_SMOKE) {
            spawn_object(obj, MODEL_SMOKE, bhvWhitePuffSmoke);
        }
        if (particles & OBJ_STEP_PARTICLE_WATER_TRAIL) {
            spawn_object(obj, MODEL_WAVE_TRAIL, bhvObjectWaveTrail);
        }
        if (particles & OBJ_STEP_PARTICLE_WATER_DROPLET) {
            for (s32 i = 0; i != 2; ++i) {
                struct Object *drop = spawn_object_with_scale(obj, MODEL_WHITE_PARTICLE_SMALL, bhvWaterDroplet, 1.5f);
                drop->oVelY = random_float() * 30.0f;
                obj_translate_xz_random(drop, 110.0f);
            }
        }
        if (particles & OBJ_STEP_PARTICLE_FIRE) {
            spawn_object(obj, MODEL_RED_FLAME, bhvFireSmoke);
        }
        if (particles & OBJ_STEP_PARTICLE_FLAME) {
            for (s32 i = 0; i != 2; ++i) {
                spawn_object(obj, MODEL_RED_FLAME, bhvKoopaShellFlame);
            }
        }

        // Sound effect
        if (soundBits != -1) {
            play_sound(soundBits, obj->header.gfx.cameraToObject);
        }
    }
}

void obj_set_animation_with_accel(struct Object *obj, s32 animIndex, f32 accel) {
    struct Animation **anims = obj->oAnimations;
    s32 animAccel = (s32)(accel * 65536.0f);
    geo_obj_init_animation_accel(&obj->header.gfx, &anims[animIndex], animAccel);
    obj->oSoundStateID = animIndex;
}

void obj_spawn_white_puff(struct Object *obj, s32 soundBits) {
    return obj_spawn_white_puff_at(obj->oPosX, obj->oPosY, obj->oPosZ, soundBits);
}

void obj_spawn_white_puff_at(f32 x, f32 y, f32 z, s32 soundBits) {
    for (s32 i = 0; i < 16; ++i) {
        struct Object *particle = spawn_object(gMarioObject, MODEL_MIST, bhvWhitePuffExplosion);
        if (particle != NULL) {
            particle->oPosX = x;
            particle->oPosY = y;
            particle->oPosZ = z;
            particle->oBehParams2ndByte = 2;
            particle->oMoveAngleYaw = random_u16();
            particle->oGravity = -4.f;
            particle->oDragStrength = 30.f;
            particle->oForwardVel = random_float() * 5.f + 40.f;
            particle->oVelY = random_float() * 20.f + 30.f;
            f32 scale = random_float() * 0.25f + 4.6f;
            obj_scale_xyz(particle, scale, scale, scale);
        }
    }
    if (soundBits != -1) {
        struct Object *sound = spawn_object(gMarioObject, 0, bhvSoundSpawner);
        if (sound != NULL) {
            sound->oPosX = x;
            sound->oPosY = y;
            sound->oPosZ = z;
            if (soundBits == 0) {
                sound->oSoundEffectUnkF4 = SOUND_OBJ_DEFAULT_DEATH;
            } else {
                sound->oSoundEffectUnkF4 = soundBits;
            }
        }
    }
}

void obj_spawn_triangle_break_particles(struct Object *obj, s32 numTris, s16 triModel, f32 triSize, s16 triAnimState) {
    for (s32 i = 0; i < numTris; i++) {
        struct Object *triangle = spawn_object(obj, triModel, bhvBreakBoxTriangle);
        if (triangle != NULL) {
            triangle->oAnimState = triAnimState;
            triangle->oPosY += 100.0f;
            triangle->oMoveAngleYaw = random_u16();
            triangle->oFaceAngleYaw = triangle->oMoveAngleYaw;
            triangle->oFaceAnglePitch = random_u16();
            triangle->oVelY = random_f32_around_zero(50.0f);
            if (triModel == 138 || triModel == 56) {
                triangle->oAngleVelPitch = 0xF00;
                triangle->oAngleVelYaw = 0x500;
                triangle->oForwardVel = 30.0f;
            } else {
                triangle->oAngleVelPitch = 0x80 * (s32)(random_float() + 50.0f);
                triangle->oForwardVel = 30.0f;
            }
            obj_scale(triangle, triSize);
        }
    }
}

void obj_instant_death(struct Object *obj) {
    // Spawn coins
    if (obj->oNumLootCoins < 0) {
        spawn_object(obj, MODEL_BLUE_COIN, bhvMrIBlueCoin);
    } else {
        obj_spawn_loot_yellow_coins(obj, obj->oNumLootCoins, 20.0f);
    }

    // ded
    obj_spawn_white_puff(obj, obj->oDeathSound);
    obj_mark_for_deletion(obj);
}

void obj_explode(struct Object *obj, s32 soundBits) {
    // Spawn coins
    if (obj->oNumLootCoins < 0) {
        spawn_object(obj, MODEL_BLUE_COIN, bhvMrIBlueCoin);
    } else {
        obj_spawn_loot_yellow_coins(obj, obj->oNumLootCoins, 20.0f);
    }

    // explode
    obj_spawn_white_puff(obj, soundBits);
    obj_spawn_triangle_break_particles(obj, 30, 138, 3.0f, 4);
    obj_mark_for_deletion(obj);
}

void obj_update_gfx(struct Object *obj) {
    obj->header.gfx.pos[0] = obj->oPosX;
    obj->header.gfx.pos[1] = obj->oPosY + obj->oGraphYOffset;
    obj->header.gfx.pos[2] = obj->oPosZ;
    obj->header.gfx.angle[0] = obj->oFaceAnglePitch & 0xFFFF;
    obj->header.gfx.angle[1] = obj->oFaceAngleYaw   & 0xFFFF;
    obj->header.gfx.angle[2] = obj->oFaceAngleRoll  & 0xFFFF;
}

void vec3f_rotate_zxy(Vec3f v, s16 pitch, s16 yaw, s16 roll) {
    Vec3f t = { 0, 0, 0 };
    Vec3s r = { pitch, yaw, roll };
    Mat4 mtx;
    mtxf_rotate_zxy_and_translate(mtx, t, r);
    f32 x = v[0];
    f32 y = v[1];
    f32 z = v[2];
    v[0] = x * mtx[0][0] + y * mtx[1][0] + z * mtx[2][0] + mtx[3][0];
    v[1] = x * mtx[0][1] + y * mtx[1][1] + z * mtx[2][1] + mtx[3][1];
    v[2] = x * mtx[0][2] + y * mtx[1][2] + z * mtx[2][2] + mtx[3][2];
}