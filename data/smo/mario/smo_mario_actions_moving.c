#include "../smo_c_includes.h"

#define ROLLING_MAX_DURATION 16
#define ROLLING_START_REPEAT 8

static void smo_update_facing_yaw(struct MarioState *m) {
    // More responsive controls
    if (IS_SMO_MARIO) {
        m->faceAngle[1] = m->intendedYaw - approach_s32((s16)(m->intendedYaw - m->faceAngle[1]), 0, 0x800, 0x800);
    }
}

//
// Actions
//

static s32 smo_act_walking(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_MOVE_PUNCHING, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    smo_update_facing_yaw(m);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_hold_walking(struct MarioState *m) {
    smo_update_facing_yaw(m);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_hold_heavy_walking(struct MarioState *m) {
    smo_update_facing_yaw(m);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_turning_around(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_MOVE_PUNCHING, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_finish_turning_around(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    smo_update_facing_yaw(m);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_braking(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_decelerating(struct MarioState *m) {
    if (!(m->input & INPUT_FIRST_PERSON)) {
        RETURN_CANCEL_IF_ACTION_SET(ACT_MOVE_PUNCHING, IS_SMO_MARIO);
        RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_crawling(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_ROLLING, IS_SMO_MARIO);
    smo_update_facing_yaw(m);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_crouch_slide(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_ROLLING, IS_SMO_MARIO);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_jump_land(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_freefall_land(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_double_jump_land(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_side_flip_land(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_triple_jump_land(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_backflip_land(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_quicksand_jump_land(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_long_jump_land(struct MarioState *m) {
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_GROUND, IS_SMO_CAPPY_AVAILABLE);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_burning_ground(struct MarioState *m) {
    if (IS_SMO_HEALTH && m->marioObj->oMarioBurnTimer == 0) {
        m->hurtCounter += 1;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_rolling(struct MarioState *m) {

    if (m->actionTimer >= ROLLING_MAX_DURATION) {
        set_mario_action(m, ACT_WALKING, 0);
        return ACTION_RESULT_CANCEL;
    }

    if (m->actionTimer >= ROLLING_START_REPEAT && (m->controller->buttonDown & Z_TRIG)) {
        RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_ROLLING, IS_SMO_MARIO);
    }

    if (m->input & INPUT_A_PRESSED) {
        set_jump_from_landing(m);
        return ACTION_RESULT_CANCEL;
    }

    if ((m->actionTimer % 8) == 0) {
        play_sound(SOUND_ACTION_TWIRL, m->marioObj->header.gfx.cameraToObject);
    }

    m->actionTimer++;
    set_mario_animation(m, MARIO_ANIM_FORWARD_SPINNING);
    mario_set_forward_vel(m, 60.f);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
            set_mario_action(m, ACT_FREEFALL, 0);
            return ACTION_RESULT_CANCEL;

        case GROUND_STEP_HIT_WALL:
            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            set_mario_action(m, ACT_BACKWARD_GROUND_KB, 0);
            return ACTION_RESULT_CANCEL;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_cappy_throw_ground(struct MarioState *m) {
    update_mario_throw_anim(m, ACT_IDLE);
    mario_set_forward_vel(m, m->forwardVel * 0.85f);
    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            m->forwardVel = 0;
            break;
    }
    return ACTION_RESULT_CONTINUE;
}

s32 mario_check_smo_moving_action(struct MarioState *m) {
    m->marioObj->oCappyJumped = FALSE;
    m->marioObj->oWallSlide = TRUE;

    // Fall damage
    if (IS_SMO_MARIO) {
        f32 fallHeight = m->marioObj->oPeakHeight - m->pos[1];
        m->peakHeight = m->pos[1];
        m->marioObj->oPeakHeight = m->pos[1];
        if ((fallHeight > SMO_FALL_DAMAGE_HEIGHT) && (m->vel[1] < -50.0f) && (m->action != ACT_TWIRLING) && (m->floor->type != SURFACE_BURNING)) {
            drop_and_set_mario_action(m, ACT_SMO_SHOCKED_FROM_HIGH_FALL, 0);
            return ACTION_RESULT_CANCEL;
        }
    }

    switch (m->action) {
        case ACT_WALKING:                   return smo_act_walking(m);    
        case ACT_HOLD_WALKING:              return smo_act_hold_walking(m);    
        case ACT_HOLD_HEAVY_WALKING:        return smo_act_hold_heavy_walking(m);    
        case ACT_TURNING_AROUND:            return smo_act_turning_around(m);           
        case ACT_FINISH_TURNING_AROUND:     return smo_act_finish_turning_around(m);
        case ACT_BRAKING:                   return smo_act_braking(m);
        case ACT_CRAWLING:                  return smo_act_crawling(m);         
        case ACT_DECELERATING:              return smo_act_decelerating(m);             
        case ACT_CROUCH_SLIDE:              return smo_act_crouch_slide(m);             
        case ACT_JUMP_LAND:                 return smo_act_jump_land(m);                
        case ACT_FREEFALL_LAND:             return smo_act_freefall_land(m);            
        case ACT_DOUBLE_JUMP_LAND:          return smo_act_double_jump_land(m);         
        case ACT_SIDE_FLIP_LAND:            return smo_act_side_flip_land(m);
        case ACT_TRIPLE_JUMP_LAND:          return smo_act_triple_jump_land(m);         
        case ACT_BACKFLIP_LAND:             return smo_act_backflip_land(m);            
        case ACT_QUICKSAND_JUMP_LAND:       return smo_act_quicksand_jump_land(m);
        case ACT_LONG_JUMP_LAND:            return smo_act_long_jump_land(m);
        case ACT_BURNING_GROUND:            return smo_act_burning_ground(m);
        case ACT_SMO_ROLLING:               return smo_act_rolling(m);
        case ACT_SMO_CAPPY_THROW_GROUND:    return smo_act_cappy_throw_ground(m);
    }

    return ACTION_RESULT_CONTINUE;
}
