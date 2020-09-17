#ifndef SMO_TT_H
#define SMO_TT_H

#include "types.h"

#define SMO_TT_MAX_ALLOWED_TIME 17999 // 9'59"99

extern const GeoLayout cosmic_mario_geo[];

s16 smo_tt_get_time(s32 fileIndex, s32 course, s32 star);
s32 smo_tt_get_course_time(s32 fileIndex, s32 course);
s32 smo_tt_get_bowser_time(s32 fileIndex);
s32 smo_tt_get_secret_time(s32 fileIndex);
s32 smo_tt_get_total_time(s32 fileIndex);

void smo_tt_init_times();
void smo_tt_save_time(s32 fileIndex, s32 course, s32 level, s32 star, s32 noExit);

void smo_tt_start_timer(s32 fileIndex, s32 course, s32 star);
void smo_tt_update_timer(struct MarioState *m);
void smo_tt_stop_timer();

void smo_tt_render_pause_castle_main_strings(s32 fileIndex, s8 *index);
void smo_tt_render_hud_timers();
void smo_tt_render_star_select_time(s32 fileIndex, s32 course, s32 star);

#endif // SMO_TT_H
