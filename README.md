# CHEATERex
Fork of [sm64pc/sm64ex](https://github.com/sm64pc/sm64ex) with additional features. 

Feel free to report bugs and contribute, but remember, there must be **no upload of any copyrighted asset**. 
Run `./extract_assets.py --clean && make clean` or `make distclean` to remove ROM-originated content.

## Additional Cheats
  * Instant Death/Level Exit L Trigger + R Trigger + A Button + B Button
  * Speed Display, ported by ferris the crab(io12)
  * T Pose Float?
  * Jukebox song selector
  * Quick Ending
    -w hile Enabled, if player has 120 stars, warp directly to End Cutscene
  * Hurt Mario = L Trigger + A Button
    - Burn
    - Shock
    - Set Health to One HP
  * Cannon Anywhere = L Trigger + C Up Button
    - spawns a cannon on player to trigger firing
    *note: if used deep underwater, player will be sent to surface
  * AutoWallKick
    - while enabled, player will automatically wallkick after hitting wall
    - Hold A to increase height
    *note: Bonks will occur if wallkick is not possible
  * Get Shell v2 = L Trigger + R Trigger
    - now spawns underwater shells when in water
  * Get Bomomb = L Trigger + B Button
  * Spamba v2 (Spawns common0 actors)= L Trigger + Z Trigger
  * Swift Swim v3
      - max speed submerged doubled when holding A Button, now with bubbles
  * JAGSTAX's Cap cheats
    - REMOVE cap and DISABLE Cap Music added
  * GateGuy's port of Kaze Emanuar's BLJ Anywhere

## New features

 * Tighter Controls by Keanine included as a configurable option under Controls
 * Exit to Main Menu by Adya included
 * Optional patches in /enhancements
 * Most data pertaining to external cheats have their own new files : cheats_menu.h, mario_cheats.c, text_cheats_strings.h.in 

## Building
For building instructions, please refer to the [wiki](https://github.com/sm64pc/sm64ex/wiki).

**Make sure you have MXE first before attempting to compile for Windows on Linux and WSL. Follow the guide on the wiki.**
