#ifndef SMO_C_INCLUDES_H
#define SMO_C_INCLUDES_H

/* Include this file only in .c files */

/* include */
#include "sm64.h"
#include "behavior_data.h"
#include "gfx_dimensions.h"
#include "dialog_ids.h"
#include "libc/string.h"
#include <stdio.h>

/* src/game */
#include "game/object_list_processor.h"
#include "game/object_helpers.h"
#include "game/spawn_object.h"
#include "game/spawn_sound.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/interaction.h"
#include "game/ingame_menu.h"
#include "game/mario.h"
#include "game/mario_step.h"
#include "game/print.h"
#include "game/save_file.h"
#include "game/segment2.h"

/* src/engine */
#include "engine/surface_collision.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"

/* src/audio */
#include "audio/external.h"

/* pc */
#include "pc/configfile.h"
<<<<<<< HEAD
#include "pc/controller/controller_api.h"
=======
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

#endif // SMO_C_INCLUDES_H
