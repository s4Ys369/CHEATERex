#include "../smo_c_includes.h"

//
// Sound data
//

#define SMO_SEQ_INITCHANNELS_BYTE_0    0x07
#define SMO_SEQ_INITCHANNELS_BYTE_1    0xFF

static u8 sPlaySmoSoundEffects = TRUE;
void smo_perform_sequence_checks(void *data) {
    u8 *byteData = (u8 *) data;

    // Number of sequences | Must be 0x23
    if (byteData[0x02] != 0x23) {
        sPlaySmoSoundEffects = FALSE;
        return;
    }

    // Sequence 0 byte pattern | Should start at 0x240
    // 0xD3 0x60 0xD5 0x00 0xDB 0x7F 0xDD 0x78
    // Corresponds to the first lines of code of "00_sound_player.s":
    // seq_setmutebhv 0x60
    // seq_setmutescale 0
    // seq_setvol 127
    // seq_settempo 120
    if (byteData[0x240] != 0xD3 ||
        byteData[0x241] != 0x60 ||
        byteData[0x242] != 0xD5 ||
        byteData[0x243] != 0x00 ||
        byteData[0x244] != 0xDB ||
        byteData[0x245] != 0x7F ||
        byteData[0x246] != 0xDD ||
        byteData[0x247] != 0x78) {
        sPlaySmoSoundEffects = FALSE;
        return;
    }

    // Sound Bank 10 init
    // 0xD7 0x07 0xFF
    if (byteData[0x248] != 0xD7 ||
        byteData[0x249] != SMO_SEQ_INITCHANNELS_BYTE_0 ||
        byteData[0x24A] != SMO_SEQ_INITCHANNELS_BYTE_1) {
        sPlaySmoSoundEffects = FALSE;
        return;
    }

    // Sound Banks start
    // 0x90 xx xx
    // 0x91 xx xx
    // 0x92 xx xx
    // 0x93 xx xx
    // 0x94 xx xx
    // 0x95 xx xx
    // 0x96 xx xx
    // 0x97 xx xx
    // 0x98 xx xx
    // 0x99 xx xx
    // 0x9A xx xx
    if (byteData[0x24B] != 0x90 ||
        byteData[0x24E] != 0x91 ||
        byteData[0x251] != 0x92 ||
        byteData[0x254] != 0x93 ||
        byteData[0x257] != 0x94 ||
        byteData[0x25A] != 0x95 ||
        byteData[0x25D] != 0x96 ||
        byteData[0x260] != 0x97 ||
        byteData[0x263] != 0x98 ||
        byteData[0x266] != 0x99 ||
        byteData[0x269] != 0x9A) {
        sPlaySmoSoundEffects = FALSE;
        return;
    }

    // Now we are sure that there are 11 sound banks filled with data
    // We can stop the checks here
    sPlaySmoSoundEffects = TRUE;
    return;
}

void smo_play_sound_effect(s32 soundBits, f32 *pos) {
    if (sPlaySmoSoundEffects) {
        if (!pos) {
            play_sound(soundBits, gDefaultSoundArgs);
        } else {
            play_sound(soundBits, pos);
        }
    }
}

void smo_soften_music() {
    func_8031FFB4(SEQ_PLAYER_LEVEL, 60, 40);
}

void smo_unsoften_music() {
    sequence_player_unlower(SEQ_PLAYER_LEVEL, 60);
}
