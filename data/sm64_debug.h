#ifndef SM64_DEBUG_H
#define SM64_DEBUG_H

// To use this, write
// #include "data/sm64_debug.h"
// in ultra64.h

void debug_add_box(int x, int y, unsigned int w, unsigned int h, unsigned int rgbaColor);
void debug_add_string(int x, int y, unsigned int rgbaFront, unsigned int rgbaBack, const unsigned char *str64);
void debug_set_key_down(int scancode);
void debug_set_key_up(int scancode);
void debug_update();

#endif // SM64_DEBUG_H
