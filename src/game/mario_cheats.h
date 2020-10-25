#ifndef MARIO_CHEATS_H
#define MARIO_CHEATS_H

#include <PR/ultratypes.h>

#include "macros.h"
#include "types.h"

void cheats_set_model(struct MarioState *m);
void cheats_swimming_speed(struct MarioState *m);
void cheats_air_step(struct MarioState *m);
void cheats_long_jump(struct MarioState *m);
void cheats_mario_inputs(struct MarioState *m);

/* Options */
#define TIME_BUTTON 0x0080

#include "pc/dynamic_options.h"
#define __chaos_mode__   dynos_get_value("chaos_mode")
#define __time_button__ dynos_get_value("time_button")
#define __no_heavy__ dynos_get_value("no_heavy")
#define __swim_any__ dynos_get_value("swim_any")
#define CHAOS_MODE (__chaos_mode__ == 1)
#define NO_HEAVY (__no_heavy__ == 1)
#define SWIM_ANY (__swim_any__ == 1)

#endif // MARIO_CHEATS_H
