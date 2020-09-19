#ifndef TIME_TRIALS_H
#define TIME_TRIALS_H

#include "types.h"

s16  time_trials_get_time(s32 fileIndex, s32 course, s32 star);
s32  time_trials_get_course_time(s32 fileIndex, s32 course);
s32  time_trials_get_bowser_time(s32 fileIndex);
s32  time_trials_get_secret_time(s32 fileIndex);
s32  time_trials_get_total_time(s32 fileIndex);

void time_trials_init_times();
void time_trials_save_time(s32 fileIndex, s32 course, s32 level, s32 star, s32 noExit);

void time_trials_start_timer(struct MarioState *m, s32 fileIndex, s32 course, s32 star, s32 forceRestart);
void time_trials_update_timer(struct MarioState *m);
void time_trials_stop_timer();

void time_trials_render_pause_castle_main_strings(s32 fileIndex, s8 *index);
void time_trials_render_hud_timer();
void time_trials_render_star_select_time(s32 fileIndex, s32 course, s32 star);

/* Constants */
extern const u8 gTimeTrialsText100CoinsStar[];
#define TIME_TRIALS_MAX_ALLOWED_TIME		17999	// 9'59"99
#define TIME_TRIALS_UNDEFINED_TIME			-1
#define MODEL_COSMIC_MARIO					0xFF

/* Options */
#ifdef DYNOS
#include "pc/dynamic_options.h"
#define TIME_TRIALS_ID					"TimeTrials"
#define TIME_TRIALS						dynos_get_value(TIME_TRIALS_ID, "time_trials")
#define TIME_TRIALS_GHOST				dynos_get_value(TIME_TRIALS_ID, "time_trials_ghost")
#define TIME_TRIALS_MAIN_COURSE_STAR	dynos_get_value(TIME_TRIALS_ID, "time_trials_main")
#define TIME_TRIALS_BOWSER_STAR			dynos_get_value(TIME_TRIALS_ID, "time_trials_bowser")
#define TIME_TRIALS_PSS_STAR			dynos_get_value(TIME_TRIALS_ID, "time_trials_pss")
#define TIME_TRIALS_100_COINS_STAR_EXIT	dynos_get_value(TIME_TRIALS_ID, "time_trials_100")
#define TIME_TRIALS_CHEATS_ENABLED		dynos_get_value(TIME_TRIALS_ID, "time_trials_cheats")
#else
#define TIME_TRIALS						1
#define TIME_TRIALS_GHOST				1
#define TIME_TRIALS_MAIN_COURSE_STAR	0
#define TIME_TRIALS_BOWSER_STAR			0
#define TIME_TRIALS_PSS_STAR			0
#define TIME_TRIALS_100_COINS_STAR_EXIT 0
#define TIME_TRIALS_CHEATS_ENABLED		0
#endif

#endif // TIME_TRIALS_H
