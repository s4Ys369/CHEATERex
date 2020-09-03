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
    - while Enabled, if player has 120 stars, warp directly to End Cutscene
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
  * Play as cheats (WIP), much thanks to ferris the crab(io12)
    - Black Bobomb
    - Bobomb Buddy
    - Goomba
    - Amp
    - chuckya
    - FlyGuy

## New features
 * X, Y, and D PAD added (check `sm64.h` for defines)
 * Tighter Controls by Keanine included as a configurable option under Controls
 * Exit to Main Menu by Adya included
 * Optional patches in /enhancements
 * Most data pertaining to external cheats have their own new files : cheats_menu.h, mario_cheats.c, text_cheats_strings.h.in 


## How to add cheats/mods
 * Use `mario_cheats.c` for code
  - each of the actions files of a single line function
  - Use with `mario_cheats.h` to add new functionality
 * Use cheats_menu.h for in game options
  - add TEXT_OPT_<NAME> to optsCheatsStr
  - if using a list, you make make an array for TEXT_OPT
    as well as one for the strings (see file for example)
 * Use text_cheats_strings to tell the game what to print
 - ie `#define TEXT_OPT_HEY _("Hey")`
 - should be done for both JP and US
  + JP only uses capital letters
  + In game buttons example _("[A]") or _("[C]<") no L though
 * Use options_menu.c to add to `static struct Option optCheats`
 * If adding new files, the Makefile will mostly need to be edited
  - tutorial soon-ish

# feel free to ask questions, request pulls, open issues
 
**Make sure you have MXE first before attempting to compile for Windows on Linux and WSL. Follow the guide on the wiki.**
