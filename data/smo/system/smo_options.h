#ifndef SMO_OPTIONS_H
#define SMO_OPTIONS_H

#include "types.h"

#define SMO_CAPPY_BUTTON 0x0080

#ifdef DYNOS
#include "pc/dynamic_options.h"
#define SMO_ID		"SuperMarioOdyssey"
#define SMO_MARIO	dynos_get_value(SMO_ID, "smo_mario")
#define SMO_CAPPY	dynos_get_value(SMO_ID, "smo_cappy")
#define SMO_HEALTH	dynos_get_value(SMO_ID, "smo_health")
#define SMO_UCJ		dynos_get_value(SMO_ID, "smo_ucj")
#else
#define SMO_MARIO	1
#define SMO_CAPPY	2
#define SMO_HEALTH	1
#define SMO_UCJ		0
extern u32 gSmoConfigKeyCappy[];
#endif

#endif // SMO_OPTIONS_H
