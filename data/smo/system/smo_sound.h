#ifndef SMO_SOUND_H
#define SMO_SOUND_H

#include "types.h"

/*This mod defines new sounds in the source code (see "00.json" and "00_sound_player.s"
for more details), and uses Sound Bank 10, which is normally not accessible.
The game must perform checks when loading external data to make sure there is
a Sound Bank 10, or it will crash as soon as it'll try to play those sounds. */
void smo_perform_sequence_checks(void *data);
void smo_play_sound_effect(s32 soundBits, f32 *pos);
void smo_soften_music();
void smo_unsoften_music();

#endif // SMO_SOUND_H
