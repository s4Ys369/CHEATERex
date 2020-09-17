# SMO Mod sound effects

.channelSmo:
chan_largenoteson
chan_setinstr 0
chan_setpanmix 127
chan_setnotepriority 14
chan_setval 0
chan_iowriteval 5
chan_stereoheadseteffects 1
chan_setdyntable .channelSmo_table
chan_jump .main_loop_smo

.main_loop_smo:
chan_delay1
chan_ioreadval 0
chan_bltz .main_loop_smo
.start_playing_smo:
chan_freelayer 0
chan_freelayer 1
chan_freelayer 2
chan_setval 0
chan_iowriteval 5
chan_ioreadval 4
chan_dyncall

.poll_smo:
chan_delay1
chan_ioreadval 0
chan_bltz .skip_smo
  chan_beqz .force_stop_smo
  chan_jump .start_playing_smo
.skip_smo:
chan_testlayerfinished 0
chan_beqz .poll_smo
chan_jump .main_loop_smo
.force_stop_smo:
chan_freelayer 0
chan_freelayer 1
chan_freelayer 2
chan_jump .main_loop_smo

.channelSmo_table:
sound_ref .sound_action_smo_1
sound_ref .sound_action_smo_2
sound_ref .sound_action_smo_3
sound_ref .sound_action_smo_4
sound_ref .sound_action_smo_5

.sound_action_smo_1:
chan_setbank 0
chan_setinstr 6
chan_setlayer 0, .layer_action_smo_1
chan_end

.layer_action_smo_1:
layer_note1 39, 0xc8, 120
layer_end

.sound_action_smo_2:
chan_setbank 0
chan_setinstr 7
chan_setlayer 0, .layer_action_smo_2
chan_end

.layer_action_smo_2:
layer_note1 39, 0xc8, 120
layer_end

.sound_action_smo_3:
chan_setbank 0
chan_setinstr 8
chan_setlayer 0, .layer_action_smo_3
chan_end

.layer_action_smo_3:
layer_note1 39, 0xc8, 127
layer_end

.sound_action_smo_4:
chan_setbank 0
chan_setinstr 9
chan_setlayer 0, .layer_action_smo_4
chan_end

.layer_action_smo_4:
layer_note1 39, 0xc8, 100
layer_end

.sound_action_smo_5:
chan_setbank 0
chan_setinstr 10
chan_setlayer 0, .layer_action_smo_5
chan_end

.layer_action_smo_5:
layer_note1 39, 0xc8, 100
layer_end
