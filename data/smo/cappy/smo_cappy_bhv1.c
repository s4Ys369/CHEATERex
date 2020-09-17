<<<<<<< HEAD
#include "smo_cappy_common.inl"
=======
#include "../smo_c_includes.h"

#define o gCurrentObject
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

//
// Init
//

static const f32 sCappyMovStraightLineParams[][5] = {
    { CAPPY_DELTA_VEL,             0.f, 0, 1, 50 },
    { CAPPY_DELTA_VEL,             0.f, 0, 1, 50 },
    { CAPPY_DELTA_VEL, CAPPY_DELTA_VEL, 1, 1,  0 },
    { CAPPY_DELTA_VEL,             0.f, 0, 1, 50 },
};

<<<<<<< HEAD
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
=======
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
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
}

//
// Movement
//

<<<<<<< HEAD
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
=======
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
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

    for (s32 i = 0; i != STEPS; ++i) {
        f32 x = cappy->oPosX;
        f32 y = cappy->oPosY;
        f32 z = cappy->oPosZ;
<<<<<<< HEAD
        cappy->oPosX += (cappy->oVelX / STEPS);
        cappy->oPosY += (cappy->oVelY / STEPS);
        cappy->oPosZ += (cappy->oVelZ / STEPS);
=======
        cappy->oPosX += (cappy->oCappyVelX / STEPS);
        cappy->oPosY += (cappy->oCappyVelY / STEPS);
        cappy->oPosZ += (cappy->oCappyVelZ / STEPS);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

        // Walls
        struct WallCollisionData hitbox;
        hitbox.x = cappy->oPosX;
        hitbox.y = cappy->oPosY;
        hitbox.z = cappy->oPosZ;
<<<<<<< HEAD
        hitbox.offsetY = cappy->hitboxHeight / 2;
        hitbox.radius = cappy->oWallHitboxRadius;
        if (find_wall_collisions(&hitbox) != 0) {
            cappy_register_wall_object(cappy, m, hitbox.walls[0]->object);
            cappy_bhv1_stop(cappy, hitbox.x, hitbox.y, hitbox.z);
=======
        hitbox.offsetY = CAPPY_HITBOX_HEIGHT / 2;
        hitbox.radius = CAPPY_WALL_RADIUS;
        if (find_wall_collisions(&hitbox) != 0) {
            cappy_register_wall_object(cappy, hitbox.walls[0]->object);
            cappy_stop_movement(cappy, hitbox.x, hitbox.y, hitbox.z);
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
            break;
        }

        // Floor
        struct Surface *floor = NULL;
        f32 floorY = find_floor(cappy->oPosX, cappy->oPosY, cappy->oPosZ, &floor);
        if (floor != NULL) {
            f32 diffY = cappy->oPosY - floorY;
            if (diffY < 0) {
<<<<<<< HEAD
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
=======
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
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
            break;
        }

        // Ceiling
        struct Surface *ceil = NULL;
        f32 ceilY = find_ceil(cappy->oPosX, cappy->oPosY, cappy->oPosZ, &ceil);
        if (ceil != NULL) {
<<<<<<< HEAD
            f32 h = (cappy->hitboxHeight - cappy->hitboxDownOffset);
            if (cappy->oPosY <= ceilY && cappy->oPosY + h > ceilY) {
                cappy_register_ceiling_object(cappy, m, ceil->object);
                if (ceil->normal.y > -0.707f) { // ~45 deg
                    cappy_bhv1_stop(cappy, x, y, z);
                    break;
                } else {
                    cappy->oPosY = MAX(ceilY - h, floorY);
                    cappy->oVelY = 0;
=======
            f32 h = (CAPPY_HITBOX_HEIGHT - CAPPY_HITBOX_DOWNOFFSET);
            if (cappy->oPosY <= ceilY && cappy->oPosY + h > ceilY) {
                cappy_register_ceiling_object(cappy, ceil->object);
                if (ceil->normal.y > -0.707f) { // ~45 deg
                    cappy_stop_movement(cappy, x, y, z);
                    break;
                } else {
                    cappy->oPosY = MAX(ceilY - h, floorY);
                    cappy->oCappyVelY = 0;
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
                }
            }
        }
    }
}
#undef STEPS

//
<<<<<<< HEAD
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
=======
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
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
