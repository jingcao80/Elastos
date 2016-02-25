
#ifndef __ELASTOS_DROID_PREFERENCE_CVOLUMEPREFERENCESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_CVOLUMEPREFERENCESAVEDSTATE_H__

#include "_Elastos_Droid_Preference_CVolumePreferenceSavedState.h"
#include "elastos/droid/preference/VolumePreferenceSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CVolumePreferenceSavedState)
    , public VolumePreferenceSavedState
{
public:
    CAR_OBJECT_DECL()
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_CVOLUMEPREFERENCESAVEDSTATE_H__
