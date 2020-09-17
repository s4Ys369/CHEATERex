<<<<<<< HEAD
#include "smo_cappy_common.inl"
#include "smo_cappy_collision.inl"
=======
#include "../smo_c_includes.h"
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
#ifdef SMO_SGI
#include "sgi/utils/characters.h"
#endif

<<<<<<< HEAD
//
// Behavior Script
//

static const BehaviorScript bhvCappy[] = {
    0x00060000,
    0x08000000,
    0x09000000
};

static u8 cappy_get_behavior(struct MarioState *m, u8 actFlag) {
=======
static const BehaviorScript *cappy_get_behavior(struct MarioState *m, u8 actFlag, u32 *throwType) {
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

    // Flying throw
    if (actFlag == CAPPY_ACT_FLAG_FLYING) {
        m->input &= ~(INPUT_A_PRESSED | INPUT_A_DOWN);
<<<<<<< HEAD
        return 3;
=======
        *throwType = 3;
        return bhvCappy3;
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
    }

    // Spin throw
    if (m->input & INPUT_A_PRESSED) {
        m->input &= ~(INPUT_A_PRESSED | INPUT_A_DOWN);
<<<<<<< HEAD
        return 2;
    }

    // Normal throw
    return 1;
=======
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
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
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
<<<<<<< HEAD

    // Cappy must be available
    if (SMO_CAPPY == 0 || !(m->flags & MARIO_CAP_ON_HEAD)) {
=======
    u32 throwType = 0;

    // Cappy must be available
    if (!IS_SMO_CAPPY_AVAILABLE || !(m->flags & MARIO_CAP_ON_HEAD)) {
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
        return 0;
    }

    // Unspawn Cappy if spawned for more than 30 frames (1 second)
<<<<<<< HEAD
    struct Object *cappy = m->oCappyObject;
    if (cappy) {
        if (cappy->oTimer < 30) {
            return 0;
        }
        obj_mark_for_deletion(cappy);
    }

    // Spawn Cappy according to the current cap
    m->oCappyBehavior = cappy_get_behavior(m, actFlag);
#ifdef SMO_SGI
    if (getCharacterType() == LUIGI) {
        cappy = spawn_object(m->marioObj, sLuigisCapModel[(m->flags >> 1) & 0b111], bhvCappy);
=======
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
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
        cappy->header.gfx.scale[0] = LUIGIS_CAPPY_SCALE_MULTIPLIER;
        cappy->header.gfx.scale[1] = LUIGIS_CAPPY_SCALE_MULTIPLIER;
        cappy->header.gfx.scale[2] = LUIGIS_CAPPY_SCALE_MULTIPLIER;
    } else {
#endif
<<<<<<< HEAD
        cappy = spawn_object(m->marioObj, sMariosCapModel[(m->flags >> 1) & 0b111], bhvCappy);
        cappy->header.gfx.scale[0] = 1.f;
        cappy->header.gfx.scale[1] = 1.f;
        cappy->header.gfx.scale[2] = 1.f;
=======
        cappy = spawn_object(m->marioObj, sMariosCapModel[(m->flags >> 1) & 0b111], bhv);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
#ifdef SMO_SGI
    }
#endif
    
<<<<<<< HEAD
    // Init    
    cappy->hitboxRadius = 80.f;
    cappy->hitboxHeight = 112.f;
    cappy->hitboxDownOffset = 32.f;
    cappy->oWallHitboxRadius = 48.f;
    m->oCappyObject = cappy;
    m->oCappyActionFlag = actFlag;
    m->oCappyInitialized = FALSE;
    return m->oCappyBehavior;
}

void mario_update_cappy(struct MarioState *m) {
    struct Object *cappy = m->oCappyObject;
    if (cappy) {

        // Check if Mario should have his cap on his head
        if (!(m->flags & MARIO_CAP_ON_HEAD)) {
            mario_unload_cappy(m);
            return;
        }

        // Init behavior
        if (!m->oCappyInitialized) {
            switch (m->oCappyBehavior) {
                case 1: cappy_bhv1_init(cappy, m); break;
                case 2: cappy_bhv2_init(cappy, m); break;
                case 3: cappy_bhv3_init(cappy, m); break;
            }
            m->oCappyInitialized = TRUE;
        }

        // Update behavior
        if (cappy->oTimer < CAPPY_MAX_LIFETIME) {
            switch (m->oCappyBehavior) {
                case 1: cappy_bhv1_update(cappy, m); break;
                case 2: cappy_bhv2_update(cappy, m); break;
                case 3: cappy_bhv3_update(cappy, m); break;
            }
            cappy_process_object_collision(cappy, m);
        } else {
            if (cappy_return_to_mario(cappy, m)) {
                mario_unload_cappy(m);
                return;
            }
        }

        // Update Gfx
        if (cappy->activeFlags) {
            cappy->header.gfx.pos[0] = cappy->oPosX;
            cappy->header.gfx.pos[1] = cappy->oPosY;
            cappy->header.gfx.pos[2] = cappy->oPosZ;
            cappy->header.gfx.angle[0] = 0;
            cappy->header.gfx.angle[1] += 0x2000;
            cappy->header.gfx.angle[2] = 0;
            cappy->oOpacity = ((m->flags & MARIO_VANISH_CAP) ? 0x80 : 0xFF);
            cappy->activeFlags |= GRAPH_RENDER_ACTIVE;
            cappy->activeFlags &= ~GRAPH_RENDER_INVISIBLE;
            spawn_object(cappy, MODEL_NONE, bhvSparkleSpawn);

            // Update Mario's cap state
            m->marioBodyState->capState = MARIO_HAS_DEFAULT_CAP_OFF;
        }
    }
}

void mario_unload_cappy(struct MarioState *m) {
    if (m->marioObj->smoData && m->oCappyObject) {
        obj_mark_for_deletion(m->oCappyObject);
        m->oCappyObject = NULL;
    }
}
=======
    // Init
    smo_obj_alloc_data(cappy, m);
    cappy->oCappyActionFlag = actFlag;
    cappy->hitboxRadius = CAPPY_HITBOX_RADIUS;
    cappy->hitboxHeight = CAPPY_HITBOX_HEIGHT;
    cappy->hitboxDownOffset = CAPPY_HITBOX_DOWNOFFSET;
    return throwType;
}
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
