#ifndef MARIO_ACTIONS_AIRBORNE_H
#define MARIO_ACTIONS_AIRBORNE_H

#include <PR/ultratypes.h>

#include "types.h"

u32 common_air_action_step(struct MarioState *m, u32 landAction, s32 animation, u32 stepArg);
s32 mario_execute_airborne_action(struct MarioState *m);

#endif // MARIO_ACTIONS_AIRBORNE_H
