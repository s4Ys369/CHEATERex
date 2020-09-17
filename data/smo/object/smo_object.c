#include "../smo_c_includes.h"
#include "actors/group12.h"

//
// SMO Fields
//

#define MAX_SMO_FIELDS 0x40

struct SmoFields {
    struct Object *ref;
    struct SmoData data;
};
static struct SmoFields sSmoFields[MAX_SMO_FIELDS];

static struct SmoFields *find_smo_fields(struct Object *obj) {
    for (s32 i = 0; i != MAX_SMO_FIELDS; ++i) {
        if (sSmoFields[i].ref == obj) {
            return &sSmoFields[i];
        }
    }
    return NULL;
}

void smo_obj_alloc_data(struct Object *obj, struct MarioState *m) {
    struct SmoFields *sf = find_smo_fields(NULL);
    bzero(&sf->data, sizeof(sf->data));
    obj->smoData = &sf->data;
    obj->smoData->mario = m;
    sf->ref = obj;
}

void smo_obj_free_data(struct Object *obj) {
    struct SmoFields *sf = find_smo_fields(obj);
    if (sf != NULL) {
        sf->ref = NULL;
    }
}

void smo_free_all_data() {
    bzero(sSmoFields, sizeof(sSmoFields));
}

void smo_object_reset() {
    smo_free_all_data();
}

//
// SMO Object Models
//

void smo_load_models() {
    struct AllocOnlyPool *pool = alloc_only_pool_init(main_pool_available() - sizeof(struct AllocOnlyPool), MEMORY_POOL_LEFT);
    gLoadedGraphNodes[MODEL_SHOCK_WAVE] = process_geo_layout(pool, (void *) invisible_bowser_accessory_geo);
    DEBUG_ONLY(gLoadedGraphNodes[MODEL_DBG_CYLINDER_HITBOX] = process_geo_layout(pool, (void *) dbg_cylinder_hitbox_geo);)
    DEBUG_ONLY(gLoadedGraphNodes[MODEL_DBG_CYLINDER_HURTBOX] = process_geo_layout(pool, (void *) dbg_cylinder_hurtbox_geo);)
    alloc_only_pool_resize(pool, pool->usedSpace);
}
