#include "../smo_c_includes.h"

#define o gCurrentObject

//
// Init
//

static const f32 sCappyMovStraightLineParams[][5] = {
    { CAPPY_DELTA_VEL,             0.f, 0, 1, 50 },
    { CAPPY_DELTA_VEL,             0.f, 0, 1, 50 },
    { CAPPY_DELTA_VEL, CAPPY_DELTA_VEL, 1, 1,  0 },
    { CAPPY_DELTA_VEL,             0.f, 0, 1, 50 },
};

static void bhv_cappy1_init() {
    f32 hVel    = sCappyMovStraightLineParams[o->oCappyActionFlag][0];
    f32 vVel    = sCappyMovStraightLineParams[o->oCappyActionFlag][1];
    s16 mPitch  = sCappyMovStraightLineParams[o->oCappyActionFlag][2] * o->oMario->faceAngle[0];
    s16 mYaw    = sCappyMovStraightLineParams[o->oCappyActionFlag][3] * o->oMario->faceAngle[1];
    f32 yOffset = sCappyMovStraightLineParams[o->oCappyActionFlag][4];

    o->oCappyVelX           = coss(mPitch) * sins(mYaw) * hVel;
    o->oCappyVelY           = sins(mPitch) * vVel;
    o->oCappyVelZ           = coss(mPitch) * coss(mYaw) * hVel;
    o->oCappyYaw            = mYaw;
    o->oCappyInvincible     = FALSE;
    o->oCappyInteractMario  = FALSE;
    o->oPosX                = o->oMario->pos[0];
    o->oPosY                = o->oMario->pos[1] + yOffset;
    o->oPosZ                = o->oMario->pos[2];
    o->oFaceAngleYaw        = 0;
    o->oFaceAnglePitch      = 0;
    o->oFaceAngleRoll       = 0;
    o->oCappyTimer          = 0;
}

//
// Movement
//

static void cappy_stop_movement(struct Object *cappy, f32 x, f32 y, f32 z) {
    cappy->oPosX = x;
    cappy->oPosY = y;
    cappy->oPosZ = z;
    cappy->oCappyVelX = 0;
    cappy->oCappyVelY = 0;
    cappy->oCappyVelZ = 0;
}

#define STEPS 8
static void cappy_move_straight_line(struct Object *cappy) {
    cappy->oCappyWallObj = NULL;
    cappy->oCappyFloorObj = NULL;
    cappy->oCappyCeilObj = NULL;

    for (s32 i = 0; i != STEPS; ++i) {
        f32 x = cappy->oPosX;
        f32 y = cappy->oPosY;
        f32 z = cappy->oPosZ;
        cappy->oPosX += (cappy->oCappyVelX / STEPS);
        cappy->oPosY += (cappy->oCappyVelY / STEPS);
        cappy->oPosZ += (cappy->oCappyVelZ / STEPS);

        // Walls
        struct WallCollisionData hitbox;
        hitbox.x = cappy->oPosX;
        hitbox.y = cappy->oPosY;
        hitbox.z = cappy->oPosZ;
        hitbox.offsetY = CAPPY_HITBOX_HEIGHT / 2;
        hitbox.radius = CAPPY_WALL_RADIUS;
        if (find_wall_collisions(&hitbox) != 0) {
            cappy_register_wall_object(cappy, hitbox.walls[0]->object);
            cappy_stop_movement(cappy, hitbox.x, hitbox.y, hitbox.z);
            break;
        }

        // Floor
        struct Surface *floor = NULL;
        f32 floorY = find_floor(cappy->oPosX, cappy->oPosY, cappy->oPosZ, &floor);
        if (floor != NULL) {
            f32 diffY = cappy->oPosY - floorY;
            if (diffY < 0) {
                cappy_register_floor_object(cappy, floor->object);
                if (floor->normal.y < 0.707f) { // ~45 deg
                    cappy_stop_movement(cappy, x, y, z);
                    break;
                } else {
                    cappy->oPosY = floorY;
                    cappy->oCappyVelY = 0;
                }
            }
        } else {
            cappy_stop_movement(cappy, x, y, z);
            break;
        }

        // Ceiling
        struct Surface *ceil = NULL;
        f32 ceilY = find_ceil(cappy->oPosX, cappy->oPosY, cappy->oPosZ, &ceil);
        if (ceil != NULL) {
            f32 h = (CAPPY_HITBOX_HEIGHT - CAPPY_HITBOX_DOWNOFFSET);
            if (cappy->oPosY <= ceilY && cappy->oPosY + h > ceilY) {
                cappy_register_ceiling_object(cappy, ceil->object);
                if (ceil->normal.y > -0.707f) { // ~45 deg
                    cappy_stop_movement(cappy, x, y, z);
                    break;
                } else {
                    cappy->oPosY = MAX(ceilY - h, floorY);
                    cappy->oCappyVelY = 0;
                }
            }
        }
    }
}
#undef STEPS

//
// Loop
//

static void bhv_cappy1_loop(void) {
    if (o->oCappyTimer < CAPPY_MAX_LIFETIME) {
        if (o->oCappyTimer < 12) {
            o->oCappyInteractMario = FALSE;
            cappy_move_straight_line(o);
        } else {
            o->oCappyInteractMario = TRUE;
        }
        cappy_process_object_collision(o, o->oMario);
    } else {
        if (cappy_return_to_mario(o, o->oMario)) {
            smo_obj_free_data(o);
            obj_mark_for_deletion(o);
            return;
        }
    }

    spawn_object(o, MODEL_NONE, bhvSparkleSpawn);
    o->oFaceAngleYaw += 0x2000;
    o->oOpacity = ((o->oMario->flags & MARIO_VANISH_CAP) ? 0x80 : 0xFF);
    o->oCappyTimer++;
}

#undef o

//
// Behavior
//

const BehaviorScript bhvCappy1[] = {
    0x00060000,
    0x11010041,
    0x0C000000, (uintptr_t)(bhv_cappy1_init),
    0x08000000,
    0x0C000000, (uintptr_t)(bhv_cappy1_loop),
    0x09000000
};