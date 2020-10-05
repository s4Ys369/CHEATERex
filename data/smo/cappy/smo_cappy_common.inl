#include "../smo_c_includes.h"

#define CAPPY_MAX_LIFETIME  120     // 4 seconds
#define CAPPY_DELTA_VEL     80.f    // max number of units cappy moves per frame

//
// Return to Mario
//

inline static void cappy_start_return_to_mario(struct Object *cappy) {
    cappy->oTimer = CAPPY_MAX_LIFETIME;
}

inline static s32 cappy_return_to_mario(struct Object *cappy, struct MarioState *m) {
    f32 dx = m->pos[0] - cappy->oPosX;
    f32 dy = m->pos[1] - cappy->oPosY + 60;
    f32 dz = m->pos[2] - cappy->oPosZ;
    f32 dv = sqrtf(sqr(dx) + sqr(dy) + sqr(dz));
    if (dv <= cappy->hitboxRadius) {
        return TRUE;
    }

    dx /= dv;
    dy /= dv;
    dz /= dv;
    dv = MIN(dv, CAPPY_DELTA_VEL);
    cappy->oPosX += (dx * dv);
    cappy->oPosY += (dy * dv);
    cappy->oPosZ += (dz * dv);
    return FALSE;
}

//
// Register surface objects
//

inline static void cappy_register_wall_object(UNUSED struct Object *cappy, struct MarioState *m, struct Object *obj) {
    if (obj != NULL) {
        m->oCappyWallObject = obj;
    }
}

inline static void cappy_register_floor_object(UNUSED struct Object *cappy, struct MarioState *m, struct Object *obj) {
    if (obj != NULL) {
        m->oCappyFloorObject = obj;
    }
}

inline static void cappy_register_ceiling_object(UNUSED struct Object *cappy, struct MarioState *m, struct Object *obj) {
    if (obj != NULL) {
        m->oCappyCeilObject = obj;
    }
}
