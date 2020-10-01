#include "../smo_c_includes.h"
#include "actors/group12.h"
#include "data/utils.h"

//
// Attack
//

static void cappy_whomp_shock_wave_attack(struct Object *this) {

    // Surfaces
    f32 dy = this->hitboxHeight / 8;
    for (s32 i = 0; i <= 8; ++i) {
        struct WallCollisionData hitbox;
        hitbox.x = this->oPosX;
        hitbox.y = this->oPosY;
        hitbox.z = this->oPosZ;
        hitbox.offsetY = (dy * i) - this->hitboxDownOffset;
        hitbox.radius = this->hitboxRadius;
        if (find_wall_collisions(&hitbox) != 0) {
            for (s16 j = 0; j != hitbox.numWalls; ++j) {
                struct Object *obj = hitbox.walls[j]->object;
                if (obj && !(obj->oInteractStatus & INT_STATUS_INTERACTED)) {

                    // Lying Whomp
                    if (obj->behavior == bhvSmallWhomp && obj->oAction == 6) {
                        obj->oNumLootCoins = 5;
                        obj_explode(obj, SOUND_OBJ_THWOMP);
                        obj->oInteractStatus = INT_STATUS_INTERACTED;
                    }

                    // Wall behind WF tower
                    if (obj->behavior == bhvTowerDoor) {
                        obj_explode(obj, SOUND_GENERAL_WALL_EXPLOSION);
                        obj->oInteractStatus = INT_STATUS_INTERACTED;
                    }

                    // Kickable board
                    if (obj->behavior == bhvKickableBoard) {
                        obj->oAction = 2;
                        obj->oInteractStatus = INT_STATUS_INTERACTED;
                    }

                    // WF coin cannon wall
                    if (obj->behavior == bhvWfBreakableWallLeft) {
                        obj->oNumLootCoins = 1;
                        obj_explode(obj, SOUND_GENERAL_WALL_EXPLOSION);
                        obj->oInteractStatus = INT_STATUS_INTERACTED;
                    }

                    // WF star cannon wall
                    if (obj->behavior == bhvWfBreakableWallRight) {
                        play_puzzle_jingle();
                        obj_explode(obj, SOUND_GENERAL_WALL_EXPLOSION);
                        obj->oInteractStatus = INT_STATUS_INTERACTED;
                    }
                }
            }
        }
    }

    // Objects
    for (u32 objType = OBJ_LIST_DESTRUCTIVE; objType <= OBJ_LIST_POLELIKE; ++objType) {
        struct ObjectNode *list = &gObjectLists[objType];
        struct Object *obj = obj_get_first(list);
        while (obj != NULL) {
            if (obj != this) {
                if (obj_detect_overlap(this, obj, OBJ_OVERLAP_FLAG_HITBOX, OBJ_OVERLAP_FLAG_HITBOX)) {
                    switch (obj->oInteractType) {
                        case INTERACT_KOOPA:
                        case INTERACT_BOUNCE_TOP:
                        case INTERACT_BOUNCE_TOP2:
                        case INTERACT_HIT_FROM_BELOW: {
                            obj->oInteractStatus = (0x02 | INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED);
                        } break;

                        case INTERACT_BULLY: {
                            f32 knockback = 3600.f / obj->hitboxRadius;
                            s16 angle = obj_get_object1_angle_yaw_to_object2(this, obj);
                            obj->oFaceAngleYaw = angle + 0x8000;
                            obj->oMoveAngleYaw = angle;
                            obj->oForwardVel = knockback;
                            obj->oInteractStatus = (0x02 | INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED);
                            play_sound(SOUND_OBJ_BULLY_METAL, obj->header.gfx.cameraToObject);
                        } break;

                        case INTERACT_BREAKABLE: {
                            obj->oInteractStatus = (0x02 | INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED);
                            set_camera_shake_from_hit(SHAKE_ATTACK);
                        } break;

                        case INTERACT_GRABBABLE: {
                            if (obj->oInteractionSubtype & INT_SUBTYPE_KICKABLE) {
                                obj->oInteractStatus = (0x02 | INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED);
                            }
                            if (obj->behavior == bhvBreakableBoxSmall) {
                                obj->oNumLootCoins = 3;
                                obj_spawn_white_puff(obj, SOUND_GENERAL_BREAK_BOX);
                                obj_spawn_triangle_break_particles(obj, 20, 138, 0.7f, 3);
                                obj_spawn_loot_yellow_coins(obj, 3, 20.0f);
                                obj_mark_for_deletion(obj);
                            }
                        } break;
                    }
                }
            }
            obj = obj_get_next(list, obj);
        }
    }
}

#define CAPPY_WHOMP_SHOCK_WAVE_MAX_DURATION  30

void bhv_cappy_whomp_shock_wave_loop(void) {
    f32 t = (f32) gCurrentObject->oTimer / (f32) CAPPY_WHOMP_SHOCK_WAVE_MAX_DURATION;
    if (t >= 1.f) {
        obj_mark_for_deletion(gCurrentObject);
        return;
    }

    f32 scale = 30.f + 120.f * t;
    gCurrentObject->header.gfx.scale[0] = scale;
    gCurrentObject->header.gfx.scale[1] = scale;
    gCurrentObject->header.gfx.scale[2] = scale;
    gCurrentObject->hitboxRadius = 5.f * scale;
    gCurrentObject->hitboxHeight = 80.f;
    gCurrentObject->hitboxDownOffset = 40.f;
    gCurrentObject->oOpacity = MIN(255, 510 * (1.f - t));
    gCurrentObject->oIntangibleTimer = 0;
    cappy_whomp_shock_wave_attack(gCurrentObject);
}

const BehaviorScript bhvCappyWhompShockWave[] = {
    0x00020000,
    0x110100C1,
    0x08000000,
    0x0C000000, (uintptr_t) (bhv_cappy_whomp_shock_wave_loop),
    0x09000000, 
};

//
// Possessed Whomp
//

enum {
    CAPPY_WHOMP_ACTION_DEFAULT = 0,
    CAPPY_WHOMP_ACTION_BEGIN_ATTACK,
    CAPPY_WHOMP_ACTION_ATTACK,
    CAPPY_WHOMP_ACTION_FALLING,
    CAPPY_WHOMP_ACTION_LAND,
    CAPPY_WHOMP_ACTION_END_ATTACK
};

static void cappy_whomp_process_inputs(struct PObject *pobj) {
    pobj_decelerate(pobj);
    pobj_apply_gravity(pobj);

    // Inputs
    if (pobj->oUnresponsiveTimer == 0) {

        // Walk
        if (pobj->oStickMag > 0) {
            s32 faceYaw = pobj->oStickYaw - approach_s32((s16)(pobj->oStickYaw - pobj->oFaceAngleYaw), 0, 0x1000, 0x1000);
            obj_set_forward_vel(pobj, faceYaw, pobj->oStickMag, pobj->cdWalkSpeed);
            pobj->oFaceAngleYaw = faceYaw;
            pobj->oFaceAngleRoll = 0;
            pobj->oMoveAngleYaw = faceYaw;
            pobj->oMoveAngleRoll = 0;
        }

        // Jump
        if ((pobj->oButtonPressed & A_BUTTON) && obj_is_on_ground(pobj)) {
            pobj->oVelY = pobj->cdJumpVelocity;
            pobj->oFloor = NULL;
            play_sound(SOUND_OBJ_GOOMBA_ALERT, pobj->header.gfx.cameraToObject);
        }

        // Body slam
        if (pobj->oButtonPressed & B_BUTTON) {
            pobj->oCappyWhompActionState = CAPPY_WHOMP_ACTION_BEGIN_ATTACK;
        }
    } else {
        pobj->oUnresponsiveTimer--;
    }
}

static void cappy_whomp_set_properties(struct PObject *pobj) {
    pobj->hitboxRadius = pobj->cdHitboxRadius;
    pobj->hitboxHeight = pobj->cdHitboxHeight;
    pobj->hitboxDownOffset = pobj->cdHitboxDownOffset;
    pobj->oWallHitboxRadius = pobj->cdWallHitboxRadius;
    pobj->oProperties = 0;
    pobj->oProperties |= POBJ_PROP_ABOVE_WATER;

    // Body slam
    if (pobj->oCappyWhompActionState != CAPPY_WHOMP_ACTION_DEFAULT) {
        pobj->hitboxHeight *= 0.25f;
        pobj->oProperties |= POBJ_PROP_UNPUSHABLE;
        pobj->oProperties |= POBJ_PROP_INVULNERABLE;
    }
}

static void cappy_whomp_update_pos(struct PObject *pobj) {
    obj_update_pos_and_vel(pobj, pobj->oProperties & POBJ_PROP_MOVE_THROUGH_VC_WALLS, obj_is_on_ground(pobj), &pobj->oSquishedTimer);
    pobj_handle_special_floors(pobj);
}

static void cappy_whomp_process_interactions(struct PObject *pobj) {
    POBJ_INTERACTIONS_BEGIN;
    POBJ_INTERACTIONS_END;
}

static void cappy_whomp_update_action_state(struct PObject *pobj) {
    switch (pobj->oCappyWhompActionState) {
        case CAPPY_WHOMP_ACTION_BEGIN_ATTACK:
            mario_lock(pobj->oMario);
            obj_set_animation_with_accel(pobj, 1, 2.0f);
            if (obj_check_if_near_animation_end(pobj)) {
                pobj->oVelY = 40.0f;
                pobj->oCappyWhompActionState = CAPPY_WHOMP_ACTION_ATTACK;
            }
            break;

        case CAPPY_WHOMP_ACTION_ATTACK:
            mario_lock(pobj->oMario);
            if (pobj->oFaceAnglePitch < 0x4000) {
                pobj->oFaceAnglePitch += 0x400;
            } else {
                pobj->oCappyWhompActionState = CAPPY_WHOMP_ACTION_FALLING;
            }
            break;

        case CAPPY_WHOMP_ACTION_FALLING:
            mario_lock(pobj->oMario);
            if (obj_is_on_ground(pobj)) {
                f32 x = pobj->oPosX + sins(pobj->oFaceAngleYaw) * pobj->cdHitboxHeight / 2.f;
                f32 y = pobj->oPosY;
                f32 z = pobj->oPosZ + coss(pobj->oFaceAngleYaw) * pobj->cdHitboxHeight / 2.f;
                obj_spawn_white_puff_at(x, y, z, SOUND_OBJ_WHOMP_LOWPRIO);
                cur_obj_shake_screen(SHAKE_POS_LARGE);
                pobj->oCappyWhompActionState = CAPPY_WHOMP_ACTION_LAND;
                pobj->oCappyWhompActionTimer = 0;
                struct Object *shockwave = obj_spawn_with_geo(pobj, invisible_bowser_accessory_geo, bhvCappyWhompShockWave);
                shockwave->oPosX = x;
                shockwave->oPosY = x + 5;
                shockwave->oPosZ = z;
                shockwave->oFaceAnglePitch = 0;
                shockwave->oFaceAngleYaw = 0;
                shockwave->oFaceAngleRoll = 0;
                shockwave->oMoveAnglePitch = 0;
                shockwave->oMoveAngleYaw = 0;
                shockwave->oMoveAngleRoll = 0;
            }
            break;

        case CAPPY_WHOMP_ACTION_LAND:
            mario_lock(pobj->oMario);
            if (pobj->oCappyWhompActionTimer < 30) {
                pobj->oCappyWhompActionTimer++;
            } else {
                pobj->oCappyWhompActionState = CAPPY_WHOMP_ACTION_END_ATTACK;
            }
            break;

        case CAPPY_WHOMP_ACTION_END_ATTACK:
            mario_lock(pobj->oMario);
            if (pobj->oFaceAnglePitch > 0) {
                pobj->oFaceAnglePitch -= 0x200;
            } else {
                pobj->oCappyWhompActionState = CAPPY_WHOMP_ACTION_DEFAULT;
            }
            break;

        default:
            mario_unlock(pobj->oMario);
            obj_set_animation_with_accel(pobj, 0, (pobj->oVelY <= 0.f) * MAX(1.f, pobj->oForwardVel * 3.f / (pobj->cdWalkSpeed)));
            if (obj_is_on_ground(pobj)) {
                obj_make_step_sound_and_particle(pobj, &pobj->oWalkDistance, pobj->cdWalkSpeed * 8, pobj->oForwardVel, SOUND_OBJ_POUNDING1, OBJ_STEP_PARTICLE_NONE);
            }
            break;
    }
}

static void cappy_whomp_set_cappy_values(struct PObject *pobj) {
    pobj->oCappyForwardDist  = -45.f;
    pobj->oCappyVerticalDist = 430.f;
    pobj->oCappyInitialScale = 150;
}

//
// Main loop
//

const PObjMainLoopFunc cappy_whomp_mloop[] = {
    cappy_whomp_process_inputs,
    cappy_whomp_set_properties,
    cappy_whomp_update_pos,
    cappy_whomp_process_interactions,
    cappy_whomp_update_action_state,
    obj_update_gfx,
    cappy_whomp_set_cappy_values,
    NULL
};

//
// Init
//

s32 cappy_whomp_init(struct Object* obj) {
    if (!(obj->oAction == 1 || obj->oAction == 2 || obj->oAction == 3 || obj->oAction == 6 || obj->oAction == 7)) {
        return FALSE;
    }

    if (obj->oAction == 6) {
        obj->oCappyWhompActionState = CAPPY_WHOMP_ACTION_END_ATTACK;
    } else {
        obj->oCappyWhompActionState = CAPPY_WHOMP_ACTION_DEFAULT;
    }
    obj->oCappyWhompActionTimer = 0;
    return TRUE;
}

void cappy_whomp_end(struct Object *obj) {
    obj->oAction = 1;
    obj->oSubAction = 0;
    obj->oTimer = 0;
    obj->oFaceAnglePitch = 0;
}

f32 cappy_whomp_get_top(struct Object *obj) {
    return 430.f * obj->oScaleY;
}
