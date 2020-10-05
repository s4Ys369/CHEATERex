#include "smo_cappy_common.inl"

//
// Init
//

void cappy_bhv2_init(struct Object *cappy, struct MarioState *m) {
    cappy->oPosX = m->pos[0];
    cappy->oPosY = m->pos[1] + 60;
    cappy->oPosZ = m->pos[2];
    cappy->oFaceAngleYaw = m->faceAngle[1];
}

//
// Movement
//

#define STEPS 16
static void cappy_bhv2_move(struct Object *cappy, struct MarioState *m) {
    f32 radius = MIN(cappy->oTimer * 16.f, 240.f);
    s16 angle = (s16)(cappy->oFaceAngleYaw + (s32)(cappy->oTimer) * 0x0C00);
    f32 dx = radius * coss(angle);
    f32 dz = radius * sins(angle);
    cappy->oPosX = m->pos[0];
    cappy->oPosY = m->pos[1] + 60;
    cappy->oPosZ = m->pos[2];

    for (s32 i = 0; i != STEPS; ++i) {
        f32 x = cappy->oPosX;
        f32 y = cappy->oPosY;
        f32 z = cappy->oPosZ;
        cappy->oPosX += (dx / STEPS);
        cappy->oPosZ += (dz / STEPS);

        // Walls
        struct WallCollisionData hitbox;
        hitbox.x = cappy->oPosX;
        hitbox.y = cappy->oPosY;
        hitbox.z = cappy->oPosZ;
        hitbox.offsetY = cappy->hitboxHeight / 2;
        hitbox.radius = cappy->oWallHitboxRadius;
        if (find_wall_collisions(&hitbox) != 0) {
            cappy_register_wall_object(cappy, m, hitbox.walls[0]->object);
            cappy->oPosX = hitbox.x;
            cappy->oPosY = hitbox.y;
            cappy->oPosZ = hitbox.z;
            break;
        }

        // Floor
        struct Surface *floor = NULL;
        f32 floorY = find_floor(cappy->oPosX, cappy->oPosY, cappy->oPosZ, &floor);
        if (floor != NULL) {
            f32 diffY = cappy->oPosY - floorY;
            if (diffY < 0) {
                cappy_register_floor_object(cappy, m, floor->object);
                if (floor->normal.y < 0.707f) { // ~45 deg
                    cappy->oPosX = x;
                    cappy->oPosY = y;
                    cappy->oPosZ = z;
                    break;
                } else {
                    cappy->oPosY = floorY;
                }
            }
        } else {
            cappy->oPosX = x;
            cappy->oPosY = y;
            cappy->oPosZ = z;
            break;
        }

        // Ceiling
        struct Surface *ceil = NULL;
        f32 ceilY = find_ceil(cappy->oPosX, cappy->oPosY, cappy->oPosZ, &ceil);
        if (ceil != NULL) {
            f32 h = (cappy->hitboxHeight - cappy->hitboxDownOffset);
            if (cappy->oPosY <= ceilY && cappy->oPosY + h > ceilY) {
                cappy_register_ceiling_object(cappy, m, ceil->object);
                if (ceil->normal.y > -0.707f) { // ~45 deg
                    cappy->oPosX = x;
                    cappy->oPosY = y;
                    cappy->oPosZ = z;
                    break;
                } else {
                    cappy->oPosY = MAX(ceilY - h, floorY);
                }
            }
        }
    }
}
#undef STEPS

//
// Update
//

void cappy_bhv2_update(struct Object *cappy, struct MarioState *m) {
    m->oCappyInteractMario = FALSE;
    cappy_bhv2_move(cappy, m);
}

