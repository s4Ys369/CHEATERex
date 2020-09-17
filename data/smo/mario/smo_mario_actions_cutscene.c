#include "../smo_c_includes.h"

static s32 smo_launch_mario_until_land(struct MarioState *m, s32 endAction, s32 animation, f32 forwardVel) {
    mario_set_forward_vel(m, forwardVel);
    set_mario_animation(m, animation);
    s32 airStepLanded = (perform_air_step(m, 0) == AIR_STEP_LANDED);
    if (airStepLanded) {
        set_mario_action(m, endAction, 0);
    }
    return airStepLanded;
}

//
// Actions
//

static s32 smo_act_exit_airborne(struct MarioState *m) {
<<<<<<< HEAD
    if (SMO_HEALTH != 0) {
=======
    if (IS_SMO_HEALTH) {
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
        if (15 < m->actionTimer++ && smo_launch_mario_until_land(m, ACT_EXIT_LAND_SAVE_DIALOG, MARIO_ANIM_GENERAL_FALL, -32.0f)) {
            smo_set_mario_to_full_health(m);
        }
        m->marioObj->header.gfx.angle[1] += 0x8000;
        m->particleFlags |= PARTICLE_SPARKLES;
        return ACTION_RESULT_BREAK;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_falling_exit_airborne(struct MarioState *m) {
<<<<<<< HEAD
    if (SMO_HEALTH != 0) {
=======
    if (IS_SMO_HEALTH) {
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
        if (smo_launch_mario_until_land(m, ACT_EXIT_LAND_SAVE_DIALOG, MARIO_ANIM_GENERAL_FALL, 0.0f)) {
            smo_set_mario_to_full_health(m);
        }
        m->marioObj->header.gfx.angle[1] += 0x8000;
        m->particleFlags |= PARTICLE_SPARKLES;
        return ACTION_RESULT_BREAK;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_death_exit(struct MarioState *m) {
<<<<<<< HEAD
    if (SMO_HEALTH != 0) {
=======
    if (IS_SMO_HEALTH) {
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
        if (15 < m->actionTimer++ && smo_launch_mario_until_land(m, ACT_DEATH_EXIT_LAND, MARIO_ANIM_GENERAL_FALL, -32.0f)) {
#ifdef VERSION_JP
            play_sound(SOUND_MARIO_OOOF, m->marioObj->header.gfx.cameraToObject);
#else
            play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
#endif
            smo_set_mario_to_full_health(m);
        } else {
            m->health = 0xFF;
            smo_set_hp_counter_to_mario_health(m);
        }
        return ACTION_RESULT_BREAK;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_unused_death_exit(struct MarioState *m) {
<<<<<<< HEAD
    if (SMO_HEALTH != 0) {
=======
    if (IS_SMO_HEALTH) {
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
        if (smo_launch_mario_until_land(m, ACT_FREEFALL_LAND_STOP, MARIO_ANIM_GENERAL_FALL, 0.0f)) {
#ifdef VERSION_JP
            play_sound(SOUND_MARIO_OOOF, m->marioObj->header.gfx.cameraToObject);
#else
            play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
#endif
            smo_set_mario_to_full_health(m);
        } else {
            m->health = 0xFF;
            smo_set_hp_counter_to_mario_health(m);
        }
        return ACTION_RESULT_BREAK;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_falling_death_exit(struct MarioState *m) {
<<<<<<< HEAD
    if (SMO_HEALTH != 0) {
=======
    if (IS_SMO_HEALTH) {
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
        if (smo_launch_mario_until_land(m, ACT_DEATH_EXIT_LAND, MARIO_ANIM_GENERAL_FALL, 0.0f)) {
#ifdef VERSION_JP
            play_sound(SOUND_MARIO_OOOF, m->marioObj->header.gfx.cameraToObject);
#else
            play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
#endif
            smo_set_mario_to_full_health(m);
        } else {
            m->health = 0xFF;
            smo_set_hp_counter_to_mario_health(m);
        }
        return ACTION_RESULT_BREAK;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_special_exit_airborne(struct MarioState *m) {
<<<<<<< HEAD
    if (SMO_HEALTH != 0) {
=======
    if (IS_SMO_HEALTH) {
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
        if (m->actionTimer++ < 11) {
            m->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
            return ACTION_RESULT_BREAK;
        }

        play_sound_if_no_flag(m, SOUND_MARIO_YAHOO, MARIO_MARIO_SOUND_PLAYED);
        if (smo_launch_mario_until_land(m, ACT_EXIT_LAND_SAVE_DIALOG, MARIO_ANIM_SINGLE_JUMP, -24.0f)) {
            smo_set_mario_to_full_health(m);
            m->actionArg = 1;
        }

        m->particleFlags |= PARTICLE_SPARKLES;
        m->marioObj->header.gfx.angle[1] += 0x8000;
        m->marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
        return ACTION_RESULT_BREAK;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_special_death_exit(struct MarioState *m) {
<<<<<<< HEAD
    if (SMO_HEALTH != 0) {
=======
    if (IS_SMO_HEALTH) {
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
        if (m->actionTimer++ < 11) {
            m->health = 0xFF;
            smo_set_hp_counter_to_mario_health(m);
            m->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
            return ACTION_RESULT_BREAK;
        }

        if (smo_launch_mario_until_land(m, ACT_HARD_BACKWARD_GROUND_KB, MARIO_ANIM_BACKWARD_AIR_KB, -24.0f)) {
            smo_set_mario_to_full_health(m);
        }

        m->marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
        return ACTION_RESULT_BREAK;
    }
    return ACTION_RESULT_CONTINUE;
}

s32 mario_check_smo_cutscene_action(struct MarioState *m) {
<<<<<<< HEAD
    m->oCappyJumped = FALSE;
    m->oWallSlide = TRUE;
=======
    m->marioObj->oCappyJumped = FALSE;
    m->marioObj->oWallSlide = TRUE;
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

    switch (m->action) {
        case ACT_EXIT_AIRBORNE:         return smo_act_exit_airborne(m);
        case ACT_FALLING_EXIT_AIRBORNE: return smo_act_falling_exit_airborne(m);
        case ACT_DEATH_EXIT:            return smo_act_death_exit(m);
        case ACT_UNUSED_DEATH_EXIT:     return smo_act_unused_death_exit(m);
        case ACT_FALLING_DEATH_EXIT:    return smo_act_falling_death_exit(m);
        case ACT_SPECIAL_EXIT_AIRBORNE: return smo_act_special_exit_airborne(m);
        case ACT_SPECIAL_DEATH_EXIT:    return smo_act_special_death_exit(m);
        case ACT_SMO_POSSESSION:        return smo_act_possession(m);
    }

    return ACTION_RESULT_CONTINUE;
}
