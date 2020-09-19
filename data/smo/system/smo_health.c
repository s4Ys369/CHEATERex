#include "../smo_c_includes.h"

//
// Health conversion
//

#define SMO_HEALTH_MAX              0x87F
#define SMO_HEALTH_DEAD             0xFF
#define SMO_NUM_SEGMENTS            6
#define SMO_NUM_SEGMENTS_HALF       (SMO_NUM_SEGMENTS / 2)
#define SMO_NUM_TICKS_PER_SEGMENT   0x10
#define SMO_HEALTH_PER_SEGMENT      ((SMO_HEALTH_MAX - SMO_HEALTH_DEAD) / SMO_NUM_SEGMENTS)
#define SMO_HEALTH_PER_TICK         (SMO_HEALTH_PER_SEGMENT / SMO_NUM_TICKS_PER_SEGMENT)
#define SMO_NUM_COINS_PER_HEAL      10

static s32 smo_fix_health(s32 health) {
    return MAX(SMO_HEALTH_DEAD, MIN(SMO_HEALTH_MAX, (((health - SMO_HEALTH_DEAD + SMO_HEALTH_PER_SEGMENT / 2) / SMO_HEALTH_PER_SEGMENT) * SMO_HEALTH_PER_SEGMENT) + SMO_HEALTH_DEAD));
}

static s32 smo_hp_to_health(s32 hp) {
    return (hp * SMO_HEALTH_PER_TICK) + SMO_HEALTH_DEAD;
}

static s32 smo_health_to_hp(s32 health) {
    return (health - SMO_HEALTH_DEAD + SMO_HEALTH_PER_TICK / 2) / SMO_HEALTH_PER_TICK;
}

static s32 smo_get_max_health(s32 health) {
    if (SMO_HEALTH == 2 || (health - SMO_HEALTH_DEAD > SMO_HEALTH_PER_SEGMENT * SMO_NUM_SEGMENTS_HALF)) {
        return (SMO_HEALTH_PER_SEGMENT * SMO_NUM_SEGMENTS) + SMO_HEALTH_DEAD;
    }
    return (SMO_HEALTH_PER_SEGMENT * SMO_NUM_SEGMENTS_HALF) + SMO_HEALTH_DEAD;
}

s32 smo_get_max_hp(s32 hp) {
    return smo_health_to_hp(smo_get_max_health(smo_hp_to_health(hp)));
}

s32 smo_get_num_segments_per_health_gauge() {
    if (SMO_HEALTH == 2) {
        return SMO_NUM_SEGMENTS;
    }
    return SMO_NUM_SEGMENTS_HALF;
}

s32 smo_get_hp_per_segment() {
    return SMO_NUM_TICKS_PER_SEGMENT;
}

s32 smo_is_mario_losing_hp(struct MarioState *m) {
    return m->oHpCounter > smo_health_to_hp(m->health);
}

//
// O2 level
//

static void increase_O2_level(struct MarioState *m, s32 amount) {
    if (m->oO2 != 0) {
        play_sound(SOUND_MENU_POWER_METER, m->marioObj->header.gfx.cameraToObject);
    }
    m->oO2 = MAX(0, m->oO2 - amount);
}

static void decrease_O2_level(struct MarioState *m, s32 amount) {
    m->oO2 += amount;
}

static void update_O2_level(struct MarioState *m) {

    // Mario is healed -> gives 0.5 extra second of O2 per heal tick
    if (m->healCounter > 0) {
        increase_O2_level(m, 15);
        m->healCounter--;
    }

    // "Metal Mario doesn't even have to breathe"
    if (!(m->flags & MARIO_METAL_CAP) && !(m->action & ACT_FLAG_INTANGIBLE)) {
        
        // Poison gas
        if (m->input & INPUT_IN_POISON_GAS) {
            decrease_O2_level(m, 4);
        }
    
        // Water
        else if ((m->action & ACT_GROUP_MASK) == ACT_GROUP_SUBMERGED) {

            // Above water
            if (m->pos[1] >= (m->waterLevel - 140)) {
                increase_O2_level(m, 0xFFFF);
            }

            // Under water
            else {
                s32 dec = (((m->area->terrainType & TERRAIN_MASK) == TERRAIN_SNOW) ? 3 : 1);

                // Drains more O2 if Mario is plunging or dashing
                if (m->action == ACT_SMO_WATER_DESCENT) {
                    decrease_O2_level(m, 3 * dec);
                } else if (m->action == ACT_SMO_WATER_DASH) {
                    decrease_O2_level(m, 6 * dec);
                } else {
                    decrease_O2_level(m, dec);
                }
            }
        }

        // Fresh air
        else {
            increase_O2_level(m, 0xFFFF);
        }

        // Play a sound effect when low on O2
        if (m->oO2 >= MARIO_ABOUT_TO_DROWN) {
            play_sound(SOUND_MOVING_ALMOST_DROWNING, gDefaultSoundArgs);
        }

        // Drown Mario when out of O2
        if (m->oO2 >= MARIO_BREATH_MAX_DURATION) {
            m->health = 0xFF;
            m->healCounter = 0;
            m->hurtCounter = 0;
        }
    }
}

//
// SMO Life-up cutscene object
//

#define LIFE_UP_CUTSCENE_MAX_DURATION   60
#define LIFE_UP_CUTSCENE_HEAL_DURATION  45

static void update_life_up_cutscene() {
    struct Object *o = gCurrentObject;
    if (o->oTimer >= LIFE_UP_CUTSCENE_MAX_DURATION) {
        o->oMario->action &= ~ACT_FLAG_INTANGIBLE;
        o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
        clear_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
        smo_unsoften_music();
        smo_play_sound_effect(SOUND_ACTION_SMO_LIFE_UP_END, NULL);
        smo_obj_free_data(o);
        obj_mark_for_deletion(o);
    } else {
        smo_update_mario_health(o->oMario);
    }
}

static const BehaviorScript bhvLifeUpCutscene[] = {
    0x08000000,
    0x0C000000, (uintptr_t)(update_life_up_cutscene),
    0x09000000
};

f32 smo_get_life_up_gauge_position(s32 hp) {
    struct Object *obj = obj_get_first_with_behavior(NULL, bhvLifeUpCutscene);
    if (obj == NULL) {
        return (hp <= smo_get_max_hp(0) ? 1.f : 0.f);
    }

    f32 remainingTime = LIFE_UP_CUTSCENE_MAX_DURATION - MAX(LIFE_UP_CUTSCENE_HEAL_DURATION, obj->oTimer);
    f32 endCutsceneTime = (LIFE_UP_CUTSCENE_MAX_DURATION - LIFE_UP_CUTSCENE_HEAL_DURATION);
    return sqrtf(remainingTime / endCutsceneTime);
}

//
// SMO health system
//

static u32 sSmoHealthPreviousState = 0;
s32 smo_update_mario_health(struct MarioState *m) {
    if (SMO_HEALTH == 0) {
        sSmoHealthPreviousState = 0;
        return FALSE;
    }

    // Health system was changed
    // Set m->health, init m->oHpCounter and m->oCoinCounter
    if (SMO_HEALTH != sSmoHealthPreviousState) {
        m->health = MIN(m->health, smo_get_max_health(0));
        m->oHpCounter = smo_health_to_hp(m->health);
        m->oCoinCounter = m->numCoins;
        sSmoHealthPreviousState = SMO_HEALTH;
    }

    // Entered a new area
    if (m->oHpCounter == 0) {
        m->oHpCounter = smo_health_to_hp(m->health);
        m->oCoinCounter = m->numCoins;
    }

    // Update O2
    update_O2_level(m);

    // Mario takes damage
    if (m->hurtCounter != 0) {
        m->health -= SMO_HEALTH_PER_SEGMENT;
        m->hurtCounter = 0;
        smo_play_sound_effect(SOUND_ACTION_SMO_DAMAGE, NULL);
    }

    // Update coin counter and heal Mario every N coins
    if (m->oCoinCounter < m->numCoins) {
        m->oCoinCounter++;
        if ((m->oCoinCounter % SMO_NUM_COINS_PER_HEAL) == 0) {
            smo_heal_mario_for_1_health(m);
        }
    } else {
        m->oCoinCounter = m->numCoins;
    }

    // Update health counter
    m->health = smo_fix_health(m->health);
    s32 counter = smo_health_to_hp(m->health);
    if (m->oHpCounter < counter) {
        m->oHpCounter++;
        if ((m->oHpCounter % SMO_NUM_TICKS_PER_SEGMENT) == (SMO_NUM_TICKS_PER_SEGMENT / 2)) {
            smo_play_sound_effect(SOUND_ACTION_SMO_HEAL, NULL);
        }
    } else if (m->oHpCounter > counter) {
        m->oHpCounter--;
    }

    // Updated
    gHudDisplay.wedges = 0xFF;
    return TRUE;
}

void smo_heal_mario_for_1_health(struct MarioState *m) {
    m->health = MIN(m->health + SMO_HEALTH_PER_SEGMENT, smo_get_max_health(m->health));
}

void smo_set_mario_to_full_health(struct MarioState *m) {
    m->oHpCounter = MAX(m->oHpCounter, 1);
    m->health = smo_get_max_health(m->health);
}

void smo_set_hp_counter_to_mario_health(struct MarioState *m) {
    m->oHpCounter = smo_health_to_hp(m->health);
}

void smo_life_up_mario(struct MarioState *m) {
    if (SMO_HEALTH == 1 && (m->health <= smo_get_max_health(0))) {
        m->hurtCounter = 0;
        m->oHpCounter = smo_health_to_hp(smo_get_max_health(0));
        struct Object *o = spawn_object(m->marioObj, MODEL_NONE, bhvLifeUpCutscene);
        if (o != NULL) {
            smo_soften_music();
            smo_play_sound_effect(SOUND_ACTION_SMO_LIFE_UP, NULL);
            set_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
            smo_obj_alloc_data(o, m);
            o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
            m->action |= ACT_FLAG_INTANGIBLE;
        }
    }
    m->health = SMO_HEALTH_MAX;
}

void smo_health_reset() {
    sSmoHealthPreviousState = 0;
}