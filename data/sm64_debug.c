#include "sm64_debug.h"
#include "utils.h"
#include "sm64.h"
#include "seq_ids.h"
#include "behavior_data.h"
#include "course_table.h"
#include "geo_commands.h"
#include "gfx_dimensions.h"
#include "game/ingame_menu.h"
#include "game/options_menu.h"
#include "game/game_init.h"
#include "game/segment2.h"
#include "game/interaction.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "game/level_update.h"
#include "game/sound_init.h"
#include "game/spawn_sound.h"
#include "engine/math_util.h"
#include "engine/level_script.h"
#include "audio/external.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "levels/bob/header.h"
#include "levels/wf/header.h"
#include "levels/jrb/header.h"
#include "levels/ccm/header.h"
#include "levels/bbh/header.h"
#include "levels/hmc/header.h"
#include "levels/lll/header.h"
#include "levels/ssl/header.h"
#include "levels/ddd/header.h"
#include "levels/sl/header.h"
#include "levels/wdw/header.h"
#include "levels/ttm/header.h"
#include "levels/thi/header.h"
#include "levels/ttc/header.h"
#include "levels/rr/header.h"

#include "levels/bitdw/header.h"
#include "levels/bowser_1/header.h"
#include "levels/bitfs/header.h"
#include "levels/bowser_2/header.h"
#include "levels/bits/header.h"
#include "levels/bowser_3/header.h"

#include "levels/totwc/header.h"
#include "levels/vcutm/header.h"
#include "levels/cotmc/header.h"
#include "levels/pss/header.h"
#include "levels/sa/header.h"
#include "levels/wmotr/header.h"

//
// Utils
//

#define DEBUG_ARG_MAX_LENGTH 64
#define DEBUG_MAX_NUM_ARGS 32
struct DebugArgs {
    u32 argc;
    char args[DEBUG_MAX_NUM_ARGS][DEBUG_ARG_MAX_LENGTH];
};

static s32 debug_arg_as_int(const struct DebugArgs *args, u32 i, s32 *x) {
    if (i >= args->argc) {
        return FALSE;
    }
    if (strlen(args->args[i]) > 2 && args->args[i][0] == '0' && args->args[i][1] == 'x') {
        return sscanf(args->args[i], "%x", x) != 0;
    }
    return sscanf(args->args[i], "%d", x) != 0;
}

static s32 debug_arg_as_float(const struct DebugArgs *args, u32 i, f32 *x) {
    if (i >= args->argc) {
        return FALSE;
    }
    return sscanf(args->args[i], "%f", x) != 0;
}

static s32 debug_arg_as_string(const struct DebugArgs *args, u32 i, const char **s) {
    if (i >= args->argc) {
        return FALSE;
    }
    *s = (const char *) args->args[i];
    return TRUE;
}

static s32 streq(const char *str1, const char *str2) {
    u32 len = strlen(str1);
    if (strlen(str2) != len) {
        return FALSE;
    }
    for (u32 i = 0; i != len; ++i) {
        if (toupper(str1[i]) != toupper(str2[i])) {
            return FALSE;
        }
    }
    return TRUE;
}

//
// Debug Text
//

#define DEBUG_LINE_HEIGHT       14
#define DEBUG_STRING_MAX_LENGTH 1024
#define DEBUG_MAX_ITEMS         64

struct DebugText {
    s8 box;
    s32 x;
    s32 y;
    union {
        struct String {
            u8 str[DEBUG_STRING_MAX_LENGTH];
            u32 rgbaFront;
            u32 rgbaBack;
        };
        struct Box {
            u32 w;
            u32 h;
            u32 rgbaColor;
        };
    };
};
static struct DebugText sDebugTexts[DEBUG_MAX_ITEMS];
static u32 sDebugTextCount = 0;
static const u8 sDialogCharWidths[256] = {
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,
    6,  6,  5,  6,  6,  5,  8,  8,  6,  6,  6,  6,  6,  5,  6,  6,
    8,  7,  6,  6,  6,  5,  5,  6,  5,  5,  6,  5,  4,  5,  5,  3,
    7,  5,  5,  5,  6,  5,  5,  5,  5,  5,  7,  7,  5,  5,  4,  4,
    8,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    8,  8,  8,  8,  7,  7,  6,  7,  7,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  6,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    7,  5, 10,  5,  9,  8,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  5,  7,  7,  6,  6,  8,  0,  8, 10,  6,  4,  5,  0,  0
};

static void debug_render_string(s16 x, s16 y, const u8 *str) {
    create_dl_translation_matrix(MENU_MTX_PUSH, x, y, 0);
    for (; *str != DIALOG_CHAR_TERMINATOR; ++str) {
        if (*str != DIALOG_CHAR_SPACE) {
            if (*str == 253) { // '_'
                create_dl_translation_matrix(MENU_MTX_NOPUSH, -1, -5, 0);
                void **fontLUT = (void **) segmented_to_virtual(main_font_lut);
                void *packedTexture = segmented_to_virtual(fontLUT[159]);
                gDPPipeSync(gDisplayListHead++);
                gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, VIRTUAL_TO_PHYSICAL(packedTexture));
                gSPDisplayList(gDisplayListHead++, dl_ia_text_tex_settings);
                create_dl_translation_matrix(MENU_MTX_NOPUSH, 0, +5, 0);
            } else {
                void **fontLUT = (void **) segmented_to_virtual(main_font_lut);
                void *packedTexture = segmented_to_virtual(fontLUT[*str]);
                gDPPipeSync(gDisplayListHead++);
                gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, VIRTUAL_TO_PHYSICAL(packedTexture));
                gSPDisplayList(gDisplayListHead++, dl_ia_text_tex_settings);
            }
        }
        create_dl_translation_matrix(MENU_MTX_NOPUSH, sDialogCharWidths[*str], 0, 0);
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

static void debug_print_string(u32 i) {
    struct DebugText *dbgStr = &sDebugTexts[i];
    if (dbgStr->box) {
        return;
    }

    s32 x0 = GFX_DIMENSIONS_FROM_LEFT_EDGE(dbgStr->x);
    s32 y0 = dbgStr->y;

    // Background
    if ((dbgStr->rgbaBack && 0xFF) != 0) {
        gDPSetEnvColor(gDisplayListHead++, ((dbgStr->rgbaBack >> 24) & 0xFF), ((dbgStr->rgbaBack >> 16) & 0xFF), ((dbgStr->rgbaBack >> 8) & 0xFF), ((dbgStr->rgbaBack >> 0) & 0xFF));
        debug_render_string(x0 + 1, y0 - 1, dbgStr->str);
    }

    // Foreground
    if ((dbgStr->rgbaFront && 0xFF) != 0) {
        gDPSetEnvColor(gDisplayListHead++, ((dbgStr->rgbaFront >> 24) & 0xFF), ((dbgStr->rgbaFront >> 16) & 0xFF), ((dbgStr->rgbaFront >> 8) & 0xFF), ((dbgStr->rgbaFront >> 0) & 0xFF));
        debug_render_string(x0, y0, dbgStr->str);
    }
}

static void debug_print_box(u32 i) {
    struct DebugText *dbgBox = &sDebugTexts[i];
    if (!dbgBox->box) {
        return;
    }

    if ((dbgBox->rgbaColor && 0xFF) != 0) {
        Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));
        if (!matrix) {
            return;
        }

        create_dl_translation_matrix(MENU_MTX_PUSH, GFX_DIMENSIONS_FROM_LEFT_EDGE(dbgBox->x), dbgBox->y + dbgBox->h, 0);
        guScale(matrix, (f32) dbgBox->w / 130.f, (f32) dbgBox->h / 80.f, 1.f);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gDPSetEnvColor(gDisplayListHead++, ((dbgBox->rgbaColor >> 24) & 0xFF), ((dbgBox->rgbaColor >> 16) & 0xFF), ((dbgBox->rgbaColor >> 8) & 0xFF), ((dbgBox->rgbaColor >> 0) & 0xFF));
        gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    }
}

void debug_add_box(int x, int y, unsigned int w, unsigned int h, unsigned int rgbaColor) {
    if (w == 0 || h == 0 || sDebugTextCount == DEBUG_MAX_ITEMS) {
        return;
    }

    struct DebugText *dbgBox = &sDebugTexts[sDebugTextCount++];
    dbgBox->x = x;
    dbgBox->y = y;
    dbgBox->w = w;
    dbgBox->h = h;
    dbgBox->rgbaColor = rgbaColor;
    dbgBox->box = TRUE;
}

void debug_add_string(int x, int y, unsigned int rgbaFront, unsigned int rgbaBack, const unsigned char *str64) {
    if (str64 == NULL || sDebugTextCount == DEBUG_MAX_ITEMS) {
        return;
    }

    struct DebugText *dbgStr = &sDebugTexts[sDebugTextCount++];
    memcpy(dbgStr->str, str64, 1 + MIN(DEBUG_STRING_MAX_LENGTH - 1, sm64_strlen(str64)));
    dbgStr->x = x;
    dbgStr->y = y;
    dbgStr->rgbaFront = rgbaFront;
    dbgStr->rgbaBack = rgbaBack;
    dbgStr->box = FALSE;
}

static void debug_display_text() {

    // Boxes
    for (u32 i = 0; i != sDebugTextCount; ++i) debug_print_box(i);

    // Strings
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    for (u32 i = 0; i != sDebugTextCount; ++i) debug_print_string(i);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

    // Reset
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    sDebugTextCount = 0;
}

//
// Debug Objects
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

static void debug_set_vertex_data() {
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

static const GeoLayout dbg_cylinder_hitbox_geo[] = {
    GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0x9B, 0),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_TRANSPARENT, dbg_cylinder_hitbox_dl),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

static void bhv_debug_hitbox() {
    if (gCurrentObject->parentObj == NULL) {
        obj_mark_for_deletion(gCurrentObject);
    } else {
        gCurrentObject->oPosX = gCurrentObject->parentObj->oPosX;
        gCurrentObject->oPosZ = gCurrentObject->parentObj->oPosZ;
        gCurrentObject->header.gfx.pos[0] = gCurrentObject->parentObj->oPosX;
        gCurrentObject->header.gfx.pos[2] = gCurrentObject->parentObj->oPosZ;
#ifdef ACT_SMO_POSSESSION
        if (gMarioObject && gCurrentObject->parentObj == gMarioState->oPossessedObject) {
            gCurrentObject->oPosY = gCurrentObject->parentObj->oPosY - gCurrentObject->parentObj->oStickMag;
            gCurrentObject->header.gfx.pos[1] = gCurrentObject->parentObj->oPosY - gCurrentObject->parentObj->oStickMag;
            gCurrentObject->header.gfx.scale[0] = gCurrentObject->parentObj->oStickX / 100.f;
            gCurrentObject->header.gfx.scale[1] = gCurrentObject->parentObj->oStickY / 100.f;
            gCurrentObject->header.gfx.scale[2] = gCurrentObject->parentObj->oStickX / 100.f;
        } else 
#endif
        {
            gCurrentObject->oPosY = gCurrentObject->parentObj->oPosY - gCurrentObject->parentObj->hitboxDownOffset;
            gCurrentObject->header.gfx.pos[1] = gCurrentObject->parentObj->oPosY - gCurrentObject->parentObj->hitboxDownOffset;
            gCurrentObject->header.gfx.scale[0] = gCurrentObject->parentObj->hitboxRadius / 100.f;
            gCurrentObject->header.gfx.scale[1] = gCurrentObject->parentObj->hitboxHeight / 100.f;
            gCurrentObject->header.gfx.scale[2] = gCurrentObject->parentObj->hitboxRadius / 100.f;
        }
        gCurrentObject->parentObj = NULL;
    }
}

static const BehaviorScript bhvDebugHitbox[] = {
    0x000C0000,
    0x08000000,
    0x0C000000, (uintptr_t) bhv_debug_hitbox,
    0x09000000,
};

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

static const GeoLayout dbg_cylinder_hurtbox_geo[] = {
    GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0x9B, 0),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_TRANSPARENT, dbg_cylinder_hurtbox_dl),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

static void bhv_debug_hurtbox() {
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

static const BehaviorScript bhvDebugHurtbox[] = {
    0x000C0000,
    0x08000000,
    0x0C000000, (uintptr_t) bhv_debug_hurtbox,
    0x09000000,
};

static struct Object *debug_obj_get_first(struct ObjectNode *list) {
    struct Object *obj = (struct Object *) list->next;
    if (obj == (struct Object *) list) {
        return NULL;
    }
    return obj;
}

static struct Object *debug_obj_get_next(struct ObjectNode *list, struct Object *obj) {
    struct Object *next = (struct Object *) obj->header.next;
    if (next == (struct Object *) list) {
        return NULL;
    }
    return next;
}

static struct Object *debug_obj_get_first_child_with_behavior(struct Object *obj, const BehaviorScript *behavior) {
    struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behavior)];
    struct Object *next = (struct Object *) listHead->next;
    while (next != (struct Object *) listHead) {
        if (next->behavior == behavior && next->activeFlags != ACTIVE_FLAG_DEACTIVATED && next != obj && next->parentObj == obj) {
            return next;
        }
        next = (struct Object *) next->header.next;
    }
    return NULL;
}

static void debug_display_objects_hitbox_from_list(s8 type, s8 hitbox, s8 hurtbox) {
    struct ObjectNode *list = &gObjectLists[type];
    struct Object *next = debug_obj_get_first(list);
    while (next != NULL) {
        struct Object *obj = next;
#ifdef SMO_ACT_POSSESSION
        if (obj == gMarioObject && gMarioState->action == ACT_SMO_POSSESSION) {
            obj = gMarioState->oPossessedObject;
        }
#endif
        if (hitbox && debug_obj_get_first_child_with_behavior(obj, bhvDebugHitbox) == NULL) {
            obj_spawn_with_geo(obj, dbg_cylinder_hitbox_geo, bhvDebugHitbox);
        }
        if (hurtbox && debug_obj_get_first_child_with_behavior(obj, bhvDebugHurtbox) == NULL) {
            obj_spawn_with_geo(obj, dbg_cylinder_hurtbox_geo, bhvDebugHurtbox);
        }
        next = debug_obj_get_next(list, next);
    }
}

//
// Debug Commands
//

struct CourseWarp {
    const char *name;
    const LevelScript *script;
    s16 levelNum;
    s16 courseNum;
};

static const struct CourseWarp sWarps[] = {
    { "BOB",      level_bob_entry,      LEVEL_BOB,      COURSE_BOB },
    { "WF",       level_wf_entry,       LEVEL_WF,       COURSE_WF },
    { "JRB",      level_jrb_entry,      LEVEL_JRB,      COURSE_JRB },
    { "CCM",      level_ccm_entry,      LEVEL_CCM,      COURSE_CCM },
    { "BBH",      level_bbh_entry,      LEVEL_BBH,      COURSE_BBH },
    { "HMC",      level_hmc_entry,      LEVEL_HMC,      COURSE_HMC },
    { "LLL",      level_lll_entry,      LEVEL_LLL,      COURSE_LLL },
    { "SSL",      level_ssl_entry,      LEVEL_SSL,      COURSE_SSL },
    { "DDD",      level_ddd_entry,      LEVEL_DDD,      COURSE_DDD },
    { "SL",       level_sl_entry,       LEVEL_SL,       COURSE_SL },
    { "WDW",      level_wdw_entry,      LEVEL_WDW,      COURSE_WDW },
    { "TTM",      level_ttm_entry,      LEVEL_TTM,      COURSE_TTM },
    { "THI",      level_thi_entry,      LEVEL_THI,      COURSE_THI },
    { "TTC",      level_ttc_entry,      LEVEL_TTC,      COURSE_TTC },
    { "RR",       level_rr_entry,       LEVEL_RR,       COURSE_RR },

    { "BITDW",    level_bitdw_entry,    LEVEL_BITDW,    COURSE_BITDW },
    { "BOWSER_1", level_bowser_1_entry, LEVEL_BOWSER_1, COURSE_BITDW },
    { "BITFS",    level_bitfs_entry,    LEVEL_BITFS,    COURSE_BITFS },
    { "BOWSER_2", level_bowser_2_entry, LEVEL_BOWSER_2, COURSE_BITFS },
    { "BITS",     level_bits_entry,     LEVEL_BITS,     COURSE_BITS },
    { "BOWSER_3", level_bowser_3_entry, LEVEL_BOWSER_3, COURSE_BITS },

    { "TOTWC",    level_totwc_entry,    LEVEL_TOTWC,    COURSE_TOTWC },
    { "VCUTM",    level_vcutm_entry,    LEVEL_VCUTM,    COURSE_VCUTM },
    { "COTMC",    level_cotmc_entry,    LEVEL_COTMC,    COURSE_COTMC },
    { "PSS",      level_pss_entry,      LEVEL_PSS,      COURSE_PSS },
    { "SA",       level_sa_entry,       LEVEL_SA,       COURSE_SA },
    { "WMOTR",    level_wmotr_entry,    LEVEL_WMOTR,    COURSE_WMOTR },
};
static const u32 sWarpCount = sizeof(sWarps) / sizeof(sWarps[0]);

static const struct CourseWarp *get_warp(const char *name) {
    for (u32 i = 0; i != sWarpCount; ++i) {
        if (streq(sWarps[i].name, name)) {
            return &sWarps[i];
        }
    }
    return NULL;
}

static void debug_warp(const struct DebugArgs *args) {
    const char *name;
    if (!debug_arg_as_string(args, 1, &name)) {
        return;
    }

    s32 actNum;
    if (!debug_arg_as_int(args, 2, &actNum)) {
        return;
    }
    if (actNum < 1 || actNum > 6) {
        return;
    }

    const struct CourseWarp *warp = get_warp(name);
    if (warp == NULL) {
        return;
    }

    // Free everything from the level
    set_sound_disabled(FALSE);
    play_shell_music();
    stop_shell_music();
    stop_cap_music();
    clear_objects();
    clear_area_graph_nodes();
    clear_areas();
    main_pool_pop_state();

    // Reload the level
    gCurrLevelNum = warp->levelNum;
    gCurrCourseNum = warp->courseNum;
    gCurrActNum = actNum;
    gDialogCourseActNum = actNum;
    gCurrAreaIndex = 1;
    level_script_execute((struct LevelCommand *) warp->script);
    sWarpDest.type = 2;
    sWarpDest.levelNum = gCurrLevelNum;
    sWarpDest.areaIdx = 1;
    sWarpDest.nodeId = 0x0A;
    sWarpDest.arg = 0;
    gSavedCourseNum = gCurrCourseNum;

    // Reset Mario's state
    gMarioState->healCounter = 0;
    gMarioState->hurtCounter = 0;
    gMarioState->numCoins = 0;
    gHudDisplay.coins = 0;
}

static void debug_set_lives(const struct DebugArgs *args) {
    s32 x;
    if (debug_arg_as_int(args, 1, &x)) {
        gMarioState->numLives = x;
    }
}

static void debug_set_hp(const struct DebugArgs *args) {
    s32 x;
    if (debug_arg_as_int(args, 1, &x)) {
        gMarioState->health = x;
    }
}

static void debug_set_coins(const struct DebugArgs *args) {
    s32 x;
    if (debug_arg_as_int(args, 1, &x)) {
        gMarioState->numCoins = x;
    }
}

static void debug_set_cap(const struct DebugArgs *args) {
    s32 cap;
    if (!debug_arg_as_int(args, 1, &cap)) {
        return;
    }
    if (cap < 0 || cap > 3) {
        return;
    }

    s32 capTime;
    if (!debug_arg_as_int(args, 2, &capTime)) {
        capTime = 600;
    }

    switch (cap) {
        case 0:
            gMarioState->capTimer = 0;
            gMarioState->flags |= MARIO_CAP_ON_HEAD;
            gMarioState->flags &= ~MARIO_VANISH_CAP;
            gMarioState->flags &= ~MARIO_METAL_CAP;
            gMarioState->flags &= ~MARIO_WING_CAP;
            stop_cap_music();
            break;

        case 1:
            gMarioState->flags |= (MARIO_VANISH_CAP | MARIO_CAP_ON_HEAD);
            gMarioState->capTimer = MAX(gMarioState->capTimer, (u16) capTime);
            play_sound(SOUND_MENU_STAR_SOUND, gMarioState->marioObj->header.gfx.cameraToObject);
            play_sound(SOUND_MARIO_HERE_WE_GO, gMarioState->marioObj->header.gfx.cameraToObject);
            play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP));
            break;

        case 2:
            gMarioState->flags |= (MARIO_METAL_CAP | MARIO_CAP_ON_HEAD);
            gMarioState->capTimer = MAX(gMarioState->capTimer, (u16) capTime);
            play_sound(SOUND_MENU_STAR_SOUND, gMarioState->marioObj->header.gfx.cameraToObject);
            play_sound(SOUND_MARIO_HERE_WE_GO, gMarioState->marioObj->header.gfx.cameraToObject);
            play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_METAL_CAP));
            break;

        case 3:
            gMarioState->flags |= (MARIO_WING_CAP | MARIO_CAP_ON_HEAD);
            gMarioState->capTimer = MAX(gMarioState->capTimer, 3 * (u16) capTime);
            play_sound(SOUND_MENU_STAR_SOUND, gMarioState->marioObj->header.gfx.cameraToObject);
            play_sound(SOUND_MARIO_HERE_WE_GO, gMarioState->marioObj->header.gfx.cameraToObject);
            play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP));
            break;
    }
}

static void debug_spawn_star(const struct DebugArgs *args) {
    s32 starNum;
    if (!debug_arg_as_int(args, 1, &starNum)) {
        return;
    }
    if (starNum < 1 || starNum > 7) {
        return;
    }

    struct Object *star = spawn_object(gMarioState->marioObj, MODEL_STAR, bhvSpawnedStar);
    star->oPosY += 250;
    star->oBehParams = (starNum - 1) << 24;
    star->oInteractionSubtype = INT_SUBTYPE_NO_EXIT;
    obj_set_angle(star, 0, 0, 0);
}

static void debug_set_pos(const struct DebugArgs *args) {
    f32 x, y, z;
    if (debug_arg_as_float(args, 1, &x) &&
        debug_arg_as_float(args, 2, &y) &&
        debug_arg_as_float(args, 3, &z)) {
        gMarioState->pos[0] = x;
        gMarioState->pos[1] = y;
        gMarioState->pos[2] = z;
    }
}

static void debug_set_vel(const struct DebugArgs *args) {
    f32 x, y, z;
    if (debug_arg_as_float(args, 1, &x) &&
        debug_arg_as_float(args, 2, &y) &&
        debug_arg_as_float(args, 3, &z)) {
        gMarioState->vel[0] = x;
        gMarioState->vel[1] = y;
        gMarioState->vel[2] = z;
    }
}

static void debug_set_angle(const struct DebugArgs *args) {
    s32 x, y, z;
    if (debug_arg_as_int(args, 1, &x) &&
        debug_arg_as_int(args, 2, &y) &&
        debug_arg_as_int(args, 3, &z)) {
        gMarioState->faceAngle[0] = x;
        gMarioState->faceAngle[1] = y;
        gMarioState->faceAngle[2] = z;
    }
}

static void debug_move_pos(const struct DebugArgs *args) {
    f32 x, y, z;
    if (debug_arg_as_float(args, 1, &x) &&
        debug_arg_as_float(args, 2, &y) &&
        debug_arg_as_float(args, 3, &z)) {
        gMarioState->pos[0] += x;
        gMarioState->pos[1] += y;
        gMarioState->pos[2] += z;
    }
}

static void debug_move_vel(const struct DebugArgs *args) {
    f32 x, y, z;
    if (debug_arg_as_float(args, 1, &x) &&
        debug_arg_as_float(args, 2, &y) &&
        debug_arg_as_float(args, 3, &z)) {
        gMarioState->vel[0] += x;
        gMarioState->vel[1] += y;
        gMarioState->vel[2] += z;
    }
}

static void debug_move_angle(const struct DebugArgs *args) {
    s32 x, y, z;
    if (debug_arg_as_int(args, 1, &x) &&
        debug_arg_as_int(args, 2, &y) &&
        debug_arg_as_int(args, 3, &z)) {
        gMarioState->faceAngle[0] += x;
        gMarioState->faceAngle[1] += y;
        gMarioState->faceAngle[2] += z;
    }
}

static s8 sPos = FALSE;
static void debug_toggle_pos(UNUSED const struct DebugArgs *args) {
    sPos = !sPos;
}

static s8 sVel = FALSE;
static void debug_toggle_vel(UNUSED const struct DebugArgs *args) {
    sVel = !sVel;
}

static s8 sAngle = FALSE;
static void debug_toggle_angle(UNUSED const struct DebugArgs *args) {
    sAngle = !sAngle;
}

static s8 sDisplayHitbox = FALSE;
static void debug_toggle_hitbox(UNUSED const struct DebugArgs *args) {
    sDisplayHitbox = !sDisplayHitbox;
    debug_set_vertex_data();
}

static s8 sDisplayHurtbox = FALSE;
static void debug_toggle_hurtbox(UNUSED const struct DebugArgs *args) {
    sDisplayHurtbox = !sDisplayHurtbox;
    debug_set_vertex_data();
}

//
// Debug Command line
//

static const void *sDebugCommandList[][2] = {
    { "set_lives",      &debug_set_lives },
    { "set_hp",         &debug_set_hp },
    { "set_coins",      &debug_set_coins },
    { "set_cap",        &debug_set_cap },
    { "spawn_star",     &debug_spawn_star },
    { "set_pos",        &debug_set_pos },
    { "set_vel",        &debug_set_vel },
    { "set_angle",      &debug_set_angle },
    { "move_pos",       &debug_move_pos },
    { "move_vel",       &debug_move_vel },
    { "move_angle",     &debug_move_angle },
    { "pos",            &debug_toggle_pos },
    { "vel",            &debug_toggle_vel },
    { "angle",          &debug_toggle_angle },
    { "hitbox",         &debug_toggle_hitbox },
    { "hurtbox",        &debug_toggle_hurtbox },
    { "warp",           &debug_warp },
};
static const u32 sDebugCommandCount = sizeof(sDebugCommandList) / sizeof(sDebugCommandList[0]);

#define SCANCODE_LSHIFT         42
#define SCANCODE_RSHIFT         54
#define SCANCODE_BACKSPACE      14
#define SCANCODE_RETURN_1       28
#define SCANCODE_RETURN_2       284
#define SCANCODE_ESCAPE         1
#define SCANCODE(code, shift)   (((code) << 1) | (shift))
#define NO_SCANCODE             SCANCODE(0xFFFF, 0)

static const u32 sAzertyScancodes[][3] = {
    { '0', SCANCODE(11, 1), SCANCODE(82, 0) },
    { '1', SCANCODE(2, 1), SCANCODE(79, 0) },
    { '2', SCANCODE(3, 1), SCANCODE(80, 0) },
    { '3', SCANCODE(4, 1), SCANCODE(81, 0) },
    { '4', SCANCODE(5, 1), SCANCODE(75, 0) },
    { '5', SCANCODE(6, 1), SCANCODE(76, 0) },
    { '6', SCANCODE(7, 1), SCANCODE(77, 0) },
    { '7', SCANCODE(8, 1), SCANCODE(71, 0) },
    { '8', SCANCODE(9, 1), SCANCODE(72, 0) },
    { '9', SCANCODE(10, 1), SCANCODE(73, 0) },
    { 'A', SCANCODE(16, 1), NO_SCANCODE },
    { 'B', SCANCODE(48, 1), NO_SCANCODE },
    { 'C', SCANCODE(46, 1), NO_SCANCODE },
    { 'D', SCANCODE(32, 1), NO_SCANCODE },
    { 'E', SCANCODE(18, 1), NO_SCANCODE },
    { 'F', SCANCODE(33, 1), NO_SCANCODE },
    { 'G', SCANCODE(34, 1), NO_SCANCODE },
    { 'H', SCANCODE(35, 1), NO_SCANCODE },
    { 'I', SCANCODE(23, 1), NO_SCANCODE },
    { 'J', SCANCODE(36, 1), NO_SCANCODE },
    { 'K', SCANCODE(37, 1), NO_SCANCODE },
    { 'L', SCANCODE(38, 1), NO_SCANCODE },
    { 'M', SCANCODE(39, 1), NO_SCANCODE },
    { 'N', SCANCODE(49, 1), NO_SCANCODE },
    { 'O', SCANCODE(24, 1), NO_SCANCODE },
    { 'P', SCANCODE(25, 1), NO_SCANCODE },
    { 'Q', SCANCODE(30, 1), NO_SCANCODE },
    { 'R', SCANCODE(19, 1), NO_SCANCODE },
    { 'S', SCANCODE(31, 1), NO_SCANCODE },
    { 'T', SCANCODE(20, 1), NO_SCANCODE },
    { 'U', SCANCODE(22, 1), NO_SCANCODE },
    { 'V', SCANCODE(47, 1), NO_SCANCODE },
    { 'W', SCANCODE(44, 1), NO_SCANCODE },
    { 'X', SCANCODE(45, 1), NO_SCANCODE },
    { 'Y', SCANCODE(21, 1), NO_SCANCODE },
    { 'Z', SCANCODE(17, 1), NO_SCANCODE },
    { 'a', SCANCODE(16, 0), NO_SCANCODE },
    { 'b', SCANCODE(48, 0), NO_SCANCODE },
    { 'c', SCANCODE(46, 0), NO_SCANCODE },
    { 'd', SCANCODE(32, 0), NO_SCANCODE },
    { 'e', SCANCODE(18, 0), NO_SCANCODE },
    { 'f', SCANCODE(33, 0), NO_SCANCODE },
    { 'g', SCANCODE(34, 0), NO_SCANCODE },
    { 'h', SCANCODE(35, 0), NO_SCANCODE },
    { 'i', SCANCODE(23, 0), NO_SCANCODE },
    { 'j', SCANCODE(36, 0), NO_SCANCODE },
    { 'k', SCANCODE(37, 0), NO_SCANCODE },
    { 'l', SCANCODE(38, 0), NO_SCANCODE },
    { 'm', SCANCODE(39, 0), NO_SCANCODE },
    { 'n', SCANCODE(49, 0), NO_SCANCODE },
    { 'o', SCANCODE(24, 0), NO_SCANCODE },
    { 'p', SCANCODE(25, 0), NO_SCANCODE },
    { 'q', SCANCODE(30, 0), NO_SCANCODE },
    { 'r', SCANCODE(19, 0), NO_SCANCODE },
    { 's', SCANCODE(31, 0), NO_SCANCODE },
    { 't', SCANCODE(20, 0), NO_SCANCODE },
    { 'u', SCANCODE(22, 0), NO_SCANCODE },
    { 'v', SCANCODE(47, 0), NO_SCANCODE },
    { 'w', SCANCODE(44, 0), NO_SCANCODE },
    { 'x', SCANCODE(45, 0), NO_SCANCODE },
    { 'y', SCANCODE(21, 0), NO_SCANCODE },
    { 'z', SCANCODE(17, 0), NO_SCANCODE },
    { '\'', SCANCODE(5, 0), NO_SCANCODE },
    { '.', SCANCODE(51, 1), SCANCODE(83, 0) },
    { ',', SCANCODE(50, 0), NO_SCANCODE },
    { ' ', SCANCODE(57, 0), NO_SCANCODE },
    { '-', SCANCODE(74, 0), SCANCODE(7, 0) },
    { '(', SCANCODE(6, 0), NO_SCANCODE },
    { ')', SCANCODE(12, 0), NO_SCANCODE },
    { '&', SCANCODE(2, 0), NO_SCANCODE },
    { ':', SCANCODE(52, 0), NO_SCANCODE },
    { '!', SCANCODE(53, 0), NO_SCANCODE },
    { '%', SCANCODE(40, 1), NO_SCANCODE },
    { '?', SCANCODE(50, 1), NO_SCANCODE },
    { '"', SCANCODE(4, 0), NO_SCANCODE },
    { '~', SCANCODE(3, 0), NO_SCANCODE },
    { '@', SCANCODE(11, 0), NO_SCANCODE },
    { '+', SCANCODE(78, 0), SCANCODE(13, 1) },
    { '*', SCANCODE(43, 0), SCANCODE(55, 0) },
    { '_', SCANCODE(9, 0), NO_SCANCODE },
};
static const u32 sAzertyScancodeCount = sizeof(sAzertyScancodes) / sizeof(sAzertyScancodes[0]);

static char debug_get_char_from_scancode(int scancode, int shift) {
    u32 sc = SCANCODE(scancode, shift != 0);
    for (u32 i = 0; i != sAzertyScancodeCount; ++i) {
        if (sAzertyScancodes[i][1] == sc || sAzertyScancodes[i][2] == sc) {
            return (char) sAzertyScancodes[i][0];
        }
    }
    return 0;
}

#undef SCANCODE
#undef NO_SCANCODE

static struct DebugArgs debug_cmd_get_args(const char *str) {
    struct DebugArgs dbgArgs = { .argc = 0 };
    u32 i = 0;
    for (;; str++) {
        char c = *str;
        if (c == ' ' || c == 0) {
            if (i > 0) {
                dbgArgs.args[dbgArgs.argc][i] = 0;
                dbgArgs.argc++;
                if (dbgArgs.argc == DEBUG_MAX_NUM_ARGS) {
                    break;
                }
                i = 0;
            }
            if (c == 0) {
                break;
            }
        } else {
            if (i < DEBUG_ARG_MAX_LENGTH - 1) {
                dbgArgs.args[dbgArgs.argc][i] = c;
                i++;
            }
        }
    }
    return dbgArgs;
}

static const void *debug_get_command(const char *arg0) {
    for (u32 i = 0; i != sDebugCommandCount; ++i) {
        const char *command = (const char *) (sDebugCommandList[i][0]);
        if (streq(arg0, command)) {
            return sDebugCommandList[i][1];
        }
    }
    return NULL;
}

#define DEBUG_CMD_MAX_LENGTH (DEBUG_ARG_MAX_LENGTH * DEBUG_MAX_NUM_ARGS)
static int sKey = 0;
static int sShift = 0;
static s8 sDebugCmdOpen = FALSE;
static char sDebugCmdString[DEBUG_CMD_MAX_LENGTH];
static u32 sDebugCmdLength;

//
// Debug Update
//

void debug_set_key_down(int scancode) {
    if (scancode == SCANCODE_LSHIFT) {
        sShift = SCANCODE_LSHIFT;
    } else if (scancode == SCANCODE_RSHIFT) {
        sShift = SCANCODE_RSHIFT;
    } else {
        sKey = scancode;
    }
}

void debug_set_key_up(int scancode) {
    if (scancode == SCANCODE_LSHIFT || scancode == SCANCODE_RSHIFT) {
        sShift = 0;
    }
}

void debug_update() {
    
    // Command line update
    struct DebugArgs dbgArgs;
    const void *command = NULL;
    if (sKey == SCANCODE_RETURN_1 || sKey == SCANCODE_RETURN_2) {
        if (sDebugCmdOpen) {
            dbgArgs = debug_cmd_get_args(sDebugCmdString);
            if (dbgArgs.argc > 0) {
                command = debug_get_command(dbgArgs.args[0]);
            }
        }
        sDebugCmdOpen = !sDebugCmdOpen;
    } else if (sKey == SCANCODE_ESCAPE) {
        sDebugCmdOpen = FALSE;
    } else if (sKey == SCANCODE_BACKSPACE) {
        if (sDebugCmdLength > 0) {
            sDebugCmdLength--;
        }
    } else if (sDebugCmdOpen && sKey != 0 && sDebugCmdLength < DEBUG_CMD_MAX_LENGTH - 1) {
        char c = debug_get_char_from_scancode(sKey, sShift);
        if (c != 0) {
            sDebugCmdString[sDebugCmdLength++] = c;
        }
    }
    sDebugCmdString[sDebugCmdLength] = 0;
    sKey = 0;
    if (sDebugCmdOpen) {
        debug_add_box(0, 222, 480, 18, 0x00000080);
        debug_add_string(2, 224, 0xffffffff, 0, sm64_string("%s", sDebugCmdString));
    } else {
        bzero(sDebugCmdString, DEBUG_CMD_MAX_LENGTH);
        sDebugCmdLength = 0;
    }

    // Display update
    if (sPos) debug_add_string(4, 36, 0xFFFFFFFF, 0x000000FF, sm64_string("Pos %.1f %.1f %.1f", gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2]));
    if (sVel) debug_add_string(4, 20, 0xFFFFFFFF, 0x000000FF, sm64_string("Vel %.2f %.2f %.2f %.2f", gMarioState->vel[0], gMarioState->vel[1], gMarioState->vel[2], gMarioState->forwardVel));
    if (sAngle) debug_add_string(4, 4, 0xFFFFFFFF, 0x000000FF, sm64_string("Ang %04hX %04hX %04hX", gMarioState->faceAngle[0], gMarioState->faceAngle[1], gMarioState->faceAngle[2]));
    debug_display_objects_hitbox_from_list(OBJ_LIST_SURFACE, sDisplayHitbox, sDisplayHurtbox);
    debug_display_objects_hitbox_from_list(OBJ_LIST_POLELIKE, sDisplayHitbox, sDisplayHurtbox);
    debug_display_objects_hitbox_from_list(OBJ_LIST_PLAYER, sDisplayHitbox, sDisplayHurtbox);
    debug_display_objects_hitbox_from_list(OBJ_LIST_PUSHABLE, sDisplayHitbox, sDisplayHurtbox);
    debug_display_objects_hitbox_from_list(OBJ_LIST_GENACTOR, sDisplayHitbox, sDisplayHurtbox);
    debug_display_objects_hitbox_from_list(OBJ_LIST_DESTRUCTIVE, sDisplayHitbox, sDisplayHurtbox);
    debug_display_objects_hitbox_from_list(OBJ_LIST_LEVEL, sDisplayHitbox, sDisplayHurtbox);
    debug_display_text();

    // Execute command
    if (command != NULL) {
        ((void (*)(const struct DebugArgs *)) command)(&dbgArgs);
    }
}

