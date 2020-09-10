#ifndef SMO_OPTIONS_H
#define SMO_OPTIONS_H

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

#endif // SMO_OPTIONS_H
