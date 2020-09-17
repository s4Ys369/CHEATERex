#ifndef SMO_OPTIONS_H
#define SMO_OPTIONS_H

#define SMO_CAPPY_BUTTON 0x0040

#ifdef DYNOS
#include "pc/dynamic_options.h"
#define SMO_ID		"SuperMarioOdyssey"
#define SMO_MARIO	dynos_get_value(SMO_ID, 0)
#define SMO_CAPPY	dynos_get_value(SMO_ID, 1)
#define SMO_HEALTH	dynos_get_value(SMO_ID, 3)
#define SMO_UCJ		dynos_get_value(SMO_ID, 4)
#else
#define SMO_MARIO	1
#define SMO_CAPPY	2
#define SMO_HEALTH	1
#define SMO_UCJ		0
extern u32 gSmoConfigKeyCappy[];
#endif

#endif // SMO_OPTIONS_H
