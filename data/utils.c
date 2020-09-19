#include "utils.h"
#include "sm64.h"
#include "game/object_list_processor.h"
#include "game/object_helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//
// C String to SM64 String conversion
//

#define STRING_MAX_LENGTH   1024
#define STRING_MAX_COUNT    16

static const u8 sSm64CharTable[][2] = {
    { '0', 0 },
    { '1', 1 },
    { '2', 2 },
    { '3', 3 },
    { '4', 4 },
    { '5', 5 },
    { '6', 6 },
    { '7', 7 },
    { '8', 8 },
    { '9', 9 },
    { 'A', 10 },
    { 'B', 11 },
    { 'C', 12 },
    { 'D', 13 },
    { 'E', 14 },
    { 'F', 15 },
    { 'G', 16 },
    { 'H', 17 },
    { 'I', 18 },
    { 'J', 19 },
    { 'K', 20 },
    { 'L', 21 },
    { 'M', 22 },
    { 'N', 23 },
    { 'O', 24 },
    { 'P', 25 },
    { 'Q', 26 },
    { 'R', 27 },
    { 'S', 28 },
    { 'T', 29 },
    { 'U', 30 },
    { 'V', 31 },
    { 'W', 32 },
    { 'X', 33 },
    { 'Y', 34 },
    { 'Z', 35 },
    { 'a', 36 },
    { 'b', 37 },
    { 'c', 38 },
    { 'd', 39 },
    { 'e', 40 },
    { 'f', 41 },
    { 'g', 42 },
    { 'h', 43 },
    { 'i', 44 },
    { 'j', 45 },
    { 'k', 46 },
    { 'l', 47 },
    { 'm', 48 },
    { 'n', 49 },
    { 'o', 50 },
    { 'p', 51 },
    { 'q', 52 },
    { 'r', 53 },
    { 's', 54 },
    { 't', 55 },
    { 'u', 56 },
    { 'v', 57 },
    { 'w', 58 },
    { 'x', 59 },
    { 'y', 60 },
    { 'z', 61 },
    { '\'', 62 },
    { '.', 63 },
    { ',', 111 },
    { ' ', 158 },
    { '-', 159 },
    { '(', 225 },
    { ')', 227 },
    { '&', 229 },
    { ':', 230 },
    { '!', 242 },
    { '%', 243 },
    { '?', 244 },
    { '"', 246 },
    { '~', 247 },
    { '@', 249 },
    { '+', 250 },
    { '*', 251 },
    { '_', 253 },
    { 0, 255 },
};
static const u32 sSm64CharCount = sizeof(sSm64CharTable) / sizeof(sSm64CharTable[0]);

static u8 sm64_char(char c) {
    for (u32 i = 0; i != sSm64CharCount; ++i) {
        if (sSm64CharTable[i][0] == (u8) c) {
            return sSm64CharTable[i][1];
        }
    }
    return 0xFF;
}

const u8 *sm64_string(const char *fmt, ...) {
    static u8 sStringBuffers[STRING_MAX_LENGTH][STRING_MAX_COUNT];
    static u32 sStringIndex = 0;
    char buffer[STRING_MAX_LENGTH];

    // valist stuff
    char *str = &buffer[0];
    va_list arg;
    va_start(arg, fmt);
    vsnprintf(str, STRING_MAX_LENGTH, fmt, arg);
    va_end(arg);

    // Rotating buffer
    u8 *str64 = sStringBuffers[sStringIndex];
    sStringIndex = (sStringIndex + 1) % STRING_MAX_COUNT;

    // Conversion
    memset(str64, 0xFF, STRING_MAX_LENGTH);
    u32 i = 0;
    for (; *str != 0 && i < STRING_MAX_LENGTH - 1; str++) {
        u8 c = sm64_char(*str);
        if (c != 0xFF) {
            str64[i++] = c;
        }
    }
    return (const u8 *) str64;
}

u32 sm64_strlen(const u8 *str64) {
    if (str64 == NULL) {
        return 0;
    }
    u32 l = 0;
    for (; *str64 != 0xFF; ++str64, ++l);
    return l;
}

//
// Dynamic Object Geometry allocation
//

#define ALLOC_SIZE 0x1000

static void *sLoadedGraphNodes[64][2];
static u32 sLoadedGraphNodeCount = 0;

static struct GraphNode *get_graph_node_from_geo(const GeoLayout *geoLayout) {
    for (u32 i = 0; i != sLoadedGraphNodeCount; ++i) {
        if ((const GeoLayout *) sLoadedGraphNodes[i][0] == geoLayout) {
            return (struct GraphNode *) sLoadedGraphNodes[i][1];
        }
    }
    if (sLoadedGraphNodeCount == 64) {
        return NULL;
    }
    sLoadedGraphNodes[sLoadedGraphNodeCount][0] = (void *) geoLayout;
    struct AllocOnlyPool *pool = calloc(1, ALLOC_SIZE);
    pool->totalSpace = ALLOC_SIZE;
    pool->usedSpace = 0;
    pool->startPtr = (u8 *) pool + sizeof(struct AllocOnlyPool);
    pool->freePtr = (u8 *) pool + sizeof(struct AllocOnlyPool);
    sLoadedGraphNodes[sLoadedGraphNodeCount][1] = (void *) process_geo_layout(pool, (void *) geoLayout);
    return (struct GraphNode *) sLoadedGraphNodes[sLoadedGraphNodeCount++][1];
}

struct Object *obj_spawn_with_geo(struct Object *parent, const GeoLayout *geoLayout, const BehaviorScript *behavior) {
    struct Object *obj = spawn_object(parent, 0, behavior);
    obj->header.gfx.sharedChild = get_graph_node_from_geo(geoLayout);
    return obj;
}







