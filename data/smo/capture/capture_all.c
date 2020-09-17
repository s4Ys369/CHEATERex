#include "../smo_c_includes.h"
#include <string.h>
#ifdef BETTERCAMERA
#include "game/bettercamera.h"
#endif

//
// Capture Data
// These values are for an Object of scale 1.0f
//

#define ID(name) #name, cappy_##name##_init, cappy_##name##_end, cappy_##name##_get_top, cappy_##name##_mloop
struct SmoCaptureData sSmoCaptureData[] = {
    { ID(goomba),       12.f, 24.f,  24.f, 25.f, -40.f, -2.65f,  60.f,  65.f,   0.f,  50.f },
    { ID(koopa),        16.f, 24.f,  32.f, 35.f, -48.f, -2.65f,  60.f,  40.f,   0.f,  50.f },
    { ID(bobomb),       22.f, 22.f,  22.f, 36.f, -60.f, -4.00f,  65.f, 100.f,   0.f,  50.f },
    { ID(bobomb_buddy),  0.f,  0.f,   0.f,  0.f,   0.f,  0.00f,   0.f,   0.f,   0.f,   0.f },
    { ID(bobomb_king),   0.f,  0.f,   0.f,  0.f,   0.f,  0.00f,   0.f,   0.f,   0.f,   0.f },
    { ID(chain_chomp),  15.f, 15.f,  45.f, 15.f, -36.f, -2.00f, 100.f, 160.f,   0.f,  80.f },

    { ID(bullet_bill),  75.f, 75.f, 150.f,  0.f,   0.f,  0.00f, 200.f, 400.f, 200.f, 200.f },
    { ID(hoot),         20.f, 20.f,  20.f, 24.f, -16.f, -2.00f,  75.f, 100.f,   0.f,  60.f },
    { ID(whomp),        12.f, 12.f,  12.f, 40.f, -80.f, -4.00f, 100.f, 400.f,   0.f, 100.f },
    { ID(whomp_king),    0.f,  0.f,   0.f,  0.f,   0.f,  0.00f,   0.f,   0.f,   0.f,   0.f },

};
static const u32 sSmoCaptureDataCount = sizeof(sSmoCaptureData) / sizeof(sSmoCaptureData[0]);
#undef ID

//
// Capture Binds
//

struct SmoCaptureBind {
    const BehaviorScript *behavior;
    const char *id;
};

#define BIND(bhv, name) { bhv, #name }
static const struct SmoCaptureBind sSmoCaptureBinds[] = {

    /* Bob-omb Battlefield */
    BIND(bhvGoomba, goomba),
    BIND(bhvKoopa, koopa),
    BIND(bhvBobomb, bobomb),
    BIND(bhvBobombBuddy, bobomb_buddy),
    BIND(bhvBobombBuddyOpensCannon, bobomb_buddy),
    BIND(bhvKingBobomb, bobomb_king),
    BIND(bhvChainChomp, chain_chomp),
    BIND(bhvChainChompFreed, chain_chomp),

    /* Whomp Fortress */
    BIND(bhvBulletBill, bullet_bill),
    BIND(bhvHoot, hoot),
    BIND(bhvSmallWhomp, whomp),
    BIND(bhvWhompKingBoss, whomp_king),

};
static const u32 sSmoCaptureBindCount = sizeof(sSmoCaptureBinds) / sizeof(sSmoCaptureBinds[0]);
#undef BIND

//
// Possessed object stuff
//

void pobj_decelerate(struct PObject *pobj) {
    if (obj_is_on_ground(pobj) && pobj->oFloor->type == SURFACE_ICE) {
        pobj->oVelX *= 0.95f;
        pobj->oVelZ *= 0.95f;
        pobj->oForwardVel *= 0.95f;
    } else {
        pobj->oVelX *= 0.90f;
        pobj->oVelZ *= 0.90f;
        pobj->oForwardVel *= 0.90f;
    }
}

void pobj_apply_gravity(struct PObject *pobj) {
    pobj->oVelY += pobj->cdGravity;
    if (pobj->oVelY < pobj->cdTerminalVelocity) {
        pobj->oVelY = pobj->cdTerminalVelocity;
    }
}

void pobj_handle_special_floors(struct PObject *pobj) {
    pobj->oFloorType = POBJ_FLOOR_TYPE_NONE;

    // Out of bounds
    if (pobj->oFloor == NULL) {
        mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_JUMP_OUT, FALSE, 6);
        obj_instant_death(pobj);
        return;
    }

    // Squished
    if (pobj->oSquishedTimer >= 6) {
        set_camera_shake_from_hit(SHAKE_SMALL_DAMAGE);
        mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_KNOCKED_BACK, TRUE, 15);
        return;
    }

    // On ground
    s32 isOnGround = obj_is_on_ground(pobj);
    if (isOnGround) {
        pobj->oFloorType = POBJ_FLOOR_TYPE_GROUND;
    }

    // Above water
    f32 waterLevel = find_water_level(pobj->oPosX, pobj->oPosZ);
    if (!obj_is_underwater(pobj, waterLevel) && !(pobj->oProperties & POBJ_PROP_ABOVE_WATER)) {
        mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_JUMP_OUT, FALSE, 6);
        obj_instant_death(pobj);
        return;
    }

    // Walk/Slide on water
    if (pobj->oPosY <= waterLevel && (pobj->oProperties & POBJ_PROP_WALK_ON_WATER)) {
        pobj->oPosY = waterLevel;
        pobj->oVelY = MAX(pobj->oVelY, 0.f);
        pobj->oFloorHeight = 0.f;
        pobj->oFloorType = POBJ_FLOOR_TYPE_WATER;
    }
    
    // Underwater
    else if (obj_is_underwater(pobj, waterLevel) && !(pobj->oProperties & POBJ_PROP_UNDERWATER)) {
        pobj->oPosY = waterLevel + MAX(((pobj->hitboxHeight / 2) - pobj->hitboxDownOffset), 60.f);
        mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_JUMP_OUT, FALSE, 6);
        obj_instant_death(pobj);
        return;
    }

    // Special floors
    switch (pobj->oFloor->type) {
        case SURFACE_DEATH_PLANE:
        case SURFACE_VERTICAL_WIND:
            if (pobj->oFloorHeight <= 2048.f) {
                mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_JUMP_OUT, FALSE, 6);
                obj_instant_death(pobj);
            }
            break;

        case SURFACE_INSTANT_QUICKSAND:
        case SURFACE_INSTANT_MOVING_QUICKSAND:
            if (pobj->oFloorHeight <= 10.f && !(pobj->oProperties & POBJ_PROP_IMMUNE_TO_QUICKSANDS)) {
                mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_JUMP_OUT, FALSE, 6);
                obj_instant_death(pobj);
            }
            break;

        case SURFACE_BURNING:
            if (pobj->oFloorHeight <= 10.f && !(pobj->oProperties & POBJ_PROP_IMMUNE_TO_LAVA)) {
                mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_JUMP_OUT, FALSE, 6);
                obj_instant_death(pobj);
            } else if (isOnGround) {
                pobj->oFloorType = POBJ_FLOOR_TYPE_LAVA;
            }
            break;
    }
}

//
// Hits
//

#define INT_STATUS_ATTACKED         (0x02 | INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED)
#define INT_STATUS_HIT_FROM_ABOVE   (0x03 | INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED)
#define INT_STATUS_HIT_FROM_BELOW   (0x06 | INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED)

static s32 is_backwards(struct PObject *pobj, struct Object *obj) {
    s16 yawPobjToObj = obj_get_object1_angle_yaw_to_object2(pobj, obj);
    s32 yawDiff = (s32)((s16)((s32)(yawPobjToObj) - (s32)(pobj->oFaceAngleYaw)));
    pobj->oFaceAngleYaw = yawPobjToObj;
    return (-0x4000 <= yawDiff) && (yawDiff <= +0x4000);
}

//
// Interactions
//

static struct PObject *sPObj;
static struct Object *sObj;
static s32 sOverlapHitbox;
static s32 sOverlapHurtbox;

static s32 pobj_push_out_of_object_hitbox() {
    if (sOverlapHitbox) {

        // Push
        f32 dx = sPObj->oPosX - sObj->oPosX;
        f32 dz = sPObj->oPosZ - sObj->oPosZ;
        f32 d = sqrtf(sqr(dx) + sqr(dz));
        if (d == 0.f) {
            // This is unlikely, but we must
            // prevent a div/0 error
            return FALSE;
        }
        f32 diff = (sPObj->hitboxRadius + sObj->hitboxRadius) - d;
        f32 pushX = (dx / d) * diff;
        f32 pushZ = (dz / d) * diff;

        // Makes all SURFACE and POLELIKE objects unpushable, and ignore POBJ_PROP_UNPUSHABLE property
        if (obj_is_surface(sObj) || !(sPObj->oProperties & POBJ_PROP_UNPUSHABLE)) {
            sPObj->oPosX += pushX;
            sPObj->oPosZ += pushZ;
        } else {
            sObj->oPosX -= pushX;
            sObj->oPosZ -= pushZ;
        }
        return TRUE;
    }
    return FALSE;
}

static s32 pobj_push_out_of_object_hurtbox() {
    if (sOverlapHurtbox) {

        // Push
        f32 dx = sPObj->oPosX - sObj->oPosX;
        f32 dz = sPObj->oPosZ - sObj->oPosZ;
        f32 d = sqrtf(sqr(dx) + sqr(dz));
        if (d == 0.f) {
            // This is unlikely, but we must
            // prevent a div/0 error
            return FALSE;
        }
        f32 diff = (sPObj->hitboxRadius + (sObj->hurtboxRadius > 0 ? sObj->hurtboxRadius : sObj->hitboxRadius)) - d;
        f32 pushX = (dx / d) * diff;
        f32 pushZ = (dz / d) * diff;

        // Makes all SURFACE and POLELIKE objects unpushable, and ignore POBJ_PROP_UNPUSHABLE property
        if (obj_is_surface(sObj) || !(sPObj->oProperties & POBJ_PROP_UNPUSHABLE)) {
            sPObj->oPosX += pushX;
            sPObj->oPosZ += pushZ;
        } else {
            sObj->oPosX -= pushX;
            sObj->oPosZ -= pushZ;
        }
        return TRUE;
    }
    return FALSE;
}

static s32 pobj_interact_coin() {
    if (sOverlapHitbox) {

        // Add coins to Mario's
        sPObj->oMario->numCoins += sObj->oDamageOrCoinValue;
        sPObj->oMario->healCounter += 4 * sObj->oDamageOrCoinValue;
        sObj->oInteractStatus = INT_STATUS_INTERACTED;

        // Spawn 100 coins star
        if (COURSE_IS_MAIN_COURSE(gCurrCourseNum)
            && sPObj->oMario->numCoins - sObj->oDamageOrCoinValue < 100
            && sPObj->oMario->numCoins >= 100) {
            struct Object *star = spawn_object(sObj, MODEL_STAR, bhvSpawnedStarNoLevelExit);
            star->oBehParams = (6 << 24);
            star->oInteractionSubtype = INT_SUBTYPE_NO_EXIT;
            obj_set_angle(star, 0, 0, 0);
        }

        return TRUE;
    }
    return FALSE;
}

static s32 pobj_mario_take_damage_and_unpossess() {

    // No damage, push out of object
    if (sOverlapHurtbox) {
        if (sObj->oDamageOrCoinValue == 0 || (sPObj->oProperties & POBJ_PROP_INVULNERABLE)) {
            return pobj_push_out_of_object_hurtbox();
        }
    }

    // Damage Mario
    if (sOverlapHurtbox) {
        sObj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_ATTACKED_MARIO;
        sPObj->oMario->hurtCounter += 4 * sObj->oDamageOrCoinValue;
        sPObj->oMario->interactObj = sObj;
        set_camera_shake_from_hit(SHAKE_SMALL_DAMAGE + MIN(sObj->oDamageOrCoinValue / 2, 2));
        mario_unpossess_object(sPObj->oMario, MARIO_UNPOSSESS_ACT_KNOCKED_BACK, is_backwards(sPObj, sObj), 15);
        return TRUE;
    }

    return FALSE;
}

static s32 pobj_interact_bounce_top() {

    // Hit and bounce
    if (sOverlapHitbox) {
        if (obj_is_object2_hit_from_above(sPObj, sObj)) {
            sObj->oInteractStatus = INT_STATUS_HIT_FROM_ABOVE;
            sPObj->oPosY = sObj->oPosY + sObj->hitboxHeight - sObj->hitboxDownOffset + 20.f;
            sPObj->oVelY = 20.f * sqrtf(absx(sPObj->smoData->captureData->gravity * sPObj->oScaleY));
            play_sound(SOUND_ACTION_BOUNCE_OFF_OBJECT, sPObj->header.gfx.cameraToObject);
            return TRUE;
        }
    }

    // Attack
    if (sOverlapHitbox) {
        if (sPObj->oProperties & (POBJ_PROP_ATTACK | POBJ_PROP_STRONG_ATTACK)) {
            sObj->oInteractStatus = INT_STATUS_ATTACKED;
            return TRUE;
        }
    }

    // Damage Mario
    if (sOverlapHurtbox) {
        return pobj_mario_take_damage_and_unpossess(sPObj, sObj, sPObj->oMario);
    }

    return FALSE;
}

static s32 pobj_interact_hit_from_below() {
    
    // Hit from below
    if (sOverlapHitbox) {
        if (obj_is_object2_hit_from_below(sPObj, sObj)) {
            sObj->oInteractStatus = INT_STATUS_HIT_FROM_BELOW;
            sPObj->oVelY = 0;
            play_sound(SOUND_ACTION_BOUNCE_OFF_OBJECT, sPObj->header.gfx.cameraToObject);
            set_camera_shake_from_hit(SHAKE_HIT_FROM_BELOW);
            return TRUE;
        }
    }

    // Hit from above
    if (sOverlapHitbox) {
        return pobj_interact_bounce_top(sPObj, sObj, sPObj->oMario);
    }

    return FALSE;
}

static s32 pobj_mario_burn_and_unpossess() {

    // Burn Mario
    if (sOverlapHurtbox) {

        // No damage if underwater or immune
        if (obj_is_underwater(sPObj, find_water_level(sPObj->oPosX, sPObj->oPosZ)) || (sPObj->oProperties & POBJ_PROP_IMMUNE_TO_FIRE)) {
            return FALSE;
        }

        sObj->oInteractStatus = INT_STATUS_INTERACTED;
        sPObj->oMario->interactObj = sObj;
        sPObj->oMario->marioObj->oMarioBurnTimer = 0;
        play_sound(SOUND_MARIO_ON_FIRE, sPObj->oMario->marioObj->header.gfx.cameraToObject);
        mario_unpossess_object(sPObj->oMario, MARIO_UNPOSSESS_ACT_BURNT, FALSE, 15);
        return TRUE;
    }

    return FALSE;
}

static s32 pobj_interact_bully() {

    // Bully
    if (sOverlapHitbox) {
        f32 knockback = 3600.f / sObj->hitboxRadius;
        s16 angle = obj_get_object1_angle_yaw_to_object2(sPObj, sObj);

        // Bully the bully if sPObj attacks or is unpushable
        if (sPObj->oProperties & (POBJ_PROP_UNPUSHABLE | POBJ_PROP_ATTACK | POBJ_PROP_STRONG_ATTACK)) {
            // sPObj
            sPObj->oUnresponsiveTimer = 6;
            sPObj->oFaceAngleYaw = angle;
            sPObj->oMoveAngleYaw = angle;
            obj_set_forward_vel(sPObj, sPObj->oFaceAngleYaw, 0.2f, -knockback);
            pobj_push_out_of_object_hitbox();

            // Bully
            sObj->oFaceAngleYaw = angle + 0x8000;
            sObj->oMoveAngleYaw = angle;
            sObj->oForwardVel = knockback;
            sObj->oInteractStatus = INT_STATUS_ATTACKED;
            play_sound(SOUND_OBJ_BULLY_METAL, sObj->header.gfx.cameraToObject);

        } else {
            // sPObj
            sPObj->oUnresponsiveTimer = 30;
            sPObj->oFaceAngleYaw = angle;
            sPObj->oMoveAngleYaw = angle;
            obj_set_forward_vel(sPObj, sPObj->oFaceAngleYaw, 1.f, -knockback);
            pobj_push_out_of_object_hitbox();
            play_sound(sPObj->oDeathSound, sPObj->header.gfx.cameraToObject);

            // Bully
            sObj->oFaceAngleYaw = angle + 0x8000;
            sObj->oMoveAngleYaw = angle + 0x8000;
            sObj->oInteractStatus = INT_STATUS_INTERACTED;
            play_sound(SOUND_OBJ_BULLY_METAL, sObj->header.gfx.cameraToObject);
        }
        return TRUE;
    }

    return FALSE;
}

static s32 pobj_interact_breakable() {
    
    // Hit from below
    if (sOverlapHitbox) {
        if (obj_is_object2_hit_from_below(sPObj, sObj)) {
            sObj->oInteractStatus = INT_STATUS_HIT_FROM_BELOW;
            sPObj->oVelY = 0;
            set_camera_shake_from_hit(SHAKE_HIT_FROM_BELOW);
            return TRUE;
        }
    }

    // Strong attack
    if (sOverlapHitbox) {
        if (sPObj->oProperties & POBJ_PROP_STRONG_ATTACK) {
            sObj->oInteractStatus = INT_STATUS_ATTACKED;
            set_camera_shake_from_hit(SHAKE_ATTACK);
            return TRUE;
        }
    }

    // Push
    if (sOverlapHitbox) {
        return pobj_push_out_of_object_hitbox();
    }

    return FALSE;
}

static s32 pobj_interact_grabbable() {
    
    // Is attackable (bob-ombs)
    if (sOverlapHitbox) {
        if ((sObj->oInteractionSubtype & INT_SUBTYPE_KICKABLE) && (sPObj->oProperties & (POBJ_PROP_ATTACK | POBJ_PROP_STRONG_ATTACK))) {
            sObj->oInteractStatus = INT_STATUS_ATTACKED;
            return FALSE;
        }
    }

    // Is destructible (small boxes)
    if (sOverlapHitbox) {
        if (sPObj->oProperties & POBJ_PROP_STRONG_ATTACK) {
            if (sObj->behavior == bhvBreakableBoxSmall) {
                sObj->oNumLootCoins = 3;
                obj_spawn_white_puff(sObj, SOUND_GENERAL_BREAK_BOX);
                obj_spawn_triangle_break_particles(sObj, 20, 138, 0.7f, 3);
                obj_spawn_loot_yellow_coins(sObj, 3, 20.0f);
                obj_mark_for_deletion(sObj);
                return TRUE;
            }
        }
    }
    
    // Grabs Mario out of his possession
    if (sOverlapHitbox) {
        if ((sObj->oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO) && obj_is_object1_facing_object2(sObj, sPObj, 0x2800)) {
            sObj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_GRABBED_MARIO;
            sPObj->oFaceAngleYaw = sObj->oMoveAngleYaw;
            sPObj->oMario->interactObj = sObj;
            sPObj->oMario->usedObj = sObj;
            play_sound(SOUND_MARIO_OOOF, sPObj->oMario->marioObj->header.gfx.cameraToObject);
            mario_unpossess_object(sPObj->oMario, MARIO_UNPOSSESS_ACT_GRABBED, FALSE, 15);
            return TRUE;
        }
    }

    // Damage Mario
    if (sOverlapHurtbox) {
        return pobj_mario_take_damage_and_unpossess(sPObj, sObj, sPObj->oMario);
    }

    return FALSE;
}

static s32 pobj_interact_clam_or_bubba() {

    // Eats Mario and his possession
    if (sOverlapHurtbox) {
        if (sObj->oInteractionSubtype & INT_SUBTYPE_EATS_MARIO) {
            sObj->oInteractStatus = INT_STATUS_INTERACTED;
            sPObj->oMario->interactObj = sObj;
            mario_unpossess_object(sPObj->oMario, MARIO_UNPOSSESS_ACT_EATEN, FALSE, 15);
            obj_instant_death(sPObj);
            return TRUE;
        }
    }

    // Damage Mario
    if (sOverlapHurtbox) {
        return pobj_mario_take_damage_and_unpossess(sPObj, sObj, sPObj->oMario);
    }

    return FALSE;
}

static s32 pobj_interact_strong_wind() {

    // Blow Mario away, out of his possession
    if (sOverlapHitbox) {

        // SL walking penguin
        if (sPObj->oProperties & POBJ_PROP_RESIST_STRONG_WINDS) {
            sObj->oIntangibleTimer = -1;
            obj_mark_for_deletion(sObj);
            return FALSE;
        }

        sObj->oInteractStatus = INT_STATUS_INTERACTED;
        sPObj->oMario->interactObj = sObj;
        sPObj->oMario->usedObj = sObj;
        sPObj->oMario->unkC4 = 0.4f;
        sPObj->oFaceAngleYaw = sObj->oMoveAngleYaw + 0x8000;
        play_sound(SOUND_MARIO_WAAAOOOW, sPObj->oMario->marioObj->header.gfx.cameraToObject);
        mario_unpossess_object(sPObj->oMario, MARIO_UNPOSSESS_ACT_BLOWN, FALSE, 15);
        return TRUE;
    }
    
    return FALSE;
}

static s32 pobj_interact_tornado() {

    // Suck Mario out of his possession
    if (sOverlapHitbox) {

        // Same flag used to resist tornados
        if (sPObj->oProperties & POBJ_PROP_RESIST_STRONG_WINDS) {
            return FALSE;
        }

        sObj->oInteractStatus = INT_STATUS_INTERACTED;
        sPObj->oMario->interactObj = sObj;
        sPObj->oMario->usedObj = sObj;
        sPObj->oMario->marioObj->oMarioTornadoYawVel = 0x400;
        sPObj->oMario->marioObj->oMarioTornadoPosY = sPObj->oPosY - sObj->oPosY;
        play_sound(SOUND_MARIO_WAAAOOOW, sPObj->oMario->marioObj->header.gfx.cameraToObject);
        mario_unpossess_object(sPObj->oMario, MARIO_UNPOSSESS_ACT_TORNADO, FALSE, 15);
        return TRUE;
    }

    return FALSE;
}

s32 pobj_process_interaction(struct PObject *pobj, struct Object *obj, u32 interactType) {

    // Set up statics
    sPObj = pobj;
    sObj  = obj;
    sOverlapHitbox  = obj_detect_overlap(pobj, obj, OBJ_OVERLAP_FLAG_HITBOX, OBJ_OVERLAP_FLAG_HITBOX);
    sOverlapHurtbox = obj_detect_overlap(pobj, obj, OBJ_OVERLAP_FLAG_HITBOX, OBJ_OVERLAP_FLAG_HURTBOX_HITBOX_IF_ZERO);

    // Interaction
    switch (interactType) {
        case INTERACT_COIN:
            return pobj_interact_coin();

        case INTERACT_SNUFIT_BULLET:
        case INTERACT_MR_BLIZZARD:
        case INTERACT_UNKNOWN_08:
        case INTERACT_SHOCK:
        case INTERACT_DAMAGE:
            return pobj_mario_take_damage_and_unpossess();

        case INTERACT_KOOPA:
        case INTERACT_BOUNCE_TOP:
        case INTERACT_BOUNCE_TOP2:
            return pobj_interact_bounce_top();

        case INTERACT_HIT_FROM_BELOW:
            return pobj_interact_hit_from_below();

        case INTERACT_FLAME:
            return pobj_mario_burn_and_unpossess();

        case INTERACT_BULLY:
            return pobj_interact_bully();

        case INTERACT_BREAKABLE:
            return pobj_interact_breakable();

        case INTERACT_GRABBABLE:
            return pobj_interact_grabbable();

        case INTERACT_CLAM_OR_BUBBA:
            return pobj_interact_clam_or_bubba();

        case INTERACT_STRONG_WIND:
            return pobj_interact_strong_wind();

        case INTERACT_TORNADO:
            return pobj_interact_tornado();

        case INTERACT_DOOR:
        case INTERACT_WARP_DOOR:
        case INTERACT_POLE:
        case INTERACT_IGLOO_BARRIER:
        case INTERACT_KOOPA_SHELL:
        case INTERACT_UNKNOWN_31:
            return pobj_push_out_of_object_hitbox();

        case INTERACT_CAP:
        case INTERACT_WARP:
        case INTERACT_STAR_OR_KEY:
        case INTERACT_WATER_RING:
        case INTERACT_WHIRLPOOL:
        case INTERACT_TEXT:
        case INTERACT_BBH_ENTRANCE:
        case INTERACT_CANNON_BASE:
        default:
            break; // Do nothing
    }
    return FALSE;
}

//
// Dialog
//

enum {
    DIALOG_STATE_NONE,
    DIALOG_STATE_BEGIN,
    DIALOG_STATE_WAIT,
    DIALOG_STATE_END
};
static u8 sDialogState = DIALOG_STATE_NONE;
static s16 sDialogId = -1;

s32 dialog_start(s16 dialogId) {
    if (sDialogState != DIALOG_STATE_NONE || dialogId < 0) {
        return FALSE;
    }
    sDialogId = dialogId;
    sDialogState = DIALOG_STATE_BEGIN;
    return TRUE;
}

s32 dialog_update() {
    switch (sDialogState) {
        case DIALOG_STATE_NONE:
            return TRUE;

        case DIALOG_STATE_BEGIN:
            create_dialog_box(sDialogId);
            sDialogState = DIALOG_STATE_WAIT;
            break;

        case DIALOG_STATE_WAIT:
            if (get_dialog_id() < 0) {
                sDialogState = DIALOG_STATE_END;
            }
            break;

        case DIALOG_STATE_END:
            sDialogId = -1;
            sDialogState = DIALOG_STATE_NONE;
            break;
    }
    return FALSE;
}

enum {
    CUTSCENE_STATE_NONE,
    CUTSCENE_STATE_BEGIN,
    CUTSCENE_STATE_WAIT,
    CUTSCENE_STATE_END
};
static u8 sCutsceneState = CUTSCENE_STATE_NONE;
static u8 sCutsceneId = 0;
static struct Object *sCutsceneObject = NULL;

s32 cutscene_start(u8 cutsceneId, struct Object* obj) {
    if (sCutsceneState != CUTSCENE_STATE_NONE || cutsceneId == 0 || obj == NULL) {
        return FALSE;
    }
    sCutsceneId = cutsceneId;
    sCutsceneObject = obj;
    sCutsceneState = CUTSCENE_STATE_BEGIN;
    return TRUE;
}

s32 cutscene_update() {
    switch (sCutsceneState) {
        case CUTSCENE_STATE_NONE:
            return TRUE;

        case CUTSCENE_STATE_BEGIN:
            if (cutscene_object(sCutsceneId, sCutsceneObject) == 1) {
                sCutsceneState = CUTSCENE_STATE_WAIT;
            } else {
                sCutsceneState = CUTSCENE_STATE_END;
            }
            break;

        case CUTSCENE_STATE_WAIT:
            if (cutscene_object(sCutsceneId, sCutsceneObject) == -1) {
                sCutsceneState = CUTSCENE_STATE_END;
            }
            break;

        case CUTSCENE_STATE_END:
            sCutsceneId = 0;
            sCutsceneObject = NULL;
            sCutsceneState = CUTSCENE_STATE_NONE;
            break;
    }
    return FALSE;
}

//
// Camera
//

void smo_capture_set_camera_behind_mario(struct PObject *pobj) {
    if (pobj->oMario->area->camera->mode != CAMERA_MODE_BEHIND_MARIO) {
#ifndef BETTERCAMERA
        set_camera_mode(pobj->oMario->area->camera, CAMERA_MODE_BEHIND_MARIO, 1);
#else
        if (newcam_active == 0) {
            set_camera_mode(pobj->oMario->area->camera, CAMERA_MODE_BEHIND_MARIO, 1);
        } else {
            pobj->oCameraBehindMario = TRUE;
            pobj->oMario->area->camera->mode = CAMERA_MODE_NEWCAM;
            gLakituState.mode = CAMERA_MODE_NEWCAM;
        }
#endif
    }
}

void smo_capture_reset_camera(struct PObject *pobj) {
    if (pobj->oMario->area->camera->mode == CAMERA_MODE_BEHIND_MARIO) {
#ifndef BETTERCAMERA
        set_camera_mode(pobj->oMario->area->camera, pobj->oMario->area->camera->defMode, 1);
#else
        if (newcam_active == 0) {
            set_camera_mode(pobj->oMario->area->camera, pobj->oMario->area->camera->defMode, 1);
        } else {
            pobj->oCameraBehindMario = FALSE;
            pobj->oMario->area->camera->mode = CAMERA_MODE_NEWCAM;
            gLakituState.mode = CAMERA_MODE_NEWCAM;
        }
#endif
    }
}

//
// Capture
//

const struct SmoCaptureData *cappy_capture(struct Object *obj) {
    const BehaviorScript *behavior = obj->behavior;
	for (u32 i = 0; i != sSmoCaptureBindCount; ++i) {
        if (behavior == sSmoCaptureBinds[i].behavior) {
            for (u32 j = 0; j != sSmoCaptureDataCount; ++j) {
                if (strcmp(sSmoCaptureBinds[i].id, sSmoCaptureData[j].id) == 0) {
                    return &sSmoCaptureData[j];
                }
            }
            return NULL; // Uh oh, looks like I forgot an Id
        }
	}
    return NULL;
}

