#include "../smo_c_includes.h"

enum {
    CAPPY_KOOPA_STATE_SHELLED_IDLE,
    CAPPY_KOOPA_STATE_SHELLED_WALKING,
    CAPPY_KOOPA_STATE_SHELLED_SLIDING,
    CAPPY_KOOPA_STATE_SHELLED_JUMPING,
    CAPPY_KOOPA_STATE_UNSHELLED_IDLE,
    CAPPY_KOOPA_STATE_UNSHELLED_WALKING,
    CAPPY_KOOPA_STATE_UNSHELLED_RUNNING,
    CAPPY_KOOPA_STATE_UNSHELLED_JUMPING,
};

/* AnimIndex, Accel, Sfx, Model */
static s32 sCappyKoopaStateData[][4] = {
    {  7,   0, 0, MODEL_KOOPA_WITH_SHELL    },
    {  9,   1, 1, MODEL_KOOPA_WITH_SHELL    },
    {  0,   0, 2, MODEL_KOOPA_SHELL         },
    { 12, 100, 0, MODEL_KOOPA_WITH_SHELL    },
    {  7,   0, 0, MODEL_KOOPA_WITHOUT_SHELL },
    {  9,   1, 1, MODEL_KOOPA_WITHOUT_SHELL },
    {  3,   0, 1, MODEL_KOOPA_WITHOUT_SHELL },
    { 12, 100, 0, MODEL_KOOPA_WITHOUT_SHELL }
};

/* fdist, ldist, vdist, pitch, yaw, roll, scale */
static s32 sCappyKoopaCappyParams[][7] = {
    { 33, 0, 68, 0xE000, 0x0000, 0x0000, 65 },
    { 26, 0, 74, 0xDC00, 0x0000, 0x0000, 65 },
    {  0, 0,  0, 0x0000, 0x0000, 0x0000,  0 },
    {  6, 0, 70, 0xB400, 0x0000, 0x0000, 65 },
    { 33, 0, 68, 0xE000, 0x0000, 0x0000, 65 },
    { 26, 0, 74, 0xDC00, 0x0000, 0x0000, 65 },
    { -4, 0, 82, 0xD400, 0x0000, 0x0000, 65 },
    {  6, 0, 70, 0xB400, 0x0000, 0x0000, 65 },
};

//
// Possessed Koopa
//

static void cappy_koopa_process_inputs(struct PObject *pobj) {
    obj_scale(pobj, pobj->smoData->objData.gfxScaleY);
    pobj_decelerate(pobj);
    pobj_apply_gravity(pobj);

    // Inputs
    if (pobj->oUnresponsiveTimer == 0) {

        // Walk
        f32 hvel = pobj->cdWalkSpeed;
        if (pobj->oButtonDown & B_BUTTON) {
            if (pobj->oKoopaMovementType != KOOPA_BP_UNSHELLED) {
                hvel = pobj->cdDashSpeed;
            } else {
                hvel = pobj->cdRunSpeed;
            }
        }
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
    } else {
        pobj->oUnresponsiveTimer--;
    }
}

static void cappy_koopa_update_state(struct PObject *pobj) {
    if (pobj->oKoopaMovementType != KOOPA_BP_UNSHELLED) {
        if (pobj->oButtonDown & B_BUTTON) {
            pobj->oCappyKoopaState = CAPPY_KOOPA_STATE_SHELLED_SLIDING;
        } else if (obj_is_on_ground(pobj)) {
            if (pobj->oForwardVel > 1.f) {
                pobj->oCappyKoopaState = CAPPY_KOOPA_STATE_SHELLED_WALKING;
            } else {
                pobj->oCappyKoopaState = CAPPY_KOOPA_STATE_SHELLED_IDLE;
            }
        } else {
            pobj->oCappyKoopaState = CAPPY_KOOPA_STATE_SHELLED_JUMPING;
        }
    } else {
        if (obj_is_on_ground(pobj)) {
            if (pobj->oForwardVel > pobj->cdWalkSpeed) {
                pobj->oCappyKoopaState = CAPPY_KOOPA_STATE_UNSHELLED_RUNNING;
            } else if (pobj->oForwardVel > 1.f) {
                pobj->oCappyKoopaState = CAPPY_KOOPA_STATE_UNSHELLED_WALKING;
            } else {
                pobj->oCappyKoopaState = CAPPY_KOOPA_STATE_UNSHELLED_IDLE;
            }
        } else {
            pobj->oCappyKoopaState = CAPPY_KOOPA_STATE_UNSHELLED_JUMPING;
        }
    }
}

static void cappy_koopa_set_properties(struct PObject *pobj) {
    pobj->hitboxRadius = pobj->cdHitboxRadius;
    pobj->hitboxHeight = pobj->cdHitboxHeight;
    pobj->hitboxDownOffset = pobj->cdHitboxDownOffset;
    pobj->oWallHitboxRadius = pobj->cdWallHitboxRadius;
    pobj->oProperties = 0;
    pobj->oProperties |= POBJ_PROP_ABOVE_WATER;
    if (pobj->oCappyKoopaState == CAPPY_KOOPA_STATE_SHELLED_SLIDING) {
        pobj->oProperties |= POBJ_PROP_ATTACK;
        pobj->oProperties |= POBJ_PROP_IMMUNE_TO_FIRE;
        pobj->oProperties |= POBJ_PROP_IMMUNE_TO_LAVA;
        pobj->oProperties |= POBJ_PROP_IMMUNE_TO_QUICKSANDS;
        pobj->oProperties |= POBJ_PROP_WALK_ON_WATER;
    }
}

static void cappy_koopa_update_pos(struct PObject *pobj) {
    obj_update_pos_and_vel(pobj, pobj->oProperties & POBJ_PROP_MOVE_THROUGH_VC_WALLS, obj_is_on_ground(pobj), &pobj->oSquishedTimer);
    pobj_handle_special_floors(pobj);
}

static void cappy_koopa_process_interactions(struct PObject *pobj) {
    POBJ_INTERACTIONS_BEGIN;

    // Koopa shell
    if (obj->behavior == bhvKoopaShell && pobj->oKoopaMovementType == KOOPA_BP_UNSHELLED) {
        if (obj_detect_overlap(pobj, obj, OBJ_OVERLAP_FLAG_HITBOX, OBJ_OVERLAP_FLAG_HITBOX)) {
            pobj->oKoopaMovementType = KOOPA_BP_NORMAL;
            obj_spawn_white_puff(obj, SOUND_MENU_STAR_SOUND);
            obj_mark_for_deletion(obj);
            obj->oInteractStatus = INT_STATUS_INTERACTED;
        }
    }

    POBJ_INTERACTIONS_END;
}

static void cappy_koopa_update_animation_and_sound(struct PObject *pobj) {

    // Animation
    s32 *animData = sCappyKoopaStateData[pobj->oCappyKoopaState];
    pobj->header.gfx.sharedChild = gLoadedGraphNodes[animData[3]];
    obj_set_animation_with_accel(pobj, animData[0], MAX(1.f, animData[1] * MAX(1.f, pobj->oForwardVel * 2.f / (pobj->cdWalkSpeed))));
    obj_update_blink_state(pobj, &pobj->oKoopaBlinkTimer, 20, 50, 4);

    if (pobj->oCappyKoopaState == CAPPY_KOOPA_STATE_SHELLED_SLIDING) {
        obj_scale(pobj, pobj->smoData->objData.gfxScaleY / 1.5f);
        pobj->header.gfx.angle[0] = 0;
        pobj->header.gfx.angle[1] = pobj->oTimer * 0x2000;
        pobj->header.gfx.angle[2] = 0;
    } else {
        obj_scale(pobj, pobj->smoData->objData.gfxScaleY);
    }

    // Sound effect
    switch (animData[2]) {
        case 1:
            if (obj_is_on_ground(pobj)) {
                obj_make_step_sound_and_particle(pobj, &pobj->oWalkDistance, pobj->cdWalkSpeed * 8, pobj->oForwardVel, SOUND_OBJ_KOOPA_WALK, OBJ_STEP_PARTICLE_NONE);
            }
            break;

        case 2:
            switch (pobj->oFloorType) {
                case POBJ_FLOOR_TYPE_GROUND:
                    obj_make_step_sound_and_particle(pobj,
                        &pobj->oWalkDistance, 0.f, 0.f,
                        SOUND_MOVING_TERRAIN_RIDING_SHELL + pobj->oMario->terrainSoundAddend,
                        OBJ_STEP_PARTICLE_MIST);
                    break;

                case POBJ_FLOOR_TYPE_WATER:
                    obj_make_step_sound_and_particle(pobj,
                        &pobj->oWalkDistance, 0.f, 0.f,
                        SOUND_MOVING_TERRAIN_RIDING_SHELL + SOUND_TERRAIN_WATER,
                        OBJ_STEP_PARTICLE_WATER_TRAIL | OBJ_STEP_PARTICLE_WATER_DROPLET);
                    break;

                case POBJ_FLOOR_TYPE_LAVA:
                    obj_make_step_sound_and_particle(pobj,
                        &pobj->oWalkDistance, 0.f, 0.f,
                        SOUND_MOVING_RIDING_SHELL_LAVA,
                        OBJ_STEP_PARTICLE_FLAME);
                    break;
            }
            break;
    }
}

static void cappy_koopa_set_cappy_values(struct PObject *pobj) {
    s32 *cappyParams = sCappyKoopaCappyParams[pobj->oCappyKoopaState];
    pobj->oCappyForwardDist  = (f32) cappyParams[0];
    pobj->oCappyLateralDist  = (f32) cappyParams[1];
    pobj->oCappyVerticalDist = (f32) cappyParams[2];
    pobj->oCappyInitialPitch = (s16) cappyParams[3];
    pobj->oCappyInitialYaw   = (s16) cappyParams[4];
    pobj->oCappyInitialRoll  = (s16) cappyParams[5];
    pobj->oCappyInitialScale = (s16) cappyParams[6];
}

//
// Main loop
//

const PObjMainLoopFunc cappy_koopa_mloop[] = {
    cappy_koopa_process_inputs,
    cappy_koopa_update_state,
    cappy_koopa_set_properties,
    cappy_koopa_update_pos,
    cappy_koopa_process_interactions,
    obj_update_gfx,
    cappy_koopa_update_animation_and_sound,
    cappy_koopa_set_cappy_values,
    NULL
};

//
// Init
//

s32 cappy_koopa_init(struct Object* obj) {

    // Can't possess KTQ
    if (obj->oBehParams2ndByte == KOOPA_BP_KOOPA_THE_QUICK_BOB ||
        obj->oBehParams2ndByte == KOOPA_BP_KOOPA_THE_QUICK_THI) {
        return FALSE;
    }

    return TRUE;
}

void cappy_koopa_end(struct Object *obj) {
    if (obj->oCappyKoopaState < CAPPY_KOOPA_STATE_UNSHELLED_IDLE) {
        obj->oKoopaMovementType = KOOPA_BP_NORMAL;
        obj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_KOOPA_WITH_SHELL];
    } else {
        obj->oKoopaMovementType = KOOPA_BP_UNSHELLED;
        obj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_KOOPA_WITHOUT_SHELL];
    }
}

f32 cappy_koopa_get_top(struct Object *obj) {
    return 70.f * obj->oScaleY;
}