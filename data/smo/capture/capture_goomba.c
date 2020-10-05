#include "../smo_c_includes.h"

//
// Goomba stack
//

#define CAPPY_GOOMBA_STACK_MAX_COUNT    7

static const uintptr_t bhvGoombaStack[] = {
    0x08000000, 
    0x09000000
};

static void goomba_fix(struct Object *obj) {

    // Goomba triplet spawner
    if (obj->parentObj != obj) {

        // Mark the goomba as dead inside the triplet
        set_object_respawn_info_bits(obj->parentObj, (obj->oBehParams2ndByte & GOOMBA_BP_TRIPLET_FLAG_MASK) >> 2);
        obj->parentObj->oBehParams = obj->parentObj->oBehParams | (obj->oBehParams2ndByte & GOOMBA_BP_TRIPLET_FLAG_MASK) << 6;

        // Set the goomba as a standalone goomba
        obj->oBehParams = (obj->oGoombaSize & GOOMBA_BP_SIZE_MASK) << 16;
        obj->oBehParams2ndByte = (obj->oGoombaSize & GOOMBA_BP_SIZE_MASK);
        obj->parentObj = obj;
        obj->prevObj = NULL;
    }
}

static void add_goomba_to_stack(struct PObject* pobj, struct Object *obj) {
    s32 i = pobj->oCappyGoombaStackCount;
    pobj->oCappyGoombaStackCurOffset(i) = (s16)((s64)(obj->curBhvCommand) - (s64)(bhvGoomba));
    pobj->oCappyGoombaStackStkIndex(i) = (s16)(obj->bhvStackIndex);
    pobj->oCappyGoombaStackObject(i) = obj;
    pobj->oCappyGoombaStackCount++;

    obj->behavior = bhvGoombaStack;
    obj->curBhvCommand = bhvGoombaStack;
    goomba_fix(obj);
}

void cappy_goomba_update_goomba_stack(struct PObject *pobj) {
    for (s32 i = 0; i != pobj->oCappyGoombaStackCount; ++i) {
        struct Object *obj = pobj->oCappyGoombaStackObject(i);
        obj->oPosX = pobj->oPosX;
        obj->oPosY = pobj->oPosY + pobj->cdHitboxHeight * (i + 1);
        obj->oPosZ = pobj->oPosZ;
        obj->oHomeX = pobj->oPosX;
        obj->oHomeY = pobj->oPosY;
        obj->oHomeZ = pobj->oPosZ;
        obj->oFaceAnglePitch = pobj->oFaceAnglePitch;
        obj->oFaceAngleYaw = pobj->oFaceAngleYaw;
        obj->oFaceAngleRoll = pobj->oFaceAngleRoll;
        obj->oMoveAnglePitch = pobj->oMoveAnglePitch;
        obj->oMoveAngleYaw = pobj->oMoveAngleYaw;
        obj->oMoveAngleRoll = pobj->oMoveAngleRoll;
        obj->header.gfx.pos[0] = obj->oPosX;
        obj->header.gfx.pos[1] = obj->oPosY; 
        obj->header.gfx.pos[2] = obj->oPosZ;
        obj->header.gfx.angle[0] = obj->oFaceAnglePitch;
        obj->header.gfx.angle[1] = obj->oFaceAngleYaw;
        obj->header.gfx.angle[2] = obj->oFaceAngleRoll;
        obj->header.gfx.scale[0] = pobj->header.gfx.scale[0];
        obj->header.gfx.scale[1] = pobj->header.gfx.scale[1];
        obj->header.gfx.scale[2] = pobj->header.gfx.scale[2];
        obj_set_animation_with_accel(obj, 0, 1.f);
        obj_update_blink_state(obj, &obj->oGoombaBlinkTimer, 30, 50, 5);
    }
}

//
// Possessed Goomba
//

static void cappy_goomba_set_properties(struct PObject *pobj) {
    pobj->hitboxRadius = pobj->cdHitboxRadius;
    pobj->hitboxHeight = pobj->cdHitboxHeight * (pobj->oCappyGoombaStackCount + 1);
    pobj->hurtboxRadius = 0;
    pobj->hurtboxHeight = 0;
    pobj->hitboxDownOffset = pobj->cdHitboxDownOffset;
    pobj->oWallHitboxRadius = pobj->cdWallHitboxRadius;
    pobj->oProperties = 0;
    pobj->oProperties |= POBJ_PROP_ABOVE_WATER;
}

static void cappy_goomba_process_inputs(struct PObject *pobj) {
    pobj_decelerate(pobj);
    pobj_apply_gravity(pobj);

    // Inputs
    if (pobj->oUnresponsiveTimer == 0) {

        // Walk
        f32 hvel = ((pobj->oButtonDown & B_BUTTON) ? pobj->cdRunSpeed : pobj->cdWalkSpeed);
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

static void cappy_goomba_update_pos(struct PObject *pobj) {
    obj_update_pos_and_vel(pobj, pobj->oProperties & POBJ_PROP_MOVE_THROUGH_VC_WALLS, obj_is_on_ground(pobj), &pobj->oSquishedTimer);
    pobj_handle_special_floors(pobj);
}

static void cappy_goomba_process_interactions(struct PObject *pobj) {
    POBJ_INTERACTIONS_BEGIN;

    // Goomba stack
    if (pobj->oCappyGoombaStackCount < CAPPY_GOOMBA_STACK_MAX_COUNT && obj->behavior == bhvGoomba) {
        if (obj_detect_overlap(pobj, obj, OBJ_OVERLAP_FLAG_HITBOX, OBJ_OVERLAP_FLAG_HITBOX)) {
            if (obj_is_object2_hit_from_above(pobj, obj)) {
                add_goomba_to_stack(pobj, obj);
                obj_spawn_white_puff(obj, SOUND_OBJ_DEFAULT_DEATH);
                obj->oInteractStatus = INT_STATUS_INTERACTED;
            }
        }
    }
    
    POBJ_INTERACTIONS_END;
}

static void cappy_goomba_update_animation_and_sound(struct PObject *pobj) {
    obj_set_animation_with_accel(pobj, 0, (pobj->oVelY <= 0.f) * MAX(1.f, pobj->oForwardVel * 2.f / (pobj->cdWalkSpeed)));
    obj_update_blink_state(pobj, &pobj->oGoombaBlinkTimer, 30, 50, 5);
    if (obj_is_on_ground(pobj)) {
        obj_make_step_sound_and_particle(pobj, &pobj->oWalkDistance, pobj->cdWalkSpeed * 8, pobj->oForwardVel, SOUND_OBJ_GOOMBA_WALK, OBJ_STEP_PARTICLE_NONE);
    }
}

static void cappy_goomba_set_cappy_values(struct PObject *pobj) {
    pobj->oCappyVerticalDist = (pobj->cdHitboxHeight / pobj->header.gfx.scale[1]) * (1 + pobj->oCappyGoombaStackCount);
    pobj->oCappyInitialScale = 80;
}

//
// Main loop
//

const PObjMainLoopFunc cappy_goomba_mloop[] = {
    cappy_goomba_set_properties,
    cappy_goomba_process_inputs,
    cappy_goomba_update_pos,
    cappy_goomba_process_interactions,
    obj_update_gfx,
    cappy_goomba_update_animation_and_sound,
    cappy_goomba_update_goomba_stack,
    cappy_goomba_set_cappy_values,
    NULL
};

//
// Init
//

s32 cappy_goomba_init(struct Object* obj) {
    goomba_fix(obj);
    obj->oCappyGoombaStackCount = 0;
    return TRUE;
}

void cappy_goomba_end(struct Object *obj) {
    for (s32 i = 0; i != obj->oCappyGoombaStackCount; ++i) {
        struct Object *o = obj->oCappyGoombaStackObject(i);
        o->behavior      = bhvGoomba;
        o->curBhvCommand = (const BehaviorScript *)((s64)(bhvGoomba) + (s64)(obj->oCappyGoombaStackCurOffset(i)));
        o->bhvStackIndex = (u32)(obj->oCappyGoombaStackStkIndex(i));
    }
}

f32 cappy_goomba_get_top(struct Object *pobj) {
    return pobj->cdHitboxHeight * (1 + pobj->oCappyGoombaStackCount);
}
