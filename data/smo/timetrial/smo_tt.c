#include "../smo_c_includes.h"
#include "pc/fs/fs.h"
#include "pc/cheats.h"
#include <time.h>

static const char *smo_tt_get_save_filename() {
    static char buffer[256];
    snprintf(buffer, 256, "%s", SAVE_FILENAME);
    buffer[strlen(buffer) - 4] = 0; // cut extension
    return buffer;
}

static const char *smo_tt_get_folder(const char *path) {
    static char buffer[256];
    snprintf(buffer, 256, "%s/%s", path, smo_tt_get_save_filename());
    return buffer;
}

static const char *smo_tt_get_filename(const char *path, s32 fileIndex, s32 slotIndex) {
    static char buffer[256];
    snprintf(buffer, 256, "%s/%s_%d_%d.ttg", smo_tt_get_folder(path), smo_tt_get_save_filename(), fileIndex, slotIndex);
    return buffer;
}

//
// Time Trials Slots
//

#define NUM_SLOTS                   sizeof(sSmoTimeTrialSlots) / sizeof(sSmoTimeTrialSlots[0])
#define STAR_CODE(course, star)     (u16)(((u16)(course & 0xFF) << 8) | ((u16)(star & 0xFF) << 0))

static const u16 sSmoTimeTrialSlots[] = {

    /* Main courses */
    STAR_CODE(COURSE_BOB, 0),
    STAR_CODE(COURSE_BOB, 1),
    STAR_CODE(COURSE_BOB, 2),
    STAR_CODE(COURSE_BOB, 3),
    STAR_CODE(COURSE_BOB, 4),
    STAR_CODE(COURSE_BOB, 5),
    STAR_CODE(COURSE_BOB, 6),
    STAR_CODE(COURSE_WF, 0),
    STAR_CODE(COURSE_WF, 1),
    STAR_CODE(COURSE_WF, 2),
    STAR_CODE(COURSE_WF, 3),
    STAR_CODE(COURSE_WF, 4),
    STAR_CODE(COURSE_WF, 5),
    STAR_CODE(COURSE_WF, 6),
    STAR_CODE(COURSE_JRB, 0),
    STAR_CODE(COURSE_JRB, 1),
    STAR_CODE(COURSE_JRB, 2),
    STAR_CODE(COURSE_JRB, 3),
    STAR_CODE(COURSE_JRB, 4),
    STAR_CODE(COURSE_JRB, 5),
    STAR_CODE(COURSE_JRB, 6),
    STAR_CODE(COURSE_CCM, 0),
    STAR_CODE(COURSE_CCM, 1),
    STAR_CODE(COURSE_CCM, 2),
    STAR_CODE(COURSE_CCM, 3),
    STAR_CODE(COURSE_CCM, 4),
    STAR_CODE(COURSE_CCM, 5),
    STAR_CODE(COURSE_CCM, 6),
    STAR_CODE(COURSE_BBH, 0),
    STAR_CODE(COURSE_BBH, 1),
    STAR_CODE(COURSE_BBH, 2),
    STAR_CODE(COURSE_BBH, 3),
    STAR_CODE(COURSE_BBH, 4),
    STAR_CODE(COURSE_BBH, 5),
    STAR_CODE(COURSE_BBH, 6),
    STAR_CODE(COURSE_HMC, 0),
    STAR_CODE(COURSE_HMC, 1),
    STAR_CODE(COURSE_HMC, 2),
    STAR_CODE(COURSE_HMC, 3),
    STAR_CODE(COURSE_HMC, 4),
    STAR_CODE(COURSE_HMC, 5),
    STAR_CODE(COURSE_HMC, 6),
    STAR_CODE(COURSE_LLL, 0),
    STAR_CODE(COURSE_LLL, 1),
    STAR_CODE(COURSE_LLL, 2),
    STAR_CODE(COURSE_LLL, 3),
    STAR_CODE(COURSE_LLL, 4),
    STAR_CODE(COURSE_LLL, 5),
    STAR_CODE(COURSE_LLL, 6),
    STAR_CODE(COURSE_SSL, 0),
    STAR_CODE(COURSE_SSL, 1),
    STAR_CODE(COURSE_SSL, 2),
    STAR_CODE(COURSE_SSL, 3),
    STAR_CODE(COURSE_SSL, 4),
    STAR_CODE(COURSE_SSL, 5),
    STAR_CODE(COURSE_SSL, 6),
    STAR_CODE(COURSE_DDD, 0),
    STAR_CODE(COURSE_DDD, 1),
    STAR_CODE(COURSE_DDD, 2),
    STAR_CODE(COURSE_DDD, 3),
    STAR_CODE(COURSE_DDD, 4),
    STAR_CODE(COURSE_DDD, 5),
    STAR_CODE(COURSE_DDD, 6),
    STAR_CODE(COURSE_SL, 0),
    STAR_CODE(COURSE_SL, 1),
    STAR_CODE(COURSE_SL, 2),
    STAR_CODE(COURSE_SL, 3),
    STAR_CODE(COURSE_SL, 4),
    STAR_CODE(COURSE_SL, 5),
    STAR_CODE(COURSE_SL, 6),
    STAR_CODE(COURSE_WDW, 0),
    STAR_CODE(COURSE_WDW, 1),
    STAR_CODE(COURSE_WDW, 2),
    STAR_CODE(COURSE_WDW, 3),
    STAR_CODE(COURSE_WDW, 4),
    STAR_CODE(COURSE_WDW, 5),
    STAR_CODE(COURSE_WDW, 6),
    STAR_CODE(COURSE_TTM, 0),
    STAR_CODE(COURSE_TTM, 1),
    STAR_CODE(COURSE_TTM, 2),
    STAR_CODE(COURSE_TTM, 3),
    STAR_CODE(COURSE_TTM, 4),
    STAR_CODE(COURSE_TTM, 5),
    STAR_CODE(COURSE_TTM, 6),
    STAR_CODE(COURSE_THI, 0),
    STAR_CODE(COURSE_THI, 1),
    STAR_CODE(COURSE_THI, 2),
    STAR_CODE(COURSE_THI, 3),
    STAR_CODE(COURSE_THI, 4),
    STAR_CODE(COURSE_THI, 5),
    STAR_CODE(COURSE_THI, 6),
    STAR_CODE(COURSE_TTC, 0),
    STAR_CODE(COURSE_TTC, 1),
    STAR_CODE(COURSE_TTC, 2),
    STAR_CODE(COURSE_TTC, 3),
    STAR_CODE(COURSE_TTC, 4),
    STAR_CODE(COURSE_TTC, 5),
    STAR_CODE(COURSE_TTC, 6),
    STAR_CODE(COURSE_RR, 0),
    STAR_CODE(COURSE_RR, 1),
    STAR_CODE(COURSE_RR, 2),
    STAR_CODE(COURSE_RR, 3),
    STAR_CODE(COURSE_RR, 4),
    STAR_CODE(COURSE_RR, 5),
    STAR_CODE(COURSE_RR, 6),

    /* Bowser Courses */
    STAR_CODE(COURSE_BITDW, 0),
    STAR_CODE(COURSE_BITDW, 1),
    STAR_CODE(COURSE_BITFS, 0),
    STAR_CODE(COURSE_BITFS, 1),
    STAR_CODE(COURSE_BITS, 0),
    STAR_CODE(COURSE_BITS, 1),

    /* Secret Courses */
    STAR_CODE(COURSE_PSS, 0),
    STAR_CODE(COURSE_PSS, 1),
    STAR_CODE(COURSE_SA, 0),
    STAR_CODE(COURSE_WMOTR, 0),
    STAR_CODE(COURSE_TOTWC, 0),
    STAR_CODE(COURSE_VCUTM, 0),
    STAR_CODE(COURSE_COTMC, 0),
};

static s32 smo_tt_get_slot_index(s32 course, s32 star) {
    u16 starCode = STAR_CODE(course, star);
    for (u32 i = 0; i != NUM_SLOTS; ++i) {
        if (sSmoTimeTrialSlots[i] == starCode) {
            return (s32) i;
        }
    }
    return -1;
}

#undef STAR_CODE

//
// Ghost Frame data
//

struct SmoTimeTrialGhostFrameData {
    s16 posX;
    s16 posY;
    s16 posZ;
    s8 pitch;
    s8 yaw;
    s8 roll;
    u8 scaleX;
    u8 scaleY;
    u8 scaleZ;
    u8 animIndex;
    u8 animFrame;
    u8 level;
    u8 area;
};

//
// Data
//

#define SMO_TT_UNDEFINED_TIME -1

enum { TT_TIMER_DISABLED, TT_TIMER_RUNNING, TT_TIMER_STOPPED };
static s8 sSmoTimeTrialTimerState = TT_TIMER_DISABLED;
static s16 sSmoTimeTrialTimer = 0;
static s8 sSmoTimeTrialHiScore = FALSE;
static s16 sSmoTimeTrialTimes[NUM_SAVE_FILES][NUM_SLOTS];
static struct SmoTimeTrialGhostFrameData sSmoTimeTrialGhostData[SMO_TT_MAX_ALLOWED_TIME];

//
// Read
//

static u16 read_u16(FILE *f) {
    u8 low, high;
    if (fread(&high, 1, 1, f)) {
        if (fread(&low, 1, 1, f)) {
            return (u16)(((u16)(high) << 8) | ((u16)(low)));
        }
    }
    return 0;
}

static u8 read_u8(FILE *f) {
    u8 byte;
    if (fread(&byte, 1, 1, f)) {
        return byte;
    }
    return 0;
}

static s16 smo_tt_read_ghost_data(s32 fileIndex, s32 slot, s8 readLengthOnly) {
    bzero(sSmoTimeTrialGhostData, sizeof(sSmoTimeTrialGhostData));

    // Valid slot
    if (slot == -1) {
        return 0;
    }

    // Open file
    const char *filename = smo_tt_get_filename(fs_writepath, fileIndex, slot);
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        return 0;
    }

    // Read length
    s16 length = (s16) read_u16(f);
    if (readLengthOnly) {
        fclose(f);
        return length;
    }

    // Read data
    for (s16 i = 0; i != length; ++i) {
        sSmoTimeTrialGhostData[i].posX      = (s16) read_u16(f);
        sSmoTimeTrialGhostData[i].posY      = (s16) read_u16(f);
        sSmoTimeTrialGhostData[i].posZ      = (s16) read_u16(f);
        sSmoTimeTrialGhostData[i].pitch     = (s8) read_u8(f);
        sSmoTimeTrialGhostData[i].yaw       = (s8) read_u8(f);
        sSmoTimeTrialGhostData[i].roll      = (s8) read_u8(f);
        sSmoTimeTrialGhostData[i].scaleX    = read_u8(f);
        sSmoTimeTrialGhostData[i].scaleY    = read_u8(f);
        sSmoTimeTrialGhostData[i].scaleZ    = read_u8(f);
        sSmoTimeTrialGhostData[i].animIndex = read_u8(f);
        sSmoTimeTrialGhostData[i].animFrame = read_u8(f);
        sSmoTimeTrialGhostData[i].level     = read_u8(f);
        sSmoTimeTrialGhostData[i].area      = read_u8(f);
    }

    fclose(f);
    return length;
}

void smo_tt_init_times() {
    for (s32 fileIndex = 0; fileIndex != NUM_SAVE_FILES; ++fileIndex) {
        for (u32 slot = 0; slot != NUM_SLOTS; ++slot) {
            s16 t = smo_tt_read_ghost_data(fileIndex, slot, TRUE);
            if (t <= 0 || t > SMO_TT_MAX_ALLOWED_TIME) {
                t = SMO_TT_UNDEFINED_TIME;
            }
            sSmoTimeTrialTimes[fileIndex][slot] = t;
        }
    }
}

//
// Write
//

static void write_u16(FILE *f, u16 word) {
    u8 low = (u8)(word);
    u8 high = (u8)(word >> 8);
    fwrite(&high, 1, 1, f);
    fwrite(&low, 1, 1, f);
}

static void write_u8(FILE *f, u8 byte) {
    fwrite(&byte, 1, 1, f);
}

static void smo_tt_write_ghost_data(s32 fileIndex, s32 slot) {

    // Make folder
    fs_sys_mkdir(smo_tt_get_folder(fs_writepath));

    // Open file
    const char *filename = smo_tt_get_filename(fs_writepath, fileIndex, slot);
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        return;
    }

    // Write length
    s16 length = sSmoTimeTrialTimer;
    write_u16(f, length);

    // Write data
    for (s16 i = 0; i != length; ++i) {
        write_u16(f, (u16) sSmoTimeTrialGhostData[i].posX);
        write_u16(f, (u16) sSmoTimeTrialGhostData[i].posY);
        write_u16(f, (u16) sSmoTimeTrialGhostData[i].posZ);
        write_u8(f, (u8) sSmoTimeTrialGhostData[i].pitch);
        write_u8(f, (u8) sSmoTimeTrialGhostData[i].yaw);
        write_u8(f, (u8) sSmoTimeTrialGhostData[i].roll);
        write_u8(f, sSmoTimeTrialGhostData[i].scaleX);
        write_u8(f, sSmoTimeTrialGhostData[i].scaleY);
        write_u8(f, sSmoTimeTrialGhostData[i].scaleZ);
        write_u8(f, sSmoTimeTrialGhostData[i].animIndex);
        write_u8(f, sSmoTimeTrialGhostData[i].animFrame);
        write_u8(f, sSmoTimeTrialGhostData[i].level);
        write_u8(f, sSmoTimeTrialGhostData[i].area);
    }

    // Update times
    sSmoTimeTrialTimes[fileIndex][slot] = length;
    fclose(f);
}

void smo_tt_save_time(s32 fileIndex, s32 course, s32 level, s32 star, s32 noExit) {
    if (sSmoTimeTrialTimerState == TT_TIMER_RUNNING) {

        // Bowser Key or Grand Star
        if ((!noExit) && (level == LEVEL_BOWSER_1 || level == LEVEL_BOWSER_2 || level == LEVEL_BOWSER_3)) {
            star = 1;
        }

        // Write time and ghost data if new record
        s32 slot = smo_tt_get_slot_index(course, star);
        if (slot != -1) {
            s16 t = sSmoTimeTrialTimes[fileIndex][slot];
            if (t == SMO_TT_UNDEFINED_TIME || t > sSmoTimeTrialTimer) {
                smo_tt_write_ghost_data(fileIndex, slot);
                sSmoTimeTrialHiScore = TRUE;
            }
        }
    }
}

//
// Get
//

s16 smo_tt_get_time(s32 fileIndex, s32 course, s32 star) {
    s32 slot = smo_tt_get_slot_index(course, star);
    if (slot == -1) {
        return SMO_TT_UNDEFINED_TIME;
    }
    return sSmoTimeTrialTimes[fileIndex][slot];
}

s32 smo_tt_get_course_time(s32 fileIndex, s32 course) {
    s32 courseTime = 0;
    for (s32 star = 0;; star++) {
        s32 slot = smo_tt_get_slot_index(course, star);
        if (slot == -1) {
            break;
        }

        s16 t = sSmoTimeTrialTimes[fileIndex][slot];
        if (t == SMO_TT_UNDEFINED_TIME) {
            return SMO_TT_UNDEFINED_TIME;
        }

        courseTime += (s32) t;
    }
    return courseTime;
}

s32 smo_tt_get_bowser_time(s32 fileIndex) {
    s32 bitdw = smo_tt_get_course_time(fileIndex, COURSE_BITDW);
    if (bitdw == SMO_TT_UNDEFINED_TIME) {
        return SMO_TT_UNDEFINED_TIME;
    }
    s32 bitfs = smo_tt_get_course_time(fileIndex, COURSE_BITFS);
    if (bitfs == SMO_TT_UNDEFINED_TIME) {
        return SMO_TT_UNDEFINED_TIME;
    }
    s32 bits = smo_tt_get_course_time(fileIndex, COURSE_BITS);
    if (bits == SMO_TT_UNDEFINED_TIME) {
        return SMO_TT_UNDEFINED_TIME;
    }
    return bitdw + bitfs + bits;
}

s32 smo_tt_get_secret_time(s32 fileIndex) {
    s32 pss = smo_tt_get_course_time(fileIndex, COURSE_PSS);
    if (pss == SMO_TT_UNDEFINED_TIME) {
        return SMO_TT_UNDEFINED_TIME;
    }
    s32 cotmc = smo_tt_get_course_time(fileIndex, COURSE_COTMC);
    if (cotmc == SMO_TT_UNDEFINED_TIME) {
        return SMO_TT_UNDEFINED_TIME;
    }
    s32 totwc = smo_tt_get_course_time(fileIndex, COURSE_TOTWC);
    if (totwc == SMO_TT_UNDEFINED_TIME) {
        return SMO_TT_UNDEFINED_TIME;
    }
    s32 vcutm = smo_tt_get_course_time(fileIndex, COURSE_VCUTM);
    if (vcutm == SMO_TT_UNDEFINED_TIME) {
        return SMO_TT_UNDEFINED_TIME;
    }
    s32 wmotr = smo_tt_get_course_time(fileIndex, COURSE_WMOTR);
    if (wmotr == SMO_TT_UNDEFINED_TIME) {
        return SMO_TT_UNDEFINED_TIME;
    }
    s32 sa = smo_tt_get_course_time(fileIndex, COURSE_SA);
    if (sa == SMO_TT_UNDEFINED_TIME) {
        return SMO_TT_UNDEFINED_TIME;
    }
    return pss + cotmc + totwc + vcutm + wmotr + sa;
}

s32 smo_tt_get_total_time(s32 fileIndex) {
    s32 coursesTime = 0;
    for (s32 course = COURSE_MIN; course <= COURSE_STAGES_MAX; course++) {
        s32 t = smo_tt_get_course_time(fileIndex, course);
        if (t == SMO_TT_UNDEFINED_TIME) {
            return SMO_TT_UNDEFINED_TIME;
        }
        coursesTime += t;
    }

    s32 bowserTime = smo_tt_get_bowser_time(fileIndex);
    if (bowserTime == SMO_TT_UNDEFINED_TIME) {
        return SMO_TT_UNDEFINED_TIME;
    }

    s32 secretTime = smo_tt_get_secret_time(fileIndex);
    if (secretTime == SMO_TT_UNDEFINED_TIME) {
        return SMO_TT_UNDEFINED_TIME;
    }

    return coursesTime + bowserTime + secretTime;
}

//
// Ghost
//

static const BehaviorScript bhvTimeTrialGhost[] = {
    0x08000000,
    0x09000000
};

struct SmoTimeTrialGhostAnimationData {
    struct Animation animation;
    u8 animationData[0x40000];
};
static struct SmoTimeTrialGhostAnimationData sGhostAnimData;

static u16 smo_tt_ghost_retrieve_current_animation_index(struct MarioState *m) {
    struct MarioAnimDmaRelatedThing *animDmaTable = m->animation->animDmaTable;
    void *a = m->animation->currentAnimAddr;
    for (u16 i = 0; i != animDmaTable->count; ++i) {
        void *b = animDmaTable->srcAddr + animDmaTable->anim[i].offset;
        if (a == b) {
            return i;
        }
    }
    return 0;
}

static s32 smo_tt_ghost_load_animation(struct MarioState *m, u16 animIndex) {
    struct MarioAnimDmaRelatedThing *animDmaTable = m->animation->animDmaTable;
    if (animIndex >= animDmaTable->count) {
        return FALSE;
    }

    // Raw data
    u8 *addr = animDmaTable->srcAddr + animDmaTable->anim[animIndex].offset;
    u32 size = animDmaTable->anim[animIndex].size;        
    bcopy(addr, sGhostAnimData.animationData, size);

    // Seek index and values pointers
    struct Animation *a = (struct Animation *) sGhostAnimData.animationData;
    const u16 *indexStart = (const u16 *) ((uintptr_t)(sGhostAnimData.animationData) + (uintptr_t)(a->index));
    const s16 *valueStart = (const s16 *) ((uintptr_t)(sGhostAnimData.animationData) + (uintptr_t)(a->values));

    // Fill ghost animation data
    bcopy(sGhostAnimData.animationData, &sGhostAnimData.animation, sizeof(struct Animation));
    sGhostAnimData.animation.index = indexStart;
    sGhostAnimData.animation.values = valueStart;
    return TRUE;
}

static void smo_tt_ghost_update_animation(struct Object *obj, struct MarioState *m, u16 animIndex, u16 animFrame) {
    static u16 sPreviousValidAnimIndex = 0xFFFF;
    if (obj->header.gfx.unk38.curAnim == NULL) {
        sPreviousValidAnimIndex = 0xFFFF;
    }

    // Load & set animation
    if (animIndex != sPreviousValidAnimIndex && smo_tt_ghost_load_animation(m, animIndex)) {
        obj->header.gfx.unk38.curAnim = &sGhostAnimData.animation;
        obj->header.gfx.unk38.animAccel = 0;
        obj->header.gfx.unk38.animYTrans = m->unkB0;
        sPreviousValidAnimIndex = animIndex;
    }

    // Set frame
    if (obj->header.gfx.unk38.curAnim != NULL) {
        obj->header.gfx.unk38.animFrame = MIN(animFrame, obj->header.gfx.unk38.curAnim->unk08 - 1);
    }
}

static void smo_tt_update_ghost(struct MarioState *m, s16 frame, s16 level, s16 area) {
    struct Object *ghost = obj_get_first_with_behavior(NULL, bhvTimeTrialGhost);

    // If timer reached its max or frame data is ended, unload the ghost
    if (frame >= SMO_TT_MAX_ALLOWED_TIME || !sSmoTimeTrialGhostData[frame].level) {
        if (ghost != NULL) {
            obj_mark_for_deletion(ghost);
        }
        return;
    }

    // Spawn ghost if not loaded
    if (ghost == NULL) {
        ghost = spawn_object(m->marioObj, MODEL_COSMIC_MARIO, bhvTimeTrialGhost);
    }

    // Hide ghost if disabled or its level or area differs from Mario
    if (gSmoTimeTrialsGhost == 0 ||
        sSmoTimeTrialGhostData[frame].level != (u8) level ||
        sSmoTimeTrialGhostData[frame].area != (u8) area) {
        ghost->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
        return;
    }

    // Update ghost
    ghost->hitboxRadius         = 0;
    ghost->hitboxHeight         = 0;
    ghost->oOpacity             = 200;
    ghost->oPosX                = (f32) sSmoTimeTrialGhostData[frame].posX;
    ghost->oPosY                = (f32) sSmoTimeTrialGhostData[frame].posY;
    ghost->oPosZ                = (f32) sSmoTimeTrialGhostData[frame].posZ;
    ghost->oFaceAnglePitch      = (s16) sSmoTimeTrialGhostData[frame].pitch * 0x100;
    ghost->oFaceAngleYaw        = (s16) sSmoTimeTrialGhostData[frame].yaw * 0x100;
    ghost->oFaceAngleRoll       = (s16) sSmoTimeTrialGhostData[frame].roll * 0x100;
    ghost->oMoveAnglePitch      = (s16) sSmoTimeTrialGhostData[frame].pitch * 0x100;
    ghost->oMoveAngleYaw        = (s16) sSmoTimeTrialGhostData[frame].yaw * 0x100;
    ghost->oMoveAngleRoll       = (s16) sSmoTimeTrialGhostData[frame].roll * 0x100;
    ghost->header.gfx.pos[0]    = (f32) sSmoTimeTrialGhostData[frame].posX;
    ghost->header.gfx.pos[1]    = (f32) sSmoTimeTrialGhostData[frame].posY;
    ghost->header.gfx.pos[2]    = (f32) sSmoTimeTrialGhostData[frame].posZ;
    ghost->header.gfx.angle[0]  = (s16) sSmoTimeTrialGhostData[frame].pitch * 0x100;
    ghost->header.gfx.angle[1]  = (s16) sSmoTimeTrialGhostData[frame].yaw * 0x100;
    ghost->header.gfx.angle[2]  = (s16) sSmoTimeTrialGhostData[frame].roll * 0x100;
    ghost->header.gfx.scale[0]  = (f32) sSmoTimeTrialGhostData[frame].scaleX / 100.f;
    ghost->header.gfx.scale[1]  = (f32) sSmoTimeTrialGhostData[frame].scaleY / 100.f;
    ghost->header.gfx.scale[2]  = (f32) sSmoTimeTrialGhostData[frame].scaleZ / 100.f;
    ghost->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
    smo_tt_ghost_update_animation(ghost, m, (u16) sSmoTimeTrialGhostData[frame].animIndex, (u16) sSmoTimeTrialGhostData[frame].animFrame);
}

static void smo_tt_record_ghost(struct MarioState *m, s32 frame) {
    if (frame < 0 || frame >= SMO_TT_MAX_ALLOWED_TIME) {
        return;
    }

    sSmoTimeTrialGhostData[frame].posX      = (s16) m->marioObj->oPosX;
    sSmoTimeTrialGhostData[frame].posY      = (s16) m->marioObj->oPosY;
    sSmoTimeTrialGhostData[frame].posZ      = (s16) m->marioObj->oPosZ;
    sSmoTimeTrialGhostData[frame].pitch     = (s8)(m->marioObj->oFaceAnglePitch / 0x100);
    sSmoTimeTrialGhostData[frame].yaw       = (s8)(m->marioObj->oFaceAngleYaw / 0x100);
    sSmoTimeTrialGhostData[frame].roll      = (s8)(m->marioObj->oFaceAngleRoll / 0x100);
    sSmoTimeTrialGhostData[frame].scaleX    = (u8) MAX(0, MIN(255, m->marioObj->header.gfx.scale[0] * 100.f));
    sSmoTimeTrialGhostData[frame].scaleY    = (u8) MAX(0, MIN(255, m->marioObj->header.gfx.scale[1] * 100.f));
    sSmoTimeTrialGhostData[frame].scaleZ    = (u8) MAX(0, MIN(255, m->marioObj->header.gfx.scale[2] * 100.f));
    sSmoTimeTrialGhostData[frame].animIndex = (u8) smo_tt_ghost_retrieve_current_animation_index(m);
    sSmoTimeTrialGhostData[frame].animFrame = (u8) MAX(0, MIN(255, MIN(m->marioObj->header.gfx.unk38.animFrame, m->marioObj->header.gfx.unk38.curAnim->unk08 - 1)));
    sSmoTimeTrialGhostData[frame].level     = (u8) gCurrLevelNum;
    sSmoTimeTrialGhostData[frame].area      = (u8) gCurrAreaIndex;
}

static void smo_tt_ghost_unload() {
    struct Object *ghost = obj_get_first_with_behavior(NULL, bhvTimeTrialGhost);
    if (ghost != NULL) {
        obj_mark_for_deletion(ghost);
    }
}

//
// Timer
//

static s32 smo_tt_get_ghost_data_star_to_load(s32 course, s32 star) {
    
    // Main courses
    if (course >= COURSE_BOB && course <= COURSE_RR) {
        return (gSmoTimeTrialsMainCourseStar == 1 ? 6 : MAX(0, MIN(star, 5)));
    }

    // Bowser courses
    if (course >= COURSE_BITDW && course <= COURSE_BITS) {
        return (gSmoTimeTrialsBowserStar == 1 ? 0 : 1);
    }

    // PSS
    if (course == COURSE_PSS) {
        return (gSmoTimeTrialsPSSStar == 1 ? 1 : 0);
    }

    // Default
    return 0;
}

void smo_tt_start_timer(s32 fileIndex, s32 course, s32 star) {
    static s32 sPrevCourse = -1;
    if (IS_SMO_TIME_TRIALS) {

        // Runs timer if course is TTable
        if (smo_tt_get_slot_index(course, 0) != -1) {
            sSmoTimeTrialTimerState = TT_TIMER_RUNNING;
            
            if (Cheats.LevelReset) {
                sSmoTimeTrialTimer = 0;
                star = smo_tt_get_ghost_data_star_to_load(course, star);
                smo_tt_read_ghost_data(fileIndex, smo_tt_get_slot_index(course, star), FALSE);
            }

            // Restart timer and init ghost data if different course
            if (course != sPrevCourse) {
                sSmoTimeTrialTimer = 0;
                star = smo_tt_get_ghost_data_star_to_load(course, star);
                smo_tt_read_ghost_data(fileIndex, smo_tt_get_slot_index(course, star), FALSE);
            }
        } else {
            sSmoTimeTrialTimerState = TT_TIMER_DISABLED;
        }
    }
    sSmoTimeTrialHiScore = FALSE;
    sPrevCourse = course;
}

void smo_tt_update_timer(struct MarioState *m) {

    // Disable timer and set timer to max time if Time Trials is disabled
    if (!IS_SMO_TIME_TRIALS) {
        sSmoTimeTrialTimerState = TT_TIMER_DISABLED;
        sSmoTimeTrialTimer = SMO_TT_MAX_ALLOWED_TIME;
        smo_tt_ghost_unload();
    }

    // Update running ghost, record ghost data, and advance timer if it's running
    if (sSmoTimeTrialTimerState == TT_TIMER_RUNNING) {
        smo_tt_update_ghost(m, sSmoTimeTrialTimer, gCurrLevelNum, gCurrAreaIndex);
        smo_tt_record_ghost(m, sSmoTimeTrialTimer);
        sSmoTimeTrialTimer = MIN(sSmoTimeTrialTimer + 1, SMO_TT_MAX_ALLOWED_TIME);
    }

    // Hide timer if disabled
    if (sSmoTimeTrialTimerState == TT_TIMER_DISABLED) {
        gHudDisplay.flags &= ~HUD_DISPLAY_FLAG_TIME_TRIAL_TIMER;
    } else {
        gHudDisplay.flags |= HUD_DISPLAY_FLAG_TIME_TRIAL_TIMER;
    }
}

void smo_tt_stop_timer() {
    if (sSmoTimeTrialTimerState == TT_TIMER_RUNNING) {
        sSmoTimeTrialTimerState = TT_TIMER_STOPPED;
    }
}

//
// Int to String
//

#include "../smo_alpha_begin.h"

static const u8 *smo_tt_time_to_string(s16 time) {
    static u8 buffer[16];

    if (time == SMO_TT_UNDEFINED_TIME) {
        buffer[0] = SLH
        buffer[1] = APO
        buffer[2] = SLH
        buffer[3] = SLH
        buffer[4] = DBQ
        buffer[5] = SLH
        buffer[6] = SLH
        buffer[7] = DIALOG_CHAR_TERMINATOR;
        return buffer;
    }

    buffer[0] = (time / 1800) % 10;
    buffer[1] = APO
    buffer[2] = (time / 300) % 6;
    buffer[3] = (time / 30) % 10;
    buffer[4] = DBQ
    buffer[5] = (time / 3) % 10;
    buffer[6] = ((time * 10) / 3) % 10;
    buffer[7] = DIALOG_CHAR_TERMINATOR;
    return buffer;
}

static const u8 *smo_tt_course_time_to_string(s32 coursetime) {
    static u8 buffer[16];

    if (coursetime == SMO_TT_UNDEFINED_TIME) {
        buffer[0] = SLH
        buffer[1] = SLH
        buffer[2] = APO
        buffer[3] = SLH
        buffer[4] = SLH
        buffer[5] = DBQ
        buffer[6] = SLH
        buffer[7] = SLH
        buffer[8] = DIALOG_CHAR_TERMINATOR;
        return buffer;
    }

    buffer[0] = (coursetime / 18000) % 10;
    buffer[1] = (coursetime / 1800) % 10;
    buffer[2] = APO
    buffer[3] = (coursetime / 300) % 6;
    buffer[4] = (coursetime / 30) % 10;
    buffer[5] = DBQ
    buffer[6] = (coursetime / 3) % 10;
    buffer[7] = ((coursetime * 10) / 3) % 10;
    buffer[8] = DIALOG_CHAR_TERMINATOR;
    return buffer;
}

static const u8 *smo_tt_total_time_to_string(s32 totalTime) {
    static u8 buffer[16];

    if (totalTime == SMO_TT_UNDEFINED_TIME) {
        buffer[0] = SLH
        buffer[1] = SLH
        buffer[2] = COL
        buffer[3] = SLH
        buffer[4] = SLH
        buffer[5] = COL
        buffer[6] = SLH
        buffer[7] = SLH
        buffer[8] = PER
        buffer[9] = SLH
        buffer[10] = SLH
        buffer[11] = DIALOG_CHAR_TERMINATOR;
        return buffer;
    }

    buffer[0] = (totalTime / 1080000) % 10;
    buffer[1] = (totalTime / 108000) % 10;
    buffer[2] = COL
    buffer[3] = (totalTime / 18000) % 6;
    buffer[4] = (totalTime / 1800) % 10;
    buffer[5] = COL
    buffer[6] = (totalTime / 300) % 6;
    buffer[7] = (totalTime / 30) % 10;
    buffer[8] = PER
    buffer[9] = (totalTime / 3) % 10;
    buffer[10] = ((totalTime * 10) / 3) % 10;
    buffer[11] = DIALOG_CHAR_TERMINATOR;
    return buffer;
}

//
// Render
//

static const u8 sSmoTimeTrialTextStarFilled[]   = { ST1                                     DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextStarHollow[]   = { ST0                                     DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextApostrophe[]   = { GLYPH_APOSTROPHE,                       DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextDoubleQuote[]  = { GLYPH_DOUBLE_QUOTE,                     DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextPause[]        = { P A U S E                               DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextTime[]         = { T I M E                                 DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextHiScore[]      = { H I _ S C O R E                         DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextStar1[]        = { S T A R _ 1,                            DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextStar2[]        = { S T A R _ 2,                            DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextStar3[]        = { S T A R _ 3,                            DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextStar4[]        = { S T A R _ 4,                            DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextStar5[]        = { S T A R _ 5,                            DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextStar6[]        = { S T A R _ 6,                            DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextStar7[]        = { S T A R _ COI                           DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextKey1[]         = { K E Y _ 1,                              DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextKey2[]         = { K E Y _ 2,                              DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextFinal[]        = { F I N A L                               DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextCourse[]       = { C O U R S E                             DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextTotal[]        = { T O T A L                               DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextBowser[]       = { _ _ _ _ _ B O W S E R _ C O U R S E S   DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextPSS1[]         = { P S S _ 1,                              DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextPSS2[]         = { P S S _ 2,                              DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextSA[]           = { S A                                     DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextTOTWC[]        = { T O T W C                               DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextVCUTM[]        = { V C U T M                               DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextCOTMC[]        = { C O T M C                               DIALOG_CHAR_TERMINATOR };
static const u8 sSmoTimeTrialTextWMOTR[]        = { W M O T R                               DIALOG_CHAR_TERMINATOR };
static const u8 *sSmoTimeTrialTextStarGlyph[]   = { sSmoTimeTrialTextStarHollow, sSmoTimeTrialTextStarFilled };

struct RenderParams { const u8 *label; u8 starGlyph; s16 time; };
static const struct RenderParams sNoParam = { NULL, 0, 0 };

#include "../smo_alpha_end.h"

/*
Position:
Title
1 5
2 6
3 7
4 8
C T
*/
static void smo_tt_render_pause_times(struct RenderParams params[8], const u8 *title, s32 courseTime, s32 totalTime) {

    // Title
    print_generic_string(96, 168, title);

    // Stars
    for (s32 i = 0; i != 8; ++i) {
        if (params[i].label == NULL) {
            continue;
        }

        s16 x = 36 + 144 * (i / 4);
        s16 y = 144 - 24 * (i % 4);
        print_generic_string(x, y, params[i].label);
        print_generic_string(x + 46, y, sSmoTimeTrialTextStarGlyph[params[i].starGlyph]);
        print_generic_string(x + 60, y, smo_tt_time_to_string(params[i].time));
    }

    // Course time
    print_generic_string(36, 48, sSmoTimeTrialTextCourse);
    print_generic_string(89, 48, smo_tt_course_time_to_string(courseTime));

    // Total time
    print_generic_string(180, 48, sSmoTimeTrialTextTotal);
    print_generic_string(217, 48, smo_tt_total_time_to_string(totalTime));
}

static void smo_tt_render_pause_course_times(s32 fileIndex, s32 course) {
    u8 starFlags = save_file_get_star_flags(fileIndex, course - COURSE_MIN);

    smo_tt_render_pause_times((struct RenderParams[]) {
        (struct RenderParams) { sSmoTimeTrialTextStar1, (starFlags >> 0) & 1, smo_tt_get_time(fileIndex, course, 0) }, 
        (struct RenderParams) { sSmoTimeTrialTextStar2, (starFlags >> 1) & 1, smo_tt_get_time(fileIndex, course, 1) },
        (struct RenderParams) { sSmoTimeTrialTextStar3, (starFlags >> 2) & 1, smo_tt_get_time(fileIndex, course, 2) },
        sNoParam,
        (struct RenderParams) { sSmoTimeTrialTextStar4, (starFlags >> 3) & 1, smo_tt_get_time(fileIndex, course, 3) },
        (struct RenderParams) { sSmoTimeTrialTextStar5, (starFlags >> 4) & 1, smo_tt_get_time(fileIndex, course, 4) },
        (struct RenderParams) { sSmoTimeTrialTextStar6, (starFlags >> 5) & 1, smo_tt_get_time(fileIndex, course, 5) },
        (struct RenderParams) { sSmoTimeTrialTextStar7, (starFlags >> 6) & 1, smo_tt_get_time(fileIndex, course, 6) },
    },
        ((const u8 **) seg2_course_name_table)[course - COURSE_MIN],
        smo_tt_get_course_time(fileIndex, course),
        smo_tt_get_total_time(fileIndex)
    );
}

static void smo_tt_render_pause_bowser_times(s32 fileIndex) {
    u8 bitdwStarFlags = save_file_get_star_flags(fileIndex, COURSE_BITDW - COURSE_MIN);
    u8 bitfsStarFlags = save_file_get_star_flags(fileIndex, COURSE_BITFS - COURSE_MIN);
    u8 bitsStarFlags  = save_file_get_star_flags(fileIndex, COURSE_BITS - COURSE_MIN);
    u8 bitdwKeyFlag   = save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_UNLOCKED_BASEMENT_DOOR);
    u8 bitfsKeyFlag   = save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_2 | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR);
    u8 bitsGrandStar  = smo_tt_get_time(fileIndex, COURSE_BITS, 1) != SMO_TT_UNDEFINED_TIME;

    smo_tt_render_pause_times((struct RenderParams[]) {
        (struct RenderParams) { sSmoTimeTrialTextStar1, (bitdwStarFlags >> 0) & 1, smo_tt_get_time(fileIndex, COURSE_BITDW, 0) }, 
        (struct RenderParams) { sSmoTimeTrialTextStar2, (bitfsStarFlags >> 0) & 1, smo_tt_get_time(fileIndex, COURSE_BITFS, 0) },
        (struct RenderParams) { sSmoTimeTrialTextStar3, (bitsStarFlags  >> 0) & 1, smo_tt_get_time(fileIndex, COURSE_BITS,  0) },
        sNoParam,
        (struct RenderParams) { sSmoTimeTrialTextKey1,  (bitdwKeyFlag)       != 0, smo_tt_get_time(fileIndex, COURSE_BITDW, 1) },
        (struct RenderParams) { sSmoTimeTrialTextKey2,  (bitfsKeyFlag)       != 0, smo_tt_get_time(fileIndex, COURSE_BITFS, 1) },
        (struct RenderParams) { sSmoTimeTrialTextFinal, (bitsGrandStar)      != 0, smo_tt_get_time(fileIndex, COURSE_BITS,  1) },
        sNoParam,
    },
        sSmoTimeTrialTextBowser,
        smo_tt_get_bowser_time(fileIndex),
        smo_tt_get_total_time(fileIndex)
    );
}

static void smo_tt_render_pause_secret_times(s32 fileIndex) {
    u8 totwcStarFlags = save_file_get_star_flags(fileIndex, COURSE_TOTWC - COURSE_MIN);
    u8 vcutmStarFlags = save_file_get_star_flags(fileIndex, COURSE_VCUTM - COURSE_MIN);
    u8 cotmcStarFlags = save_file_get_star_flags(fileIndex, COURSE_COTMC - COURSE_MIN);
    u8 pssStarFlags   = save_file_get_star_flags(fileIndex, COURSE_PSS - COURSE_MIN);
    u8 saStarFlags    = save_file_get_star_flags(fileIndex, COURSE_SA - COURSE_MIN);
    u8 wmotrStarFlags = save_file_get_star_flags(fileIndex, COURSE_WMOTR - COURSE_MIN);

    smo_tt_render_pause_times((struct RenderParams[]) {
        (struct RenderParams) { sSmoTimeTrialTextPSS1,  (pssStarFlags   >> 0) & 1, smo_tt_get_time(fileIndex, COURSE_PSS,   0) },
        (struct RenderParams) { sSmoTimeTrialTextPSS2,  (pssStarFlags   >> 1) & 1, smo_tt_get_time(fileIndex, COURSE_PSS,   1) },
        (struct RenderParams) { sSmoTimeTrialTextSA,    (saStarFlags    >> 0) & 1, smo_tt_get_time(fileIndex, COURSE_SA,    0) },
        (struct RenderParams) { sSmoTimeTrialTextWMOTR, (wmotrStarFlags >> 0) & 1, smo_tt_get_time(fileIndex, COURSE_WMOTR, 0) },
        (struct RenderParams) { sSmoTimeTrialTextTOTWC, (totwcStarFlags >> 0) & 1, smo_tt_get_time(fileIndex, COURSE_TOTWC, 0) }, 
        (struct RenderParams) { sSmoTimeTrialTextVCUTM, (vcutmStarFlags >> 0) & 1, smo_tt_get_time(fileIndex, COURSE_VCUTM, 0) },
        (struct RenderParams) { sSmoTimeTrialTextCOTMC, (cotmcStarFlags >> 0) & 1, smo_tt_get_time(fileIndex, COURSE_COTMC, 0) },
        sNoParam,
    },
        ((const u8 **) seg2_course_name_table)[COURSE_MAX],
        smo_tt_get_secret_time(fileIndex),
        smo_tt_get_total_time(fileIndex)
    );
}


void smo_tt_render_pause_castle_main_strings(s32 fileIndex, s8 *index) {
    s8 prevIndex = *index;
    handle_menu_scrolling(MENU_SCROLL_VERTICAL, index, -1, COURSE_STAGES_COUNT + 2);
    *index = (*index + COURSE_STAGES_COUNT + 2) % (COURSE_STAGES_COUNT + 2);
    
    // Hide the undiscovered courses
    if (*index < COURSE_STAGES_COUNT) {
        s8 inc = (*index >= prevIndex ? +1 : -1);
        while (save_file_get_course_star_count(fileIndex, *index) == 0) {
            *index += inc;
            if (*index == COURSE_STAGES_COUNT || *index == -1) {
                *index = COURSE_STAGES_COUNT;
                break;
            }
        }
    }

    // Render the colorful "PAUSE"
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    print_hud_lut_string(HUD_LUT_GLOBAL, 127, 14, sSmoTimeTrialTextPause);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    // Render the black box behind the text
    create_dl_translation_matrix(MENU_MTX_PUSH, 16, 194, 0); // position from bottom-left
    create_dl_scale_matrix(MENU_MTX_NOPUSH, 2.215f, 1.925f, 1.0f); // 2.2*130=286px wide, 1.925*80=154px high # 2.21538
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 105);
    gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    create_dl_translation_matrix(MENU_MTX_PUSH, 166, 198, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, 90.0f, 0, 0, 1.0f);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    create_dl_translation_matrix(MENU_MTX_PUSH, 151, 36, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, 270.0f, 0, 0, 1.0f);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    // Render the text
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    switch (*index) {
        case COURSE_STAGES_COUNT:
            smo_tt_render_pause_secret_times(fileIndex);
            break;
        case COURSE_STAGES_COUNT + 1:
            smo_tt_render_pause_bowser_times(fileIndex);
            break;
        default:
            smo_tt_render_pause_course_times(fileIndex, *index + COURSE_MIN);
            break;
    }
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

//
// HUD
//

static void smo_tt_render_timer(s16 x, s16 y, s16 time, s32 hiScore) {
    const u8 *text;
    u8 colorFade;
    s16 xOffset;
    
    // Print a flashing HI SCORE if the recorded time is better than the previous one
    if (hiScore) {
        text = sSmoTimeTrialTextHiScore;
        colorFade = sins(gGlobalTimer * 0x1000) * 50.f + 205.f;
        xOffset = 44;
    } else {
        text = sSmoTimeTrialTextTime;
        colorFade = 255;
        xOffset = 0;
    }

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, colorFade, colorFade, colorFade, 255);
    print_hud_lut_string(HUD_LUT_GLOBAL, GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(x + xOffset), 225 - y, text);
    print_hud_lut_string(HUD_LUT_GLOBAL, GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(x - 60), 225 - y, int_to_sm64_string((time / 1800) % 60, "%0d"));
    print_hud_lut_string(HUD_LUT_GLOBAL, GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(x - 80), 225 - y, int_to_sm64_string((time / 30) % 60, "%02d"));
    print_hud_lut_string(HUD_LUT_GLOBAL, GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(x - 114), 225 - y, int_to_sm64_string(((time * 10) / 3) % 100, "%02d"));
    print_hud_lut_string(HUD_LUT_GLOBAL, GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(x - 70), 217 - y, sSmoTimeTrialTextApostrophe);
    print_hud_lut_string(HUD_LUT_GLOBAL, GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(x - 105), 217 - y, sSmoTimeTrialTextDoubleQuote);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
}

void smo_tt_render_hud_timers() {
    if (gHudDisplay.flags != HUD_DISPLAY_NONE && configHUD) {
        create_dl_ortho_matrix();

        s16 pssTimerX, pssTimerY, ttTimerX, ttTimerY;
        if (IS_SMO_HEALTH) {
            ttTimerX = 294;
            ttTimerY = 209;
            pssTimerX = 154;
            pssTimerY = 12;
        } else {
            if (sSmoTimeTrialTimerState != TT_TIMER_DISABLED) {
                ttTimerX = 164;
                ttTimerY = 185;
                pssTimerX = 164;
                pssTimerY = 161;
            } else {
                ttTimerX = 0;
                ttTimerY = 0;
                pssTimerX = 164;
                pssTimerY = 185;
            }
        }

        // TT Timer
        if (gHudDisplay.flags & HUD_DISPLAY_FLAG_TIME_TRIAL_TIMER) {
            smo_tt_render_timer(ttTimerX, ttTimerY, sSmoTimeTrialTimer, sSmoTimeTrialHiScore && sSmoTimeTrialTimerState == TT_TIMER_STOPPED);
        }

        // PSS/KtQ Timer
        if (gHudDisplay.flags & HUD_DISPLAY_FLAG_TIMER) {
            smo_tt_render_timer(pssTimerX, pssTimerY, gHudDisplay.timer, FALSE);
        }
    }
}

void smo_tt_render_star_select_time(s32 fileIndex, s32 course, s32 star) {
    s16 time = smo_tt_get_time(fileIndex, course, star);
    if (time != SMO_TT_UNDEFINED_TIME) {
        smo_tt_render_timer(294, 216, time, FALSE);
    }
}
