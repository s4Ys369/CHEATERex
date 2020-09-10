#include "../smo_c_includes.h"

static s32 mario_throw_cappy(struct MarioState *m, u32 action) {
    u32 throwType;
    switch (action) {
        case ACT_SMO_CAPPY_THROW_GROUND:
            throwType = mario_spawn_cappy(m, CAPPY_ACT_FLAG_GROUND);
            if (throwType) {
                set_mario_action(m, ACT_SMO_CAPPY_THROW_GROUND, throwType);
                m->faceAngle[1] = m->intendedYaw;
                return TRUE;
            }
            break;

        case ACT_SMO_CAPPY_THROW_AIRBORNE:
            throwType = mario_spawn_cappy(m, CAPPY_ACT_FLAG_AIRBORNE);
            if (throwType) {
                set_mario_action(m, ACT_SMO_CAPPY_THROW_AIRBORNE, throwType);
                mario_set_forward_vel(m, MIN(m->forwardVel, 8.f));
                m->vel[1] = 16.f;
                m->faceAngle[1] = m->intendedYaw;
                return TRUE;
            }
            break;

        case ACT_SMO_CAPPY_THROW_WATER:
            throwType = mario_spawn_cappy(m, CAPPY_ACT_FLAG_WATER);
            if (throwType) {
                set_mario_action(m, ACT_SMO_CAPPY_THROW_WATER, throwType);
                return TRUE;
            }
            break;

        case ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR:
            throwType = mario_spawn_cappy(m, CAPPY_ACT_FLAG_METAL_WATER);
            if (throwType) {
                set_mario_action(m, ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR, throwType);
                m->faceAngle[1] = m->intendedYaw;
                return TRUE;
            }
            break;

        case ACT_SMO_CAPPY_THROW_METAL_WATER_AIR:
            throwType = mario_spawn_cappy(m, CAPPY_ACT_FLAG_METAL_WATER);
            if (throwType) {
                set_mario_action(m, ACT_SMO_CAPPY_THROW_METAL_WATER_AIR, throwType);
                mario_set_forward_vel(m, MIN(m->forwardVel, 8.f));
                m->vel[1] = 16.f;
                m->faceAngle[1] = m->intendedYaw;
                return TRUE;
            }
            break;
    }
    return FALSE;
}

s32 mario_set_smo_action(struct MarioState *m, u32 action) {
    switch (action) {
    
        /* Cappy throws */
        case ACT_SMO_CAPPY_THROW_GROUND:
        case ACT_SMO_CAPPY_THROW_AIRBORNE:
        case ACT_SMO_CAPPY_THROW_WATER:
        case ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR:
        case ACT_SMO_CAPPY_THROW_METAL_WATER_AIR:
            if (m->controller->buttonPressed & X_BUTTON) {
                if (mario_throw_cappy(m, action)) {
                    return TRUE;
                }
            }
            break;

        /* Roll */
        case ACT_SMO_ROLLING:
            if (m->input & INPUT_B_PRESSED) {
                set_mario_action(m, ACT_SMO_ROLLING, 0);
                set_mario_animation(m, MARIO_ANIM_FORWARD_SPINNING);
                mario_set_forward_vel(m, 56.f);
                m->particleFlags |= PARTICLE_HORIZONTAL_STAR;
                return TRUE;
            }
            break;

        /* Ground pound jump */
        case ACT_SMO_GROUND_POUND_JUMP:
            if (m->input & INPUT_A_PRESSED) {
                set_mario_action(m, ACT_SMO_GROUND_POUND_JUMP, 0);
                m->vel[1] = 66.f;
                return TRUE;
            }
            break;

        /* Moving punch */
        case ACT_MOVE_PUNCHING:
            if (m->input & INPUT_B_PRESSED) {
                set_mario_action(m, ACT_MOVE_PUNCHING, 0);
                return TRUE;
            }
            break;

        /* Air dive */
        case ACT_DIVE:
            if ((m->input & INPUT_B_PRESSED) && ((m->input & INPUT_Z_PRESSED) || (m->action == ACT_GROUND_POUND))) {
                set_mario_action(m, ACT_DIVE, 0);
                mario_set_forward_vel(m, 32.f);
                m->vel[1] = 40.f;
                m->particleFlags |= PARTICLE_MIST_CIRCLE;
                return TRUE;
            }
            break;

        /* Jump kick */
        case ACT_JUMP_KICK:
            if (m->input & INPUT_B_PRESSED) {
                set_mario_action(m, ACT_JUMP_KICK, 0);
                return TRUE;
            }
            break;

        /* Ground pound */
        case ACT_GROUND_POUND:
            if (m->input & INPUT_Z_PRESSED) {
                set_mario_action(m, ACT_GROUND_POUND, 0);
                return TRUE;
            }
            break;

        /* Water descent */
        case ACT_SMO_WATER_DESCENT:
            if (m->input & INPUT_Z_PRESSED) {
                set_mario_action(m, ACT_SMO_WATER_DESCENT, 0);
                return TRUE;
            }
            break;


        /* Water dash */
        case ACT_SMO_WATER_DASH:
            if ((m->input & INPUT_B_PRESSED) && ((m->input & INPUT_Z_PRESSED) || (m->action == ACT_SMO_WATER_DESCENT))) {
                set_mario_action(m, ACT_SMO_WATER_DASH, 0);
                return TRUE;
            }
            break;

        /* Metal water punch */
        case ACT_SMO_METAL_WATER_PUNCH:
            if (m->input & INPUT_B_PRESSED) {
                set_mario_action(m, ACT_SMO_METAL_WATER_PUNCH, 0);
                play_sound(SOUND_MARIO_PUNCH_YAH, m->marioObj->header.gfx.cameraToObject);
                m->marioObj->header.gfx.unk38.animID = -1;
                set_mario_animation(m, MARIO_ANIM_FIRST_PUNCH);
                return TRUE;
            }
            break;

        /* Metal water kick */
        case ACT_SMO_METAL_WATER_KICK:
            if (m->input & INPUT_B_PRESSED) {
                set_mario_action(m, ACT_SMO_METAL_WATER_KICK, 0);
                play_sound_if_no_flag(m, SOUND_MARIO_PUNCH_HOO, MARIO_ACTION_SOUND_PLAYED);
                m->marioObj->header.gfx.unk38.animID = -1;
                set_mario_animation(m, MARIO_ANIM_AIR_KICK);
                m->vel[1] = 16.f;
                return TRUE;
            }
            break;
    }
    return FALSE;
}

s32 mario_check_wall_slide(struct MarioState *m) {
    // Wall slide available
    if (!IS_SMO_MARIO || !m->marioObj->oWallSlide) {
        return FALSE;
    }

    // There must be a wall
    if (m->wall == NULL) {
        return FALSE;
    }

    // Not a painting
    s16 stype = m->wall->type;
    if (stype >= SURFACE_PAINTING_WOBBLE_A6 && stype <= SURFACE_WOBBLING_WARP) {
        return FALSE;
    }

    // Mario must not be holding something
    if (m->heldObj != NULL) {
        return FALSE;
    }

    return TRUE;
}

static const s32 sThrowSounds[] = {
    SOUND_MARIO_PUNCH_YAH,
    SOUND_MARIO_PUNCH_WAH,
    SOUND_MARIO_PUNCH_HOO,
    SOUND_MARIO_YAHOO_WAHA_YIPPEE + (0 << 16),  // Yahoo
    SOUND_MARIO_YAHOO_WAHA_YIPPEE + (3 << 16),  // Waha
    SOUND_MARIO_YAHOO_WAHA_YIPPEE + (4 << 16),  // Yippee
};

void update_mario_throw_anim(struct MarioState *m, u32 actEnd) {
    switch (m->actionArg) {

        // Cappy normal throw
        case 1:
            m->marioObj->header.gfx.unk38.animID = -1;
            play_sound(sThrowSounds[random_u16() % 3], m->marioObj->header.gfx.cameraToObject);
            set_mario_anim_with_accel(m, MARIO_ANIM_GROUND_THROW, 0x18000);
            // Fall-through:
        case 3:
            m->actionArg = 3;
            if (is_anim_past_end(m)) {
                set_mario_action(m, actEnd, 0);
            }
            break;

        // Cappy spin throw
        case 2:
            m->marioObj->header.gfx.unk38.animID = -1;
            play_sound(sThrowSounds[3 + (random_u16() % 3)], m->marioObj->header.gfx.cameraToObject);
            set_mario_animation(m, MARIO_ANIM_STAR_DANCE);
            m->marioObj->header.gfx.unk38.animFrame = 4;
            // Fall-through:
        case 4:
            m->actionArg = 4;
            if (m->marioObj->header.gfx.unk38.animFrame >= 20) {
                set_mario_action(m, actEnd, 0);
            }
            break;
    }
}

//
// Execution
//

#include "game/mario_actions_stationary.h"
#include "game/mario_actions_moving.h"
#include "game/mario_actions_airborne.h"
#include "game/mario_actions_submerged.h"
#include "game/mario_actions_cutscene.h"
#include "game/mario_actions_automatic.h"
#include "game/mario_actions_object.h"

static s32 (*sCheckActionFunction[])(struct MarioState *) = {
    mario_check_smo_stationary_action,
    mario_check_smo_moving_action,
    mario_check_smo_airborne_action,
    mario_check_smo_submerged_action,
    mario_check_smo_cutscene_action,
    NULL,
    NULL
};

static s32 (*sExecuteActionFunction[])(struct MarioState *) = {
    mario_execute_stationary_action,
    mario_execute_moving_action,
    mario_execute_airborne_action,
    mario_execute_submerged_action,
    mario_execute_cutscene_action,
    mario_execute_automatic_action,
    mario_execute_object_action,
};

void mario_execute_action(struct MarioState *m) {
    for (;;) {
        s32 group = ((m->action & ACT_GROUP_MASK) >> 6);
        s32 actResult = ACTION_RESULT_CONTINUE;
        s32 (*check)(struct MarioState *) = sCheckActionFunction[group];
        s32 (*exec)(struct MarioState *) = sExecuteActionFunction[group];

        // Check SMO actions
        if (check != NULL) {
            actResult = check(m);
        }

        // Execute SM64 actions
        if (actResult == ACTION_RESULT_CONTINUE) {
            actResult = exec(m);
        }
        
        // Cancel? -> loop again
        // Break? -> end loop
        if (actResult == ACTION_RESULT_CANCEL) {
            continue;
        } else if (actResult == ACTION_RESULT_BREAK) {
            break;
        }
    }
}
