
#ifndef __ELASTOS_DROID_PREFERENCE_CPREFERENCESCREENSAVEDSTATE_H__
#define  __ELASTOS_DROID_PREFERENCE_CPREFERENCESCREENSAVEDSTATE_H__

#include "_Elastos_Droid_Preference_CPreferenceScreenSavedState.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

class PreferenceScreen;

CarClass(CPreferenceScreenSavedState)
    , public PreferenceBaseSavedState
{
public:
    CAR_OBJECT_DECL()
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_CPREFERENCESCREENSAVEDSTATE_H__
