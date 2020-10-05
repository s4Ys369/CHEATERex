#include "../smo_c_includes.h"

//
// Possessed Bob-omb King
//

static void cappy_bobomb_king_update(struct PObject *pobj) {
    // Delay
    if (pobj->oCappyBobombKingTimer > 0) {
        pobj->oCappyBobombKingTimer--;
        return;
    }

    switch (pobj->oCappyBobombKingState) {
        case 0: // Start dialog
            if (mario_lock(pobj->oMario) && dialog_start(DIALOG_SMO_KING_BOBOMB)) {
                play_sound(SOUND_OBJ_KING_BOBOMB_TALK, pobj->header.gfx.cameraToObject);
                pobj->oCappyBobombKingState = 1;
            }
            break;

        case 1: // Wait for the dialog to end
            if (dialog_update()) {
                pobj->oCappyBobombKingState = 2;
                pobj->oCappyBobombKingTimer = 5;
            }
            break;

        case 2: // Release Mario
            if (mario_unlock(pobj->oMario)) {
                pobj->oCappyBobombKingState = 3;
                play_sound(SOUND_MARIO_OOOF, pobj->oMarioObj->header.gfx.cameraToObject);
                mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_GRABBED, FALSE, 15);
            }
            break;
    }
}

static void cappy_bobomb_king_update_animation_and_sound(struct PObject *pobj) {
    obj_set_animation_with_accel(pobj, 5, 1.f);
}

static void cappy_bobomb_king_set_cappy_values(struct PObject *pobj) {
    pobj->oCappyVerticalDist = 260.f;
    pobj->oCappyInitialScale = 250;
}

//
// Main loop
//

const PObjMainLoopFunc cappy_bobomb_king_mloop[] = {
    cappy_bobomb_king_update,
    obj_update_gfx,
    cappy_bobomb_king_update_animation_and_sound,
    cappy_bobomb_king_set_cappy_values,
    NULL
};

//
// Init
//

s32 cappy_bobomb_king_init(struct Object *obj) {
    if (obj->oAction != 1 && obj->oAction != 2) {
        return FALSE;
    }

    obj->oCappyBobombKingState = 0;
    obj->oCappyBobombKingTimer = 10;
    return TRUE;
}

void cappy_bobomb_king_end(struct Object *obj) {
    if (obj->oCappyBobombKingState == 3) {
        obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_GRABBED_MARIO;
        obj->oKingBobombUnk88 = 1;
        obj->oIntangibleTimer = -1;
        obj->oAction = 3;
        obj->oSubAction = 0;
        obj->oMario->interactObj = obj;
        obj->oMario->usedObj = obj;
    }
}

f32 cappy_bobomb_king_get_top(struct Object *obj) {
    return 260.f * obj->oScaleY;
}