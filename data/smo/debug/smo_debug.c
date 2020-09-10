#include "../smo_c_includes.h"
#include "geo_commands.h"
#include <stdarg.h>

#include "smo_debug_obj.inl"

#include "smo_debug_text.inl"

u8 gSmoDebug = FALSE;

void smo_debug_update() {
    if (gSmoDebug) {
        smo_debug_display_objects_hitbox();
        smo_debug_display_text();
    }
}

void smo_debug_reset() {
    gSmoDebug = FALSE;
}
