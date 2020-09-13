# CHEATERex
Fork of [sm64pc/sm64ex](https://github.com/sm64pc/sm64ex) with additional patches built-in.
To be used as a template for modders, hackers and cheaters.

**no upload of any copyrighted asset**. 
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
 * 60 FPS by Emil included
 * Enable Debug Level Select by KiritoDev included
 * Tighter Controls by Keanine included as a configurable option under Controls
 * Leave Ending Screen by GateGuy included
 * Exit to Main Menu by Adya included
 * 50 Coin 1UP by Keanine included
 * Odyssey Moveset featuring Time Trials v2 by PeachyPeach (Time Trials by GateGuy)
  - must include https://cdn.discordapp.com/attachments/723992726319792140/754402820588961882/smo.cappy_assets.zip
 * Stay In Level After Star v2 by GateGuy
 * Menu C button support by engima907
 * Dynamic Options (DynOS.0.2) by PeachyPeach included
 * And of course, CHEATERv6.5 by $4Y$, me

## How to add cheats/mods
 * Use `src/game/mario_cheats.c` for code
   - each of the `actions` files has a single line function
   - Use with `mario_cheats.h` to add new functionality
 * Use `src/game/options_menu.c` for in game options
   - add `TEXT_OPT_<NAME>` to `optsCheatsStr`
   - if using a list, you need to make an array for `TEXT_OPT`
     as well as one for the strings (see file for example)
 * Use `include/text_options_strings.h.in` to tell the game what to print
   - ie `#define TEXT_OPT_HEY _("Hey")`
   - should be done for both JP and US
   - JP only uses capital letters
   - In game buttons examples `_("[A]")` or `_("[C]<")` no L though
 * Use `src/game/options_menu.c` to add to `static struct Option optCheats`
 * If adding new files, the `Makefile` will most likely need to be edited
   - External Data aka `build/res/base.zip` is handled partly in both the `Makefile` and `Makefile.split`
 * tutorial soon-ish

# feel free to ask questions, request pulls, open issues

