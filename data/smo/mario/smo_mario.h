#ifndef SMO_MARIO_H
#define SMO_MARIO_H

#include "types.h"

/* Actions */
enum {
    ACTION_RESULT_BREAK,
    ACTION_RESULT_CANCEL,
    ACTION_RESULT_CONTINUE
};

#define RETURN_CANCEL_IF_ACTION_SET(action, flag)    { if ((flag) && mario_set_smo_action(m, action)) { return ACTION_RESULT_CANCEL; } }
s32 mario_set_smo_action(struct MarioState *m, u32 action);
s32 mario_check_wall_slide(struct MarioState *m);
void update_mario_throw_anim(struct MarioState *m, u32 actEnd);

/* Checks */
s32 mario_check_smo_stationary_action(struct MarioState *m);
s32 mario_check_smo_moving_action(struct MarioState *m);
s32 mario_check_smo_airborne_action(struct MarioState *m);
s32 mario_check_smo_submerged_action(struct MarioState *m);
s32 mario_check_smo_cutscene_action(struct MarioState *m);

/* Execution */
void mario_execute_action(struct MarioState *m);

/* Capture */
enum {
    MARIO_UNPOSSESS_ACT_JUMP_OUT,
    MARIO_UNPOSSESS_ACT_KNOCKED_BACK,
    MARIO_UNPOSSESS_ACT_KNOCKED_OUT_OF_KING_WHOMP,
    MARIO_UNPOSSESS_ACT_BURNT,
    MARIO_UNPOSSESS_ACT_GRABBED,
    MARIO_UNPOSSESS_ACT_EATEN,
    MARIO_UNPOSSESS_ACT_BLOWN,
    MARIO_UNPOSSESS_ACT_TORNADO,
    MARIO_UNPOSSESS_ACT_COUNT
};

s32 mario_possess_object(struct MarioState *m, struct Object *obj);
s32 mario_unpossess_object(struct MarioState *m, u8 unpossessAct, s32 isBackwards, u32 objIntangibleFrames);

s32 mario_lock(struct MarioState *m);
s32 mario_unlock(struct MarioState *m);
s32 smo_act_possession(struct MarioState *m);

#endif // SMO_MARIO_H
