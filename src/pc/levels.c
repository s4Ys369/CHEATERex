#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "levels.h"
#define STUB_LEVEL(_0, _1, _2, _3, _4, _5, _6, _7, _8)
#define DEFINE_LEVEL(_0, _1, _2, _name, _4, _5, _6, _7, _8, _9, _10) extern const LevelScript level_##_name##_entry[];
#include "levels/level_defines.h"
#undef DEFINE_LEVEL
#undef STUB_LEVEL

//
// Level, Course, Script
//

struct LevelCourseScript {
    enum LevelNum level;
    enum CourseNum course;
    const LevelScript *script;
};

#define STUB_LEVEL(_0, _1, _2, _3, _4, _5, _6, _7, _8)
#define DEFINE_LEVEL(_0, _level, _course, _name, _4, _5, _6, _7, _8, _9, _10) { _level, _course, level_##_name##_entry },
static const struct LevelCourseScript sLCS[] = {
#include "levels/level_defines.h"
};
#undef DEFINE_LEVEL
#undef STUB_LEVEL

static const LevelScript *lcs_get_script(enum LevelNum levelId) {
    int lcsCount = (int) (sizeof(sLCS) / sizeof(sLCS[0]));
    for (int i = 0; i != lcsCount; ++i) {
        if (sLCS[i].level == levelId) {
            return sLCS[i].script;
        }
    }
    return NULL;
}

static enum CourseNum lcs_get_course(enum LevelNum levelId) {
    int lcsCount = (int) (sizeof(sLCS) / sizeof(sLCS[0]));
    for (int i = 0; i != lcsCount; ++i) {
        if (sLCS[i].level == levelId) {
            return sLCS[i].course;
        }
    }
    return COURSE_NONE;
}

//
// Course, Name
//

struct CourseName {
    enum CourseNum course;
    const char *name;
};

#define _(x) x
#define COURSE_ACTS(_course, _name, _2, _3, _4, _5, _6, _7) { _course, _name },
#define SECRET_STAR(_course, _name) { _course, _name },
#define CASTLE_SECRET_STARS(_0)
#define EXTRA_TEXT(_0, _1)
static const struct CourseName sCN[] = {
#include "text/us/courses.h"
};
#undef _
#undef COURSE_ACTS
#undef SECRET_STAR
#undef CASTLE_SECRET_STARS
#undef EXTRA_TEXT

static const char *cn_get_name(enum LevelNum levelId) {
    static const char *sDefault = "   CASTLE";
    static const char *sBowser1 = "   BOWSER 1";
    static const char *sBowser2 = "   BOWSER 2";
    static const char *sBowser3 = "   BOWSER 3";
    if (levelId == LEVEL_BOWSER_1) return sBowser1;
    if (levelId == LEVEL_BOWSER_2) return sBowser2;
    if (levelId == LEVEL_BOWSER_3) return sBowser3;
    int cnCount = (int) (sizeof(sCN) / sizeof(sCN[0]));
    for (int i = 0; i != cnCount; ++i) {
        if (sCN[i].course == lcs_get_course(levelId)) {
            return (strlen(sCN[i].name) > 3 ? sCN[i].name : sDefault);
        }
    }
    return sDefault;
}

static const char *level_create_name(const char *cname) {
    cname += 3; // remove course number
    int len = (int) strlen(cname);
    char *name = calloc(len + 1, sizeof(char));
    memcpy(name, cname, len + 1);
    return name;
}

static const char *level_create_name_decapitalized(const char *name) {
    int len = (int) strlen(name);
    char *decaps = calloc(len + 1, sizeof(char));
    memcpy(decaps, name, len + 1);
    bool wasSpace = true;
    for (int i = 0; i != len; ++i) {
        char c = decaps[i];
        if (c >= 'A' && c <= 'Z') {
            if (wasSpace) {
                wasSpace = false;
            } else {
                decaps[i] += ('a' - 'A');
            }
        } else if (c != '\'') {
            wasSpace = true;
        }
    }
    return decaps;
}

//
// Data
//

static int sLevelCount                          = 0;
static int sLevelCountNoCastle                  = 0;
static enum LevelNum *sLevelList                = NULL;
static enum LevelNum *sLevelListNoCastle        = NULL;
static enum LevelNum *sLevelListOrdered         = NULL;
static enum LevelNum *sLevelListOrderedNoCastle = NULL;
static const char **sLevelNames                 = NULL;
static const char **sLevelNamesDecaps           = NULL;
static const LevelScript **sLevelScripts        = NULL;
static enum CourseNum *sLevelCourses            = NULL;

// Runs only once
static void level_init_data() {
    static bool inited = false;
    if (!inited) {
        
        // Level count
        sLevelCount = (int) sizeof(sLCS) / sizeof(sLCS[0]);

        // Level count (no Castle)
        sLevelCountNoCastle = 0;
        for (int i = 0; i != sLevelCount; ++i) {
            if (sLCS[i].course != COURSE_NONE && sLCS[i].course != COURSE_CAKE_END) {
                sLevelCountNoCastle++;
            }
        }

        // Lists allocation
        sLevelList                = calloc(sLevelCount, sizeof(enum LevelNum));
        sLevelListNoCastle        = calloc(sLevelCountNoCastle, sizeof(enum LevelNum));
        sLevelListOrdered         = calloc(sLevelCount, sizeof(enum LevelNum));
        sLevelListOrderedNoCastle = calloc(sLevelCountNoCastle, sizeof(enum LevelNum));
        sLevelNames               = calloc(LEVEL_COUNT, sizeof(const char *));
        sLevelNamesDecaps         = calloc(LEVEL_COUNT, sizeof(const char *));
        sLevelScripts             = calloc(LEVEL_COUNT, sizeof(const LevelScript *));
        sLevelCourses             = calloc(LEVEL_COUNT, sizeof(enum CourseNum));

        // Level list
        for (int i = 0; i != sLevelCount; ++i) {
            sLevelList[i] = sLCS[i].level;
        }

        // Level list ordered by course id
        for (int i = 0, k = 0; i < COURSE_END; ++i) {
            for (int j = 0; j < sLevelCount; ++j) {
                if (sLCS[j].course == (enum CourseNum) i) {
                    sLevelListOrdered[k++] = sLCS[j].level;
                }
            }
        }

        // Level list (no Castle)
        for (int i = 0, k = 0; i != sLevelCount; ++i) {
            if (sLCS[i].course != COURSE_NONE && sLCS[i].course != COURSE_CAKE_END) {
                sLevelListNoCastle[k++] = sLCS[i].level;
            }
        }

        // Level list ordered by course id (no Castle)
        for (int i = 0, k = 0; i < COURSE_END; ++i) {
            if (i != COURSE_NONE && i != COURSE_CAKE_END) {
                for (int j = 0; j < sLevelCount; ++j) {
                    if (sLCS[j].course == (enum CourseNum) i) {
                        sLevelListOrderedNoCastle[k++] = sLCS[j].level;
                    }
                }
            }
        }
        
        // Level names
        for (int i = 0; i != LEVEL_COUNT; ++i) {
            const char *cname = cn_get_name(i);
            sLevelNames[i] = level_create_name(cname);
        }

        // Level names decapitalized
        for (int i = 0; i != LEVEL_COUNT; ++i) {
            sLevelNamesDecaps[i] = level_create_name_decapitalized(sLevelNames[i]);
        }

        // Level scripts
        for (int i = 0; i != LEVEL_COUNT; ++i) {
            sLevelScripts[i] = lcs_get_script(i);
        }

        // Level courses
        for (int i = 0; i != LEVEL_COUNT; ++i) {
            sLevelCourses[i] = lcs_get_course(i);
        }

        // Done
        inited = true;
    }
}

//
// Getters
//

int level_get_count(bool noCastle) {
    level_init_data();
    return (noCastle ? sLevelCountNoCastle : sLevelCount);
}

const enum LevelNum *level_get_list(bool ordered, bool noCastle) {
    level_init_data();
    if (ordered) return (noCastle ? sLevelListOrderedNoCastle : sLevelListOrdered);
    return (noCastle ? sLevelListNoCastle : sLevelList);
}

const char *level_get_name(enum LevelNum levelId) {
    level_init_data();
    return sLevelNames[levelId];
}

const char *level_get_name_decapitalized(enum LevelNum levelId) {
    level_init_data();
    return sLevelNamesDecaps[levelId];
}

const LevelScript *level_get_script(enum LevelNum levelId) {
    level_init_data();
    return sLevelScripts[levelId];
}

enum CourseNum level_get_course(enum LevelNum levelId) {
    level_init_data();
    return sLevelCourses[levelId];
}

