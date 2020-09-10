//
// Text constants
//

#include "../smo_alpha_begin.h"

static const u8 sOptionsTitle[]                 = { O P T I O N S                                           0xFF };
static const u8 sCameraTitle[]                  = { C A M E R A                                             0xFF };
static const u8 sControlsTitle[]                = { C O N T R O L S                                         0xFF };
static const u8 sDisplayTitle[]                 = { D I S P L A Y                                           0xFF };
static const u8 sAudioTitle[]                   = { S O U N D                                               0xFF };
static const u8 sSmoTitle[]                     = { S U P E R _ M A R I O _ O D Y S S E Y                   0xFF };
static const u8 sSmoTimeTrialsTitle[]           = { T I M E _ T R I A L S                                   0xFF };
static const u8 sCheatsTitle[]                  = { C H E A T S                                             0xFF };
static const u8 sGoBackToMainMenu[]             = { G O _ B A C K _ T O _ M A I N _ M E N U                 0xFF };
static const u8 sExitGame[]                     = { E X I T _ G A M E                                       0xFF };

static const u8 sXButton[]                      = { X _ B u t t o n                                         0xFF };
static const u8 sSmoMario[]                     = { M a r i o APO s _ M o v e s e t                         0xFF };
static const u8 sSmoCappy[]                     = { C a p p y                                               0xFF };
static const u8 sSmoCappyDisabled[]             = { D i s a b l e d                                         0xFF };
static const u8 sSmoCappyNoCapture[]            = { N o _ C a p t u r e                                     0xFF };
static const u8 sSmoCappyCapture[]              = { C a p t u r e                                           0xFF };
static const u8 sSmoHealth[]                    = { H e a l t h _ S y s t e m                               0xFF };
static const u8 sSmoHealthDisabled[]            = { D i s a b l e d                                         0xFF };
static const u8 sSmoHealth3Health[]             = { 3,SLH H e a l t h                                       0xFF };
static const u8 sSmoHealth6Health[]             = { 6,SLH H e a l t h                                       0xFF };
static const u8 sSmoTimeTrials[]                = { T i m e _ T r i a l s                                   0xFF };
static const u8 sSmoTimeTrialsGhost[]           = { T i m e _ T r i a l _ G h o s t                         0xFF };
static const u8 sSmoTimeTrialsGhostDisplay[]    = { S h o w                                                 0xFF };
static const u8 sSmoTimeTrialsGhostHide[]       = { H i d e                                                 0xFF };
static const u8 sSmoTimeTrialsMainCourseStar[]  = { M a i n _ C o u r s e s _ G h o s t                     0xFF };
static const u8 sSmoTimeTrialsSelectedActStar[] = { S e l e c t e d _ A c t _ S t a r                       0xFF };
static const u8 sSmoTimeTrials100CoinsStar[]    = { 1,0,0,_ C o i n s _ S t a r                             0xFF };
static const u8 sSmoTimeTrialsBowserStar[]      = { B o w s e r _ C o u r s e s _ G h o s t                 0xFF };
static const u8 sSmoTimeTrialsBowserFight[]     = { B o w s e r _ F i g h t                                 0xFF };
static const u8 sSmoTimeTrialsRedCoinsStar[]    = { R e d _ C o i n s _ S t a r                             0xFF };
static const u8 sSmoTimeTrialsPSSStar[]         = { P S S _ G h o s t                                       0xFF };
static const u8 sSmoTimeTrialsPSSStar1[]        = { S t a r _ 1,                                            0xFF };
static const u8 sSmoTimeTrialsPSSStar2[]        = { S t a r _ 2,                                            0xFF };
static const u8 sSmoUnlimitedCappyJumps[]       = { U n l i m i t e d _ C a p p y _ J u m p s               0xFF };
DEBUG_ONLY(static const u8 sSmoDebug[]          = { D e b u g _ M o d e                                     0xFF };)

#include "../smo_alpha_end.h"

static const u8 *sSmoCappyChoices[] = {
    sSmoCappyDisabled,
    sSmoCappyNoCapture,
    sSmoCappyCapture
};

static const u8 *sSmoHealthChoices[] = {
    sSmoHealthDisabled,
    sSmoHealth3Health,
    sSmoHealth6Health
};

static const u8 *sSmoTTGhostChoices[] = {
    sSmoTimeTrialsGhostHide,
    sSmoTimeTrialsGhostDisplay,
};

static const u8 *sSmoTTMainCourseChoices[] = {
    sSmoTimeTrialsSelectedActStar,
    sSmoTimeTrials100CoinsStar,
};

static const u8 *sSmoTTBowserChoices[] = {
    sSmoTimeTrialsBowserFight,
    sSmoTimeTrialsRedCoinsStar,
};

static const u8 *sSmoTTPSSChoices[] = {
    sSmoTimeTrialsPSSStar1,
    sSmoTimeTrialsPSSStar2,
};

//
// Options
//

static struct Option sOptsSmo[] = {
    DEF_OPT_TOGGLE(sSmoMario, (bool *) &gSmoMario),
    DEF_OPT_CHOICE(sSmoCappy, &gSmoCappy, sSmoCappyChoices),
    DEF_OPT_CHOICE(sSmoHealth, &gSmoHealth, sSmoHealthChoices),
    DEF_OPT_TOGGLE(sSmoUnlimitedCappyJumps, (bool *) &gSmoUnlimitedCappyJumps),
    DEBUG_ONLY(DEF_OPT_TOGGLE(sSmoDebug, (bool *) &gSmoDebug),)
};
static struct SubMenu sMenuSmo = DEF_SUBMENU(sSmoTitle, sOptsSmo);

static struct Option sOptsSmoTT[] = {
    DEF_OPT_TOGGLE(sSmoTimeTrials, (bool *) &gSmoTimeTrials),
    DEF_OPT_CHOICE(sSmoTimeTrialsGhost, &gSmoTimeTrialsGhost, sSmoTTGhostChoices),
    DEF_OPT_CHOICE(sSmoTimeTrialsMainCourseStar, &gSmoTimeTrialsMainCourseStar, sSmoTTMainCourseChoices),
    DEF_OPT_CHOICE(sSmoTimeTrialsBowserStar, &gSmoTimeTrialsBowserStar, sSmoTTBowserChoices),
    DEF_OPT_CHOICE(sSmoTimeTrialsPSSStar, &gSmoTimeTrialsPSSStar, sSmoTTPSSChoices),
};
static struct SubMenu sMenuSmoTT = DEF_SUBMENU(sSmoTimeTrialsTitle, sOptsSmoTT);

//
// Buttons
//

static void optmenu_act_leave(UNUSED struct Option *self, s32 arg) {
    if (!arg) {
        optmenu_toggle();
        unpause_game();
        DEBUG_ONLY(smo_debug_reset();)
        smo_hud_reset();
        smo_health_reset();
        smo_options_reset();
        smo_object_reset();
        fade_into_special_warp(-2, 0);
    }
}

//
// Submenu definitions
//

#ifdef BETTERCAMERA
static struct SubMenu menuCamera   = DEF_SUBMENU(sCameraTitle, optsCamera);
#endif
static struct SubMenu menuControls = DEF_SUBMENU(sControlsTitle, optsControls);
static struct SubMenu menuVideo    = DEF_SUBMENU(sDisplayTitle, optsVideo);
static struct SubMenu menuAudio    = DEF_SUBMENU(sAudioTitle, optsAudio);
static struct SubMenu menuCheats   = DEF_SUBMENU(sCheatsTitle, optsCheats);

//
// Main options menu definition
//

static struct Option optsMain[] = {
#ifdef BETTERCAMERA
    DEF_OPT_SUBMENU(sCameraTitle,         &menuCamera),
#endif
    DEF_OPT_SUBMENU( sControlsTitle,      &menuControls),
    DEF_OPT_SUBMENU( sDisplayTitle,       &menuVideo),
    DEF_OPT_SUBMENU( sAudioTitle,         &menuAudio),
    DEF_OPT_SUBMENU( sSmoTitle,           &sMenuSmo),
    DEF_OPT_SUBMENU( sSmoTimeTrialsTitle, &sMenuSmoTT),
    DEF_OPT_SUBMENU( sCheatsTitle,        &menuCheats),
    DEF_OPT_BUTTON(  sGoBackToMainMenu,   optmenu_act_leave),
    DEF_OPT_BUTTON(  sExitGame,           optmenu_act_exit),
};

static struct SubMenu menuMain = DEF_SUBMENU(sOptionsTitle, optsMain);
