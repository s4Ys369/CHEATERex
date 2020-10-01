#include "../smo_c_includes.h"

//
// List of enemies attackable by Cappy (behaviors)
//

static const BehaviorScript* sAttackableBehaviors[] = {
    bhvPiranhaPlant,
    bhvBoo,
    bhvBooInCastle,
    bhvBooWithCage,
    bhvBalconyBigBoo,
    bhvMerryGoRoundBigBoo,
    bhvGhostHuntBigBoo,
    bhvMerryGoRoundBoo,
    bhvGhostHuntBoo,
    bhvHauntedBookshelf,
    bhvScuttlebug,
    bhvMoneybag,
    bhvMoneybagHidden,
    bhvKoopa,
    bhvPokey,
    bhvPokeyBodyPart,
    bhvSwoop,
    bhvFlyGuy,
    bhvGoomba,
    bhvEnemyLakitu,
    bhvSpiny,
    bhvMontyMole,
    bhvFlyingBookend,
    bhvFirePiranhaPlant,
    bhvSnufit,
    bhvKlepto,
    bhvSkeeter,
    bhvSmallBully,
    bhvBigBully,
    bhvBigBullyWithMinions,
    bhvSmallChillBully,
    bhvBigChillBully,
    bhvSpindrift
};
static const u32 sAttackableCount = sizeof(sAttackableBehaviors) / sizeof(const BehaviorScript*);

//
// Bullies
//

static const BehaviorScript* sBullyBehaviors[] = {
    bhvSmallBully,
    bhvBigBully,
    bhvBigBullyWithMinions,
    bhvSmallChillBully,
    bhvBigChillBully
};
static const u32 sBullyCount = sizeof(sBullyBehaviors) / sizeof(const BehaviorScript*);

//
// Collision handler
//

static s32 cappy_detect_hitbox_overlap(struct Object *cappy, struct Object *obj) {
    if (obj->hitboxRadius == 0 || obj->hitboxHeight == 0) {
        return FALSE;
    }

    // Radius check
    f32 dx = cappy->oPosX - obj->oPosX;
    f32 dz = cappy->oPosZ - obj->oPosZ;
    f32 collisionRadius2 = (cappy->hitboxRadius + obj->hitboxRadius) * (cappy->hitboxRadius + obj->hitboxRadius);
    f32 distance2 = (dx * dx + dz * dz);
    if (distance2 > collisionRadius2) {
        return FALSE;
    }

    // Height check
    f32 capLowerBound = cappy->oPosY - cappy->hitboxDownOffset;
    f32 capUpperBound = cappy->hitboxHeight + capLowerBound;
    f32 objLowerBound = obj->oPosY - obj->hitboxDownOffset;
    f32 objUpperBound = obj->hitboxHeight + objLowerBound;
    f32 sumOfHeights = cappy->hitboxHeight + obj->hitboxHeight;
    if (((objUpperBound - capLowerBound) > sumOfHeights) ||
        ((capUpperBound - objLowerBound) > sumOfHeights)) {
        return FALSE;
    }

    return TRUE;
}

static s32 is_coin_or_secret(struct Object *obj) {

    // Coin
    if (obj->oInteractType & INTERACT_COIN) {
        return TRUE;
    }

    // Star secret
    if (obj->behavior == bhvHiddenStarTrigger) {
        return TRUE;
    }

    return FALSE;
}

static s32 is_grabbable(struct Object *obj) {

    // Not Bowser
    if (obj->behavior == bhvBowser) {
        return FALSE;
    }

    // Not that @#%! hat stealer ukiki
    if (obj->behavior == bhvUkiki && obj->oBehParams2ndByte == UKIKI_HAT) {
        return FALSE;
    }
    if (obj->behavior == bhvMacroUkiki && obj->oBehParams2ndByte == UKIKI_HAT) {
        return FALSE;
    }

    // Not Heave ho
    if (obj->oInteractionSubtype & INT_SUBTYPE_NOT_GRABBABLE) {
        return FALSE;
    }

    // Grabbable
    if (obj->oInteractType & INTERACT_GRABBABLE) {
        return TRUE;
    }

    return FALSE;
}

static s32 is_attackable(struct Object *obj) {
    for (u32 i = 0; i != sAttackableCount; ++i) {
        if (obj->behavior == sAttackableBehaviors[i]) {
            return TRUE;
        }
    }
    return FALSE;
}

static s32 is_bully(struct Object *obj) {
    for (u32 i = 0; i != sBullyCount; ++i) {
        if (obj->behavior == sBullyBehaviors[i]) {
            return TRUE;
        }
    }
    return FALSE;
}

static s32 cappy_give_collectible_to_mario(UNUSED struct Object *cappy, struct MarioState *m, struct Object *obj) {

    // Teleports collectible directly to Mario
    // The object is then collected on the very next frame
    obj->oPosX = m->marioObj->oPosX;
    obj->oPosY = m->marioObj->oPosY + 80;
    obj->oPosZ = m->marioObj->oPosZ;
    return TRUE;
}

static s32 cappy_try_to_grab_object(struct Object *cappy, struct MarioState *m, struct Object *obj) {

    // Even if an object is grabbable by Cappy,
    // the actual grab is performed only by grounded Mario
    if (!(m->action & (ACT_FLAG_AIR | ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER))) {
        m->input |= INPUT_INTERACT_OBJ_GRABBABLE;
        m->interactObj = obj;
        m->usedObj = obj;
        m->vel[0] = 0;
        m->vel[1] = 0;
        m->vel[2] = 0;
        set_mario_animation(m, MARIO_ANIM_FIRST_PUNCH);
        set_mario_action(m, ACT_PICKING_UP, 0);
        cappy_start_return_to_mario(cappy);
        return TRUE;
    }
    return FALSE;
}

static s32 cappy_attack_object(struct Object *cappy, UNUSED struct MarioState *m, struct Object *obj) {
    
    // Attack
    if (is_bully(obj)) {
        obj->oMoveAngleYaw = atan2s(cappy->oPosZ, cappy->oPosX);
        obj->oForwardVel = 3392.0f / obj->hitboxRadius;
    }
    obj->oInteractStatus = (0x02 | INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED);

    // Return on contact
    if (m->oCappyBehavior == 1) {
        spawn_object(cappy, MODEL_NONE, bhvHorStarParticleSpawner);
        play_sound(SOUND_OBJ_DEFAULT_DEATH, cappy->header.gfx.cameraToObject);
        cappy_start_return_to_mario(cappy);
        return TRUE;
    }

    return FALSE;    
}

static s32 cappy_process_collision(struct Object *cappy, struct MarioState *m, struct Object *obj) {

    // Collectible
    if (is_coin_or_secret(obj)) {
        cappy_give_collectible_to_mario(cappy, m, obj);
        return FALSE;
    }

    // Capturable
    if (mario_possess_object(m, obj)) {
        mario_unload_cappy(m);
        return TRUE;
    }

    // Grabbable
    if (is_grabbable(obj)) {
        return cappy_try_to_grab_object(cappy, m, obj);
    }

    // Attackable
    if (is_attackable(obj)) {
        return cappy_attack_object(cappy, m, obj);
    }

    return FALSE;
}

static s32 cappy_process_collision_in_list(struct Object *cappy, struct MarioState *m, struct Object *obj, struct ObjectNode *list) {
    while (obj != NULL) {
        if (obj != cappy && obj->oIntangibleTimer == 0 &&
            cappy_detect_hitbox_overlap(cappy, obj) &&
            cappy_process_collision(cappy, m, obj)) {
            return TRUE;
        }
        obj = obj_get_next(list, obj);
    }
    return FALSE;
}

static s32 check_cappy_interact_with_mario(struct Object *cappy, struct MarioState *m) {

    // Is Cappy tangible for Mario?
    if (!m->oCappyInteractMario || m->oCappyJumped) {
        return FALSE;
    }

    // Intangible/Underwater (non Metal) Mario can't interact with Cappy
    if (m->action & (ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE | ACT_FLAG_SWIMMING)) {
        return FALSE;
    }

    // Hitbox overlapping
    if (!cappy_detect_hitbox_overlap(cappy, m->marioObj)) {
        return FALSE;
    }

    // Counts as a Cappy jump only
    // if Mario is airborne
    if (m->action & ACT_FLAG_AIR) {
        if (SMO_UCJ == 0) {
            m->oCappyJumped = TRUE;
        }
    }

    if (m->action & ACT_FLAG_METAL_WATER) {
        set_mario_action(m, ACT_METAL_WATER_JUMP, 0);
        m->vel[1] = 32.0f;
    } else {
        set_mario_action(m, ACT_SMO_CAPPY_JUMP, 0);
        mario_set_forward_vel(m, m->forwardVel * 0.8f);
        m->vel[1] = 56.0f;
    }

    spawn_object(m->marioObj, MODEL_NONE, bhvHorStarParticleSpawner);
    play_sound(SOUND_GENERAL_BOING1, m->marioObj->header.gfx.cameraToObject);
    cappy_start_return_to_mario(cappy);
    return TRUE;
}

static void cappy_process_object_collision(struct Object *cappy, struct MarioState *m) {
    static s8 sObjTypeInteractCappy[] = {
        OBJ_LIST_SURFACE,
        OBJ_LIST_POLELIKE,
        OBJ_LIST_PUSHABLE,
        OBJ_LIST_GENACTOR,
        OBJ_LIST_DESTRUCTIVE,
        OBJ_LIST_LEVEL,
        -1
    };

    // Mario
    if (check_cappy_interact_with_mario(cappy, m)) {
        return;
    }

    // Surface objects
    if (m->oCappyWallObject && cappy_process_collision(cappy, m, m->oCappyWallObject)) {
        return;
    }
    if (m->oCappyFloorObject && cappy_process_collision(cappy, m, m->oCappyFloorObject)) {
        return;
    }
    if (m->oCappyCeilObject && cappy_process_collision(cappy, m, m->oCappyCeilObject)) {
        return;
    }

    // Objects
    s8 *type = sObjTypeInteractCappy;
    for (; *type != -1; ++type) {
        struct ObjectNode *list = &gObjectLists[*type];
        if (cappy_process_collision_in_list(cappy, m, obj_get_first(list), list)) {
            return;
        }
    }
}
