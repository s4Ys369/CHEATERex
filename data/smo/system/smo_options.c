#include "../smo_c_includes.h"

//
// Options
//

u8 gSmoMario = 1; // Enabled by default
u32 gSmoCappy = 2; // Enabled by default
u32 gSmoHealth = 1; // Enabled by default
u8 gSmoTimeTrials = 1; // Enabled by default
u32 gSmoTimeTrialsGhost = 1;
u32 gSmoTimeTrialsMainCourseStar = 0;
u32 gSmoTimeTrialsBowserStar = 0;
u32 gSmoTimeTrialsPSSStar = 0;
u8 gSmoUnlimitedCappyJumps = 0;

void smo_options_reset() {
    gSmoMario = 1;
    gSmoCappy = 2;
    gSmoHealth = 1;
    gSmoTimeTrials = 1;
    gSmoTimeTrialsGhost = 1;
    gSmoTimeTrialsMainCourseStar = 0;
    gSmoTimeTrialsBowserStar = 0;
    gSmoTimeTrialsPSSStar = 0;
    gSmoUnlimitedCappyJumps = 0;
}


