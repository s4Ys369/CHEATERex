#ifndef CAPTURE_ALL_H
#define CAPTURE_ALL_H

#include "types.h"

/* Possessed Object type */
#define PObject Object

// Init function
// Called before trying to possess the object.
// Returns TRUE if the object can be possessed.
typedef s32 (*PObjInitFunc)(struct Object *);

// Main loop entry point
// A list of this type of function is used as the behavior
// of the possessed object. Always ends with NULL.
typedef void (*PObjMainLoopFunc)(struct PObject *);

// End function
// Called after "resetting" the object, i.e. after restoring his
// object fields to their values before the possession.
// SMO fields are not deallocated yet, and can be still accessed.
typedef void (*PObjEndFunc)(struct Object *);

// Get top of object function
// Used as a y offset for both animations of Mario
// entering the object and leaving the object
typedef f32 (*PObjGetTopFunc)(struct Object *);

/* Possessed object constants */
#define POBJ_FLOOR_TYPE_NONE	0
#define POBJ_FLOOR_TYPE_GROUND	1
#define POBJ_FLOOR_TYPE_WATER	2
#define POBJ_FLOOR_TYPE_LAVA	3

/* Possessed object stuff */
void pobj_decelerate(struct PObject *pobj);
void pobj_apply_gravity(struct PObject *pobj);
void pobj_handle_special_floors(struct PObject *pobj);
s32  pobj_process_interaction(struct PObject *pobj, struct Object *obj, u32 interactType);

/* Dialog */
s32 dialog_start(s16 dialogId);
s32 dialog_update(); // Returns TRUE if the dialog is finished
s32 cutscene_start(u8 cutsceneId, struct Object *obj);
s32 cutscene_update(); // Returns TRUE if the cutscene is finished

/* Camera */
void smo_capture_set_camera_behind_mario(struct PObject *pobj);
void smo_capture_reset_camera(struct PObject *pobj);

/* Interaction macros */
// Only for use inside the cappy_***_process_interactions functions
// Make sure to replace these monstruosities by lambdas if it's ported to C++
#define POBJ_INTERACTIONS_BEGIN \
    for (u32 objType = OBJ_LIST_DESTRUCTIVE; objType <= OBJ_LIST_POLELIKE; ++objType) { \
        struct ObjectNode *list = &gObjectLists[objType]; \
        struct Object *obj = obj_get_first(list); \
        while (obj != NULL) { \
            if (obj != pobj) {

#define POBJ_INTERACTIONS_END \
                u32 interactType = obj->oInteractType; \
                if (!(obj->oInteractStatus & INT_STATUS_INTERACTED)) { \
                    pobj_process_interaction(pobj, obj, interactType); \
                } \
            } \
            obj = obj_get_next(list, obj); \
        } \
    }

/* Capture data */
struct SmoCaptureData {
    const char *id;
    const PObjInitFunc init;
    const PObjEndFunc end;
    const PObjGetTopFunc gettop;
    const PObjMainLoopFunc *mloop;
    f32 walkSpeed;
    f32 runSpeed;
    f32 dashSpeed;
    f32 jumpVelocity;
    f32 terminalVelocity;
    f32 gravity;
    f32 hitboxRadius;
    f32 hitboxHeight;
    f32 hitboxDownOffset;
    f32 wallHitboxRadius;
};

/* Cappy Capture */
const struct SmoCaptureData *cappy_capture(struct Object *obj);

#define CAPTURE_BEHAVIOR(name)							\
extern const PObjMainLoopFunc cappy_##name##_mloop[];	\
s32 cappy_##name##_init(struct Object *obj);			\
void cappy_##name##_end(struct Object *obj);			\
f32 cappy_##name##_get_top(struct Object *obj)

/* Bob-omb Battlefield */
CAPTURE_BEHAVIOR(goomba);				// bhvGoomba
CAPTURE_BEHAVIOR(koopa);				// bhvKoopa
CAPTURE_BEHAVIOR(bobomb);				// bhvBobomb
CAPTURE_BEHAVIOR(bobomb_buddy);			// bhvBobombBuddy, bhvBobombBuddyOpensCannon
CAPTURE_BEHAVIOR(bobomb_king);			// bhvKingBobomb
CAPTURE_BEHAVIOR(chain_chomp);			// bhvChainChomp, bhvChainChompFreed

/* Whomp Fortress */
CAPTURE_BEHAVIOR(bullet_bill);			// bhvBulletBill
CAPTURE_BEHAVIOR(hoot);					// bhvHoot
CAPTURE_BEHAVIOR(whomp);				// bhvSmallWhomp
CAPTURE_BEHAVIOR(whomp_king);			// bhvWhompKingBoss

/* Jolly Roger Bay */

/* Cool, Cool Mountain */
CAPTURE_BEHAVIOR(spindrift);			// bhvSpindrift
CAPTURE_BEHAVIOR(tuxies_mother);		// bhvTuxiesMother
CAPTURE_BEHAVIOR(penguin_small);		// bhvPenguinBaby, bhvSmallPenguin
CAPTURE_BEHAVIOR(snowmans_head);		// bhvSnowmansHead
CAPTURE_BEHAVIOR(snowmans_body);		// bhvSnowmansBottom
CAPTURE_BEHAVIOR(snowman_big);			// bhvBigSnowmanWhole

/* Big Boo's Haunt */
CAPTURE_BEHAVIOR(boo);					// bhvBoo, bhvBooInCastle, bhvBooWithCage, bhvGhostHuntBoo, bhvMerryGoRoundBoo
CAPTURE_BEHAVIOR(big_boo);				// bhvGhostHuntBigBoo, bhvBalconyBigBoo, bhvMerryGoRoundBigBoo
CAPTURE_BEHAVIOR(crazy_box);			// bhvJumpingBox
CAPTURE_BEHAVIOR(scuttlebug);			// bhvScuttlebug
CAPTURE_BEHAVIOR(mad_piano);			// bhvMadPiano

/* Hazy Maze Cave */
CAPTURE_BEHAVIOR(swoop);				// bhvSwoop
CAPTURE_BEHAVIOR(snufit);				// bhvSnufit
CAPTURE_BEHAVIOR(dorrie);				// bhvDorrie

/* Lethal Lava Land */
CAPTURE_BEHAVIOR(bully);				// bhvSmallBully
CAPTURE_BEHAVIOR(big_bully);			// bhvBigBully, bhvBigBullyWithMinions

/* Shifting Sand Land */
CAPTURE_BEHAVIOR(fly_guy);				// bhvFlyGuy
CAPTURE_BEHAVIOR(pokey);				// bhvPokey, bhvPokeyBodyPart
CAPTURE_BEHAVIOR(amp);					// bhvHomingAmp, bhvCirclingAmp
CAPTURE_BEHAVIOR(klepto);				// bhvKlepto
CAPTURE_BEHAVIOR(eyerock);				// bhvEyerokBoss, bhvEyerokHand

/* Dire, Dire Docks */
CAPTURE_BEHAVIOR(fire_spitter);			// bhvFireSpitter
CAPTURE_BEHAVIOR(sushi_shark);			// bhvSushiShark

/* Snowman's Land */
CAPTURE_BEHAVIOR(moneybag);				// bhvMoneybag, bhvMoneybagHidden
CAPTURE_BEHAVIOR(big_chill_bully);		// bhvBigChillBully
CAPTURE_BEHAVIOR(sl_walking_penguin);	// bhvSLWalkingPenguin

/* Wet-Dry World */
CAPTURE_BEHAVIOR(skeeter);              // bhvSkeeter
CAPTURE_BEHAVIOR(heave_ho);             // bhvHeaveHo
CAPTURE_BEHAVIOR(chuckya);              // bhvChuckya

/* Tall, Tall Mountain */
CAPTURE_BEHAVIOR(ukiki);                // bhvUkiki, bhvMacroUkiki
//extern const BehaviorScript bhvCloud[];
//extern const BehaviorScript bhvCloudPart[];

/* Tiny-Huge Island */
CAPTURE_BEHAVIOR(enemy_lakitu);         // bhvEnemyLakitu
CAPTURE_BEHAVIOR(spiny);                // bhvSpiny
CAPTURE_BEHAVIOR(wiggler);              // bhvWigglerHead, bhvWigglerBody

/* Tick Tock Clock */

/* Rainbow Ride */

/* Misc */
CAPTURE_BEHAVIOR(bowser);               // bhvBowser
CAPTURE_BEHAVIOR(toad);                 // bhvToadMessage
CAPTURE_BEHAVIOR(mips);                 // bhvMips
CAPTURE_BEHAVIOR(yoshi);                // bhvYoshi

/* New behaviors */
extern const BehaviorScript bhvChainChompFreed[];

#undef CAPTURE_BEHAVIOR
#endif // CAPTURE_ALL_H
