#ifndef _SM64_TYPES_H_
#define _SM64_TYPES_H_

// This file contains various data types used in Super Mario 64 that don't yet
// have an appropriate header.

#include <ultra64.h>
#include "macros.h"


// Certain functions are marked as having return values, but do not
// actually return a value. This causes undefined behavior, which we'd rather
// avoid on modern GCC. This only impacts -O2 and can matter for both the function
// itself and functions that call it.
#ifdef AVOID_UB
    #define BAD_RETURN(cmd) void
#else
    #define BAD_RETURN(cmd) cmd
#endif

#define ANGLE_QUEUE_SIZE 9

struct Controller
{
  /*0x00*/ s32 angleDeltaQueue[ANGLE_QUEUE_SIZE];
  /*0x24*/ s16 rawStickX;       //
  /*0x26*/ s16 rawStickY;       //
  /*0x28*/ float stickX;        // [-64, 64] positive is right
  /*0x2C*/ float stickY;        // [-64, 64] positive is up
  /*0x30*/ float stickMag;      // distance from center [0, 64]
  /*0x34*/ u16 buttonDown;
  /*0x36*/ u16 buttonPressed;
  /*0x38*/ OSContStatus *statusData;
  /*0x3C*/ OSContPad *controllerData;
  /*0x40*/ s16 stickLastAngle;
  /*0x42*/ int port;
  /*ext */ s16 extStickX;       // additional (right) stick values
  /*ext */ s16 extStickY;
};

typedef f32 Vec2f[2];
typedef f32 Vec3f[3]; // X, Y, Z, where Y is up
typedef s16 Vec3s[3];
typedef s32 Vec3i[3];
typedef f32 Vec4f[4];
typedef s16 Vec4s[4];

typedef f32 Mat4[4][4];

typedef uintptr_t GeoLayout;
typedef uintptr_t LevelScript;
typedef s16 Movtex;
typedef s16 MacroObject;
typedef s16 Collision;
typedef s16 Trajectory;
typedef s16 PaintingData;
typedef uintptr_t BehaviorScript;

enum SpTaskState {
    SPTASK_STATE_NOT_STARTED,
    SPTASK_STATE_RUNNING,
    SPTASK_STATE_INTERRUPTED,
    SPTASK_STATE_FINISHED,
    SPTASK_STATE_FINISHED_DP
};

struct SPTask
{
    /*0x00*/ OSTask task;
    /*0x40*/ OSMesgQueue *msgqueue;
    /*0x44*/ OSMesg msg;
    /*0x48*/ enum SpTaskState state;
}; // size = 0x4C, align = 0x8

struct VblankHandler
{
    OSMesgQueue *queue;
    OSMesg msg;
};

#define ANIM_FLAG_NOLOOP     (1 << 0) // 0x01
#define ANIM_FLAG_FORWARD    (1 << 1) // 0x02
#define ANIM_FLAG_2          (1 << 2) // 0x04
#define ANIM_FLAG_HOR_TRANS  (1 << 3) // 0x08
#define ANIM_FLAG_VERT_TRANS (1 << 4) // 0x10
#define ANIM_FLAG_5          (1 << 5) // 0x20
#define ANIM_FLAG_6          (1 << 6) // 0x40
#define ANIM_FLAG_7          (1 << 7) // 0x80

struct Animation {
    /*0x00*/ s16 flags;
    /*0x02*/ s16 unk02;
    /*0x04*/ s16 unk04;
    /*0x06*/ s16 unk06;
    /*0x08*/ s16 unk08;
    /*0x0A*/ s16 unk0A;
    /*0x0C*/ const s16 *values;
    /*0x10*/ const u16 *index;
    /*0x14*/ u32 length; // only used with Mario animations to determine how much to load. 0 otherwise.
};

#define ANIMINDEX_NUMPARTS(animindex) (sizeof(animindex) / sizeof(u16) / 6 - 1)

struct GraphNode
{
    /*0x00*/ s16 type; // structure type
    /*0x02*/ s16 flags; // hi = drawing layer, lo = rendering modes
    /*0x04*/ struct GraphNode *prev;
    /*0x08*/ struct GraphNode *next;
    /*0x0C*/ struct GraphNode *parent;
    /*0x10*/ struct GraphNode *children;
};

// struct AnimInfo?
struct GraphNodeObject_sub
{
    /*0x00 0x38*/ s16 animID;
    /*0x02 0x3A*/ s16 animYTrans;
    /*0x04 0x3C*/ struct Animation *curAnim;
    /*0x08 0x40*/ s16 animFrame;
    /*0x0A 0x42*/ u16 animTimer;
    /*0x0C 0x44*/ s32 animFrameAccelAssist;
    /*0x10 0x48*/ s32 animAccel;
    s16 prevAnimFrame;
    s16 prevAnimID;
    u32 prevAnimFrameTimestamp;
    struct Animation *prevAnimPtr;
};

struct GraphNodeObject
{
    /*0x00*/ struct GraphNode node;
    /*0x14*/ struct GraphNode *sharedChild;
    /*0x18*/ s8 unk18;
    /*0x19*/ s8 unk19;
    /*0x1A*/ Vec3s angle;
    /*0x20*/ Vec3f pos;
    Vec3s prevAngle;
    Vec3f prevPos;
    u32 prevTimestamp;
    Vec3f prevShadowPos;
    u32 prevShadowPosTimestamp;
    /*0x2C*/ Vec3f scale;
    Vec3f prevScale;
    u32 prevScaleTimestamp;
    /*0x38*/ struct GraphNodeObject_sub unk38;
    /*0x4C*/ struct SpawnInfo *unk4C;
    /*0x50*/ Mat4 *throwMatrix; // matrix ptr
    Mat4 prevThrowMatrix;
    u32 prevThrowMatrixTimestamp;
    Mat4 *throwMatrixInterpolated;
    /*0x54*/ Vec3f cameraToObject;
    u32 skipInterpolationTimestamp;
};

struct ObjectNode
{
    struct GraphNodeObject gfx;
    struct ObjectNode *next;
    struct ObjectNode *prev;
};

// NOTE: Since ObjectNode is the first member of Object, it is difficult to determine
// whether some of these pointers point to ObjectNode or Object.

struct SmoData;

struct Object
{
    /*0x000*/ struct ObjectNode header;
    /*0x068*/ struct Object *parentObj;
    /*0x06C*/ struct Object *prevObj;
    /*0x070*/ u32 collidedObjInteractTypes;
    /*0x074*/ s16 activeFlags;
    /*0x076*/ s16 numCollidedObjs;
    /*0x078*/ struct Object *collidedObjs[4];
    /*0x088*/
    union
    {
        // Object fields. See object_fields.h.
        u32 asU32[0x50];
        s32 asS32[0x50];
        s16 asS16[0x50][2];
        f32 asF32[0x50];
#if !IS_64_BIT
        s16 *asS16P[0x50];
        s32 *asS32P[0x50];
        struct Animation **asAnims[0x50];
        struct Waypoint *asWaypoint[0x50];
        struct ChainSegment *asChainSegment[0x50];
        struct Object *asObject[0x50];
        struct Surface *asSurface[0x50];
        void *asVoidPtr[0x50];
        const void *asConstVoidPtr[0x50];
#endif
    } rawData;
#if IS_64_BIT
    union {
        s16 *asS16P[0x50];
        s32 *asS32P[0x50];
        struct Animation **asAnims[0x50];
        struct Waypoint *asWaypoint[0x50];
        struct ChainSegment *asChainSegment[0x50];
        struct Object *asObject[0x50];
        struct Surface *asSurface[0x50];
        void *asVoidPtr[0x50];
        const void *asConstVoidPtr[0x50];
    } ptrData;
#endif
    /*0x1C8*/ u32 unused1;
    /*0x1CC*/ const BehaviorScript *curBhvCommand;
    /*0x1D0*/ u32 bhvStackIndex;
    /*0x1D4*/ uintptr_t bhvStack[8];
    /*0x1F4*/ s16 bhvDelayTimer;
    /*0x1F6*/ s16 respawnInfoType;
    /*0x1F8*/ f32 hitboxRadius;
    /*0x1FC*/ f32 hitboxHeight;
    /*0x200*/ f32 hurtboxRadius;
    /*0x204*/ f32 hurtboxHeight;
    /*0x208*/ f32 hitboxDownOffset;
    /*0x20C*/ const BehaviorScript *behavior;
    /*0x210*/ u32 unused2;
    /*0x214*/ struct Object *platform;
    /*0x218*/ void *collisionData;
    /*0x21C*/ Mat4 transform;
    /*0x25C*/ void *respawnInfo;

    // SMO related data
    struct SmoData *smoData;
};

struct ObjectHitbox
{
    /*0x00*/ u32 interactType;
    /*0x04*/ u8 downOffset;
    /*0x05*/ s8 damageOrCoinValue;
    /*0x06*/ s8 health;
    /*0x07*/ s8 numLootCoins;
    /*0x08*/ s16 radius;
    /*0x0A*/ s16 height;
    /*0x0C*/ s16 hurtboxRadius;
    /*0x0E*/ s16 hurtboxHeight;
};

struct Waypoint
{
    s16 flags;
    Vec3s pos;
};

struct Surface
{
    /*0x00*/ s16 type;
    /*0x02*/ s16 force;
    /*0x04*/ s8 flags;
    /*0x05*/ s8 room;
    /*0x06*/ s16 lowerY;
    /*0x08*/ s16 upperY;
    /*0x0A*/ Vec3s vertex1;
    /*0x10*/ Vec3s vertex2;
    /*0x16*/ Vec3s vertex3;
    /*0x1C*/ struct {
        f32 x;
        f32 y;
        f32 z;
    } normal;
    /*0x28*/ f32 originOffset;
    /*0x2C*/ struct Object *object;
    Vec3s prevVertex1;
    Vec3s prevVertex2;
    Vec3s prevVertex3;
    u32 modifiedTimestamp;
};

struct MarioBodyState
{
    /*0x00*/ u32 action;
    /*0x04*/ s8 capState; /// see MarioCapGSCId
    /*0x05*/ s8 eyeState;
    /*0x06*/ s8 handState;
    /*0x07*/ s8 wingFlutter; /// whether Mario's wing cap wings are fluttering
    /*0x08*/ s16 modelState;
    /*0x0A*/ s8 grabPos;
    /*0x0B*/ u8 punchState; /// 2 bits for type of punch, 6 bits for punch animation timer
    /*0x0C*/ Vec3s torsoAngle;
    /*0x12*/ Vec3s headAngle;
    /*0x18*/ Vec3f heldObjLastPosition; /// also known as HOLP
    u8 padding[4];
};

struct OffsetSizePair
{
    u32 offset;
    u32 size;
};

struct MarioAnimDmaRelatedThing
{
    u32 count;
    u8 *srcAddr;
    struct OffsetSizePair anim[1]; // dynamic size
};

struct MarioAnimation
{
    struct MarioAnimDmaRelatedThing *animDmaTable;
    u8 *currentAnimAddr;
    struct Animation *targetAnim;
    u8 padding[4];
};

struct MarioState
{
    u16 unk00;
    u32 input;
    u32 flags;
    u32 particleFlags;
    u32 action;
    u32 prevAction;
    u32 terrainSoundAddend;
    u16 actionState;
    u16 actionTimer;
    u32 actionArg;
    f32 intendedMag;
    s16 intendedYaw;
    s16 invincTimer;
    u8 framesSinceA;
    u8 framesSinceB;
    u8 wallKickTimer;
    u8 doubleJumpTimer;
    Vec3s faceAngle;
    Vec3s angleVel;
    s16 slideYaw;
    s16 twirlYaw;
    Vec3f pos;
    Vec3f vel;
    f32 forwardVel;
    f32 slideVelX;
    f32 slideVelZ;
    struct Surface *wall;
    struct Surface *ceil;
    struct Surface *floor;
    f32 ceilHeight;
    f32 floorHeight;
    s16 floorAngle;
    s16 waterLevel;
    struct Object *interactObj;
    struct Object *heldObj;
    struct Object *usedObj;
    struct Object *riddenObj;
    struct Object *marioObj;
    struct SpawnInfo *spawnInfo;
    struct Area *area;
    struct PlayerCameraState *statusForCamera;
    struct MarioBodyState *marioBodyState;
    struct Controller *controller;
    struct MarioAnimation *animation;
    u32 collidedObjInteractTypes;
    s16 numCoins;
    s16 numStars;
    s8 numKeys; // Unused key mechanic
    s8 numLives;
    s16 health;
    s16 unkB0;
    u8 hurtCounter;
    u8 healCounter;
    u8 squishTimer;
    u8 fadeWarpOpacity;
    u16 capTimer;
    s16 prevNumStarsForDialog;
    f32 peakHeight;
    f32 quicksandDepth;
    f32 unkC4;
    f32 spareFloat;
    s32 spareInt;
    s16 spinDirection;
    u16 spinBufferTimer;
};

#endif // _SM64_TYPES_H_
