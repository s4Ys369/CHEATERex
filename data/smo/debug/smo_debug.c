#include "../smo_c_includes.h"
#include "geo_commands.h"
#include <stdarg.h>
#include "smo_debug_obj.inl"
#include "smo_debug_text.inl"

void smo_debug_update() {
    smo_debug_display_objects_hitbox();
    smo_debug_display_text();
}
