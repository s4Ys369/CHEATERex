#include "../smo_c_includes.h"

//
// Possessed Bob-omb buddy
//

static void cappy_bobomb_buddy_advice_update(struct PObject *pobj) {
    // Delay
    if (pobj->oCappyBobombBuddyTimer > 0) {
        pobj->oCappyBobombBuddyTimer--;
        return;
    }

    switch (pobj->oCappyBobombBuddyState) {
        case 0: // Start dialog
            if (mario_lock(pobj->oMario) && dialog_start(pobj->oBehParams2ndByte)) {
                pobj->oCappyBobombBuddyState = 1;
            }
            break;

        case 1: // Wait for the dialog to end
            if (dialog_update()) {
                pobj->oCappyBobombBuddyState = 2;
                pobj->oCappyBobombBuddyTimer = 5;
            }
            break;

        case 2: // Release Mario
            if (mario_unlock(pobj->oMario)) {
                mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_JUMP_OUT, FALSE, 6);
                pobj->oCappyBobombBuddyState = 3;
            }
            break;
    }
}

static void cappy_bobomb_buddy_cannon_update(struct PObject *pobj) {
    // Delay
    if (pobj->oCappyBobombBuddyTimer > 0) {
        pobj->oCappyBobombBuddyTimer--;
        return;
    }

    // Dialog ids
    s16 dialog1Id;
    s16 dialog2Id;
    if (gCurrCourseNum == COURSE_BOB) {
        dialog1Id = DIALOG_004;
        dialog2Id = DIALOG_105;
    } else {
        dialog1Id = DIALOG_047;
        dialog2Id = DIALOG_106;
    }

    switch (pobj->oCappyBobombBuddyState) {
        case 0: // Check if cannon is opened
            if (mario_lock(pobj->oMario)) {
                if (cur_obj_nearest_object_with_behavior(bhvCannonClosed) != NULL) {
                    pobj->oCappyBobombBuddyState = 1;
                } else {
                    pobj->oCappyBobombBuddyState = 5;
                }
            }
            break;

        case 1: // Start dialog 1
            if (dialog_start(dialog1Id)) {
                pobj->oCappyBobombBuddyState = 2;
            }
            break;

        case 2: // Wait for the dialog 1 to end
            if (dialog_update()) {
                pobj->oCappyBobombBuddyState = 3;
                pobj->oCappyBobombBuddyTimer = 5;
            }
            break;

        case 3: // Start cannon opening cutscene
            if (cutscene_start(CUTSCENE_PREPARE_CANNON, cur_obj_nearest_object_with_behavior(bhvCannonClosed))) {
                save_file_set_cannon_unlocked();
                pobj->oCappyBobombBuddyState = 4;
            }
            break;

        case 4: // Wait for the cannon cutscene to end
            if (cutscene_update()) {
                pobj->oCappyBobombBuddyState = 5;
                pobj->oCappyBobombBuddyTimer = 10;
            }
            break;

        case 5: // Start dialog 2
            if (dialog_start(dialog2Id)) {
                pobj->oCappyBobombBuddyState = 6;
            }
            break;

        case 6: // Wait for the dialog 2 to end
            if (dialog_update()) {
                pobj->oCappyBobombBuddyState = 7;
                pobj->oCappyBobombBuddyTimer = 5;
            }
            break;

        case 7: // Release Mario
            if (mario_unlock(pobj->oMario)) {
                mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_JUMP_OUT, FALSE, 6);
                pobj->oCappyBobombBuddyState = 8;
            }
            break;
    }
}

static void cappy_bobomb_buddy_update(struct PObject *pobj) {
    switch (pobj->oBobombBuddyRole) {
        case BOBOMB_BUDDY_ROLE_ADVICE:
            cappy_bobomb_buddy_advice_update(pobj);
            break;
        
        case BOBOMB_BUDDY_ROLE_CANNON:
            cappy_bobomb_buddy_cannon_update(pobj);
            break;
    }
}

static void cappy_bobomb_buddy_update_animation_and_sound(struct PObject *pobj) {
    obj_random_blink(pobj, &pobj->oBobombBuddyBlinkTimer);
}

static void cappy_bobomb_buddy_set_cappy_values(struct PObject *pobj) {
    pobj->oCappyVerticalDist = 94.f;
    pobj->oCappyInitialScale = 120;
}

//
// Main loop
//

const PObjMainLoopFunc cappy_bobomb_buddy_mloop[] = {
    cappy_bobomb_buddy_update,
    obj_update_gfx,
    cappy_bobomb_buddy_update_animation_and_sound,
    cappy_bobomb_buddy_set_cappy_values,
    NULL
};

//
// Init
//

s32 cappy_bobomb_buddy_init(struct Object *obj) {
    obj->oCappyBobombBuddyState = 0;
    obj->oCappyBobombBuddyTimer = 15;
    return TRUE;
}

void cappy_bobomb_buddy_end(UNUSED struct Object *obj) {
}

f32 cappy_bobomb_buddy_get_top(struct Object *obj) {
    return 94.f * obj->oScaleY;
}