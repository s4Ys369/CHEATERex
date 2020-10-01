#include "../smo_c_includes.h"

//
// Actions
//

static s32 smo_act_idle(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_start_sleeping(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_sleeping(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_coughing(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_shivering(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_standing_against_wall(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_in_quicksand(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_panting(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_crouching(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_ROLLING, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_braking_stop(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_jump_land_stop(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_double_jump_land_stop(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_side_flip_land_stop(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_freefall_land_stop(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_triple_jump_land_stop(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_backflip_land_stop(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_long_jump_land_stop(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_ground_pound_land(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_GROUND_POUND_JUMP, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_ROLLING, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, SMO_CAPPY != 0);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_shocked_from_high_fall(struct MarioState *m) {
    if (m->actionState == 0) {
        m->particleFlags |= PARTICLE_MIST_CIRCLE;
        set_mario_animation(m, MARIO_ANIM_SHOCKED);
        play_sound_if_no_flag(m, SOUND_MARIO_ATTACKED, MARIO_ACTION_SOUND_PLAYED);
        play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
        m->actionState = 1;
    } else {
        m->actionTimer++;
    }

    mario_set_forward_vel(m, 0.0f);
    stop_and_set_height_to_floor(m);

    if (m->actionTimer < 30) {
        set_camera_shake_from_hit(SHAKE_SHOCK);
    }
    if (m->actionTimer >= 45) {
        set_mario_action(m, ACT_IDLE, 0);
        return ACTION_RESULT_CANCEL;
    }
    return ACTION_RESULT_CONTINUE;
}

s32 mario_check_smo_stationary_action(struct MarioState *m) {
    m->oCappyJumped = FALSE;
    m->oWallSlide = TRUE;

    // Fall damage
    if (SMO_MARIO == 1) {
        f32 fallHeight = m->oPeakHeight - m->pos[1];
        m->peakHeight = m->pos[1];
        m->oPeakHeight = m->pos[1];
        if ((fallHeight > SMO_FALL_DAMAGE_HEIGHT) && (m->vel[1] < -50.0f) && (m->action != ACT_TWIRLING) && (m->floor->type != SURFACE_BURNING)) {
            drop_and_set_mario_action(m, ACT_SMO_SHOCKED_FROM_HIGH_FALL, 0);
            return ACTION_RESULT_CANCEL;
        }
    }

    switch (m->action) {
        case ACT_IDLE:                          return smo_act_idle(m);
        case ACT_START_SLEEPING:                return smo_act_start_sleeping(m);
        case ACT_SLEEPING:                      return smo_act_sleeping(m);
        case ACT_COUGHING:                      return smo_act_coughing(m);
        case ACT_SHIVERING:                     return smo_act_shivering(m);
        case ACT_STANDING_AGAINST_WALL:         return smo_act_standing_against_wall(m);
        case ACT_IN_QUICKSAND:                  return smo_act_in_quicksand(m);
        case ACT_PANTING:                       return smo_act_panting(m);
        case ACT_CROUCHING:                     return smo_act_crouching(m);
        case ACT_BRAKING_STOP:                  return smo_act_braking_stop(m);
        case ACT_JUMP_LAND_STOP:                return smo_act_jump_land_stop(m);
        case ACT_DOUBLE_JUMP_LAND_STOP:         return smo_act_double_jump_land_stop(m);
        case ACT_SIDE_FLIP_LAND_STOP:           return smo_act_side_flip_land_stop(m);
        case ACT_FREEFALL_LAND_STOP:            return smo_act_freefall_land_stop(m);
        case ACT_TRIPLE_JUMP_LAND_STOP:         return smo_act_triple_jump_land_stop(m);
        case ACT_BACKFLIP_LAND_STOP:            return smo_act_backflip_land_stop(m);
        case ACT_LONG_JUMP_LAND_STOP:           return smo_act_long_jump_land_stop(m);
        case ACT_GROUND_POUND_LAND:             return smo_act_ground_pound_land(m);
        case ACT_SMO_SHOCKED_FROM_HIGH_FALL:    return smo_act_shocked_from_high_fall(m);
    }

    return ACTION_RESULT_CONTINUE;
}
