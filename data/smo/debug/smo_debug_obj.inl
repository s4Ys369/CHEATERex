//
// Debug Cylinder data
//

static Vtx dbg_cylinder_vertex[80];

static const Gfx dbg_cylinder_vertex_dl[] = {
    // Upper first-half
    gsSPVertex(dbg_cylinder_vertex + 0, 10, 0),
    gsSP2Triangles(0, 2, 1, 0x0, 0, 3, 2, 0x0),
    gsSP2Triangles(0, 4, 3, 0x0, 0, 5, 4, 0x0),
    gsSP2Triangles(0, 6, 5, 0x0, 0, 7, 6, 0x0),
    gsSP2Triangles(0, 8, 7, 0x0, 0, 9, 8, 0x0),

    // Upper second-half
    gsSPVertex(dbg_cylinder_vertex + 10, 10, 0),
    gsSP2Triangles(0, 2, 1, 0x0, 0, 3, 2, 0x0),
    gsSP2Triangles(0, 4, 3, 0x0, 0, 5, 4, 0x0),
    gsSP2Triangles(0, 6, 5, 0x0, 0, 7, 6, 0x0),
    gsSP2Triangles(0, 8, 7, 0x0, 0, 9, 8, 0x0),

    // Lower first-half
    gsSPVertex(dbg_cylinder_vertex + 20, 10, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSP2Triangles(0, 3, 4, 0x0, 0, 4, 5, 0x0),
    gsSP2Triangles(0, 5, 6, 0x0, 0, 6, 7, 0x0),
    gsSP2Triangles(0, 7, 8, 0x0, 0, 8, 9, 0x0),

    // Lower second-half
    gsSPVertex(dbg_cylinder_vertex + 30, 10, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSP2Triangles(0, 3, 4, 0x0, 0, 4, 5, 0x0),
    gsSP2Triangles(0, 5, 6, 0x0, 0, 6, 7, 0x0),
    gsSP2Triangles(0, 7, 8, 0x0, 0, 8, 9, 0x0),

    // First wall
    gsSPVertex(dbg_cylinder_vertex + 40, 10, 0),
    gsSP2Triangles(0, 2, 1, 0x0, 1, 2, 3, 0x0),
    gsSP2Triangles(2, 4, 3, 0x0, 3, 4, 5, 0x0),
    gsSP2Triangles(4, 6, 5, 0x0, 5, 6, 7, 0x0),
    gsSP2Triangles(6, 8, 7, 0x0, 7, 8, 9, 0x0),

    // Second wall
    gsSPVertex(dbg_cylinder_vertex + 50, 10, 0),
    gsSP2Triangles(0, 2, 1, 0x0, 1, 2, 3, 0x0),
    gsSP2Triangles(2, 4, 3, 0x0, 3, 4, 5, 0x0),
    gsSP2Triangles(4, 6, 5, 0x0, 5, 6, 7, 0x0),
    gsSP2Triangles(6, 8, 7, 0x0, 7, 8, 9, 0x0),

    // Third wall
    gsSPVertex(dbg_cylinder_vertex + 60, 10, 0),
    gsSP2Triangles(0, 2, 1, 0x0, 1, 2, 3, 0x0),
    gsSP2Triangles(2, 4, 3, 0x0, 3, 4, 5, 0x0),
    gsSP2Triangles(4, 6, 5, 0x0, 5, 6, 7, 0x0),
    gsSP2Triangles(6, 8, 7, 0x0, 7, 8, 9, 0x0),

    // Fourth wall
    gsSPVertex(dbg_cylinder_vertex + 70, 10, 0),
    gsSP2Triangles(0, 2, 1, 0x0, 1, 2, 3, 0x0),
    gsSP2Triangles(2, 4, 3, 0x0, 3, 4, 5, 0x0),
    gsSP2Triangles(4, 6, 5, 0x0, 5, 6, 7, 0x0),
    gsSP2Triangles(6, 8, 7, 0x0, 7, 8, 9, 0x0),

    gsSPEndDisplayList(),
};

static inline Vtx get_center(f32 y) {
    Vtx v = { { { 0, 100.f * y, 0 }, 0, { 0, 0 }, { 0xff, 0xff, 0xff, 0xff } } };
    return v;
}

static inline Vtx get_point(s16 a, f32 y) {
    Vtx v = { { { 100.f * coss(a), 100.f * y, 100.f * sins(a) }, 0, { 0, 0 }, { 0xff, 0xff, 0xff, 0xff } } };
    return v;
}

static void smo_debug_set_vertex_data() {
    // Upper first-half
    dbg_cylinder_vertex[0] = get_center(1);
    dbg_cylinder_vertex[1] = get_point(0x0000, 1);
    dbg_cylinder_vertex[2] = get_point(0x1000, 1);
    dbg_cylinder_vertex[3] = get_point(0x2000, 1);
    dbg_cylinder_vertex[4] = get_point(0x3000, 1);
    dbg_cylinder_vertex[5] = get_point(0x4000, 1);
    dbg_cylinder_vertex[6] = get_point(0x5000, 1);
    dbg_cylinder_vertex[7] = get_point(0x6000, 1);
    dbg_cylinder_vertex[8] = get_point(0x7000, 1);
    dbg_cylinder_vertex[9] = get_point(0x8000, 1);

    // Upper second-half
    dbg_cylinder_vertex[10] = get_center(1);
    dbg_cylinder_vertex[11] = get_point(0x8000, 1);
    dbg_cylinder_vertex[12] = get_point(0x9000, 1);
    dbg_cylinder_vertex[13] = get_point(0xA000, 1);
    dbg_cylinder_vertex[14] = get_point(0xB000, 1);
    dbg_cylinder_vertex[15] = get_point(0xC000, 1);
    dbg_cylinder_vertex[16] = get_point(0xD000, 1);
    dbg_cylinder_vertex[17] = get_point(0xE000, 1);
    dbg_cylinder_vertex[18] = get_point(0xF000, 1);
    dbg_cylinder_vertex[19] = get_point(0x0000, 1);

    // Lower first-half
    dbg_cylinder_vertex[20] = get_center(0);
    dbg_cylinder_vertex[21] = get_point(0x0000, 0);
    dbg_cylinder_vertex[22] = get_point(0x1000, 0);
    dbg_cylinder_vertex[23] = get_point(0x2000, 0);
    dbg_cylinder_vertex[24] = get_point(0x3000, 0);
    dbg_cylinder_vertex[25] = get_point(0x4000, 0);
    dbg_cylinder_vertex[26] = get_point(0x5000, 0);
    dbg_cylinder_vertex[27] = get_point(0x6000, 0);
    dbg_cylinder_vertex[28] = get_point(0x7000, 0);
    dbg_cylinder_vertex[29] = get_point(0x8000, 0);

    // Lower second-half
    dbg_cylinder_vertex[30] = get_center(0);
    dbg_cylinder_vertex[31] = get_point(0x8000, 0);
    dbg_cylinder_vertex[32] = get_point(0x9000, 0);
    dbg_cylinder_vertex[33] = get_point(0xA000, 0);
    dbg_cylinder_vertex[34] = get_point(0xB000, 0);
    dbg_cylinder_vertex[35] = get_point(0xC000, 0);
    dbg_cylinder_vertex[36] = get_point(0xD000, 0);
    dbg_cylinder_vertex[37] = get_point(0xE000, 0);
    dbg_cylinder_vertex[38] = get_point(0xF000, 0);
    dbg_cylinder_vertex[39] = get_point(0x0000, 0);

    // First wall
    dbg_cylinder_vertex[40] = get_point(0x0000, 1);
    dbg_cylinder_vertex[41] = get_point(0x0000, 0);
    dbg_cylinder_vertex[42] = get_point(0x1000, 1);
    dbg_cylinder_vertex[43] = get_point(0x1000, 0);
    dbg_cylinder_vertex[44] = get_point(0x2000, 1);
    dbg_cylinder_vertex[45] = get_point(0x2000, 0);
    dbg_cylinder_vertex[46] = get_point(0x3000, 1);
    dbg_cylinder_vertex[47] = get_point(0x3000, 0);
    dbg_cylinder_vertex[48] = get_point(0x4000, 1);
    dbg_cylinder_vertex[49] = get_point(0x4000, 0);

    // Second wall
    dbg_cylinder_vertex[50] = get_point(0x4000, 1);
    dbg_cylinder_vertex[51] = get_point(0x4000, 0);
    dbg_cylinder_vertex[52] = get_point(0x5000, 1);
    dbg_cylinder_vertex[53] = get_point(0x5000, 0);
    dbg_cylinder_vertex[54] = get_point(0x6000, 1);
    dbg_cylinder_vertex[55] = get_point(0x6000, 0);
    dbg_cylinder_vertex[56] = get_point(0x7000, 1);
    dbg_cylinder_vertex[57] = get_point(0x7000, 0);
    dbg_cylinder_vertex[58] = get_point(0x8000, 1);
    dbg_cylinder_vertex[59] = get_point(0x8000, 0);

    // Third wall
    dbg_cylinder_vertex[60] = get_point(0x8000, 1);
    dbg_cylinder_vertex[61] = get_point(0x8000, 0);
    dbg_cylinder_vertex[62] = get_point(0x9000, 1);
    dbg_cylinder_vertex[63] = get_point(0x9000, 0);
    dbg_cylinder_vertex[64] = get_point(0xA000, 1);
    dbg_cylinder_vertex[65] = get_point(0xA000, 0);
    dbg_cylinder_vertex[66] = get_point(0xB000, 1);
    dbg_cylinder_vertex[67] = get_point(0xB000, 0);
    dbg_cylinder_vertex[68] = get_point(0xC000, 1);
    dbg_cylinder_vertex[69] = get_point(0xC000, 0);

    // Fourth wall
    dbg_cylinder_vertex[70] = get_point(0xC000, 1);
    dbg_cylinder_vertex[71] = get_point(0xC000, 0);
    dbg_cylinder_vertex[72] = get_point(0xD000, 1);
    dbg_cylinder_vertex[73] = get_point(0xD000, 0);
    dbg_cylinder_vertex[74] = get_point(0xE000, 1);
    dbg_cylinder_vertex[75] = get_point(0xE000, 0);
    dbg_cylinder_vertex[76] = get_point(0xF000, 1);
    dbg_cylinder_vertex[77] = get_point(0xF000, 0);
    dbg_cylinder_vertex[78] = get_point(0x0000, 1);
    dbg_cylinder_vertex[79] = get_point(0x0000, 0);
}

//
// Debug Hitbox
//

static const Lights1 dbg_cylinder_hitbox_light = gdSPDefLights1(
    0x40, 0xc0, 0xff,
    0x40, 0xc0, 0xff,
    0x28, 0x28, 0x28
);

static const Gfx dbg_cylinder_hitbox_dl[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsDPSetEnvColor(255, 255, 255, 120),
    gsSPLight(&dbg_cylinder_hitbox_light.l, 1),
    gsSPLight(&dbg_cylinder_hitbox_light.a, 2),
    gsSPDisplayList(dbg_cylinder_vertex_dl),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

const GeoLayout dbg_cylinder_hitbox_geo[] = {
    GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0x9B, 0),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_TRANSPARENT, dbg_cylinder_hitbox_dl),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

static void bhv_smo_debug_hitbox() {
    if (gCurrentObject->parentObj == NULL) {
        obj_mark_for_deletion(gCurrentObject);
    } else {
        gCurrentObject->oPosX = gCurrentObject->parentObj->oPosX;
        gCurrentObject->oPosZ = gCurrentObject->parentObj->oPosZ;
        gCurrentObject->header.gfx.pos[0] = gCurrentObject->parentObj->oPosX;
        gCurrentObject->header.gfx.pos[2] = gCurrentObject->parentObj->oPosZ;
        if (gCurrentObject->parentObj == gMarioObject->oPossessedObject) {
            gCurrentObject->oPosY = gCurrentObject->parentObj->oPosY - gCurrentObject->parentObj->oStickMag;
            gCurrentObject->header.gfx.pos[1] = gCurrentObject->parentObj->oPosY - gCurrentObject->parentObj->oStickMag;
            gCurrentObject->header.gfx.scale[0] = gCurrentObject->parentObj->oStickX / 100.f;
            gCurrentObject->header.gfx.scale[1] = gCurrentObject->parentObj->oStickY / 100.f;
            gCurrentObject->header.gfx.scale[2] = gCurrentObject->parentObj->oStickX / 100.f;
        } else {
            gCurrentObject->oPosY = gCurrentObject->parentObj->oPosY - gCurrentObject->parentObj->hitboxDownOffset;
            gCurrentObject->header.gfx.pos[1] = gCurrentObject->parentObj->oPosY - gCurrentObject->parentObj->hitboxDownOffset;
            gCurrentObject->header.gfx.scale[0] = gCurrentObject->parentObj->hitboxRadius / 100.f;
            gCurrentObject->header.gfx.scale[1] = gCurrentObject->parentObj->hitboxHeight / 100.f;
            gCurrentObject->header.gfx.scale[2] = gCurrentObject->parentObj->hitboxRadius / 100.f;
        }
        gCurrentObject->parentObj = NULL;
    }
}

static const BehaviorScript bhvSmoDebugHitbox[] = {
    0x000C0000,
    0x08000000,
    0x0C000000, (uintptr_t) bhv_smo_debug_hitbox,
    0x09000000,
};

//
// Debug Hurtbox
//

static const Lights1 dbg_cylinder_hurtbox_light = gdSPDefLights1(
    0xff, 0x40, 0x40,
    0xff, 0x40, 0x40,
    0x28, 0x28, 0x28
);

static const Gfx dbg_cylinder_hurtbox_dl[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsDPSetEnvColor(255, 255, 255, 120),
    gsSPLight(&dbg_cylinder_hurtbox_light.l, 1),
    gsSPLight(&dbg_cylinder_hurtbox_light.a, 2),
    gsSPDisplayList(dbg_cylinder_vertex_dl),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

const GeoLayout dbg_cylinder_hurtbox_geo[] = {
    GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0x9B, 0),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_TRANSPARENT, dbg_cylinder_hurtbox_dl),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

static void bhv_smo_debug_hurtbox() {
    if (gCurrentObject->parentObj == NULL) {
        obj_mark_for_deletion(gCurrentObject);
    } else {
        gCurrentObject->oPosX = gCurrentObject->parentObj->oPosX;
        gCurrentObject->oPosY = gCurrentObject->parentObj->oPosY - gCurrentObject->parentObj->hitboxDownOffset;
        gCurrentObject->oPosZ = gCurrentObject->parentObj->oPosZ;
        gCurrentObject->header.gfx.pos[0] = gCurrentObject->parentObj->oPosX;
        gCurrentObject->header.gfx.pos[1] = gCurrentObject->parentObj->oPosY - gCurrentObject->parentObj->hitboxDownOffset;
        gCurrentObject->header.gfx.pos[2] = gCurrentObject->parentObj->oPosZ;
        gCurrentObject->header.gfx.scale[0] = gCurrentObject->parentObj->hurtboxRadius / 100.f;
        gCurrentObject->header.gfx.scale[1] = gCurrentObject->parentObj->hurtboxHeight / 100.f;
        gCurrentObject->header.gfx.scale[2] = gCurrentObject->parentObj->hurtboxRadius / 100.f;
        gCurrentObject->parentObj = NULL;
    }
}

static const BehaviorScript bhvSmoDebugHurtbox[] = {
    0x000C0000,
    0x08000000,
    0x0C000000, (uintptr_t) bhv_smo_debug_hurtbox,
    0x09000000,
};

//
// Debug Object
//

static void smo_debug_display_objects_hitbox_from_list(s8 type) {
    struct ObjectNode *list = &gObjectLists[type];
    struct Object *next = obj_get_first(list);
    while (next != NULL) {
        struct Object *obj = next;
        if (obj == gMarioObject && gMarioState->action == ACT_SMO_POSSESSION) {
            obj = obj->oPossessedObject;
        }
        if (obj_get_first_child_with_behavior(obj, bhvSmoDebugHitbox)  == NULL &&
            obj_get_first_child_with_behavior(obj, bhvSmoDebugHurtbox) == NULL) {
            spawn_object(obj, MODEL_DBG_CYLINDER_HITBOX, bhvSmoDebugHitbox);
            spawn_object(obj, MODEL_DBG_CYLINDER_HURTBOX, bhvSmoDebugHurtbox);
        }
        next = obj_get_next(list, next);
    }
}

static void smo_debug_display_objects_hitbox() {
    smo_debug_set_vertex_data();
    smo_debug_display_objects_hitbox_from_list(OBJ_LIST_SURFACE);
    smo_debug_display_objects_hitbox_from_list(OBJ_LIST_POLELIKE);
    smo_debug_display_objects_hitbox_from_list(OBJ_LIST_PLAYER);
    smo_debug_display_objects_hitbox_from_list(OBJ_LIST_PUSHABLE);
    smo_debug_display_objects_hitbox_from_list(OBJ_LIST_GENACTOR);
    smo_debug_display_objects_hitbox_from_list(OBJ_LIST_DESTRUCTIVE);
    smo_debug_display_objects_hitbox_from_list(OBJ_LIST_LEVEL);
}
