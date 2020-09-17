#include "../smo_c_includes.h"

#define WATER_DASH_MAX_DURATION 16
#define WATER_DASH_START_REPEAT 12

static void smo_stationary_slow_down(struct MarioState *m) {

    // Face angles
    m->angleVel[0] = 0;
    m->angleVel[1] = 0;
    m->faceAngle[0] = approach_s32(m->faceAngle[0], 0, 0x200, 0x200);
    m->faceAngle[2] = approach_s32(m->faceAngle[2], 0, 0x100, 0x100);

    // Velocity
    m->forwardVel = approach_f32(m->forwardVel, 0.0f, 1.0f, 1.0f);
    m->vel[0] = m->forwardVel * coss(m->faceAngle[0]) * sins(m->faceAngle[1]);
    m->vel[2] = m->forwardVel * coss(m->faceAngle[0]) * coss(m->faceAngle[1]);
    m->vel[1] = approach_f32(m->vel[1], 0.0f, 2.0f, 1.0f);
}

static u32 smo_perform_water_full_step(struct MarioState *m, Vec3f nextPos) {
    
    // Wall collision
    struct Surface *wall = resolve_and_return_wall_collisions(nextPos, 10.0f, 110.0f);
    
    // Floor collision
    struct Surface *floor = NULL;
    f32 floorHeight = find_floor(nextPos[0], nextPos[1], nextPos[2], &floor);
    if (floor == NULL) {
        return WATER_STEP_CANCELLED;
    }

    // Ceiling collision
    struct Surface *ceil = NULL;
    f32 ceilHeight = vec3f_find_ceil(nextPos, floorHeight, &ceil);

    // Above floor
    if (nextPos[1] >= floorHeight) {

        // Below ceiling
        if (ceilHeight - nextPos[1] >= 160.0f) {
            vec3f_copy(m->pos, nextPos);
            m->floor = floor;
            m->floorHeight = floorHeight;

            if (wall != NULL) {
                return WATER_STEP_HIT_WALL;
            } else {
                return WATER_STEP_NONE;
            }
        }

        // Not enough space to fit Mario
        if (ceilHeight - floorHeight < 160.0f) {
            return WATER_STEP_CANCELLED;
        }

        // Hit ceiling
        vec3f_set(m->pos, nextPos[0], ceilHeight - 160.0f, nextPos[2]);
        m->floor = floor;
        m->floorHeight = floorHeight;
        return WATER_STEP_HIT_CEILING;
    }

    // Not enough space to fit Mario
    if (ceilHeight - floorHeight < 160.0f) {
        return WATER_STEP_CANCELLED;
    }

    // Hit floor
    vec3f_set(m->pos, nextPos[0], floorHeight, nextPos[2]);
    m->floor = floor;
    m->floorHeight = floorHeight;
    return WATER_STEP_HIT_FLOOR;
}

static u32 smo_perform_water_step(struct MarioState *m) {
    Vec3f nextPos = { m->pos[0] + m->vel[0], m->pos[1] + m->vel[1], m->pos[2] + m->vel[2] };
    if (nextPos[1] > m->waterLevel - 80) {
        nextPos[1] = m->waterLevel - 80;
        m->vel[1] = 0.0f;
    }

    u32 stepResult = smo_perform_water_full_step(m, nextPos);
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, -m->faceAngle[0], m->faceAngle[1], m->faceAngle[2]);
    return stepResult;
}

//
// Actions
//

static s32 smo_act_water_idle(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DESCENT, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_WATER, SMO_CAPPY != 0);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DESCENT, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_WATER, IS_SMO_CAPPY_AVAILABLE);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_water_action_end(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DESCENT, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_WATER, SMO_CAPPY != 0);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DESCENT, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_WATER, IS_SMO_CAPPY_AVAILABLE);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_breaststroke(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DESCENT, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_WATER, SMO_CAPPY != 0);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DESCENT, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_WATER, IS_SMO_CAPPY_AVAILABLE);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_swimming_end(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DESCENT, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_WATER, SMO_CAPPY != 0);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DESCENT, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_WATER, IS_SMO_CAPPY_AVAILABLE);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_flutter_kick(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DESCENT, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_WATER, SMO_CAPPY != 0);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DESCENT, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_WATER, IS_SMO_CAPPY_AVAILABLE);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_metal_water_standing(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_PUNCH, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR, SMO_CAPPY != 0);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_PUNCH, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR, IS_SMO_CAPPY_AVAILABLE);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_metal_water_walking(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_PUNCH, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR, SMO_CAPPY != 0);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_PUNCH, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR, IS_SMO_CAPPY_AVAILABLE);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_metal_water_jump(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_AIR, SMO_CAPPY != 0);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_KICK, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_AIR, IS_SMO_CAPPY_AVAILABLE);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_metal_water_falling(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_KICK, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_AIR, SMO_CAPPY != 0);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_KICK, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_AIR, IS_SMO_CAPPY_AVAILABLE);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_metal_water_jump_land(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_PUNCH, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR, SMO_CAPPY != 0);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_PUNCH, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR, IS_SMO_CAPPY_AVAILABLE);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_metal_water_fall_land(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_PUNCH, SMO_MARIO == 1);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR, SMO_CAPPY != 0);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_PUNCH, IS_SMO_MARIO);
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR, IS_SMO_CAPPY_AVAILABLE);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_water_dash(struct MarioState *m) {

    if (m->actionTimer >= WATER_DASH_MAX_DURATION) {
        set_mario_action(m, ACT_BREASTSTROKE, 0);
        return ACTION_RESULT_CANCEL;
    }

    if (m->actionTimer >= WATER_DASH_START_REPEAT) {
<<<<<<< HEAD
        RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DASH, SMO_MARIO == 1);
=======
        RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DASH, IS_SMO_MARIO);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    }

    if ((m->actionTimer % 4) == 0) {
        play_sound(SOUND_ACTION_UNKNOWN434, m->marioObj->header.gfx.cameraToObject);
    }

    set_mario_anim_with_accel(m, MARIO_ANIM_FLUTTERKICK, 0x30000);
    m->particleFlags |= (PARTICLE_PLUNGE_BUBBLE | PARTICLE_BUBBLE);
    m->vel[0] = coss(m->faceAngle[0]) * sins(m->faceAngle[1]) * 56.0f;
    m->vel[1] = sins(m->faceAngle[0]) * 56.0f;
    m->vel[2] = coss(m->faceAngle[0]) * coss(m->faceAngle[1]) * 56.0f;
    m->actionTimer++;

    switch (smo_perform_water_step(m)) {
        case WATER_STEP_HIT_WALL:
            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
            set_mario_action(m, ACT_BACKWARD_WATER_KB, 0);
            return ACTION_RESULT_CANCEL;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_water_descent(struct MarioState *m) {
    if (m->actionState == 0) {
        if (m->actionTimer < 10) {
            m->pos[1] += (20 - 2 * m->actionTimer);
            vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
        }

        m->vel[0] = 0;
        m->vel[1] = 0;
        m->vel[2] = 0;

<<<<<<< HEAD
        RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DASH, SMO_MARIO == 1);
=======
        RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_WATER_DASH, IS_SMO_MARIO);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

        set_mario_animation(m, MARIO_ANIM_START_GROUND_POUND);
        if (m->actionTimer == 0) {
            play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
        }

        if (++(m->actionTimer) >= m->marioObj->header.gfx.unk38.curAnim->unk08 + 4) {
            play_sound(SOUND_MARIO_GROUND_POUND_WAH, m->marioObj->header.gfx.cameraToObject);
            play_sound(SOUND_ACTION_UNKNOWN430, m->marioObj->header.gfx.cameraToObject);
            m->actionState = 1;
            m->vel[1] = -105.f;
            m->faceAngle[0] = 0;
            m->faceAngle[2] = 0;
        }

    } else {

        set_mario_animation(m, MARIO_ANIM_GROUND_POUND);
        m->particleFlags |= (PARTICLE_PLUNGE_BUBBLE | PARTICLE_BUBBLE);
        m->vel[1] += 5.0f;

        if (m->vel[1] >= 0) {
            m->vel[1] = 0;
            set_mario_action(m, ACT_WATER_IDLE, 0);
            return ACTION_RESULT_CANCEL;
        }

        u32 stepResult = smo_perform_water_step(m);
        if (stepResult == WATER_STEP_HIT_FLOOR) {
            set_mario_action(m, ACT_WATER_IDLE, 0);
            return ACTION_RESULT_CANCEL;
        }
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_leave_object_water(struct MarioState *m) {
    smo_stationary_slow_down(m);
    smo_perform_water_step(m);
    set_mario_animation(m, MARIO_ANIM_WATER_IDLE);
    m->marioBodyState->headAngle[0] = 0;
    m->particleFlags |= PARTICLE_SPARKLES;

    if (absx(m->vel[1]) < 1.f) {
        set_mario_action(m, ACT_WATER_IDLE, 0);
        return ACTION_RESULT_CANCEL;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_metal_water_punch(struct MarioState *m) {

    if (!(m->flags & MARIO_METAL_CAP)) {
        set_mario_action(m, ACT_WATER_IDLE, 0);
        return ACTION_RESULT_CANCEL;
    }

    mario_set_forward_vel(m, 2.f);
    switch (m->actionArg) {
        case 0:
            m->actionArg = 1;
            m->flags |= MARIO_PUNCHING;
            if (is_anim_past_end(m)) {
                m->actionArg = 2;
            }
            break;

        case 1:
            set_mario_animation(m, MARIO_ANIM_FIRST_PUNCH_FAST);
            if (is_anim_at_end(m)) {
                set_mario_action(m, ACT_METAL_WATER_STANDING, 0);
            }
            break;
    }

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_METAL_WATER_FALLING, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            m->forwardVel = 0;
            break;
    }

    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_metal_water_kick(struct MarioState *m) {
    
    if (!(m->flags & MARIO_METAL_CAP)) {
        set_mario_action(m, ACT_WATER_IDLE, 0);
        return ACTION_RESULT_CANCEL;
    }

    if (!is_anim_at_end(m)) {
        m->flags |= MARIO_KICKING;
    }

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_METAL_WATER_FALL_LAND, 0);
            break;

        case AIR_STEP_HIT_WALL:
            mario_set_forward_vel(m, 0.0f);
            break;
    }

    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_cappy_throw_water(struct MarioState *m) {
    update_mario_throw_anim(m, ACT_WATER_IDLE);
    play_sound_if_no_flag(m, SOUND_ACTION_SWIM, MARIO_ACTION_SOUND_PLAYED);
    mario_set_forward_vel(m, m->forwardVel * 0.9f);
    smo_perform_water_step(m);
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_cappy_throw_metal_water_floor(struct MarioState *m) {
    update_mario_throw_anim(m, ACT_METAL_WATER_STANDING);
    mario_set_forward_vel(m, m->forwardVel * 0.85f);
    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_METAL_WATER_FALLING, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            m->forwardVel = 0;
            break;
    }
    return ACTION_RESULT_CONTINUE;
}

static s32 smo_act_cappy_throw_metal_water_air(struct MarioState *m) {
<<<<<<< HEAD
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_KICK, SMO_MARIO == 1);
=======
    RETURN_CANCEL_IF_ACTION_SET(ACT_SMO_METAL_WATER_KICK, IS_SMO_MARIO);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    update_mario_throw_anim(m, ACT_METAL_WATER_FALLING);
    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_METAL_WATER_FALL_LAND, 0);
            break;

        case AIR_STEP_HIT_WALL:
            mario_set_forward_vel(m, 0.0f);
            break;
    }
    return ACTION_RESULT_CONTINUE;
}

s32 mario_check_smo_submerged_action(struct MarioState *m) {
<<<<<<< HEAD
    m->oCappyJumped = FALSE;
    m->oWallSlide = TRUE;

    // Fall height
    if (SMO_MARIO == 1) {
        m->peakHeight = m->pos[1];
        m->oPeakHeight = m->pos[1];
=======
    m->marioObj->oCappyJumped = FALSE;
    m->marioObj->oWallSlide = TRUE;

    // Fall height
    if (IS_SMO_MARIO) {
        m->peakHeight = m->pos[1];
        m->marioObj->oPeakHeight = m->pos[1];
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    }

    switch (m->action) {
        case ACT_WATER_IDLE:                        return smo_act_water_idle(m);
        case ACT_WATER_ACTION_END:                  return smo_act_water_action_end(m);
        case ACT_BREASTSTROKE:                      return smo_act_breaststroke(m);
        case ACT_SWIMMING_END:                      return smo_act_swimming_end(m);
        case ACT_FLUTTER_KICK:                      return smo_act_flutter_kick(m);
        case ACT_METAL_WATER_STANDING:              return smo_act_metal_water_standing(m);
        case ACT_METAL_WATER_WALKING:               return smo_act_metal_water_walking(m);
        case ACT_METAL_WATER_JUMP:                  return smo_act_metal_water_jump(m);
        case ACT_METAL_WATER_FALLING:               return smo_act_metal_water_falling(m);
        case ACT_METAL_WATER_JUMP_LAND:             return smo_act_metal_water_jump_land(m);
        case ACT_METAL_WATER_FALL_LAND:             return smo_act_metal_water_fall_land(m);
        case ACT_SMO_WATER_DESCENT:                 return smo_act_water_descent(m);
        case ACT_SMO_WATER_DASH:                    return smo_act_water_dash(m);
        case ACT_SMO_LEAVE_OBJECT_WATER:            return smo_act_leave_object_water(m);
        case ACT_SMO_METAL_WATER_PUNCH:             return smo_act_metal_water_punch(m);
        case ACT_SMO_METAL_WATER_KICK:              return smo_act_metal_water_kick(m);
        case ACT_SMO_CAPPY_THROW_WATER:             return smo_act_cappy_throw_water(m);
        case ACT_SMO_CAPPY_THROW_METAL_WATER_FLOOR: return smo_act_cappy_throw_metal_water_floor(m);
        case ACT_SMO_CAPPY_THROW_METAL_WATER_AIR:   return smo_act_cappy_throw_metal_water_air(m);
    }

    return ACTION_RESULT_CONTINUE;
}
