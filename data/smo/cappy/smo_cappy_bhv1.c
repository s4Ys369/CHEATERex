#include "smo_cappy_common.inl"

//
// Init
//

static const f32 sCappyMovStraightLineParams[][5] = {
    { CAPPY_DELTA_VEL,             0.f, 0, 1, 50 },
    { CAPPY_DELTA_VEL,             0.f, 0, 1, 50 },
    { CAPPY_DELTA_VEL, CAPPY_DELTA_VEL, 1, 1,  0 },
    { CAPPY_DELTA_VEL,             0.f, 0, 1, 50 },
};

void cappy_bhv1_init(struct Object *cappy, struct MarioState *m) {
    f32 hVel     = sCappyMovStraightLineParams[m->oCappyActionFlag][0];
    f32 vVel     = sCappyMovStraightLineParams[m->oCappyActionFlag][1];
    s16 mPitch   = sCappyMovStraightLineParams[m->oCappyActionFlag][2] * m->faceAngle[0];
    s16 mYaw     = sCappyMovStraightLineParams[m->oCappyActionFlag][3] * m->faceAngle[1];
    f32 yOffset  = sCappyMovStraightLineParams[m->oCappyActionFlag][4];

    cappy->oPosX = m->pos[0];
    cappy->oPosY = m->pos[1] + yOffset;
    cappy->oPosZ = m->pos[2];
    cappy->oVelX = coss(mPitch) * sins(mYaw) * hVel;
    cappy->oVelY = sins(mPitch) * vVel;
    cappy->oVelZ = coss(mPitch) * coss(mYaw) * hVel;
}

//
// Movement
//

static void cappy_bhv1_stop(struct Object *cappy, f32 x, f32 y, f32 z) {
    cappy->oPosX = x;
    cappy->oPosY = y;
    cappy->oPosZ = z;
    cappy->oVelX = 0;
    cappy->oVelY = 0;
    cappy->oVelZ = 0;
}

#define STEPS 8
static void cappy_bhv1_move(struct Object *cappy, struct MarioState *m) {
    m->oCappyWallObject  = NULL;
    m->oCappyFloorObject = NULL;
    m->oCappyCeilObject  = NULL;

    for (s32 i = 0; i != STEPS; ++i) {
        f32 x = cappy->oPosX;
        f32 y = cappy->oPosY;
        f32 z = cappy->oPosZ;
        cappy->oPosX += (cappy->oVelX / STEPS);
        cappy->oPosY += (cappy->oVelY / STEPS);
        cappy->oPosZ += (cappy->oVelZ / STEPS);

        // Walls
        struct WallCollisionData hitbox;
        hitbox.x = cappy->oPosX;
        hitbox.y = cappy->oPosY;
        hitbox.z = cappy->oPosZ;
        hitbox.offsetY = cappy->hitboxHeight / 2;
        hitbox.radius = cappy->oWallHitboxRadius;
        if (find_wall_collisions(&hitbox) != 0) {
            cappy_register_wall_object(cappy, m, hitbox.walls[0]->object);
            cappy_bhv1_stop(cappy, hitbox.x, hitbox.y, hitbox.z);
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
                    cappy_bhv1_stop(cappy, x, y, z);
                    break;
                } else {
                    cappy->oPosY = floorY;
                    cappy->oVelY = 0;
                }
            }
        } else {
            cappy_bhv1_stop(cappy, x, y, z);
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
                    cappy_bhv1_stop(cappy, x, y, z);
                    break;
                } else {
                    cappy->oPosY = MAX(ceilY - h, floorY);
                    cappy->oVelY = 0;
                }
            }
        }
    }
}
#undef STEPS

//
// Update
//

void cappy_bhv1_update(struct Object *cappy, struct MarioState *m) {
    if (cappy->oTimer < 12) {
        m->oCappyInteractMario = FALSE;
        cappy_bhv1_move(cappy, m);
    } else {
        m->oCappyInteractMario = TRUE;
    }
}
