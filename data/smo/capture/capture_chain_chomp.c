#include "../smo_c_includes.h"
#include "actors/group14.h"

//
// Chain Chomp parts
// (Code from chain_chomp.inc.c, adapted to the situation)
//

static void chain_chomp_update_chain_parts(struct Object* obj, s32 isFreed) {
    struct Object *pivot = obj->parentObj;

    // Segment 0 connects the pivot to the chain chomp itself
    struct ChainSegment *seg0 = &obj->oChainChompSegments[0];
    seg0->posX = obj->oPosX - pivot->oPosX;
    seg0->posY = obj->oPosY - pivot->oPosY;
    seg0->posZ = obj->oPosZ - pivot->oPosZ;
    obj->oChainChompDistToPivot = sqrtf(sqr(seg0->posX) + sqr(seg0->posY) + sqr(seg0->posZ));

    // If the chain is fully stretched
    f32 maxDistToPivot = obj->oChainChompMaxDistFromPivotPerChainPart * 5.f;
    if (obj->oChainChompDistToPivot > maxDistToPivot) {
        f32 ratio = maxDistToPivot / obj->oChainChompDistToPivot;
        obj->oChainChompDistToPivot = maxDistToPivot;
        seg0->posX *= ratio;
        seg0->posY *= ratio;
        seg0->posZ *= ratio;

        // If freed, move pivot like the chain chomp is pulling it along
        if (isFreed) {
            pivot->oVelY -= 4.f;
            pivot->oPosX = obj->oPosX - seg0->posX;
            pivot->oPosZ = obj->oPosZ - seg0->posZ;
            pivot->oPosY += pivot->oVelY;
            struct Surface *floor = NULL;
            f32 floorY = find_floor(pivot->oPosX, pivot->oPosY, pivot->oPosZ, &floor);
            if (floor != NULL) {
                if (pivot->oPosY <= floorY) {
                    pivot->oPosY = floorY;
                    pivot->oVelY = 0.f;
                }
            } else {
                pivot->oPosY = obj->oPosY;
                pivot->oVelY = 0.f;
            }
        }
    }

    // Segments 1+ connect the pivot to chain part i
    // 1 is closest to the chain chomp
    // 4 is closest to the pivot
    for (s32 i = 1; i <= 4; i++) {
        struct ChainSegment *prevSegment = &obj->oChainChompSegments[i - 1];
        struct ChainSegment *segment = &obj->oChainChompSegments[i];

        // Apply gravity (using pitch for velY)
        segment->pitch -= 4.f;
        segment->posY += segment->pitch;
        f32 sx = segment->posX + pivot->oPosX;
        f32 sy = segment->posY + pivot->oPosY;
        f32 sz = segment->posZ + pivot->oPosZ;
        struct Surface *floor = NULL;
        f32 floorY = find_floor(sx, sy, sz, &floor);
        if (floor != NULL) {
            if (sy <= floorY) {
                segment->posY = floorY - pivot->oPosY;
                segment->pitch = 0;
            }
        } else {
            floorY = find_floor(sx, pivot->oPosY, sz, &floor);
            if (floor != NULL) {
                if (sy <= floorY) {
                    segment->posY = floorY - pivot->oPosY;
                    segment->pitch = 0;
                }
            } else {
                segment->posY = 0;
                segment->pitch = 0;
            }
        }

        // Distance to previous chain part (so that the tail follows the chomp)
        f32 offsetX = segment->posX - prevSegment->posX;
        f32 offsetY = segment->posY - prevSegment->posY;
        f32 offsetZ = segment->posZ - prevSegment->posZ;
        f32 offset = sqrtf(sqr(offsetX) + sqr(offsetY) + sqr(offsetZ));
        if (offset > obj->oChainChompMaxDistFromPivotPerChainPart) {
            offset = obj->oChainChompMaxDistFromPivotPerChainPart / offset;
            offsetX *= offset;
            offsetY *= offset;
            offsetZ *= offset;
        }

        // Distance to pivot (so that it stretches when the chomp moves far from the wooden post)
        offsetX += prevSegment->posX;
        offsetY += prevSegment->posY;
        offsetZ += prevSegment->posZ;
        offset = sqrtf(sqr(offsetX) + sqr(offsetY) + sqr(offsetZ));
        f32 maxTotalOffset = obj->oChainChompMaxDistFromPivotPerChainPart * (5 - i);
        if (offset > maxTotalOffset) {
            offset = maxTotalOffset / offset;
            offsetX *= offset;
            offsetY *= offset;
            offsetZ *= offset;
        }

        // Segment final pos
        segment->posX = offsetX;
        segment->posY = offsetY;
        segment->posZ = offsetZ;
    }
}

//
// Freed Chain Chomp
//

enum {
    CHAIN_CHOMP_FREED_STATE_WANDERING,
    CHAIN_CHOMP_FREED_STATE_CHASING_MARIO,
};

static struct ObjectHitbox sChainChompFreedHitbox = {
    /* interactType: */ INTERACT_MR_BLIZZARD,
    /* downOffset: */ 0,
    /* damageOrCoinValue: */ 4,
    /* health: */ 1,
    /* numLootCoins: */ 0,
    /* radius: */ 80,
    /* height: */ 160,
    /* hurtboxRadius: */ 80,
    /* hurtboxHeight: */ 160,
};

static void bhv_chain_chomp_freed_init() {
    gCurrentObject->oFlags = (OBJ_FLAG_COMPUTE_ANGLE_TO_MARIO | OBJ_FLAG_ACTIVE_FROM_AFAR | OBJ_FLAG_COMPUTE_DIST_TO_MARIO | OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW | OBJ_FLAG_UPDATE_GFX_POS_AND_ANGLE);
    gCurrentObject->oGraphYOffset = 240.f;
    gCurrentObject->oWallHitboxRadius = 120.f;
    gCurrentObject->oGravity = -4.f;
    gCurrentObject->oBounciness = -0.5f;
    gCurrentObject->oFriction = 10.f;
    gCurrentObject->oBuoyancy = 2.f;
    gCurrentObject->oChainChompMaxDistFromPivotPerChainPart = 750.f / 5.f;
    gCurrentObject->oChainChompFreedState = CHAIN_CHOMP_FREED_STATE_WANDERING;
    gCurrentObject->oChainChompFreedTimer = 0;
    gCurrentObject->oChainChompFreedAngle = 0;
    gCurrentObject->header.gfx.scale[0] = 2.f;
    gCurrentObject->header.gfx.scale[1] = 2.f;
    gCurrentObject->header.gfx.scale[2] = 2.f;
    obj_set_hitbox(gCurrentObject, &sChainChompFreedHitbox);
    cur_obj_set_pos_to_home();
}

static s32 chain_chomp_freed_is_ready(struct Object *obj) {
    if (obj->oChainChompFreedTimer > 0) {
        obj->oVelX *= 0.90f;
        obj->oVelZ *= 0.90f;
        obj->oChainChompFreedTimer--;
        return FALSE;
    }
    return TRUE;
}

static void bhv_chain_chomp_freed_wander(struct Object* obj) {
    if (!chain_chomp_freed_is_ready(obj)) {
        return;
    }

    if (obj_is_on_ground(obj)) {
        if (obj->oDistanceToMario <= 1000.f) {

            // Start chasing Mario
            obj->oVelX = 0;
            obj->oVelY = 48.f;
            obj->oVelZ = 0;
            obj->oForwardVel = 0;
            obj->oFaceAngleYaw = obj->oAngleToMario;
            obj->oMoveAngleYaw = obj->oAngleToMario;
            obj->oFloor = NULL;
            obj->oChainChompFreedTimer = 45;
            obj->oChainChompFreedState = CHAIN_CHOMP_FREED_STATE_CHASING_MARIO;
            play_sound(SOUND_GENERAL_CHAIN_CHOMP2, obj->header.gfx.cameraToObject);
        } else {

            // Make a small hop
            s16 nextAngle = (s16)(obj->oChainChompFreedAngle + (random_u16() & 0x3FFF) - 0x2000);
            obj_set_forward_vel(obj, nextAngle, 1.f, 12.f);
            obj->oVelY = 36.f;
            obj->oFloor = NULL;
            obj->oFaceAngleYaw = nextAngle;
            obj->oMoveAngleYaw = nextAngle;
            obj->oChainChompFreedAngle = nextAngle;
            play_sound(SOUND_GENERAL_CHAIN_CHOMP1, obj->header.gfx.cameraToObject);
        }
    }
}

static void bhv_chain_chomp_freed_chase(struct Object *obj) {
    obj->oFaceAngleYaw = obj->oAngleToMario;
    obj->oMoveAngleYaw = obj->oAngleToMario;

    if (obj_is_on_ground(obj)) {
        if (obj->oDistanceToMario > 1500.f) {

            // Stop chasing Mario
            obj->oChainChompFreedTimer = 30;
            obj->oChainChompFreedState = CHAIN_CHOMP_FREED_STATE_WANDERING;
        } else {

            // Make a fast hop
            obj_set_forward_vel(obj, obj->oAngleToMario, 1.f, 28.f);
            obj->oVelY = 20.f;
            obj->oFloor = NULL;
            obj->oChainChompFreedAngle = obj->oAngleToMario;
            play_sound(SOUND_GENERAL_CHAIN_CHOMP1, obj->header.gfx.cameraToObject);
        }
    }
}

static void bhv_chain_chomp_freed_update() {
    switch (gCurrentObject->oChainChompFreedState) {
        case CHAIN_CHOMP_FREED_STATE_WANDERING:
            bhv_chain_chomp_freed_wander(gCurrentObject);
            break;

        case CHAIN_CHOMP_FREED_STATE_CHASING_MARIO:
            bhv_chain_chomp_freed_chase(gCurrentObject);
            break;
    }

    if (gCurrentObject->oInteractStatus & (INT_STATUS_INTERACTED | INT_STATUS_ATTACKED_MARIO)) {
        gCurrentObject->oChainChompFreedTimer = 90;
        gCurrentObject->oChainChompFreedState = CHAIN_CHOMP_FREED_STATE_WANDERING;
        gCurrentObject->oInteractStatus = 0;
    }

    gCurrentObject->oVelY = MAX(gCurrentObject->oVelY + gCurrentObject->oGravity, -75.f);
    obj_update_pos_and_vel(gCurrentObject, FALSE, obj_is_on_ground(gCurrentObject), NULL);
    obj_set_animation_with_accel(gCurrentObject, 0, 1.f);
    chain_chomp_update_chain_parts(gCurrentObject, TRUE);
}

const BehaviorScript bhvChainChompFreed[] = {
    0x00040000,
    0x1E000000,
    0x27260000,
    (uintptr_t) chain_chomp_seg6_anims_06025178,
    0x28000000,
    0x2D000000,
    0x0C000000,
    (uintptr_t) bhv_chain_chomp_freed_init,
    0x08000000,
    0x0C000000,
    (uintptr_t) bhv_chain_chomp_freed_update,
    0x09000000,
};

//
// Possessed Chain Chomp
//

static void cappy_chain_chomp_set_properties(struct PObject *pobj) {
    pobj->hitboxRadius = pobj->cdHitboxRadius;
    pobj->hitboxHeight = pobj->cdHitboxHeight;
    pobj->hitboxDownOffset = pobj->cdHitboxDownOffset;
    pobj->oWallHitboxRadius = pobj->cdWallHitboxRadius;
    pobj->oProperties = 0;
    pobj->oProperties |= POBJ_PROP_ABOVE_WATER;
    pobj->oProperties |= POBJ_PROP_INVULNERABLE;
    pobj->oProperties |= POBJ_PROP_UNPUSHABLE;
    pobj->oProperties |= POBJ_PROP_IMMUNE_TO_FIRE;
    pobj->oProperties |= POBJ_PROP_IMMUNE_TO_LAVA;
    pobj->oProperties |= POBJ_PROP_IMMUNE_TO_QUICKSANDS;
    pobj->oProperties |= POBJ_PROP_RESIST_STRONG_WINDS;
    pobj->oProperties |= POBJ_PROP_ATTACK;
    if (pobj->oCappyChainChompIsBiting) {
        pobj->oProperties |= POBJ_PROP_STRONG_ATTACK;
    }
}

static void cappy_chain_chomp_process_inputs(struct PObject *pobj) {
    pobj_decelerate(pobj);
    pobj_apply_gravity(pobj);

    // Inputs
    if (pobj->oUnresponsiveTimer == 0) {
        pobj->oCappyChainChompIsBiting = FALSE;

        // Walk
        f32 hvel = pobj->cdWalkSpeed;
        if (pobj->oStickMag > 0) {
            s32 faceYaw = pobj->oStickYaw - approach_s32((s16)(pobj->oStickYaw - pobj->oFaceAngleYaw), 0, 0x1000, 0x1000);
            obj_set_forward_vel(pobj, faceYaw, pobj->oStickMag, hvel);
            pobj->oFaceAngleYaw = faceYaw;
            pobj->oFaceAnglePitch = 0;
            pobj->oFaceAngleRoll = 0;
            pobj->oMoveAngleYaw = faceYaw;
            pobj->oMoveAnglePitch = 0;
            pobj->oMoveAngleRoll = 0;
        }

        // Jump
        if (obj_is_on_ground(pobj)) {
            if (pobj->oButtonDown & A_BUTTON) {
                pobj->oVelY = pobj->cdJumpVelocity * 1.5f;
                pobj->oFloor = NULL;
                play_sound(SOUND_OBJ_GOOMBA_ALERT, pobj->header.gfx.cameraToObject);
                play_sound(SOUND_GENERAL_CHAIN_CHOMP1, pobj->header.gfx.cameraToObject);
            } else if (pobj->oForwardVel > pobj->cdWalkSpeed / 4.f) {
                pobj->oVelY = pobj->cdJumpVelocity;
                pobj->oFloor = NULL;
                play_sound(SOUND_GENERAL_CHAIN_CHOMP1, pobj->header.gfx.cameraToObject);
            }
        }

        // Bite
        if (pobj->oButtonPressed & B_BUTTON) {
            obj_set_forward_vel(pobj, pobj->oFaceAngleYaw, 1.f, pobj->cdDashSpeed);
            pobj->oUnresponsiveTimer = 30;
            play_sound(SOUND_GENERAL_CHAIN_CHOMP2, pobj->header.gfx.cameraToObject);
            pobj->oCappyChainChompIsBiting = TRUE;
        }
    } else {
        pobj->oUnresponsiveTimer--;
    }
}

struct Object *get_chain_chomp_wooden_post(struct PObject *pobj) {
    struct ObjectNode *list = &gObjectLists[OBJ_LIST_SURFACE];
    struct Object *obj = obj_get_first(list);
    while (obj != NULL) {
        if (obj->behavior == bhvWoodenPost && obj->parentObj == pobj) {
            return obj;
        }
        obj = obj_get_next(list, obj);
    }
    return NULL;
}

static void cappy_chain_chomp_break_free(struct PObject *pobj) {
    struct Object *obj = get_chain_chomp_wooden_post(pobj);
    if (obj != NULL) {
        set_camera_shake_from_point(SHAKE_POS_SMALL, pobj->oPosX, pobj->oPosY, pobj->oPosZ);
        obj_spawn_white_puff(obj, SOUND_GENERAL_WALL_EXPLOSION);
        obj_spawn_triangle_break_particles(obj, 24, 0x8A, 2.0f, 4);
        obj_mark_for_deletion(obj);
        pobj->oCappyChainChompIsFreed = TRUE;
    }
}

static void cappy_chain_chomp_update_pos(struct PObject *pobj) {
    obj_update_pos_and_vel(pobj, pobj->oProperties & POBJ_PROP_MOVE_THROUGH_VC_WALLS, obj_is_on_ground(pobj), &pobj->oSquishedTimer);
    pobj_handle_special_floors(pobj);

    // If not freed, restrict position
    if (!pobj->oCappyChainChompIsFreed) {
        f32 dx = pobj->oPosX - pobj->oCappyChainChompPivotX;
        f32 dz = pobj->oPosZ - pobj->oCappyChainChompPivotZ;
        f32 dist = sqrtf(sqr(dx) + sqr(dz));
        if (dist > 750.f) {

            // Break free if enough speed
            if (pobj->oForwardVel > 60.f) { 
                cappy_chain_chomp_break_free(pobj);
            } else {
                dx *= (750.f / dist);
                dz *= (750.f / dist);
                pobj->oPosX = pobj->oCappyChainChompPivotX + dx;
                pobj->oPosZ = pobj->oCappyChainChompPivotZ + dz;
                pobj->oHomeX = pobj->oPosX;
                pobj->oHomeZ = pobj->oPosZ;
            }
        }
    }

    // Break chain chomp gate if hit strong enough
    if (pobj->oProperties & POBJ_PROP_STRONG_ATTACK) {
        if (pobj->oWall && pobj->oWall->object && pobj->oWall->object->behavior == bhvChainChompGate) {
            pobj->oChainChompHitGate = TRUE;
            play_puzzle_jingle();
        }
    }
}

static void cappy_chain_chomp_process_interactions(struct PObject *pobj) {
    POBJ_INTERACTIONS_BEGIN;
    POBJ_INTERACTIONS_END;
}

static void cappy_chain_chomp_update_animation_and_sound(struct PObject *pobj) {
    obj_set_animation_with_accel(pobj, 0, (pobj->oCappyChainChompIsBiting ? 2.f : 1.f));
}

static void cappy_chain_chomp_update_chain_parts(struct PObject *pobj) {
    chain_chomp_update_chain_parts(pobj, pobj->oCappyChainChompIsFreed);
}

static void cappy_chain_chomp_set_cappy_values(struct PObject *pobj) {
    pobj->oCappyVerticalDist = 245.f;
    pobj->oCappyInitialScale = 200;
}

//
// Main loop
//

const PObjMainLoopFunc cappy_chain_chomp_mloop[] = {
    cappy_chain_chomp_process_inputs,
    cappy_chain_chomp_set_properties,
    cappy_chain_chomp_update_pos,
    cappy_chain_chomp_process_interactions,
    obj_update_gfx,
    cappy_chain_chomp_update_animation_and_sound,
    cappy_chain_chomp_update_chain_parts,
    cappy_chain_chomp_set_cappy_values,
    NULL
};

//
// Init
//

s32 cappy_chain_chomp_init(struct Object* obj) {
    if (obj->behavior == bhvChainChomp) {
        obj->oCappyChainChompPivotX = obj->oHomeX;
        obj->oCappyChainChompPivotZ = obj->oHomeZ;
        obj->oCappyChainChompIsFreed = FALSE;
    } else { // Freed
        obj->oCappyChainChompIsFreed = TRUE;
    }
    obj->oCappyChainChompIsBiting = FALSE;
    obj->oChainChompMaxDistFromPivotPerChainPart = 750.f / 5.f;
    return TRUE;
}

void cappy_chain_chomp_end(struct Object *obj) {
    if (obj->behavior == bhvChainChomp) { 
        if (obj->oCappyChainChompIsFreed) { // If freed, sets up a new behavior
            obj->behavior = bhvChainChompFreed;
            obj->curBhvCommand = bhvChainChompFreed;
            obj->bhvStackIndex = 0;
        } else {
            obj->oHomeX = obj->oCappyChainChompPivotX;
            obj->oHomeZ = obj->oCappyChainChompPivotZ;
        }
    }
    obj->oWallHitboxRadius = 0.f;
}

f32 cappy_chain_chomp_get_top(struct Object *obj) {
    return 245.f * obj->oScaleY;
}
