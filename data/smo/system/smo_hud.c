#include "../smo_c_includes.h"

//
// Power Meter data
//

static Vtx sPowerMeterSmoVertexArray[0x1000];

static u8 sPowerMeterSmoColorsRGB[][2][3] = {
          /* Center */          /* Border */
    { { 0xFF, 0xFF, 0xFF }, { 0xFF, 0xFF, 0xFF } }, // Empty segment
    { { 0xFF, 0x00, 0x00 }, { 0xD0, 0x00, 0x00 } }, // Critical life
    { { 0xFF, 0xC8, 0x00 }, { 0xD0, 0xA0, 0x00 } }, // Low life
    { { 0x00, 0xE0, 0x00 }, { 0x00, 0xB8, 0x00 } }, // Normal
    { { 0x00, 0xC8, 0xFF }, { 0x00, 0xA0, 0xD0 } }, // Life up
    { { 0xFF, 0xFF, 0xFF }, { 0xD0, 0xD0, 0xD0 } }, // Heart (white)
    { { 0xFF, 0xB8, 0xB8 }, { 0xD0, 0x90, 0x90 } }, // Heart (pale red)
    { { 0xFF, 0x40, 0x40 }, { 0xD0, 0x30, 0x30 } }, // Heart (red)
    { { 0x30, 0x30, 0x30 }, { 0x30, 0x30, 0x30 } }, // Background
};

static s32 sPowerMeterHeartPulse[30] = {
    5, 6, 7, 7, 7, 6, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};

#define BACKGROUND_NUM_TRIANGLES    64
#define NUM_SUB_SEGMENTS            32
#define HEART_NUM_PIECES            64
#define SUB_SEG_D0                  16.0f
#define SUB_SEG_D1                  18.0f
#define SUB_SEG_D2                  20.0f
#define SUB_SEG_D3                  22.0f
#define HEART_D1_X                  20.0f
#define HEART_D1_Y                  18.0f
#define HEART_D2_X                  22.0f
#define HEART_D2_Y                  20.0f
#define BACKGROUND_R                sPowerMeterSmoColorsRGB[8][0][0]
#define BACKGROUND_G                sPowerMeterSmoColorsRGB[8][0][1]
#define BACKGROUND_B                sPowerMeterSmoColorsRGB[8][0][2]
#define SUB_SEG_CENTER_R            sPowerMeterSmoColorsRGB[rgbIdx][0][0]
#define SUB_SEG_CENTER_G            sPowerMeterSmoColorsRGB[rgbIdx][0][1]
#define SUB_SEG_CENTER_B            sPowerMeterSmoColorsRGB[rgbIdx][0][2]
#define SUB_SEG_BORDER_R            sPowerMeterSmoColorsRGB[rgbIdx][1][0]
#define SUB_SEG_BORDER_G            sPowerMeterSmoColorsRGB[rgbIdx][1][1]
#define SUB_SEG_BORDER_B            sPowerMeterSmoColorsRGB[rgbIdx][1][2]
#define HEART_CENTER_R              sPowerMeterSmoColorsRGB[hIdx][0][0]
#define HEART_CENTER_G              sPowerMeterSmoColorsRGB[hIdx][0][1]
#define HEART_CENTER_B              sPowerMeterSmoColorsRGB[hIdx][0][2]
#define HEART_BORDER_R              sPowerMeterSmoColorsRGB[hIdx][1][0]
#define HEART_BORDER_G              sPowerMeterSmoColorsRGB[hIdx][1][1]
#define HEART_BORDER_B              sPowerMeterSmoColorsRGB[hIdx][1][2]

#define NUM_FULL_SEGMENTS           smo_get_num_segments_per_health_gauge()
#define HP_PER_SEGMENT              smo_get_hp_per_segment()
#define HP_MAX_NO_LIFEUP            smo_get_max_hp(0)
#define HP_CRITICAL                 ((HP_PER_SEGMENT * 3) / 2)
#define HP_LOW                      ((HP_PER_SEGMENT * 5) / 2)
#define HP_DISPLAY_VALUE            ((hp + HP_PER_SEGMENT / 2) / HP_PER_SEGMENT)

//
// Background
//

static Vtx *smo_power_meter_render_background_triangle(Vtx *vtx, f32 x, f32 y, f32 a0, f32 a1, f32 d) {
    Vtx v0 = { { { x,                y,                0 }, 0, { 0, 0 }, { BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 0xFF } } };
    Vtx v1 = { { { x + sins(a0) * d, y + coss(a0) * d, 0 }, 0, { 0, 0 }, { BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 0xFF } } };
    Vtx v2 = { { { x + sins(a1) * d, y + coss(a1) * d, 0 }, 0, { 0, 0 }, { BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 0xFF } } };
    vtx[0] = v0;
    vtx[1] = v1;
    vtx[2] = v2;
    gSPVertex(gDisplayListHead++, vtx, 3, 0);
    gSP1Triangle(gDisplayListHead++, 0, 2, 1, 0x0);
    return &vtx[3];
}


static Vtx *smo_power_meter_render_background(Vtx *vtx, f32 x, f32 y) {
    for (s32 i = 0; i != BACKGROUND_NUM_TRIANGLES; ++i) {
        f32 a0 = ((i + 0) * 65536.f) / BACKGROUND_NUM_TRIANGLES;
        f32 a1 = ((i + 1) * 65536.f) / BACKGROUND_NUM_TRIANGLES;
        vtx = smo_power_meter_render_background_triangle(vtx, x, y, a0, a1, SUB_SEG_D3 + 0.5f);
    }
    return vtx;
}

//
// Health segments
//

static Vtx *smo_power_meter_render_sub_segment(Vtx *vtx, f32 x, f32 y, f32 a0, f32 a1, s32 rgbIdx) {
    Vtx v0 = { { { x + sins(a0) * SUB_SEG_D0, y + coss(a0) * SUB_SEG_D0, 0 }, 0, { 0, 0 }, { SUB_SEG_BORDER_R, SUB_SEG_BORDER_G, SUB_SEG_BORDER_B, 0xFF } } };
    Vtx v1 = { { { x + sins(a0) * SUB_SEG_D1, y + coss(a0) * SUB_SEG_D1, 0 }, 0, { 0, 0 }, { SUB_SEG_CENTER_R, SUB_SEG_CENTER_G, SUB_SEG_CENTER_B, 0xFF } } };
    Vtx v2 = { { { x + sins(a0) * SUB_SEG_D2, y + coss(a0) * SUB_SEG_D2, 0 }, 0, { 0, 0 }, { SUB_SEG_CENTER_R, SUB_SEG_CENTER_G, SUB_SEG_CENTER_B, 0xFF } } };
    Vtx v3 = { { { x + sins(a0) * SUB_SEG_D3, y + coss(a0) * SUB_SEG_D3, 0 }, 0, { 0, 0 }, { SUB_SEG_BORDER_R, SUB_SEG_BORDER_G, SUB_SEG_BORDER_B, 0xFF } } };
    Vtx v4 = { { { x + sins(a1) * SUB_SEG_D0, y + coss(a1) * SUB_SEG_D0, 0 }, 0, { 0, 0 }, { SUB_SEG_BORDER_R, SUB_SEG_BORDER_G, SUB_SEG_BORDER_B, 0xFF } } };
    Vtx v5 = { { { x + sins(a1) * SUB_SEG_D1, y + coss(a1) * SUB_SEG_D1, 0 }, 0, { 0, 0 }, { SUB_SEG_CENTER_R, SUB_SEG_CENTER_G, SUB_SEG_CENTER_B, 0xFF } } };
    Vtx v6 = { { { x + sins(a1) * SUB_SEG_D2, y + coss(a1) * SUB_SEG_D2, 0 }, 0, { 0, 0 }, { SUB_SEG_CENTER_R, SUB_SEG_CENTER_G, SUB_SEG_CENTER_B, 0xFF } } };
    Vtx v7 = { { { x + sins(a1) * SUB_SEG_D3, y + coss(a1) * SUB_SEG_D3, 0 }, 0, { 0, 0 }, { SUB_SEG_BORDER_R, SUB_SEG_BORDER_G, SUB_SEG_BORDER_B, 0xFF } } };
    vtx[0] = v0;
    vtx[1] = v1;
    vtx[2] = v2;
    vtx[3] = v3;
    vtx[4] = v4;
    vtx[5] = v5;
    vtx[6] = v6;
    vtx[7] = v7;
    gSPVertex(gDisplayListHead++, vtx, 8, 0);
    gSP2Triangles(gDisplayListHead++, 2, 6, 3, 0x0, 3, 6, 7, 0x0);
    gSP2Triangles(gDisplayListHead++, 1, 5, 2, 0x0, 2, 5, 6, 0x0);
    gSP2Triangles(gDisplayListHead++, 0, 4, 1, 0x0, 1, 4, 5, 0x0);
    return &vtx[8];
}

static Vtx *smo_power_meter_render_segment(Vtx *vtx, f32 x, f32 y, s32 hp, s32 segment, s32 isLifeUp) {
    f32 aMax = (hp * 65536.f) / (NUM_FULL_SEGMENTS * HP_PER_SEGMENT);
    f32 da = 65536.f / (NUM_SUB_SEGMENTS * NUM_FULL_SEGMENTS);
    for (s32 i = 0; i != NUM_SUB_SEGMENTS; ++i) {
        f32 a0 = da * (segment * NUM_SUB_SEGMENTS + i + 0);
        f32 a1 = da * (segment * NUM_SUB_SEGMENTS + i + 1);
        s32 rgbIdx = 0;
        if ((a0 + a1) / 2 <= aMax) {
            if (isLifeUp) {
                rgbIdx = 4;
            } else if (hp > HP_LOW) {
                rgbIdx = 3;
            } else if (hp > HP_CRITICAL) {
                rgbIdx = 2;
            } else {
                rgbIdx = 1;
            }
        }
        vtx = smo_power_meter_render_sub_segment(vtx, x, y, a0, a1, rgbIdx);
    }
    return vtx;
}

static Vtx *smo_power_meter_render_segments(Vtx *vtx, f32 x, f32 y, s32 hp, s32 isLifeUp) {
    for (s32 segment = 0; segment != NUM_FULL_SEGMENTS; ++segment) {
        f32 sa = (segment * 65536 + 32768) / NUM_FULL_SEGMENTS;
        f32 dx = sins(sa);
        f32 dy = coss(sa);
        vtx = smo_power_meter_render_segment(vtx, x + dx, y + dy, hp, segment, isLifeUp);
    }
    return vtx;
}

//
// Heart
//

static Vtx *smo_power_meter_render_heart_piece(Vtx *vtx, f32 x, f32 y, f32 t0, f32 t1, s32 hIdx) {

    // Heart shape <3
    f32 a0 = t0 * 65536.f / (2.f * 3.14159265359f);
    f32 a1 = t1 * 65536.f / (2.f * 3.14159265359f);
    f32 r0 = (1.f - absx(t0)) * (1.f + 3.f * absx(t0));
    f32 r1 = (1.f - absx(t1)) * (1.f + 3.f * absx(t1));
    f32 dx0 = r0 * sins(a0);
    f32 dy0 = r0 * coss(a0) - 0.65f;
    f32 dx1 = r1 * sins(a1);
    f32 dy1 = r1 * coss(a1) - 0.65f;

    Vtx v0 = { { { x,                    y,                    0 }, 0, { 0, 0 }, { HEART_CENTER_R, HEART_CENTER_G, HEART_CENTER_B, 0xFF } } };
    Vtx v1 = { { { x + dx0 * HEART_D1_X, y + dy0 * HEART_D1_Y, 0 }, 0, { 0, 0 }, { HEART_CENTER_R, HEART_CENTER_G, HEART_CENTER_B, 0xFF } } };
    Vtx v2 = { { { x + dx0 * HEART_D2_X, y + dy0 * HEART_D2_Y, 0 }, 0, { 0, 0 }, { HEART_BORDER_R, HEART_BORDER_G, HEART_BORDER_B, 0xFF } } };
    Vtx v3 = { { { x + dx1 * HEART_D1_X, y + dy1 * HEART_D1_Y, 0 }, 0, { 0, 0 }, { HEART_CENTER_R, HEART_CENTER_G, HEART_CENTER_B, 0xFF } } };
    Vtx v4 = { { { x + dx1 * HEART_D2_X, y + dy1 * HEART_D2_Y, 0 }, 0, { 0, 0 }, { HEART_BORDER_R, HEART_BORDER_G, HEART_BORDER_B, 0xFF } } };
    vtx[0] = v0;
    vtx[1] = v1;
    vtx[2] = v2;
    vtx[3] = v3;
    vtx[4] = v4;
    gSPVertex(gDisplayListHead++, vtx, 5, 0);
    gSP2Triangles(gDisplayListHead++, 2, 1, 4, 0x0, 1, 3, 4, 0x0);
    gSP1Triangle(gDisplayListHead++, 1, 0, 3, 0x0);
    return &vtx[5];
}

static Vtx *smo_power_meter_render_heart(Vtx *vtx, f32 x, f32 y, s32 hp) {
    for (s32 i = 0; i != HEART_NUM_PIECES; ++i) {
        f32 t0 = (i + 0 - 32) / (HEART_NUM_PIECES / 2.f);
        f32 t1 = (i + 1 - 32) / (HEART_NUM_PIECES / 2.f);
        s32 hIdx = sPowerMeterHeartPulse[hp <= HP_CRITICAL ? gGlobalTimer % 30 : 0];
        vtx = smo_power_meter_render_heart_piece(vtx, x, y, t0, t1, hIdx);
    }
    return vtx;
}

//
// Health gauge
//

static Vtx *smo_power_meter_render_health_gauge(Vtx *vtx, f32 x, f32 y, s32 hp, f32 luPos) {
    hp = MIN(hp, HP_MAX_NO_LIFEUP);
    vtx = smo_power_meter_render_segments(vtx, x, y, hp, FALSE);
    vtx = smo_power_meter_render_heart(vtx, x, y - 1.5f, hp);
    if (luPos > 0.1f) {
        print_text(x - 8, y - 10, int_to_string(HP_DISPLAY_VALUE, "%d"));
    }
    return vtx;
}

static Vtx *smo_power_meter_render_health_gauge_life_up(Vtx *vtx, f32 x, f32 y, s32 hp, f32 luPos) {
    if (hp > HP_MAX_NO_LIFEUP) {
        x = x + luPos * ((SCREEN_WIDTH * 0.5f) - x);
        y = y + luPos * ((SCREEN_HEIGHT * 0.65f) - y);
        vtx = smo_power_meter_render_background(vtx, x, y);
        vtx = smo_power_meter_render_segments(vtx, x, y, hp - HP_MAX_NO_LIFEUP, TRUE);
        vtx = smo_power_meter_render_heart(vtx, x, y - 1.5f, hp);
        print_text(x - 8, y - 10, int_to_string(HP_DISPLAY_VALUE, "%d"));
    }
    return vtx;
}

//
// HUD
//

static void smo_render_power_meter(struct MarioState *m) {
    if (gHudDisplay.flags & HUD_DISPLAY_FLAG_CAMERA_AND_POWER) {
        f32 x = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(36);
        f32 y = 204.f;
        s32 hp = m->marioObj->oHpCounter;
        if (smo_is_mario_losing_hp(m)) {
            x += (random_float() * 12.f) - 6.f;
            y += (random_float() * 12.f) - 6.f;
        }
        f32 luPos = smo_get_life_up_gauge_position(hp);
        gDPPipeSync(gDisplayListHead++);
        gSPClearGeometryMode(gDisplayListHead++, G_LIGHTING);
        gDPSetCombineMode(gDisplayListHead++, G_CC_SHADE, G_CC_SHADE);
        Vtx *vtx = sPowerMeterSmoVertexArray;
        vtx = smo_power_meter_render_health_gauge(vtx, x, y, hp, luPos);
        vtx = smo_power_meter_render_health_gauge_life_up(vtx, x - 8, y, hp, luPos);
        gDPPipeSync(gDisplayListHead++);
        gSPSetGeometryMode(gDisplayListHead++, G_LIGHTING);
    }
}

static void smo_render_hud_glyph(s32 x, s32 y, u8 *texture) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
    gSPDisplayList(gDisplayListHead++, &dl_hud_img_load_tex_block);
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 15) << 2, (y + 15) << 2, G_TX_RENDERTILE, 0, 0, 4 << 10, 1 << 10);
}

static u32 sO2DisplayTimer = 60;
static void smo_render_values(struct MarioState *m) {
    f32 y = 209;

    // Star count
    if (gHudDisplay.flags & HUD_DISPLAY_FLAG_STAR_COUNT) {
        print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), y, "-"); // 'Star' glyph
        print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(38), y, "*"); // 'X' glyph
        print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(54), y, int_to_string(gHudDisplay.stars, "%d"));
        y -= 20;
    }

    // Key count
#ifdef SMO_SGI
    if (gHudDisplay.flags & HUD_DISPLAY_FLAG_KEYS) {
        if (m->numKeys > 0 && m->numKeys < 10) {
            gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);
            smo_render_hud_glyph(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), 35, (u8 *) "textures/segment2/boo_key.rgba16");
            gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(38), y, "*"); // 'X' glyph
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(54), y, int_to_string(m->numKeys, "%d"));
            y -= 20;
        }
    }
#endif

    // Coint count
    if (gHudDisplay.flags & HUD_DISPLAY_FLAG_COIN_COUNT) {
        print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), y, "+"); // 'Coin' glyph
        print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(38), y, "*"); // 'X' glyph
        print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(54), y, int_to_string(gHudDisplay.coins, "%d"));
        y -= 20;
    }

    // O2 level
    if (gHudDisplay.flags & HUD_DISPLAY_FLAG_STAR_COUNT) {
        if (m->marioObj->oO2 > 0) {
            sO2DisplayTimer = 0;
        }
        if (sO2DisplayTimer++ < 60) {
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(22), y - 6, "2");
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(13), y, "O");
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(38), y, "*"); // 'X' glyph
            print_text(GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(54), y, int_to_string(MAX((((s32)(MARIO_BREATH_MAX_DURATION - m->marioObj->oO2) * 100) / MARIO_BREATH_MAX_DURATION), 0), "%03d"));
            y -= 20;
        }
    }
}

void smo_render_hud(struct MarioState *m) {
    if (gHudDisplay.flags != HUD_DISPLAY_NONE && configHUD) {
        create_dl_ortho_matrix();

        // Cannon reticle
        if (gCurrentArea != NULL && gCurrentArea->camera->mode == CAMERA_MODE_INSIDE_CANNON) {
            render_hud_cannon_reticle();
        }

        // Values
        smo_render_values(m);

        // Power meter
        smo_render_power_meter(m);
    }
}

void smo_hud_reset() {
    sO2DisplayTimer = 60;
}
