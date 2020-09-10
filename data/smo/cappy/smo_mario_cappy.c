#include "../smo_c_includes.h"
#ifdef SMO_SGI
#include "sgi/utils/characters.h"
#endif

static const BehaviorScript *cappy_get_behavior(struct MarioState *m, u8 actFlag, u32 *throwType) {

    // Flying throw
    if (actFlag == CAPPY_ACT_FLAG_FLYING) {
        m->input &= ~(INPUT_A_PRESSED | INPUT_A_DOWN);
        *throwType = 3;
        return bhvCappy3;
    }

    // Spin throw
    if (m->input & INPUT_A_PRESSED) {
        m->input &= ~(INPUT_A_PRESSED | INPUT_A_DOWN);
        *throwType = 2;
        return bhvCappy2;
    }

    // Normal throw
    *throwType = 1;
    return bhvCappy1;
}

//
// Actions
//

struct Object *get_cappy_object(struct MarioState *m) {
    struct ObjectNode *list = &gObjectLists[OBJ_LIST_LEVEL];
    struct Object *obj = obj_get_first(list);
    while (obj != NULL) {
        if ((obj->behavior == bhvCappy1 ||
             obj->behavior == bhvCappy2 ||
             obj->behavior == bhvCappy3) &&
            (obj->activeFlags != 0) &&
            (obj->oMario == m)) {
            return obj;
        }
        obj = obj_get_next(list, obj);
    }
    return NULL;
}

// 0bWMV
// bit 0 = Vanish
// bit 1 = Metal
// bit 2 = Wing
static const s32 sMariosCapModel[8] = {
    MODEL_MARIOS_CAP,               /* 000 - Normal */ 
    MODEL_MARIOS_CAP,               /* 001 - Vanish */
    MODEL_MARIOS_METAL_CAP,         /* 010 - Metal */
    MODEL_MARIOS_METAL_CAP,         /* 011 - Metal/Vanish */
    MODEL_MARIOS_WING_CAP,          /* 100 - Wing */
    MODEL_MARIOS_WING_CAP,          /* 101 - Wing/Vanish */
    MODEL_MARIOS_WINGED_METAL_CAP,  /* 110 - Wing/Metal */
    MODEL_MARIOS_WINGED_METAL_CAP,  /* 111 - Wing/Metal/Vanish */
};

#ifdef SMO_SGI
static const s32 sLuigisCapModel[8] = {
    MODEL_LUIGIS_CAP,               /* 000 - Normal */
    MODEL_LUIGIS_CAP,               /* 001 - Vanish */
    MODEL_LUIGIS_METAL_CAP,         /* 010 - Metal */
    MODEL_LUIGIS_METAL_CAP,         /* 011 - Metal/Vanish */
    MODEL_LUIGIS_WING_CAP,          /* 100 - Wing */
    MODEL_LUIGIS_WING_CAP,          /* 101 - Wing/Vanish */
    MODEL_LUIGIS_WINGED_METAL_CAP,  /* 110 - Wing/Metal */
    MODEL_LUIGIS_WINGED_METAL_CAP,  /* 111 - Wing/Metal/Vanish */
};
#endif

u32 mario_spawn_cappy(struct MarioState *m, u8 actFlag) {
    u32 throwType = 0;

    // Cappy must be available
    if (!IS_SMO_CAPPY_AVAILABLE || !(m->flags & MARIO_CAP_ON_HEAD)) {
        return 0;
    }

    // Unspawn Cappy if spawned for more than 30 frames (1 second)
    struct Object *cappy = get_cappy_object(m);
    if (cappy) {
        if (cappy->oCappyTimer < 30) {
            return FALSE;
        }
        mark_obj_for_deletion(cappy);
        cappy = NULL;
    }

    // Spawn Cappy according to the current cap
    const BehaviorScript *bhv = cappy_get_behavior(m, actFlag, &throwType);
#ifdef SMO_SGI
    if (getCharacterType() == LUIGI) {
        cappy = spawn_object(m->marioObj, sLuigisCapModel[(m->flags >> 1) & 0b111], bhv);
        cappy->header.gfx.scale[0] = LUIGIS_CAPPY_SCALE_MULTIPLIER;
        cappy->header.gfx.scale[1] = LUIGIS_CAPPY_SCALE_MULTIPLIER;
        cappy->header.gfx.scale[2] = LUIGIS_CAPPY_SCALE_MULTIPLIER;
    } else {
#endif
        cappy = spawn_object(m->marioObj, sMariosCapModel[(m->flags >> 1) & 0b111], bhv);
#ifdef SMO_SGI
    }
#endif
    
    // Init
    smo_obj_alloc_data(cappy, m);
    cappy->oCappyActionFlag = actFlag;
    cappy->hitboxRadius = CAPPY_HITBOX_RADIUS;
    cappy->hitboxHeight = CAPPY_HITBOX_HEIGHT;
    cappy->hitboxDownOffset = CAPPY_HITBOX_DOWNOFFSET;
    return throwType;
}
