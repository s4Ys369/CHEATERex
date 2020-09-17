#ifndef SMO_DEBUG_H
#define SMO_DEBUG_H

#include "types.h"

/* Debug stuff */
/* Compile with DEBUG=1 to use the DEBUG_ONLY code */

#ifdef DEBUG
#define DEBUG_ONLY(...) __VA_ARGS__
#else
#define DEBUG_ONLY(...) 
#endif

/* Debug display */
extern const GeoLayout dbg_cylinder_hitbox_geo[];
extern const GeoLayout dbg_cylinder_hurtbox_geo[];

/* Debug text */
void smo_debug_add_string(s32 x, s32 y, u32 rgbaFront, u32 rgbaBack, const char *fmt, ...);

/* Debug mode */
<<<<<<< HEAD
void smo_debug_update();
=======
extern u8 gSmoDebug;
void smo_debug_update();
void smo_debug_reset();
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

#endif // SMO_DEBUG_H
