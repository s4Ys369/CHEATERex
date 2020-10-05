#include "../smo_c_includes.h"

//
// Possessed Whomp King
//

static void cappy_whomp_king_update(struct PObject *pobj) {
    // Standing up
    if (pobj->oFaceAnglePitch > 0) {
        pobj->oFaceAnglePitch = MAX(0, pobj->oFaceAnglePitch - 0x200);
        return;
    }

    // Delay
    if (pobj->oCappyWhompKingTimer > 0) {
        pobj->oCappyWhompKingTimer--;
        return;
    }

    switch (pobj->oCappyWhompKingState) {
        case 0: // Start dialog
            if (mario_lock(pobj->oMario) && dialog_start(DIALOG_SMO_KING_WHOMP)) {
                play_sound(SOUND_OBJ2_BOSS_DIALOG_GRUNT, pobj->header.gfx.cameraToObject);
                pobj->oCappyWhompKingState = 1;
            }
            break;

        case 1: // Wait for the dialog to end
            if (dialog_update()) {
                pobj->oCappyWhompKingState = 2;
                pobj->oCappyWhompKingTimer = 5;
            }
            break;

        case 2: // Release Mario
            if (mario_unlock(pobj->oMario)) {
                pobj->oCappyWhompKingState = 3;
                pobj->oMario->faceAngle[1] = pobj->oFaceAngleYaw;
                pobj->oMarioObj->oFaceAngleYaw = pobj->oFaceAngleYaw;
                mario_unpossess_object(pobj->oMario, MARIO_UNPOSSESS_ACT_KNOCKED_OUT_OF_KING_WHOMP, FALSE, 15);
            }
            break;
    }
}

static void cappy_whomp_king_update_animation_and_sound(struct PObject *pobj) {
    obj_set_animation_with_accel(pobj, 0, 1.f);
}

static void cappy_whomp_king_set_cappy_values(struct PObject *pobj) {
    pobj->oCappyForwardDist  = -45.f;
    pobj->oCappyVerticalDist = 430.f;
    pobj->oCappyInitialScale = 150;
}

//
// Main loop
//

const PObjMainLoopFunc cappy_whomp_king_mloop[] = {
    cappy_whomp_king_update,
    obj_update_gfx,
    cappy_whomp_king_update_animation_and_sound,
    cappy_whomp_king_set_cappy_values,
    NULL
};

//
// Init
//

s32 cappy_whomp_king_init(struct Object *obj) {
    if (!(obj->oAction == 1 || obj->oAction == 2 || obj->oAction == 3 || obj->oAction == 6 || obj->oAction == 7)) {
        return FALSE;
    }

    obj->oCappyWhompKingState = 0;
    obj->oCappyWhompKingTimer = 10;
    return TRUE;
}

void cappy_whomp_king_end(struct Object *obj) {
    obj->oAction = 3;
    obj->oSubAction = 0;
    obj->oTimer = 0;
}

f32 cappy_whomp_king_get_top(struct Object *obj) {
    return 430.f * obj->oScaleY;
}
