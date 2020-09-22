#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "actors/common0.h"
#include "audio/data.h"
#include "audio/external.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/level_script.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game_init.h"
#include "interaction.h"
#include "level_table.h"
#include "level_update.h"
#include "main.h"
#include "mario.h"
#include "mario_actions_airborne.h"
#include "mario_actions_automatic.h"
#include "mario_actions_cutscene.h"
#include "mario_actions_moving.h"
#include "mario_actions_object.h"
#include "mario_actions_stationary.h"
#include "mario_actions_submerged.h"
#include "mario_cheats.h"
#include "mario_misc.h"
#include "mario_step.h"
#include "memory.h"
#include "model_ids.h"
#include "object_fields.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "print.h"
#include "rendering_graph_node.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sound_init.h"
#include "debug.h"
#include "thread6.h"
#include "pc/configfile.h"
#include "pc/cheats.h"

#define SwiftSwim 42.0f

/*SwiftSwim Cheat*/
void cheats_swimming_speed(struct MarioState* m) {
    while (m->forwardVel < SwiftSwim && Cheats.EnableCheats == true && Cheats.Swim == true) {
        while (m->controller->buttonDown & A_BUTTON) {
            m->particleFlags |= PARTICLE_BUBBLE;
            m->forwardVel += 5.0f;
            break;
        }
        break;
    }
}

/*BLJAnywhere Cheat*/
void cheats_air_step(struct MarioState *m) {
    if (Cheats.BLJAnywhere > 0 && Cheats.EnableCheats == TRUE && m->action == ACT_LONG_JUMP
        && m->forwardVel < 1.0f && m->pos[1] - 50.0f < m->floorHeight) {
        if (Cheats.BLJAnywhere < 7) {
            if (m->controller->buttonPressed & A_BUTTON) {
                m->forwardVel -= (Cheats.BLJAnywhere - 1) * 2.5f;
                m->vel[1] = -50.0f;
            }
        } else if (m->controller->buttonDown & A_BUTTON) {
            m->forwardVel -= (Cheats.BLJAnywhere - 7) * 2.5f;
            m->vel[1] = -50.0f;
        }
    }
}

void cheats_long_jump(struct MarioState *m) {
    if (Cheats.BLJAnywhere >= 7 && Cheats.EnableCheats == true && m->forwardVel < 1.0f
        && (m->controller->buttonDown & A_BUTTON)) {
        set_jumping_action(m, ACT_LONG_JUMP, 0);
    }
}

/*Main cheat function*/
void cheats_mario_inputs(struct MarioState *m) {
    m->particleFlags = 0;
    m->collidedObjInteractTypes = m->marioObj->collidedObjInteractTypes;
    m->flags &= 0xFFFFFF;

    while (Cheats.EnableCheats == true) {
        /*Hover Cheat*/
        while (Cheats.Hover && m->controller->buttonDown & A_BUTTON) {
            if (m->action != ACT_GROUND_POUND) {
                vec3f_set(m->vel, 0.0f, 2.0f, 300.0f);
                break;
            }
            break;
        }

        /*Moon Gravity*/
        while (Cheats.Moon) {
            while ((m->action & ACT_GROUP_MASK) == ACT_GROUP_AIRBORNE) {
                if (m->action != ACT_FREEFALL) {
                    m->vel[1] += 2;
                    break;
                }
                break;
            }
            break;
        }
        /*Jump Modifier*/
        while (Cheats.Jump) {
            while ((m->action & ACT_GROUP_MASK) == ACT_GROUP_AIRBORNE) {
                if (m->action != ACT_FREEFALL) {
                    m->vel[1] += 1;
                    break;
                }
                if (m->action &= ACT_FREEFALL) {
                    m->vel[1] -= 5;
                    break;
                }
                break;
            }
            break;
        }

        /*Run Modifier Cheat*/
        switch (Cheats.Run) {
            case 0:
                break;
            case 1:
                if (m->action == ACT_WALKING) {
                    m->forwardVel = (m->forwardVel - 0.5f);
                }
                break;
            case 2:
                if (m->action == ACT_WALKING) {
                    m->forwardVel = (m->forwardVel - 0.7f);
                }
                break;
            case 3:
                if (m->action == ACT_WALKING) {
                    m->forwardVel = (m->forwardVel * 1.2f);
                }
                break;
            case 4:
                if (m->action == ACT_WALKING) {
                    m->forwardVel = (m->forwardVel * 1.8f);
                }
                break;
        }
        /*Play As Cheat*/
        switch(Cheats.PAC) {
            /*Model Choices*/
            case 0:
                m->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_MARIO]; //Use MODEL_PLAYER
                m->animation = &D_80339D10; //Only Mario's animations
                break;
            case 1:
                m->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_BLACK_BOBOMB];
                m->marioObj->header.gfx.unk38.curAnim = bobomb_seg8_anims_0802396C[0];
                is_anim_at_end(m);
                break;
            case 2:
                m->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_BOBOMB_BUDDY];
                m->marioObj->header.gfx.unk38.curAnim = bobomb_seg8_anims_0802396C[0];
                is_anim_at_end(m);
                break;
            case 3:
                m->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_GOOMBA];
                m->marioObj->header.gfx.unk38.curAnim = goomba_seg8_anims_0801DA4C[0];
                is_anim_at_end(m);
                break;
            case 4:
                m->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_AMP];
                m->marioObj->header.gfx.unk38.curAnim = amp_seg8_anims_08004034[0];
                is_anim_at_end(m);
                break;
            case 5:
                m->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_CHUCKYA];
                m->marioObj->header.gfx.unk38.curAnim = chuckya_seg8_anims_0800C070[0];
                is_anim_at_end(m);
                break; //Forgot this in v7
            case 6:
                m->marioObj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_FLYGUY];
                m->marioObj->header.gfx.unk38.curAnim = flyguy_seg8_anims_08011A64[0];
                is_anim_at_end(m);
                break;
            }
        while (Cheats.PAC > 0) {
                /*Instead of making a custom hitbox for each character,
                I neutralized the only consistent problem, doors*/
            while (m->collidedObjInteractTypes & INTERACT_DOOR) {
                obj_mark_for_deletion(m->usedObj);
                spawn_object(gCurrentObject, MODEL_SMOKE, bhvBobombBullyDeathSmoke);
                obj_scale(gCurrentObject, gCurrentObject->oTimer / 4.f + 1.0f);
                gCurrentObject->oOpacity -= 14;
                gCurrentObject->oAnimState++;
                play_sound(SOUND_GENERAL2_BOBOMB_EXPLOSION, m->marioObj->header.gfx.cameraToObject);
                m->particleFlags |= PARTICLE_TRIANGLE;
                obj_set_pos(m->marioObj, 0, 0, 100);
                break;
            }
            break;
        }


        /*Speed Display*/
        if (Cheats.SPD == true) {
            print_text_fmt_int(210, 72, "SPD %d", m->forwardVel);
        }

        /*T Pose Float? Actually it's just twirling + MoonJump*/
        while (Cheats.TPF == true) {
            if (m->controller->buttonDown & A_BUTTON) {
                m->vel[1] = 25;
                set_mario_action(m, ACT_TWIRLING, 0);
            }
            break;
        }

        /*QuickEnding cheat*/
        while (Cheats.QuikEnd == true) {
            if (m->numStars == 120) {
                level_trigger_warp(m, WARP_OP_CREDITS_START);
                Cheats.QuikEnd = false;
                save_file_do_save(gCurrSaveFileNum - 1);
            }
            break;
        }

        /*AutoWallKick cheat*/
        if (Cheats.AutoWK == true && m->action == ACT_AIR_HIT_WALL) {
            m->vel[1] = 52.0f;
            m->faceAngle[1] += 0x8000;
            set_mario_action(m, ACT_WALL_KICK_AIR, 0);
            m->wallKickTimer = 0;
            set_mario_animation(m, MARIO_ANIM_START_WALLKICK);
        }

        /*HurtMario cheat*/
        while (Cheats.Hurt > 0 && m->controller->buttonDown & L_TRIG
               && m->controller->buttonPressed & A_BUTTON) {
            if (Cheats.Hurt == 1) {
                act_burning_ground(m);
            }
            if (Cheats.Hurt == 2) {
                m->flags |= MARIO_METAL_SHOCK;
                drop_and_set_mario_action(m, ACT_SHOCKED, 0);
            }
            if (Cheats.Hurt == 3) {
                hurt_and_set_mario_action(m, ACT_GROUND_BONK, 0, 1);
                play_sound(SOUND_MARIO_OOOF, m->marioObj->header.gfx.cameraToObject);
                queue_rumble_data(5, 80);
            }
            break;
        }

        /*CannonAnywhere cheat*/
        if (Cheats.Cann == true && m->controller->buttonDown & L_TRIG
            && m->controller->buttonPressed & U_CBUTTONS) {
            spawn_object_relative(1, 0, 200, 0, gCurrentObject, MODEL_NONE, bhvCannon);
        }

        /*InstantDeath cheat*/
        if (m->controller->buttonDown & L_TRIG && m->controller->buttonDown & A_BUTTON
            && m->controller->buttonPressed & B_BUTTON && m->controller->buttonDown & R_TRIG) {
            level_trigger_warp(m, WARP_OP_DEATH);
            break;
        };

        /*CAP Cheats, this whole thing needs to be refactored, but
        I've only been adding to JAGSTAX's original patch*/
        if (Cheats.EnableCheats) {
            if (Cheats.WingCap) {
                m->flags |= MARIO_WING_CAP;
                if ((m->action & ACT_GROUP_MASK) == (!(ACT_GROUP_AIRBORNE) && !(ACT_GROUP_SUBMERGED))) {
                    set_mario_action(m, ACT_PUTTING_ON_CAP, 0);
                }
                play_cap_music(SEQ_EVENT_POWERUP);
                Cheats.WingCap = false;
            }

            if (Cheats.MetalCap) {
                m->flags |= MARIO_METAL_CAP;
                if ((m->action & ACT_GROUP_MASK) == (!(ACT_GROUP_AIRBORNE) && !(ACT_GROUP_SUBMERGED))) {
                    set_mario_action(m, ACT_PUTTING_ON_CAP, 0);
                }
                play_cap_music(SEQ_EVENT_METAL_CAP);
                Cheats.MetalCap = false;
            }

            if (Cheats.VanishCap) {
                m->flags |= MARIO_VANISH_CAP;
                if ((m->action & ACT_GROUP_MASK) == (!(ACT_GROUP_AIRBORNE) && !(ACT_GROUP_SUBMERGED))) {
                    set_mario_action(m, ACT_PUTTING_ON_CAP, 0);
                }
                play_cap_music(SEQ_EVENT_POWERUP);
                Cheats.VanishCap = false;
            }

            if (Cheats.RemoveCap) {
                m->flags &= ~MARIO_CAP_ON_HEAD;
                m->flags |= MARIO_CAP_IN_HAND;
                if ((m->action & ACT_GROUP_MASK) == (!(ACT_GROUP_AIRBORNE) && !(ACT_GROUP_SUBMERGED))) {
                    set_mario_action(m, ACT_SHIVERING, 0);
                }
                Cheats.RemoveCap = false;
            }

            if (Cheats.DCM == true) {
                stop_cap_music();
            }

            if (Cheats.NormalCap) {
                m->flags &= ~MARIO_CAP_ON_HEAD;
                m->flags &= ~(MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP);
                if ((m->action & ACT_GROUP_MASK) == (!(ACT_GROUP_AIRBORNE) && !(ACT_GROUP_SUBMERGED))) {
                    m->flags |= MARIO_CAP_IN_HAND;
                    set_mario_action(m, ACT_PUTTING_ON_CAP, 0);
                } else {
                    m->flags &= ~MARIO_CAP_IN_HAND;
                    m->flags |= MARIO_CAP_ON_HEAD;
                }
                stop_cap_music();
                Cheats.NormalCap = false;
            }
        }

        /* GetShell cheat */
        while (Cheats.GetShell == true && m->controller->buttonDown & L_TRIG
               && m->controller->buttonPressed & R_TRIG) {
            if (m->action & ACT_FLAG_RIDING_SHELL) {
                break;
            }

            /*This check should be added when creating a spawn cheat to prevent spamming*/
            struct Object *obj = (struct Object *) gObjectLists[OBJ_LIST_LEVEL].next;
            struct Object *first = (struct Object *) &gObjectLists[OBJ_LIST_LEVEL];
            while (obj != NULL && obj != first) {
                if (obj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_KOOPA_SHELL]) {
                    obj_mark_for_deletion(obj);
                    break;
                }
                obj = (struct Object *) obj->header.next;
            }

            if ((m->action & ACT_GROUP_MASK) == ACT_GROUP_SUBMERGED) {
                spawn_object_relative(0, 0, 100, 100, gCurrentObject, MODEL_KOOPA_SHELL,
                                      bhvKoopaShellUnderwater);
                break;
            } else {
                spawn_object_relative(0, 0, 100, 100, gCurrentObject, MODEL_KOOPA_SHELL, bhvKoopaShell);
                break;
            }
        }

        /* GetBobomb cheat */
        while (Cheats.GetBob == true && m->controller->buttonDown & L_TRIG
               && m->controller->buttonPressed & B_BUTTON) {
            spawn_object_relative(0, 0, 100, 100, gCurrentObject, MODEL_BLACK_BOBOMB, bhvBobomb);
            break;
        }

        /* SpawnCommon0 aka Spamba cheat*/
        switch (Cheats.Spamba) {
            case 0:
                break;
            case 1:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 100, 100, gCurrentObject, MODEL_AMP, bhvHomingAmp);
                    break;
                }
                break;
            case 2:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 0, 150, gCurrentObject, MODEL_BLUE_COIN_SWITCH,
                                          bhvBlueCoinSwitch);
                    break;
                }
                break;
            case 3:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 300, 300, gCurrentObject, MODEL_BOWLING_BALL,
                                          bhvPitBowlingBall);
                    break;
                }
                break;
            case 4:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 0, 200, gCurrentObject, MODEL_BREAKABLE_BOX,
                                          bhvBreakableBox);
                    break;
                }
                break;
            case 5:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 50, 100, gCurrentObject, MODEL_BREAKABLE_BOX_SMALL,
                                          bhvBreakableBoxSmall);
                    break;
                }
                break;
            case 6:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 10, 300, gCurrentObject, MODEL_BREAKABLE_BOX_SMALL,
                                          bhvJumpingBox);
                    break;
                }
                break;
            case 7:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, -10, 100, gCurrentObject, MODEL_CHECKERBOARD_PLATFORM,
                                          bhvCheckerboardPlatformSub);
                    break;
                }
                break;
            case 8:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 100, 100, gCurrentObject, MODEL_CHUCKYA, bhvChuckya);
                    break;
                }
                break;
            case 9:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 100, 100, gCurrentObject, MODEL_FLYGUY, bhvFlyGuy);
                    break;
                }
                break;
            case 10:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 100, 100, gCurrentObject, MODEL_NONE,
                                          bhvGoombaTripletSpawner);
                    break;
                }
                break;
            case 11:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 100, 100, gCurrentObject, MODEL_HEART,
                                          bhvRecoveryHeart);
                    break;
                }
                break;
            case 12:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 0, 200, gCurrentObject, MODEL_METAL_BOX,
                                          bhvPushableMetalBox);
                    break;
                }
                break;
            case 13:
                if (m->controller->buttonDown & L_TRIG && m->controller->buttonPressed & Z_TRIG) {
                    spawn_object_relative(0, 0, 50, 50, gCurrentObject, MODEL_PURPLE_SWITCH,
                                          bhvPurpleSwitchHiddenBoxes);
                    break;
                }
                break;
        }

        /*Jukebox*/
        if (Cheats.JBC) {
            /*JBC is the bool, acting like the on/off*/
            switch(Cheats.JB) {
                case 0:
                    play_secondary_music(SEQ_EVENT_CUTSCENE_INTRO, 0, 100, 0);
                case 1:
                    play_secondary_music(SEQ_LEVEL_GRASS, 0, 80, 0);
                case 2:
                    play_secondary_music(SEQ_LEVEL_INSIDE_CASTLE, 0, 80, 0);
                case 3:
                    play_secondary_music(SEQ_LEVEL_WATER, 0, 80, 0);
                case 4:
                    play_secondary_music(SEQ_LEVEL_HOT, 0, 80, 0);
                case 5:
                    play_secondary_music(SEQ_LEVEL_BOSS_KOOPA, 0, 80, 0);
                case 6:
                    play_secondary_music(SEQ_LEVEL_SNOW, 0, 80, 0);
                case 7:
                    play_secondary_music(SEQ_LEVEL_SLIDE, 0, 80, 0);
                case 8:
                    play_secondary_music(SEQ_LEVEL_SPOOKY, 0, 100, 0);
                case 9:
                    play_secondary_music(SEQ_LEVEL_UNDERGROUND, 0, 100, 0);
                case 10:
                    play_secondary_music(SEQ_LEVEL_KOOPA_ROAD, 0, 80, 0);
                case 11:
                    play_secondary_music(SEQ_LEVEL_BOSS_KOOPA_FINAL, 0, 80, 0);
                case 12:
                    play_secondary_music(SEQ_MENU_TITLE_SCREEN, 0, 80, 0);
                case 13:
                    play_secondary_music(SEQ_MENU_FILE_SELECT, 0, 80, 0);
                case 14:
                    play_secondary_music(SEQ_EVENT_POWERUP, 0, 80, 0);
                case 15:
                    play_secondary_music(SEQ_EVENT_METAL_CAP, 0, 80, 0);
                case 16:
                    play_secondary_music(SEQ_EVENT_BOSS, 0, 80, 0);
                case 17:
                    play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 0, 80, 0);
                case 18:
                    play_secondary_music(SEQ_EVENT_CUTSCENE_CREDITS, 0, 100, 0);
            }
        }
        break;
    }
}