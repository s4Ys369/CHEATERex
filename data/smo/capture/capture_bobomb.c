#include "../smo_c_includes.h"

//
// Possessed Bobomb explosion
//

static void cappy_bobomb_explosion_attack(struct Object *this) {
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

void bhv_cappy_bobomb_explosion_loop(void) {
    if (gCurrentObject->oTimer >= 10) {
        spawn_object(gCurrentObject, MODEL_SMOKE, bhvBobombBullyDeathSmoke);
        obj_mark_for_deletion(gCurrentObject);
        return;
    }

    f32 scale = (f32) gCurrentObject->oTimer / 3.f + 1.0f;
    gCurrentObject->header.gfx.scale[0] = scale;
    gCurrentObject->header.gfx.scale[1] = scale;
    gCurrentObject->header.gfx.scale[2] = scale;
    gCurrentObject->hitboxRadius = 360.f;
    gCurrentObject->hitboxHeight = 360.f;
    gCurrentObject->hitboxDownOffset = 0.f;
    gCurrentObject->oOpacity = MAX(0, 0xFF - (0x1C * gCurrentObject->oTimer));
    gCurrentObject->oAnimState++;
    gCurrentObject->oIntangibleTimer = 0;
    cappy_bobomb_explosion_attack(gCurrentObject);
}

const BehaviorScript bhvCappyBobombExplosion[] = {
    0x00020000,
    0x110100C1,
    0x21000000,
    0x08000000,
    0x0C000000, (uintptr_t) (bhv_cappy_bobomb_explosion_loop),
    0x09000000, 
};

//
// Possessed Bobomb
//

static void cappy_bobomb_set_properties(struct PObject *pobj) {
    pobj->hitboxRadius = pobj->cdHitboxRadius;
    pobj->hitboxHeight = pobj->cdHitboxHeight;
    pobj->hitboxDownOffset = pobj->cdHitboxDownOffset;
    pobj->oWallHitboxRadius = pobj->cdWallHitboxRadius;
    pobj->oProperties = 0;
    pobj->oProperties |= POBJ_PROP_ABOVE_WATER;
}

static void cappy_bobomb_process_inputs(struct PObject *pobj) {
    pobj_decelerate(pobj);
    pobj_apply_gravity(pobj);

    // Inputs
    if (pobj->oUnresponsiveTimer == 0) {

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
        if ((pobj->oButtonPressed & A_BUTTON) && obj_is_on_ground(pobj)) {
            pobj->oVelY = pobj->cdJumpVelocity;
            pobj->oFloor = NULL;
            play_sound(SOUND_OBJ_GOOMBA_ALERT, pobj->header.gfx.cameraToObject);
        }

        // Explosion
        if ((pobj->oButtonPressed & B_BUTTON) && pobj->oCappyBobombExplosionTimer == 0) {
            struct Object* explosion = spawn_object(pobj, MODEL_EXPLOSION, bhvCappyBobombExplosion);
            if (explosion != NULL) {
                explosion->oFlags |= OBJ_FLAG_UPDATE_GFX_POS_AND_ANGLE;
                explosion->oAnimState = -1;
                explosion->oGraphYOffset += 100.0f;
                explosion->oOpacity = 255;
                explosion->oIntangibleTimer = 0;
                create_sound_spawner(SOUND_GENERAL2_BOBOMB_EXPLOSION);
                set_environmental_camera_shake(SHAKE_ENV_EXPLOSION);

                // Make the bob-omb bloat after each explosion
                pobj->oScaleX *= 1.2f;
                pobj->oScaleY *= 1.2f;
                pobj->oScaleZ *= 1.2f;
                pobj->oCappyBobombExplosionTimer = 30;
                pobj->oCappyBobombExplosionCount++;
                
                // If used three times in a row, removes the bob-omb
                if (pobj->oCappyBobombExplosionCount == 3) {
                    mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_JUMP_OUT, FALSE, 0);
                    obj_instant_death(pobj);
                }
            }
        }
    } else {
        pobj->oUnresponsiveTimer--;
    }

    if (pobj->oCappyBobombExplosionTimer > 0) {
        pobj->oCappyBobombExplosionTimer--;
    }
}

static void cappy_bobomb_update_pos(struct PObject *pobj) {
    obj_update_pos_and_vel(pobj, pobj->oProperties & POBJ_PROP_MOVE_THROUGH_VC_WALLS, obj_is_on_ground(pobj), &pobj->oSquishedTimer);
    pobj_handle_special_floors(pobj);
}

static void cappy_bobomb_process_interactions(struct PObject *pobj) {
    POBJ_INTERACTIONS_BEGIN;

    // Possessed bobomb explosion
    if (obj->behavior == bhvCappyBobombExplosion) {
        obj->oInteractStatus = INT_STATUS_INTERACTED;
    }

    POBJ_INTERACTIONS_END;
}

static void cappy_bobomb_update_animation_and_sound(struct PObject *pobj) {
    obj_set_animation_with_accel(pobj, 0, (pobj->oVelY <= 0.f) * MAX(1.f, pobj->oForwardVel * (2.f / (pobj->cdWalkSpeed))));
    obj_random_blink(pobj, &pobj->oBobombBlinkTimer);
    if (obj_is_on_ground(pobj)) {
        obj_make_step_sound_and_particle(pobj, &pobj->oWalkDistance, pobj->cdWalkSpeed * 12, pobj->oForwardVel, SOUND_OBJ_BOBOMB_WALK, OBJ_STEP_PARTICLE_NONE);
    }
}

static void cappy_bobomb_set_cappy_values(struct PObject *pobj) {
    pobj->oCappyVerticalDist = 94.f;
    pobj->oCappyInitialScale = 120;
}

//
// Main loop
//

const PObjMainLoopFunc cappy_bobomb_mloop[] = {
    cappy_bobomb_set_properties,
    cappy_bobomb_process_inputs,
    cappy_bobomb_update_pos,
    cappy_bobomb_process_interactions,
    obj_update_gfx,
    cappy_bobomb_update_animation_and_sound,
    cappy_bobomb_set_cappy_values,
    NULL
};

//
// Init
//

s32 cappy_bobomb_init(struct Object* obj) {
    obj->oCappyBobombExplosionTimer = 0;
    obj->oCappyBobombExplosionCount = 0;
    obj->oScaleX = 1.f;
    obj->oScaleY = 1.f;
    obj->oScaleZ = 1.f;
    return TRUE;
}

void cappy_bobomb_end(struct Object *obj) {
    obj->header.gfx.unk38.curAnim = NULL;
    obj_set_animation_with_accel(obj, 0, 1.f);
}

f32 cappy_bobomb_get_top(struct Object *obj) {
    return 94.f * obj->oScaleY;
}