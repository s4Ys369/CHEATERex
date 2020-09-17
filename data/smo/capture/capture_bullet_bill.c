#include "../smo_c_includes.h"

//
// Possessed Bullet Bill explosion
//

static void cappy_bullet_bill_explosion_attack(struct Object *this) {
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

void bhv_cappy_bullet_bill_explosion_loop(void) {
    if (gCurrentObject->oTimer >= 9) {
        spawn_object(gCurrentObject, MODEL_SMOKE, bhvBobombBullyDeathSmoke);
        obj_mark_for_deletion(gCurrentObject);
        return;
    }

    f32 scale = (f32) gCurrentObject->oTimer / 4.f + 1.0f;
    gCurrentObject->header.gfx.scale[0] = scale;
    gCurrentObject->header.gfx.scale[1] = scale;
    gCurrentObject->header.gfx.scale[2] = scale;
    gCurrentObject->hitboxRadius = 250.f;
    gCurrentObject->hitboxHeight = 250.f;
    gCurrentObject->hitboxDownOffset = 0.f;
    gCurrentObject->oOpacity = MAX(0, 0xFF - (0x1C * gCurrentObject->oTimer));
    gCurrentObject->oAnimState++;
    gCurrentObject->oIntangibleTimer = 0;
    cappy_bullet_bill_explosion_attack(gCurrentObject);
}

const BehaviorScript bhvCappyBulletBillExplosion[] = {
    0x00020000,
    0x110100C1,
    0x21000000,
    0x08000000,
    0x0C000000, (uintptr_t) (bhv_cappy_bullet_bill_explosion_loop),
    0x09000000, 
};

//
// Possessed Bullet bill
//

static void cappy_bullet_bill_set_properties(struct PObject *pobj) {
    pobj->hitboxRadius = pobj->cdHitboxRadius;
    pobj->hitboxHeight = pobj->cdHitboxHeight;
    pobj->hitboxDownOffset = pobj->cdHitboxDownOffset;
    pobj->oWallHitboxRadius = pobj->cdWallHitboxRadius;
    pobj->oProperties = 0;
    pobj->oProperties |= POBJ_PROP_ABOVE_WATER;
    pobj->oProperties |= POBJ_PROP_INVULNERABLE;
    pobj->oProperties |= POBJ_PROP_IMMUNE_TO_FIRE;
    pobj->oProperties |= POBJ_PROP_IMMUNE_TO_LAVA;
    pobj->oProperties |= POBJ_PROP_IMMUNE_TO_QUICKSANDS;
    pobj->oProperties |= POBJ_PROP_RESIST_STRONG_WINDS;
    pobj->oProperties |= POBJ_PROP_ATTACK;
}

static void cappy_bullet_bill_process_inputs(struct PObject *pobj) {
    smo_capture_set_camera_behind_mario(pobj);

    // Inputs
    if (pobj->oUnresponsiveTimer == 0) {
        s32 pitch = (s32)(pobj->oStickY * 0x3000);
        s32 yaw = (s32)(pobj->oStickX * 0x200);
        s32 facePitch = pitch - approach_s32((s16)(pitch - pobj->oFaceAnglePitch), 0, 0x200, 0x200);
        s32 faceYaw = pobj->oFaceAngleYaw - yaw;
        f32 fvel = (pobj->oButtonDown & B_BUTTON ? pobj->cdDashSpeed : pobj->cdWalkSpeed);

        pobj->oVelX = coss(facePitch) * sins(faceYaw) * fvel;
        pobj->oVelY = -sins(facePitch) * fvel;
        pobj->oVelZ = coss(facePitch) * coss(faceYaw) * fvel;
        pobj->oFaceAngleYaw = faceYaw;
        pobj->oFaceAnglePitch = facePitch;
        pobj->oFaceAngleRoll = 0;
        pobj->oMoveAngleYaw = faceYaw;
        pobj->oMoveAnglePitch = facePitch;
        pobj->oMoveAngleRoll = 0;
        if (pobj->oButtonDown & B_BUTTON) {
            pobj->oProperties |= POBJ_PROP_STRONG_ATTACK;
        }
    } else {
        pobj->oUnresponsiveTimer--;
    }
}

static void cappy_bullet_bill_update_pos(struct PObject *pobj) {
    obj_update_pos_and_vel(pobj, pobj->oProperties & POBJ_PROP_MOVE_THROUGH_VC_WALLS, obj_is_on_ground(pobj),  &pobj->oSquishedTimer);
    pobj_handle_special_floors(pobj);
}

static void cappy_bullet_bill_process_interactions(struct PObject *pobj) {
    POBJ_INTERACTIONS_BEGIN;
    POBJ_INTERACTIONS_END;
}

static void cappy_bullet_bill_check_wall_hit(struct PObject *pobj) {
    if (pobj->oWall) {

        // Bullet dash
        if (pobj->oProperties & POBJ_PROP_STRONG_ATTACK) {
        
            // Dynamic wall
            struct Object *obj = pobj->oWall->object;
            if (obj) {
                
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

            // BOOM
            set_environmental_camera_shake(SHAKE_ENV_EXPLOSION);
            struct Object *explosion = spawn_object(pobj, MODEL_EXPLOSION, bhvCappyBulletBillExplosion);
            explosion->oFlags |= OBJ_FLAG_UPDATE_GFX_POS_AND_ANGLE;
            explosion->oAnimState = -1;
            explosion->oGraphYOffset += 100.0f;
            explosion->oOpacity = 255;
            explosion->oIntangibleTimer = 0;
            set_environmental_camera_shake(SHAKE_ENV_EXPLOSION);
        } else {
        
            // Poof
            obj_spawn_white_puff(pobj, 0);
        }

        create_sound_spawner(SOUND_GENERAL2_BOBOMB_EXPLOSION);
        pobj->oCappyBulletBillDestroyed = TRUE;
        mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_JUMP_OUT, FALSE, 6);
    }
}

static void cappy_bullet_bill_update_animation_and_sound(struct PObject *pobj) {
    if (pobj->oProperties & POBJ_PROP_STRONG_ATTACK) {
        obj_make_step_sound_and_particle(pobj, &pobj->oWalkDistance, 0.f, 0.f, -1, OBJ_STEP_PARTICLE_FIRE);
    } else {
        obj_make_step_sound_and_particle(pobj, &pobj->oWalkDistance, 0.f, 0.f, -1, OBJ_STEP_PARTICLE_SMOKE);
    }
}

static void cappy_bullet_bill_set_cappy_values(struct PObject *pobj) {
    pobj->oCappyForwardDist  = 50.f;
    pobj->oCappyVerticalDist = 200.f;
    pobj->oCappyInitialScale = 400;
}

//
// Main loop
//

const PObjMainLoopFunc cappy_bullet_bill_mloop[] = {
    cappy_bullet_bill_set_properties,
    cappy_bullet_bill_process_inputs,
    cappy_bullet_bill_update_pos,
    cappy_bullet_bill_process_interactions,
    cappy_bullet_bill_check_wall_hit,
    obj_update_gfx,
    cappy_bullet_bill_update_animation_and_sound,
    cappy_bullet_bill_set_cappy_values,
    NULL
};

//
// Init
//

s32 cappy_bullet_bill_init(struct Object *obj) {
    if (obj->oAction != 2 || obj->oTimer < 50) {
        return FALSE;
    }

    obj->oCappyBulletBillInitialHomeX = obj->oHomeX; 
    obj->oCappyBulletBillInitialHomeY = obj->oHomeY; 
    obj->oCappyBulletBillInitialHomeZ = obj->oHomeZ;
    obj->oCappyBulletBillDestroyed = FALSE;
    return TRUE;
}

void cappy_bullet_bill_end(struct Object *obj) {
    if (obj->oCappyBulletBillDestroyed) {
        obj->oAction = 0;
        obj->oTimer = 0;
    }
    obj->oHomeX = obj->oCappyBulletBillInitialHomeX;
    obj->oHomeY = obj->oCappyBulletBillInitialHomeY;
    obj->oHomeZ = obj->oCappyBulletBillInitialHomeZ;
    obj->oFaceAnglePitch = 0;
    obj->oMoveAnglePitch = 0;
}

f32 cappy_bullet_bill_get_top(struct Object *obj) {
    return 200.f * obj->oScaleY;
}