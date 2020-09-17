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

void time_trials_speedrun_init();
void time_trials_speedrun_end();
void time_trials_speedrun_deinit();
s32  time_trials_speedrun_is_started();

void time_trials_render_pause_castle_main_strings(s32 fileIndex, s8 *index);
void time_trials_render_hud_timer();
void time_trials_render_star_select_time(s32 fileIndex, s32 course, s32 star);

/* Constants */
extern const u8 gTimeTrialsText100CoinsStar[];
extern const u8 gTimeTrialsTextSpeedrun[];
#define TIME_TRIALS_MAX_ALLOWED_TIME		17999	// 9'59"99
#define TIME_TRIALS_UNDEFINED_TIME			-1
#define TIME_TRIALS_SPEEDRUN_TIMER_START	180		// 6 seconds, average time between power on and file selection
#define TIME_TRIALS_SPEEDRUN_MAX_TIME		1079999 // 9:59:59.99
#define TIME_TRIALS_SPEEDRUN				time_trials_speedrun_is_started()
#define MODEL_COSMIC_MARIO					0xFF

/* Options */
#ifdef DYNOS
#include "pc/dynamic_options.h"
#define TIME_TRIALS_ID					"TimeTrials"
#define TIME_TRIALS						(TIME_TRIALS_SPEEDRUN ? 0 : dynos_get_value(TIME_TRIALS_ID, 0))
#define TIME_TRIALS_GHOST				(TIME_TRIALS_SPEEDRUN ? 0 : dynos_get_value(TIME_TRIALS_ID, 1))
#define TIME_TRIALS_MAIN_COURSE_STAR	(TIME_TRIALS_SPEEDRUN ? 0 : dynos_get_value(TIME_TRIALS_ID, 2))
#define TIME_TRIALS_BOWSER_STAR			(TIME_TRIALS_SPEEDRUN ? 0 : dynos_get_value(TIME_TRIALS_ID, 3))
#define TIME_TRIALS_PSS_STAR			(TIME_TRIALS_SPEEDRUN ? 0 : dynos_get_value(TIME_TRIALS_ID, 4))
#define TIME_TRIALS_100_COINS_STAR_EXIT	(TIME_TRIALS_SPEEDRUN ? 0 : dynos_get_value(TIME_TRIALS_ID, 5))
#define TIME_TRIALS_CHEATS_ENABLED		(TIME_TRIALS_SPEEDRUN ? 0 : dynos_get_value(TIME_TRIALS_ID, 6))
#else
#define TIME_TRIALS						(TIME_TRIALS_SPEEDRUN ? 0 : 1)
#define TIME_TRIALS_GHOST				(TIME_TRIALS_SPEEDRUN ? 0 : 1)
#define TIME_TRIALS_MAIN_COURSE_STAR	0
#define TIME_TRIALS_BOWSER_STAR			0
#define TIME_TRIALS_PSS_STAR			0
#define TIME_TRIALS_100_COINS_STAR_EXIT 0
#define TIME_TRIALS_CHEATS_ENABLED		0
#endif

#endif // TIME_TRIALS_H
