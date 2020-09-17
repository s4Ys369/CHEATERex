#include "../smo_c_includes.h"
#include "game/mario_actions_airborne.h"

#define MARIO_WALL_SLIDE_SPEED -16.f

static void smo_lava_boost_on_wall(struct MarioState *m) {
    m->faceAngle[1] = atan2s(m->wall->normal.z, m->wall->normal.x);
    m->forwardVel = MAX(24.f, m->forwardVel);
    if (!(m->flags & MARIO_METAL_CAP)) {
        m->hurtCounter += (m->flags & MARIO_CAP_ON_HEAD) ? 12 : 18;
    }

    play_sound(SOUND_MARIO_ON_FIRE, m->marioObj->header.gfx.cameraToObject);
    update_mario_sound_and_camera(m);
    drop_and_set_mario_action(m, ACT_LAVA_BOOST, 1);
}

//
// Actions
//

static s32 smo_act_jump(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_double_jump(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_triple_jump(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_backflip(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_long_jump(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_freefall(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_side_flip(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_wall_kick_air(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_water_jump(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_GROUND_POUND, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_steep_jump(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_ground_pound(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_burning_jump(struct MarioState *m) {
    if (SMO_HEALTH != 0 && m->marioObj->oMarioBurnTimer == 0) {
        m->hurtCounter += 1;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_burning_fall(struct MarioState *m) {
    if (SMO_HEALTH != 0 && m->marioObj->oMarioBurnTimer == 0) {
        m->hurtCounter += 1;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_backward_air_kb(struct MarioState *m) {
    if (SMO_MARIO == 1) {
        m->input &= ~INPUT_A_PRESSED;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_forward_air_kb(struct MarioState *m) {
    if (SMO_MARIO == 1) {
        m->input &= ~INPUT_A_PRESSED;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_soft_bonk(struct MarioState *m) {
    if (SMO_MARIO == 1) {
        m->input &= ~INPUT_A_PRESSED;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_air_hit_wall(struct MarioState *m) {
    if (SMO_MARIO == 1) {
        m->input &= ~INPUT_A_PRESSED;
        if (mario_check_wall_slide(m)) {
            set_mario_action(m, ACT_SMO_WALL_SLIDE, 0);
            return ACTION_RESULT_CANCEL;
        }
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_jump_kick(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_GROUND_POUND, SMO_MARIO == 1);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_flying(struct MarioState *m) {
    if (SMO_CAPPY != 0 && (m->controller->buttonPressed & SMO_CAPPY_BUTTON) && (m->input & INPUT_A_PRESSED)) {
        if (mario_spawn_cappy(m, CAPPY_ACT_FLAG_FLYING)) {
            play_sound(SOUND_MARIO_YAHOO_WAHA_YIPPEE + ((2 + (random_u16() % 3)) << 16), m->marioObj->header.gfx.cameraToObject);
        }
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_flying_triple_jump(struct MarioState *m) {
    if (SMO_MARIO == 1) {
        m->input &= ~INPUT_B_PRESSED;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_top_of_pole_jump(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_wall_slide(struct MarioState *m) {
    mario_set_forward_vel(m, 0);
    set_mario_animation(m, MARIO_ANIM_START_WALLKICK);
    play_sound(SOUND_MOVING_TERRAIN_SLIDE + m->terrainSoundAddend, m->marioObj->header.gfx.cameraToObject);
    m->vel[1] = MARIO_WALL_SLIDE_SPEED;
    m->particleFlags |= PARTICLE_DUST;

    // Wall jump
    if (m->input & INPUT_A_PRESSED) {
        m->vel[1] = 52.0f;
        m->faceAngle[1] += 0x8000;
        mario_set_forward_vel(m, 24.f);
        set_mario_action(m, ACT_WALL_KICK_AIR, 0);
        return ACTION_RESULT_CANCEL;
    }

    // Stop wall sliding
    if (m->input & INPUT_Z_PRESSED) {
        m->oWallSlide = FALSE;
        play_sound(SOUND_MARIO_UH, m->marioObj->header.gfx.cameraToObject);
        m->input &= (~(INPUT_Z_PRESSED));
        set_mario_action(m, ACT_FREEFALL, 0);
        return ACTION_RESULT_CANCEL;
    }

    // Cling Mario to the wall before performing the air step,
    // to avoid missing slightly slanted walls (normal.y near 0, but not 0)
    if (m->wall) {
        m->pos[0] -= m->wall->normal.x * 4.f;
        m->pos[2] -= m->wall->normal.z * 4.f;
    }
    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_IDLE, 0);
            return ACTION_RESULT_CANCEL;

        case AIR_STEP_NONE:
            set_mario_action(m, ACT_FREEFALL, 0);
            return ACTION_RESULT_CANCEL;

        case AIR_STEP_HIT_LAVA_WALL:
            smo_lava_boost_on_wall(m);
            return ACTION_RESULT_CANCEL;

        case AIR_STEP_HIT_WALL:
            if (!mario_check_wall_slide(m)) {
                set_mario_action(m, ACT_FREEFALL, 0);
                return ACTION_RESULT_CANCEL;
            }
            break;
    }

    // Turn Mario away from the wall
    m->marioObj->header.gfx.angle[1] = m->faceAngle[1] + 0x8000;
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_cappy_jump(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_GROUND_POUND, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, SOUND_MARIO_HOOHOO);
    common_air_action_step(m, ACT_DOUBLE_JUMP_LAND, ((m->vel[1] >= 0.0f) ? MARIO_ANIM_DOUBLE_JUMP_RISE : MARIO_ANIM_DOUBLE_JUMP_FALL), AIR_STEP_CHECK_LEDGE_GRAB | AIR_STEP_CHECK_HANG);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_ground_pound_jump(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_GROUND_POUND, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);

    m->particleFlags |= PARTICLE_SPARKLES;
    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, SOUND_MARIO_YAHOO);
    common_air_action_step(m, ACT_JUMP_LAND, MARIO_ANIM_TRIPLE_JUMP, AIR_STEP_CHECK_LEDGE_GRAB | AIR_STEP_CHECK_HANG);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_leave_object_jump(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_GROUND_POUND, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_AIRBORNE, SMO_CAPPY != 0);

    m->particleFlags |= PARTICLE_SPARKLES;
    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, 0);
    common_air_action_step(m, ACT_JUMP_LAND, MARIO_ANIM_SINGLE_JUMP, AIR_STEP_CHECK_LEDGE_GRAB | AIR_STEP_CHECK_HANG);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_cappy_throw_airborne(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_DIVE, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_JUMP_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_GROUND_POUND, SMO_MARIO == 1);
    update_mario_throw_anim(m, ACT_FREEFALL);
    
    // Update air movement
    f32 sidewaysSpeed = 0.0f;
    if (m->input & INPUT_NONZERO_ANALOG) {
        m->forwardVel += (m->intendedMag / 32.0f) * coss(m->intendedYaw - m->faceAngle[1]) * 1.5f;
        sidewaysSpeed  = (m->intendedMag / 32.0f) * sins(m->intendedYaw - m->faceAngle[1]) * 1.0f;
        if (SMO_MARIO == 1) {
            m->faceAngle[1] += (s16) (sidewaysSpeed * 1024.f);
            sidewaysSpeed = 0;
        }
    }
    m->forwardVel = approach_f32(m->forwardVel, 0.0f, 0.35f, 0.35f);
    m->vel[0] = m->slideVelX = (m->forwardVel * sins(m->faceAngle[1])) + (sidewaysSpeed * 10.f * sins(m->faceAngle[1] + 0x4000));
    m->vel[2] = m->slideVelZ = (m->forwardVel * coss(m->faceAngle[1])) + (sidewaysSpeed * 10.f * coss(m->faceAngle[1] + 0x4000));
    m->vel[1] += 2.f;

    // Perform air step
    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_FREEFALL_LAND, 0);
            break;

        case AIR_STEP_HIT_WALL:
            mario_set_forward_vel(m, 0.0f);
            break;
    }
    return ACTION_RESULT_CONTINUE;
}

s32 mario_check_smo_airborne_action(struct MarioState *m) {

    // Fall height
    if (SMO_MARIO == 1) {
        m->peakHeight = m->pos[1];
        if (m->vel[1] >= 0.f || m->action == ACT_FLYING || m->action == ACT_TWIRLING || m->action == ACT_SMO_WALL_SLIDE) {
            m->oPeakHeight = m->pos[1];
        }
        f32 fallHeight = m->oPeakHeight - m->pos[1];
        if ((fallHeight > SMO_FALL_DAMAGE_HEIGHT) && !(m->action & ACT_FLAG_INVULNERABLE) && !(m->flags & MARIO_UNKNOWN_18)) {
            play_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);
            m->flags |= MARIO_UNKNOWN_18;
        }
        gSpecialTripleJump = FALSE;
    }

    switch (m->action) {
        case ACT_JUMP:                      return smo_act_jump(m);
        case ACT_DOUBLE_JUMP:               return smo_act_double_jump(m);
        case ACT_TRIPLE_JUMP:               return smo_act_triple_jump(m);
        case ACT_BACKFLIP:                  return smo_act_backflip(m);
        case ACT_LONG_JUMP:                 return smo_act_long_jump(m);
        case ACT_FREEFALL:                  return smo_act_freefall(m);
        case ACT_SIDE_FLIP:                 return smo_act_side_flip(m);
        case ACT_WALL_KICK_AIR:             return smo_act_wall_kick_air(m);
        case ACT_WATER_JUMP:                return smo_act_water_jump(m);
        case ACT_STEEP_JUMP:                return smo_act_steep_jump(m);
        case ACT_GROUND_POUND:              return smo_act_ground_pound(m);
        case ACT_BURNING_JUMP:              return smo_act_burning_jump(m);
        case ACT_BURNING_FALL:              return smo_act_burning_fall(m);
        case ACT_BACKWARD_AIR_KB:           return smo_act_backward_air_kb(m);
        case ACT_FORWARD_AIR_KB:            return smo_act_forward_air_kb(m);
        case ACT_SOFT_BONK:                 return smo_act_soft_bonk(m);
        case ACT_AIR_HIT_WALL:              return smo_act_air_hit_wall(m);
        case ACT_JUMP_KICK:                 return smo_act_jump_kick(m);
        case ACT_FLYING:                    return smo_act_flying(m);
        case ACT_FLYING_TRIPLE_JUMP:        return smo_act_flying_triple_jump(m);
        case ACT_TOP_OF_POLE_JUMP:          return smo_act_top_of_pole_jump(m);
        case ACT_SMO_WALL_SLIDE:            return smo_act_wall_slide(m);
        case ACT_SMO_CAPPY_JUMP:            return smo_act_cappy_jump(m);
        case ACT_SMO_GROUND_POUND_JUMP:     return smo_act_ground_pound_jump(m);
        case ACT_SMO_LEAVE_OBJECT_JUMP:     return smo_act_leave_object_jump(m);
        case ACT_SMO_CAPPY_THROW_AIRBORNE:  return smo_act_cappy_throw_airborne(m);
    }

    return ACTION_RESULT_CONTINUE;
}
