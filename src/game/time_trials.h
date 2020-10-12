#ifndef TIME_TRIALS_H
#define TIME_TRIALS_H

#include <stdbool.h>
#include "types.h"

void time_trials_update(struct MarioState *m, bool isPaused);
u32  time_trials_save_time_and_stop_timer(s32 fileIndex, s32 course, s32 level, s32 star, bool exit);
s32  time_trials_render_pause_castle_main_strings(s32 fileIndex, s8 *index);
void time_trials_render_star_select_time(s32 fileIndex, s32 course, s32 star);

/* Options */
#include "pc/dynamic_options.h"
#define __time_trials__						dynos_get_value("time_trials")
#define __time_trials_ghost1__				dynos_get_value("time_trials_ghost1")
#define __time_trials_ghost2__				dynos_get_value("time_trials_ghost2")
#define __time_trials_cheats__				dynos_get_value("time_trials_cheats")
#define TIME_TRIALS_DISABLED				(__time_trials__ == 0)
#define TIME_TRIALS_ENABLED					(__time_trials__ == 1)
#define TIME_TRIALS_GHOST1_HIDE				(__time_trials_ghost1__ == 0)
#define TIME_TRIALS_GHOST2_HIDE				(__time_trials_ghost2__ == 0)
#define TIME_TRIALS_CHEATS_DISABLED			(__time_trials_cheats__ == 0)

#endif // TIME_TRIALS_H
