#ifndef LEVELS_H
#define LEVELS_H

/*
This file helps to provide useful info about the game's levels:
level list, name, script, and course id.
*/

#include <stdbool.h>
#include "types.h"
#include "level_table.h"
#include "course_table.h"

int level_get_count(bool noCastle);
const enum LevelNum *level_get_list(bool ordered, bool noCastle);
const char *level_get_name(enum LevelNum levelId);
const char *level_get_name_decapitalized(enum LevelNum levelId);
const LevelScript *level_get_script(enum LevelNum levelId);
enum CourseNum level_get_course(enum LevelNum levelId);

#endif // LEVELS_H