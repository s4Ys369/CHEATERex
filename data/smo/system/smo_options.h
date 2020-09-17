#ifndef SMO_OPTIONS_H
#define SMO_OPTIONS_H

<<<<<<< HEAD
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
=======
#include "types.h"

#define IS_SMO_MARIO			(gSmoMario == 1)
#define IS_SMO_CAPPY_AVAILABLE	(gSmoCappy != 0)
#define IS_SMO_CAPPY_CAPTURE	(gSmoCappy == 2)
#define IS_SMO_HEALTH			(gSmoHealth >= 1)
#define IS_SMO_TIME_TRIALS		(gSmoTimeTrials == 1)

extern u8  gSmoMario;
extern u32 gSmoCappy;
extern u32 gSmoHealth;
extern u8  gSmoTimeTrials;
extern u32 gSmoTimeTrialsGhost;
extern u32 gSmoTimeTrialsMainCourseStar;
extern u32 gSmoTimeTrialsBowserStar;
extern u32 gSmoTimeTrialsPSSStar;
extern u8  gSmoUnlimitedCappyJumps;

void smo_options_reset();
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8

#endif // SMO_OPTIONS_H
