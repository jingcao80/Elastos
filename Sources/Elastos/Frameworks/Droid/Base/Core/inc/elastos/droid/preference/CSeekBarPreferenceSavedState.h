

#ifndef __ELASTOS_DROID_PREFERENCE_CSEEKBARPREFERENCESAVEDSATAE_H__
#define __ELASTOS_DROID_PREFERENCE_CSEEKBARPREFERENCESAVEDSATAE_H__

#include "_Elastos_Droid_Preference_CSeekBarPreferenceSavedState.h"
#include "elastos/droid/preference/SeekBarPreferenceSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CSeekBarPreferenceSavedState)
    , public SeekBarPreferenceSavedState
{
public:
    CAR_OBJECT_DECL()
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_CSEEKBARPREFERENCESAVEDSATAE_H__
