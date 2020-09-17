#ifndef SMO_SGI_H
#define SMO_SGI_H

// Check if SGI is installed, based on macros defined in SGI files
// May be modified in the future
#include "model_ids.h"
#if defined(MODEL_PLAYER) && defined(MODEL_LUIGIS_CAP) && defined(MODEL_LUIGIS_METAL_CAP) && defined(MODEL_LUIGIS_WING_CAP) && defined(MODEL_LUIGIS_WINGED_METAL_CAP)
#define SMO_SGI
<<<<<<< HEAD
#define LUIGIS_CAPPY_SCALE_MULTIPLIER 1.25f /* Luigi's cap is slightly smaller than Mario's */
=======
#define LUIGIS_CAPPY_SCALE_MULTIPLIER 1.2f /* Luigi's cap is slightly smaller than Mario's */
>>>>>>> ed6bf96ae1f732967e9f72ea66c102467e719cb8
#endif

#endif // SMO_SGI_H