#ifndef SM64_UTILS_H
#define SM64_UTILS_H

#include "types.h"

const u8 *sm64_string(const char *fmt, ...);
u32 sm64_strlen(const u8 *str64);
struct Object *obj_spawn_with_geo(struct Object *parent, const GeoLayout *geoLayout, const BehaviorScript *behavior);

#endif