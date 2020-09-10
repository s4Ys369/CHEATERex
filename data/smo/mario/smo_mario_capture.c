#include "../smo_c_includes.h"
#ifdef SMO_SGI
#include "sgi/utils/characters.h"
#endif

// Traps the object in an infinite loop, preventing it from updating normally
static const uintptr_t bhvCappyLock[] = {
    0x08000000, 
    0x09000000
};

//
// Object copy
//

static void copy_object_data(struct Object *obj) {

    // Copying object fields
    memcpy(&obj->smoData->objData.rawData, &obj->rawData, sizeof(obj->rawData));
#if IS_64_BIT
    memcpy(&obj->smoData->objData.ptrData, &obj->ptrData, sizeof(obj->ptrData));
#endif

    // Copying behavior data
    memcpy(&obj->smoData->objData.behavior, &obj->behavior, sizeof(obj->behavior));
    memcpy(&obj->smoData->objData.curBhvCommand, &obj->curBhvCommand, sizeof(obj->curBhvCommand));
    memcpy(&obj->smoData->objData.bhvStackIndex, &obj->bhvStackIndex, sizeof(obj->bhvStackIndex));
    memcpy(&obj->smoData->objData.bhvStack, &obj->bhvStack, sizeof(obj->bhvStack));

    // Copying scale data
    memcpy(&obj->smoData->objData.hitboxRadius, &obj->hitboxRadius, sizeof(obj->hitboxRadius));
    memcpy(&obj->smoData->objData.hitboxHeight, &obj->hitboxHeight, sizeof(obj->hitboxHeight));
    memcpy(&obj->smoData->objData.hurtboxRadius, &obj->hurtboxRadius, sizeof(obj->hurtboxRadius));
    memcpy(&obj->smoData->objData.hurtboxHeight, &obj->hurtboxHeight, sizeof(obj->hurtboxHeight));
    memcpy(&obj->smoData->objData.hitboxDownOffset, &obj->hitboxDownOffset, sizeof(obj->hitboxDownOffset));
    memcpy(&obj->smoData->objData.gfxScaleX, &obj->header.gfx.scale[0], sizeof(obj->header.gfx.scale[0]));
    memcpy(&obj->smoData->objData.gfxScaleY, &obj->header.gfx.scale[1], sizeof(obj->header.gfx.scale[1]));
    memcpy(&obj->smoData->objData.gfxScaleZ, &obj->header.gfx.scale[2], sizeof(obj->header.gfx.scale[2]));
}

static void restore_object_data(struct Object *obj) {
    f32 x = obj->oPosX;
    f32 y = obj->oPosY;
    f32 z = obj->oPosZ;
    f32 pitch = obj->oFaceAnglePitch;
    f32 yaw = obj->oFaceAngleYaw;
    f32 roll = obj->oFaceAngleRoll;

    // Restoring object fields
    memcpy(&obj->rawData, &obj->smoData->objData.rawData, sizeof(obj->rawData));
#if IS_64_BIT
    memcpy(&obj->ptrData, &obj->smoData->objData.ptrData, sizeof(obj->ptrData));
#endif

    // Restoring behavior data
    memcpy(&obj->behavior, &obj->smoData->objData.behavior, sizeof(obj->behavior));
    memcpy(&obj->curBhvCommand, &obj->smoData->objData.curBhvCommand, sizeof(obj->curBhvCommand));
    memcpy(&obj->bhvStackIndex, &obj->smoData->objData.bhvStackIndex, sizeof(obj->bhvStackIndex));
    memcpy(&obj->bhvStack, &obj->smoData->objData.bhvStack, sizeof(obj->bhvStack));

    // Restoring scale data
    memcpy(&obj->hitboxRadius, &obj->smoData->objData.hitboxRadius, sizeof(obj->hitboxRadius));
    memcpy(&obj->hitboxHeight, &obj->smoData->objData.hitboxHeight, sizeof(obj->hitboxHeight));
    memcpy(&obj->hurtboxRadius, &obj->smoData->objData.hurtboxRadius, sizeof(obj->hurtboxRadius));
    memcpy(&obj->hurtboxHeight, &obj->smoData->objData.hurtboxHeight, sizeof(obj->hurtboxHeight));
    memcpy(&obj->hitboxDownOffset, &obj->smoData->objData.hitboxDownOffset, sizeof(obj->hitboxDownOffset));
    memcpy(&obj->header.gfx.scale[0], &obj->smoData->objData.gfxScaleX, sizeof(obj->header.gfx.scale[0]));
    memcpy(&obj->header.gfx.scale[1], &obj->smoData->objData.gfxScaleY, sizeof(obj->header.gfx.scale[1]));
    memcpy(&obj->header.gfx.scale[2], &obj->smoData->objData.gfxScaleZ, sizeof(obj->header.gfx.scale[2]));

    // Updating values
    obj->oPosX = x;
    obj->oPosY = y;
    obj->oPosZ = z;
    obj->oHomeX = x;
    obj->oHomeY = y;
    obj->oHomeZ = z;
    obj->oVelX = 0;
    obj->oVelY = 0;
    obj->oVelZ = 0;
    obj->oForwardVel = 0;
    obj->oFaceAnglePitch = pitch;
    obj->oFaceAngleYaw = yaw;
    obj->oFaceAngleRoll = roll;
    obj->oMoveAnglePitch = pitch;
    obj->oMoveAngleYaw = yaw;
    obj->oMoveAngleRoll = roll;
}

//
// Act
//

s32 mario_lock(struct MarioState *m) {
    if (m->action != ACT_SMO_POSSESSION) {
        return FALSE;
    }
    m->marioObj->oPossessionLock = TRUE;
    return TRUE;
}

s32 mario_unlock(struct MarioState *m) {
    if (m->action != ACT_SMO_POSSESSION) {
        return FALSE;
    }
    m->marioObj->oPossessionLock = FALSE;
    return TRUE;
}

static void mario_cappy_update_object_and_gfx(struct MarioState *m) {
    struct Object *mobj = m->marioObj;
    struct Object *cap = mobj->oObjectCap;
    struct PObject *pobj = mobj->oPossessedObject;

    // Mario
    if (mobj->oPossessionTimer >= 20) {
        m->pos[0] = pobj->oPosX;
        m->pos[1] = pobj->oPosY;
        m->pos[2] = pobj->oPosZ;
        m->vel[0] = pobj->oVelX;
        m->vel[1] = pobj->oVelY;
        m->vel[2] = pobj->oVelZ;
        m->forwardVel = pobj->oForwardVel;
        m->faceAngle[0] = -pobj->oFaceAnglePitch;
        m->faceAngle[1] = pobj->oFaceAngleYaw;
        m->faceAngle[2] = pobj->oFaceAngleRoll;
        m->marioObj->header.gfx.scale[0] = 1.f;
        m->marioObj->header.gfx.scale[1] = 1.f;
        m->marioObj->header.gfx.scale[2] = 1.f;
        m->squishTimer = 0;
        set_mario_animation(m, MARIO_ANIM_A_POSE);
        mobj->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
    }
    mobj->oPosX = m->pos[0];
    mobj->oPosY = m->pos[1];
    mobj->oPosZ = m->pos[2];
    mobj->oFaceAnglePitch = m->faceAngle[0];
    mobj->oFaceAngleYaw   = m->faceAngle[1];
    mobj->oFaceAngleRoll  = m->faceAngle[2];
    mobj->header.gfx.pos[0] = mobj->oPosX;
    mobj->header.gfx.pos[1] = mobj->oPosY + mobj->oGraphYOffset;
    mobj->header.gfx.pos[2] = mobj->oPosZ;
    mobj->header.gfx.angle[0] = mobj->oFaceAnglePitch & 0xFFFF;
    mobj->header.gfx.angle[1] = mobj->oFaceAngleYaw   & 0xFFFF;
    mobj->header.gfx.angle[2] = mobj->oFaceAngleRoll  & 0xFFFF;

    // Cappy
    if (cap != NULL) {
        Vec3f dv = { -pobj->oCappyLateralDist * pobj->header.gfx.scale[0], pobj->oCappyVerticalDist * pobj->header.gfx.scale[1], pobj->oCappyForwardDist * pobj->header.gfx.scale[0] };
        vec3f_rotate_zxy(dv, pobj->oFaceAnglePitch, pobj->oFaceAngleYaw, pobj->oFaceAngleRoll);
        cap->oPosX = pobj->oPosX + dv[0];
        cap->oPosY = pobj->oPosY + dv[1];
        cap->oPosZ = pobj->oPosZ + dv[2];
        cap->oFaceAnglePitch = pobj->oFaceAnglePitch + pobj->oCappyInitialPitch;
        cap->oFaceAngleYaw   = pobj->oFaceAngleYaw   + pobj->oCappyInitialYaw;
        cap->oFaceAngleRoll  = pobj->oFaceAngleRoll  + pobj->oCappyInitialRoll;
        cap->header.gfx.pos[0] = cap->oPosX;
        cap->header.gfx.pos[1] = cap->oPosY + cap->oGraphYOffset;
        cap->header.gfx.pos[2] = cap->oPosZ;
        cap->header.gfx.angle[0] = cap->oFaceAnglePitch & 0xFFFF;
        cap->header.gfx.angle[1] = cap->oFaceAngleYaw   & 0xFFFF;
        cap->header.gfx.angle[2] = cap->oFaceAngleRoll  & 0xFFFF;
        cap->header.gfx.scale[0] = (pobj->oCappyInitialScale / 100.f) * pobj->header.gfx.scale[0];
        cap->header.gfx.scale[1] = (pobj->oCappyInitialScale / 100.f) * pobj->header.gfx.scale[1];
        cap->header.gfx.scale[2] = (pobj->oCappyInitialScale / 100.f) * pobj->header.gfx.scale[2];
#ifdef SMO_SGI
        cap->header.gfx.scale[0] *= LUIGIS_CAPPY_SCALE_MULTIPLIER;
        cap->header.gfx.scale[1] *= LUIGIS_CAPPY_SCALE_MULTIPLIER;
        cap->header.gfx.scale[2] *= LUIGIS_CAPPY_SCALE_MULTIPLIER;
#endif
        cap->header.gfx.node.flags = GRAPH_RENDER_ACTIVE;
        cap->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
        cap->oOpacity = 0xFF;
    }
}

// Possession phases
// 10 frames / 1: Mario spins towards the object to possess
// 04 frames / 2: Mario stops, dive, rotates towards the ground and starts shrinking
// 06 frames / 3: while shrinking, Mario dives into the object

/*From, To, Coeff, Angle, Scale, Animation */
static const s32 sCappyPossessAnimParams[20][6] = {

    // Phase 1
    { 0, 1,  19, 0x0000, 100, MARIO_ANIM_FORWARD_SPINNING },
    { 0, 1,  36, 0x0000, 100, MARIO_ANIM_FORWARD_SPINNING },
    { 0, 1,  51, 0x0000, 100, MARIO_ANIM_FORWARD_SPINNING },
    { 0, 1,  64, 0x0000, 100, MARIO_ANIM_FORWARD_SPINNING },
    { 0, 1,  75, 0x0000, 100, MARIO_ANIM_FORWARD_SPINNING },
    { 0, 1,  84, 0x0000, 100, MARIO_ANIM_FORWARD_SPINNING },
    { 0, 1,  91, 0x0000, 100, MARIO_ANIM_FORWARD_SPINNING },
    { 0, 1,  96, 0x0000, 100, MARIO_ANIM_FORWARD_SPINNING },
    { 0, 1,  99, 0x0000, 100, MARIO_ANIM_FORWARD_SPINNING },
    { 0, 1, 100, 0x0000, 100, MARIO_ANIM_FORWARD_SPINNING },

    // Phase 2
    { 1, 1,   0, 0x0000, 100, MARIO_ANIM_DIVE },
    { 1, 1,   0, 0x1000,  95, MARIO_ANIM_DIVE },
    { 1, 1,   0, 0x2000,  90, MARIO_ANIM_DIVE },
    { 1, 1,   0, 0x3000,  85, MARIO_ANIM_DIVE },

    // Phase 3
    { 1, 2,   5, 0x4000,  80, MARIO_ANIM_DIVE },
    { 1, 2,  15, 0x4000,  70, MARIO_ANIM_DIVE },
    { 1, 2,  30, 0x4000,  60, MARIO_ANIM_DIVE },
    { 1, 2,  50, 0x4000,  50, MARIO_ANIM_DIVE },
    { 1, 2,  75, 0x4000,  40, MARIO_ANIM_DIVE },
    { 1, 2, 100, 0x4000,  30, MARIO_ANIM_DIVE },
};

#define xFrom(t)        (m->marioObj->oPossessAnimPos(sCappyPossessAnimParams[t][0], 0))
#define yFrom(t)        (m->marioObj->oPossessAnimPos(sCappyPossessAnimParams[t][0], 1))
#define zFrom(t)        (m->marioObj->oPossessAnimPos(sCappyPossessAnimParams[t][0], 2))
#define xTo(t)          (m->marioObj->oPossessAnimPos(sCappyPossessAnimParams[t][1], 0))
#define yTo(t)          (m->marioObj->oPossessAnimPos(sCappyPossessAnimParams[t][1], 1))
#define zTo(t)          (m->marioObj->oPossessAnimPos(sCappyPossessAnimParams[t][1], 2))
#define coeff(t)        ((f32)(sCappyPossessAnimParams[t][2]) / 100.f)
#define angle(t)        ((s16)(sCappyPossessAnimParams[t][3]))
#define scale(t)        ((f32)(sCappyPossessAnimParams[t][4]) / 100.f)
#define anim(t)         (sCappyPossessAnimParams[t][5])
#define lerp(t, a, b)   ((a) + ((b) - (a)) * (t));

s32 smo_act_possession(struct MarioState *m) {
    if (m->marioObj->oPossessionTimer < 20) {

        // Play the possession animation for 20 frames
        if (m->marioObj->oPossessionTimer == 0) {
            smo_play_sound_effect(SOUND_ACTION_CAPPY_1, m->marioObj->header.gfx.cameraToObject);
        }
        s32 t = m->marioObj->oPossessionTimer;
        m->pos[0] = lerp(coeff(t), xFrom(t), xTo(t));
        m->pos[1] = lerp(coeff(t), yFrom(t), yTo(t));
        m->pos[2] = lerp(coeff(t), zFrom(t), zTo(t));
        m->faceAngle[0] = angle(t);
        m->squishTimer = 0xFF;
        vec3f_set(m->marioObj->header.gfx.scale, scale(t), scale(t), scale(t));
        set_mario_animation(m, anim(t));
        m->particleFlags |= PARTICLE_SPARKLES;
        m->marioObj->oPossessionTimer++;
    
    } else {

        // Spawn Cappy if not spawned
        if (!m->marioObj->oObjectCap) {
#ifdef SMO_SGI
            if (getCharacterType() == LUIGI) {
                m->marioObj->oObjectCap = spawn_object(m->marioObj, MODEL_LUIGIS_CAP, bhvCappyLock);
            } else {
#endif
                m->marioObj->oObjectCap = spawn_object(m->marioObj, MODEL_MARIOS_CAP, bhvCappyLock);
#ifdef SMO_SGI
            }
#endif
            m->marioObj->oObjectCap->oFlags = 0;
            smo_obj_alloc_data(m->marioObj->oObjectCap, m);
        }

        // If Mario is locked, don't check [Z] press and zero-init inputs
        if (m->marioObj->oPossessionLock) {
            m->marioObj->oInputStickX        = 0;
            m->marioObj->oInputStickY        = 0;
            m->marioObj->oInputStickMag      = 0;
            m->marioObj->oInputStickYaw      = 0;
            m->marioObj->oInputButtonPressed = 0;
            m->marioObj->oInputButtonDown    = 0;
        } else {

            // Press [Z] to leave the object
            if (m->input & INPUT_Z_PRESSED) {
                if (mario_unpossess_object(m, MARIO_UNPOSSESS_ACT_JUMP_OUT, FALSE, 6)) {
                    return ACTION_RESULT_CANCEL;
                }
            }

            // Update inputs
            m->marioObj->oInputStickX        = (m->controller->stickX / 64.f);
            m->marioObj->oInputStickY        = (m->controller->stickY / 64.f);
            m->marioObj->oInputStickMag      = (m->controller->stickMag / 64.f);
            m->marioObj->oInputStickYaw      = m->intendedYaw;
            m->marioObj->oInputButtonPressed = m->controller->buttonPressed;
            m->marioObj->oInputButtonDown    = m->controller->buttonDown;
        }

        // Update possessed object
        struct PObject *pobj = m->marioObj->oPossessedObject;
        const PObjMainLoopFunc *f = pobj->smoData->captureData->mloop;
        pobj->oCappyForwardDist  = 0;
        pobj->oCappyLateralDist  = 0;
        pobj->oCappyVerticalDist = 0;
        pobj->oCappyInitialPitch = 0;
        pobj->oCappyInitialYaw   = 0;
        pobj->oCappyInitialRoll  = 0;
        pobj->oCappyInitialScale = 100;
        while (*f) {
            (*f)(pobj);
            if (m->action != ACT_SMO_POSSESSION) {
                return ACTION_RESULT_CANCEL;
            }
            f++;
        }

        // Debug only: use these fields to store the hitbox values (used later in smo_debug)
        DEBUG_ONLY(pobj->oStickX = pobj->hitboxRadius);
        DEBUG_ONLY(pobj->oStickY = pobj->hitboxHeight);
        DEBUG_ONLY(pobj->oStickMag = pobj->hitboxDownOffset);

        // Unset possessed object's hitbox to avoid undesirable interactions
        pobj->hitboxRadius = 0;
        pobj->hitboxHeight = 0;
        pobj->hurtboxRadius = 0;
        pobj->hurtboxHeight = 0;
        pobj->hitboxDownOffset = 0;
        pobj->oWallHitboxRadius = 0;
    }

    // Update Mario and Cappy's objects and graphics
    mario_cappy_update_object_and_gfx(m);
    return ACTION_RESULT_BREAK;
}

#undef xFrom
#undef yFrom
#undef zFrom
#undef xTo
#undef yTo
#undef zTo
#undef coeff
#undef angle
#undef scale
#undef anim
#undef lerp

//
// Possess
//

s32 mario_possess_object(struct MarioState *m, struct Object *obj) {
    if (!IS_SMO_CAPPY_CAPTURE) {
        return FALSE;
    }

    // Oof
    if (m->health < 0x100 || m->hurtCounter != 0) {
        return FALSE;
    }

    // Mario can't possess more than
    // one object at a time
    if (m->action == ACT_SMO_POSSESSION) {
        return FALSE;
    }

    // Try to capture
    const struct SmoCaptureData *captureData = cappy_capture(obj);
    if (captureData == NULL) {
        return FALSE;
    }

    // Try to init
    smo_obj_alloc_data(obj, m);
    obj->smoData->captureData = captureData;
    if (!obj->smoData->captureData->init(obj)) {
        smo_obj_free_data(obj);
        return FALSE;
    }

    // OK
    copy_object_data(obj);
    obj->behavior = bhvCappyLock;
    obj->curBhvCommand = bhvCappyLock;
    obj->oFlags = 0;
    obj->oIntangibleTimer = 0;
    m->marioObj->oPossessedObject = obj;
    m->marioObj->oObjectCap = NULL;
    m->marioObj->oPossessionTimer = 0;
    m->marioObj->oPossessionLock = FALSE;

    // Init animation positions
    f32 x = obj->oPosX;
    f32 y = obj->oPosY + obj->smoData->captureData->gettop(obj);
    f32 z = obj->oPosZ;
    m->marioObj->oPossessAnimPos(0, 0) = m->pos[0];
    m->marioObj->oPossessAnimPos(0, 1) = m->pos[1];
    m->marioObj->oPossessAnimPos(0, 2) = m->pos[2];
    m->marioObj->oPossessAnimPos(1, 0) = x;
    m->marioObj->oPossessAnimPos(1, 1) = y + 180.f;
    m->marioObj->oPossessAnimPos(1, 2) = z;
    m->marioObj->oPossessAnimPos(2, 0) = x;
    m->marioObj->oPossessAnimPos(2, 1) = y - 30.f;
    m->marioObj->oPossessAnimPos(2, 2) = z;

    // Init face angles
    m->faceAngle[0] = 0;
    m->faceAngle[1] = mario_obj_angle_to_object(m, obj);
    m->faceAngle[2] = 0;

    smo_capture_reset_camera(obj);
    set_mario_action(m, ACT_SMO_POSSESSION, 0);
    return TRUE;
}

//
// Unpossess
//

/* action, actionArg, forward vel, non-zero vertical vel, come from top, invert face yaw */
static const s32 sMarioUnpossessActions[MARIO_UNPOSSESS_ACT_COUNT * 6][6] = {

    /* MARIO_UNPOSSESS_ACT_JUMP_OUT */
    /* GF */ { ACT_SMO_LEAVE_OBJECT_JUMP,   0,  18, 42, TRUE,  FALSE },
    /* GB */ { ACT_SMO_LEAVE_OBJECT_JUMP,   0,  18, 42, TRUE,  FALSE },
    /* AF */ { ACT_SMO_LEAVE_OBJECT_JUMP,   0,  18, 42, TRUE,  FALSE },
    /* AB */ { ACT_SMO_LEAVE_OBJECT_JUMP,   0,  18, 42, TRUE,  FALSE },
    /* WF */ { ACT_SMO_LEAVE_OBJECT_WATER,  0,   0, 20, TRUE,  FALSE },
    /* WB */ { ACT_SMO_LEAVE_OBJECT_WATER,  0,   0, 20, TRUE,  FALSE },

    /* MARIO_UNPOSSESS_ACT_KNOCKED_BACK */
    /* GF */ { ACT_HARD_FORWARD_GROUND_KB,  1,  28,  0, FALSE, TRUE  },
    /* GB */ { ACT_HARD_BACKWARD_GROUND_KB, 1, -28,  0, FALSE, FALSE },
    /* AF */ { ACT_HARD_FORWARD_AIR_KB,     1,  28,  0, FALSE, TRUE  },
    /* AB */ { ACT_HARD_BACKWARD_AIR_KB,    1, -28,  0, FALSE, FALSE },
    /* WF */ { ACT_FORWARD_WATER_KB,        1,  28,  0, FALSE, TRUE  },
    /* WB */ { ACT_BACKWARD_WATER_KB,       1, -28,  0, FALSE, FALSE },

    /* MARIO_UNPOSSESS_ACT_KNOCKED_OUT_OF_KING_WHOMP */
    /* GF */ { ACT_HARD_BACKWARD_GROUND_KB, 1,  32, 16, TRUE,  TRUE  },
    /* GB */ { ACT_HARD_BACKWARD_GROUND_KB, 1,  32, 16, TRUE,  TRUE  },
    /* AF */ { ACT_HARD_BACKWARD_AIR_KB,    1,  32, 16, TRUE,  TRUE  },
    /* AB */ { ACT_HARD_BACKWARD_AIR_KB,    1,  32, 16, TRUE,  TRUE  },
    /* WF */ { ACT_BACKWARD_WATER_KB,       1,  32, 16, TRUE,  TRUE  },
    /* WB */ { ACT_BACKWARD_WATER_KB,       1,  32, 16, TRUE,  TRUE  },

    /* MARIO_UNPOSSESS_ACT_BURNT */
    /* GF */ { ACT_BURNING_JUMP,            1,  12, 34, TRUE,  FALSE },
    /* GB */ { ACT_BURNING_JUMP,            1,  12, 34, TRUE,  FALSE },
    /* AF */ { ACT_BURNING_JUMP,            1,  12, 34, TRUE,  FALSE },
    /* AB */ { ACT_BURNING_JUMP,            1,  12, 34, TRUE,  FALSE },
    /* WF */ { ACT_BURNING_JUMP,            1,  12, 34, TRUE,  FALSE },
    /* WB */ { ACT_BURNING_JUMP,            1,  12, 34, TRUE,  FALSE },

    /* MARIO_UNPOSSESS_ACT_GRABBED */
    /* GF */ { ACT_GRABBED,                 0,   0,  0, FALSE, FALSE },
    /* GB */ { ACT_GRABBED,                 0,   0,  0, FALSE, FALSE },
    /* AF */ { ACT_GRABBED,                 0,   0,  0, FALSE, FALSE },
    /* AB */ { ACT_GRABBED,                 0,   0,  0, FALSE, FALSE },
    /* WF */ { ACT_GRABBED,                 0,   0,  0, FALSE, FALSE },
    /* WB */ { ACT_GRABBED,                 0,   0,  0, FALSE, FALSE },

    /* MARIO_UNPOSSESS_ACT_EATEN */
    /* GF */ { ACT_EATEN_BY_BUBBA,          0,   0,  0, FALSE, FALSE },
    /* GB */ { ACT_EATEN_BY_BUBBA,          0,   0,  0, FALSE, FALSE },
    /* AF */ { ACT_EATEN_BY_BUBBA,          0,   0,  0, FALSE, FALSE },
    /* AB */ { ACT_EATEN_BY_BUBBA,          0,   0,  0, FALSE, FALSE },
    /* WF */ { ACT_EATEN_BY_BUBBA,          0,   0,  0, FALSE, FALSE },
    /* WB */ { ACT_EATEN_BY_BUBBA,          0,   0,  0, FALSE, FALSE },

    /* MARIO_UNPOSSESS_ACT_BLOWN */
    /* GF */ { ACT_GETTING_BLOWN,           0, -24, 12, FALSE, FALSE },
    /* GB */ { ACT_GETTING_BLOWN,           0, -24, 12, FALSE, FALSE },
    /* AF */ { ACT_GETTING_BLOWN,           0, -24, 12, FALSE, FALSE },
    /* AB */ { ACT_GETTING_BLOWN,           0, -24, 12, FALSE, FALSE },
    /* WF */ { ACT_GETTING_BLOWN,           0, -24, 12, FALSE, FALSE },
    /* WB */ { ACT_GETTING_BLOWN,           0, -24, 12, FALSE, FALSE },

    /* MARIO_UNPOSSESS_ACT_TORNADO */
    /* GF */ { ACT_TORNADO_TWIRLING,        0,   0,  0, FALSE, FALSE },
    /* GB */ { ACT_TORNADO_TWIRLING,        0,   0,  0, FALSE, FALSE },
    /* AF */ { ACT_TORNADO_TWIRLING,        0,   0,  0, FALSE, FALSE },
    /* AB */ { ACT_TORNADO_TWIRLING,        0,   0,  0, FALSE, FALSE },
    /* WF */ { ACT_TORNADO_TWIRLING,        0,   0,  0, FALSE, FALSE },
    /* WB */ { ACT_TORNADO_TWIRLING,        0,   0,  0, FALSE, FALSE },
};

s32 mario_unpossess_object(struct MarioState *m, u8 unpossessAct, s32 isBackwards, u32 objIntangibleFrames) {
    struct PObject *pobj = m->marioObj->oPossessedObject;
    if (pobj == NULL) {
        return FALSE;
    }

    // Unpossess action
    s32 terrainType = (obj_is_underwater(pobj, find_water_level(pobj->oPosX, pobj->oPosZ)) ? 2 : (obj_is_on_ground(pobj) ? 0 : 1));
    s32 actionIndex = (unpossessAct * 6) + (terrainType * 2) + isBackwards;
    u32 action      = (u32) sMarioUnpossessActions[actionIndex][0];
    u32 actionArg   = (u32) sMarioUnpossessActions[actionIndex][1];
    f32 forwardVel  = (f32) sMarioUnpossessActions[actionIndex][2];
    f32 verticalVel = (f32) sMarioUnpossessActions[actionIndex][3];
    s32 comeFromTop = (s32) sMarioUnpossessActions[actionIndex][4];
    s16 faceYaw     = (s16) sMarioUnpossessActions[actionIndex][5];

    mario_cappy_update_object_and_gfx(m);
    mario_set_forward_vel(m, forwardVel);
    if (verticalVel != 0.f) m->vel[1] = verticalVel;
    m->pos[1] += (comeFromTop * pobj->smoData->captureData->gettop(pobj));
    m->faceAngle[1] += faceYaw * 0x8000;
    m->input &= ~(INPUT_FIRST_PERSON | INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_B_PRESSED | INPUT_Z_PRESSED);
    m->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
    set_mario_action(m, action, actionArg);

    // Unload cap, restore camera
    smo_play_sound_effect(SOUND_ACTION_CAPPY_2, m->marioObj->header.gfx.cameraToObject);
    obj_spawn_white_puff(m->marioObj, -1);
    smo_obj_free_data(m->marioObj->oObjectCap);
    obj_mark_for_deletion(m->marioObj->oObjectCap);
    smo_capture_reset_camera(pobj);

    // End object possession
    restore_object_data(pobj);
    pobj->smoData->captureData->end(pobj);
    smo_obj_free_data(pobj);
    pobj->oIntangibleTimer = objIntangibleFrames;

    // Clear fields
    m->marioObj->oPossessedObject = NULL;
    m->marioObj->oObjectCap = NULL;
    m->marioObj->oPossessionTimer = 0;
    m->marioObj->oPossessionLock = FALSE;

    return TRUE;
}

