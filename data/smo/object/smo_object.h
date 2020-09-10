#ifndef SMO_OBJECT_H
#define SMO_OBJECT_H

#include "types.h"

/* SMO Fields */
/* See smo_defines for fields macros */
struct SmoCaptureData;
struct SmoData {
    struct MarioState *mario;

    union {
        s8  asS8 [0x20][4];
        s16 asS16[0x20][2];
        s32 asS32[0x20];
        f32 asF32[0x20];
    } fields;

    union {
        struct Object *asObject[0x10];
        void *asVoidPtr[0x10];
    } refs;

    /* Possessed object only */

    const struct SmoCaptureData *captureData; // Read-only!

    struct {
        u32 rawData[0x50];
#if IS_64_BIT
        void *ptrData[0x50];
#endif
        const BehaviorScript *behavior;
        const BehaviorScript *curBhvCommand;
        u32 bhvStackIndex;
        uintptr_t bhvStack[8];
        f32 hitboxRadius;
        f32 hitboxHeight;
        f32 hurtboxRadius;
        f32 hurtboxHeight;
        f32 hitboxDownOffset;
        f32 gfxScaleX;
        f32 gfxScaleY;
        f32 gfxScaleZ;
    } objData;
};

void smo_obj_alloc_data(struct Object *obj, struct MarioState *m);
void smo_obj_free_data(struct Object *obj);
void smo_free_all_data();
void smo_object_reset();

/* SMO Object Models */
#define SMO_OBJECTS_LOAD_MODELS \
ALLOC_LEVEL_POOL(), \
LOAD_MODEL_FROM_GEO(MODEL_COSMIC_MARIO, cosmic_mario_geo), \
LOAD_MODEL_FROM_GEO(MODEL_SHOCK_WAVE, invisible_bowser_accessory_geo), \
DEBUG_ONLY(LOAD_MODEL_FROM_GEO(MODEL_DBG_CYLINDER_HITBOX, dbg_cylinder_hitbox_geo),) \
DEBUG_ONLY(LOAD_MODEL_FROM_GEO(MODEL_DBG_CYLINDER_HURTBOX, dbg_cylinder_hurtbox_geo),) \
FREE_LEVEL_POOL()

#endif // SMO_OBJECT_H
