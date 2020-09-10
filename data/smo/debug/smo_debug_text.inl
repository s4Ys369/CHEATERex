//
// Debug String
//

#define DEBUG_CHAR_WIDTH        7
#define DEBUG_LINE_HEIGHT       14
#define DEBUG_STRING_MAX_LENGTH 256
#define DEBUG_MAX_STRINGS       64

struct DebugString {
    u8 str[DEBUG_STRING_MAX_LENGTH];
    s32 x;
    s32 y;
    u32 rgbaFront;
    u32 rgbaBack;
};
static struct DebugString sDebugStrings[DEBUG_MAX_STRINGS];
static u32 sDebugStringCount = 0;

static void smo_debug_render_string(s16 x, s16 y, const u8 *str) {
    create_dl_translation_matrix(MENU_MTX_PUSH, x, y, 0.0f);
    for (; *str != DIALOG_CHAR_TERMINATOR; ++str) {
        if (*str != DIALOG_CHAR_SPACE) {
            void **fontLUT = segmented_to_virtual(main_font_lut);
            void *packedTexture = segmented_to_virtual(fontLUT[*str]);
            gDPPipeSync(gDisplayListHead++);
            gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, VIRTUAL_TO_PHYSICAL(packedTexture));
            gSPDisplayList(gDisplayListHead++, dl_ia_text_tex_settings);
        }
        create_dl_translation_matrix(MENU_MTX_NOPUSH, DEBUG_CHAR_WIDTH, 0.0f, 0.0f);
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

static void smo_debug_print_string(u32 i) {
    struct DebugString *dbgStr = &sDebugStrings[i];
    s32 x0 = GFX_DIMENSIONS_FROM_LEFT_EDGE(dbgStr->x);
    s32 y0 = dbgStr->y;

    // Background
    if ((dbgStr->rgbaBack && 0xFF) != 0) {
        gDPSetEnvColor(gDisplayListHead++, ((dbgStr->rgbaBack >> 24) & 0xFF), ((dbgStr->rgbaBack >> 16) & 0xFF), ((dbgStr->rgbaBack >> 8) & 0xFF), ((dbgStr->rgbaBack >> 0) & 0xFF));
        smo_debug_render_string(x0 + 1, y0 - 1, dbgStr->str);
    }

    // Foreground
    if ((dbgStr->rgbaFront && 0xFF) != 0) {
        gDPSetEnvColor(gDisplayListHead++, ((dbgStr->rgbaFront >> 24) & 0xFF), ((dbgStr->rgbaFront >> 16) & 0xFF), ((dbgStr->rgbaFront >> 8) & 0xFF), ((dbgStr->rgbaFront >> 0) & 0xFF));
        smo_debug_render_string(x0, y0, dbgStr->str);
    }
}

//
// Debug Text
//

void smo_debug_add_string(s32 x, s32 y, u32 rgbaFront, u32 rgbaBack, const char *fmt, ...) {
    if (!gSmoDebug || !fmt || sDebugStringCount == DEBUG_MAX_STRINGS) {
        return;
    }

    char str[1024];
    va_list arg;
    va_start(arg, fmt);
    s32 len = vsprintf(str, fmt, arg);
    va_end(arg);
    len = MAX(0, MIN(DEBUG_STRING_MAX_LENGTH - 1, len));
    if (len == 0) {
        return;
    }

    struct DebugString *dbgStr = &sDebugStrings[sDebugStringCount++];
    for (s32 i = 0; i != len; ++i) {
        char c = str[i];
        if (c >= '0' && c <= '9') {
            dbgStr->str[i] = (u8)(c - '0');
        } else if (c >= 'A' && c <= 'Z') {
            dbgStr->str[i] = (u8)(c - 'A' + 10);
        } else if (c >= 'a' && c <= 'z') {
            dbgStr->str[i] = (u8)(c - 'a' + 36);
        } else if (c == '\'') {
            dbgStr->str[i] = 62;
        } else if (c == '.') {
            dbgStr->str[i] = 63;
        } else if (c == '-') {
            dbgStr->str[i] = 159;
        } else {
            dbgStr->str[i] = DIALOG_CHAR_SPACE;
        }
    }
    dbgStr->str[len] = DIALOG_CHAR_TERMINATOR;
    dbgStr->x = x;
    dbgStr->y = y;
    dbgStr->rgbaFront = rgbaFront;
    dbgStr->rgbaBack = rgbaBack;
}

static void smo_debug_display_text() {
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 80, SCREEN_WIDTH, SCREEN_HEIGHT);
    for (u32 i = 0; i != sDebugStringCount; ++i) smo_debug_print_string(i);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    sDebugStringCount = 0;
}
