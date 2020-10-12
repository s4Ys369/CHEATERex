#include "dynamic_options.h"

//
// DynOS Init
//

extern "C" { extern void dynos_init(void); }
class DynosInitialization { public: DynosInitialization() { dynos_init(); } };
static DynosInitialization sDynosInitialization;

//
// DynOS Actions

DYNOS_DEFINE_ACTION(dynos_return_to_main_menu);
DYNOS_DEFINE_ACTION(dynos_warp_to_level);
DYNOS_DEFINE_ACTION(dynos_restart_level);
