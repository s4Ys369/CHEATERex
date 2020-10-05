#include "../smo_c_includes.h"

//
// Possessed Hoot
//

static void cappy_hoot_set_properties(struct PObject *pobj) {
    pobj->hitboxRadius = pobj->cdHitboxRadius;
    pobj->hitboxHeight = pobj->cdHitboxHeight;
    pobj->hurtboxRadius = 0;
    pobj->hurtboxHeight = 0;
    pobj->hitboxDownOffset = pobj->cdHitboxDownOffset;
    pobj->oWallHitboxRadius = pobj->cdWallHitboxRadius;
    pobj->oProperties = 0;
    pobj->oProperties |= POBJ_PROP_ABOVE_WATER;
    pobj->oProperties |= POBJ_PROP_UNDERWATER;
}

static void cappy_hoot_process_inputs(struct PObject *pobj) {
    pobj_decelerate(pobj);
    pobj_apply_gravity(pobj);

    // Inputs
    if (pobj->oUnresponsiveTimer == 0) {

        // Move
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

        // Fly
        if (pobj->oButtonPressed & A_BUTTON) {
            pobj->oVelY = pobj->cdJumpVelocity;
            pobj->oFloor = NULL;
            pobj->header.gfx.unk38.curAnim = NULL;
            play_sound(SOUND_GENERAL_SWISH_WATER, pobj->header.gfx.cameraToObject);
        }
    } else {
        pobj->oUnresponsiveTimer--;
    }
}

static void cappy_hoot_update_pos(struct PObject *pobj) {
    obj_update_pos_and_vel(pobj, pobj->oProperties & POBJ_PROP_MOVE_THROUGH_VC_WALLS, obj_is_on_ground(pobj), &pobj->oSquishedTimer);
    pobj_handle_special_floors(pobj);
}

static void cappy_hoot_process_interactions(struct PObject *pobj) {
    POBJ_INTERACTIONS_BEGIN;
    POBJ_INTERACTIONS_END;
}

static void cappy_hoot_update_animation_and_sound(struct PObject *pobj) {
    if (pobj->oVelY > 0.f) {
        obj_set_animation_with_accel(pobj, 0, 2.f);
    } else {
        obj_set_animation_with_accel(pobj, 0, 1.f);
    }
}

static void cappy_hoot_set_cappy_values(struct PObject *pobj) {
    pobj->oCappyForwardDist  = 26.f;
    pobj->oCappyVerticalDist = 100.f;
    pobj->oCappyInitialScale = 80;
}

//
// Main loop
//

const PObjMainLoopFunc cappy_hoot_mloop[] = {
    cappy_hoot_set_properties,
    cappy_hoot_process_inputs,
    cappy_hoot_update_pos,
    cappy_hoot_process_interactions,
    obj_update_gfx,
    cappy_hoot_update_animation_and_sound,
    cappy_hoot_set_cappy_values,
    NULL
};

//
// Init
//

s32 cappy_hoot_init(struct Object* obj) {
    if (obj->oHootAvailability != HOOT_AVAIL_READY_TO_FLY) {
        return FALSE;
    }

    obj->oCappyHootInitialHomeX = obj->oHomeX;
    obj->oCappyHootInitialHomeY = obj->oHomeY;
    obj->oCappyHootInitialHomeZ = obj->oHomeZ;
    return TRUE;
}

void cappy_hoot_end(struct Object *obj) {
    obj->oHomeX = obj->oCappyHootInitialHomeX;
    obj->oHomeY = obj->oCappyHootInitialHomeY;
    obj->oHomeZ = obj->oCappyHootInitialHomeZ;
    obj->header.gfx.unk38.curAnim = NULL;
    obj_set_animation_with_accel(obj, 0, 1.f);
}

f32 cappy_hoot_get_top(struct Object *obj) {
    return 100.f * obj->oScaleY;
}
