#include "../smo_c_includes.h"

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
