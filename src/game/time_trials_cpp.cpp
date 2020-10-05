#ifdef __cplusplus
extern "C" {
#endif
#include "time_trials.h"
#include "sm64.h"
#include "ingame_menu.h"
#include "level_update.h"
#include "object_list_processor.h"
#include "options_menu.h"
#include "engine/level_script.h"
#include "audio/external.h"
#include "sound_init.h"

#include "levels/bob/header.h"
#include "levels/wf/header.h"
#include "levels/jrb/header.h"
#include "levels/ccm/header.h"
#include "levels/bbh/header.h"
#include "levels/hmc/header.h"
#include "levels/lll/header.h"
#include "levels/ssl/header.h"
#include "levels/ddd/header.h"
#include "levels/sl/header.h"
#include "levels/wdw/header.h"
#include "levels/ttm/header.h"
#include "levels/thi/header.h"
#include "levels/ttc/header.h"
#include "levels/rr/header.h"

#include "levels/bitdw/header.h"
#include "levels/bitfs/header.h"
#include "levels/bits/header.h"

#include "levels/totwc/header.h"
#include "levels/vcutm/header.h"
#include "levels/cotmc/header.h"
#include "levels/pss/header.h"
#include "levels/sa/header.h"
#include "levels/wmotr/header.h"
#ifdef __cplusplus
}
#endif

#ifdef DYNOS
static const LevelScript *get_level_script(s16 *level) {
    const LevelScript *levelScript = NULL;
    switch (*level) {
        case LEVEL_BOB:      levelScript = level_bob_entry;     *level = LEVEL_BOB;     break;
        case LEVEL_WF:       levelScript = level_wf_entry;      *level = LEVEL_WF;      break;
        case LEVEL_JRB:      levelScript = level_jrb_entry;     *level = LEVEL_JRB;     break;
        case LEVEL_CCM:      levelScript = level_ccm_entry;     *level = LEVEL_CCM;     break;
        case LEVEL_BBH:      levelScript = level_bbh_entry;     *level = LEVEL_BBH;     break;
        case LEVEL_HMC:      levelScript = level_hmc_entry;     *level = LEVEL_HMC;     break;
        case LEVEL_LLL:      levelScript = level_lll_entry;     *level = LEVEL_LLL;     break;
        case LEVEL_SSL:      levelScript = level_ssl_entry;     *level = LEVEL_SSL;     break;
        case LEVEL_DDD:      levelScript = level_ddd_entry;     *level = LEVEL_DDD;     break;
        case LEVEL_SL:       levelScript = level_sl_entry;      *level = LEVEL_SL;      break;
        case LEVEL_WDW:      levelScript = level_wdw_entry;     *level = LEVEL_WDW;     break;
        case LEVEL_TTM:      levelScript = level_ttm_entry;     *level = LEVEL_TTM;     break;
        case LEVEL_THI:      levelScript = level_thi_entry;     *level = LEVEL_THI;     break;
        case LEVEL_TTC:      levelScript = level_ttc_entry;     *level = LEVEL_TTC;     break;
        case LEVEL_RR:       levelScript = level_rr_entry;      *level = LEVEL_RR;      break;

        case LEVEL_BITDW:    levelScript = level_bitdw_entry;   *level = LEVEL_BITDW;   break;
        case LEVEL_BOWSER_1: levelScript = level_bitdw_entry;   *level = LEVEL_BITDW;   break;
        case LEVEL_BITFS:    levelScript = level_bitfs_entry;   *level = LEVEL_BITFS;   break;
        case LEVEL_BOWSER_2: levelScript = level_bitfs_entry;   *level = LEVEL_BITFS;   break;
        case LEVEL_BITS:     levelScript = level_bits_entry;    *level = LEVEL_BITS;    break;
        case LEVEL_BOWSER_3: levelScript = level_bits_entry;    *level = LEVEL_BITS;    break;

        case LEVEL_TOTWC:    levelScript = level_totwc_entry;   *level = LEVEL_TOTWC;   break;
        case LEVEL_VCUTM:    levelScript = level_vcutm_entry;   *level = LEVEL_VCUTM;   break;
        case LEVEL_COTMC:    levelScript = level_cotmc_entry;   *level = LEVEL_COTMC;   break;
        case LEVEL_PSS:      levelScript = level_pss_entry;     *level = LEVEL_PSS;     break;
        case LEVEL_SA:       levelScript = level_sa_entry;      *level = LEVEL_SA;      break;
        case LEVEL_WMOTR:    levelScript = level_wmotr_entry;   *level = LEVEL_WMOTR;   break;
    }
    return levelScript;
}

static void time_trials_restart_level(UNUSED struct Option *opt, s32 arg) {
    if (!arg) {
        const LevelScript *levelScript = get_level_script(&gCurrLevelNum);
        if (levelScript == NULL) {
            return;
        }

        // Free everything from the level
        time_trials_stop_timer();
        optmenu_toggle();
        unpause_game();
        set_sound_disabled(FALSE);
        play_shell_music();
        stop_shell_music();
        stop_cap_music();
        clear_objects();
        clear_area_graph_nodes();
        clear_areas();
        main_pool_pop_state();

        // Reload the level
        level_script_execute((LevelCommand *) levelScript);
        sWarpDest.type = 2;
        sWarpDest.levelNum = gCurrLevelNum;
        sWarpDest.areaIdx = 1;
        sWarpDest.arg = 0;
        gSavedCourseNum = gCurrLevelNum;
        time_trials_start_timer(gMarioState, gCurrSaveFileNum - 1, gCurrCourseNum, gCurrActNum - 1, TRUE);

        // Reset Mario's state
        gMarioState->healCounter = 0;
        gMarioState->hurtCounter = 0;
        gMarioState->numCoins = 0;
        gHudDisplay.coins = 0;
    }
}
DYNOS_DEFINE_ACTION(time_trials_restart_level);
#endif
