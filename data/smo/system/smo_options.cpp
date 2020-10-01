#ifdef __cplusplus
extern "C" {
#endif
#include "../smo_c_includes.h"
#include "game/options_menu.h"
#ifdef __cplusplus
}
#endif

#ifdef DYNOS
static void smo_return_to_main_menu(UNUSED struct Option *opt, s32 arg) {
    if (!arg) {
        optmenu_toggle();
        unpause_game();
        smo_hud_reset();
        smo_health_reset();
        smo_object_reset();
        fade_into_special_warp(-2, 0);
    }
}

static void go_to_level_select(UNUSED struct Option *opt, s32 arg) {
    if (!arg) {
        optmenu_toggle();
        unpause_game();
        smo_hud_reset();
        smo_health_reset();
        smo_object_reset();
        fade_into_special_warp(-9, 1);
    }
}

DYNOS_DEFINE_ACTION(smo_return_to_main_menu);
DYNOS_DEFINE_ACTION(go_to_level_select);
#else
// Provides a default config if DynOS is not installed
// [X] key and (X) button for the mask 0x0040
u32 gSmoConfigKeyCappy[MAX_BINDS] = { 0x002D, 0x1002, VK_INVALID };
#endif

