
#ifndef __ELASTOS_DROID_PREFERENCE_CMULTISELECTLISTPREFERENCESAVEDSTATE_H__
#define  __ELASTOS_DROID_PREFERENCE_CMULTISELECTLISTPREFERENCESAVEDSTATE_H__

#include "_Elastos_Droid_Preference_CMultiSelectListPreferenceSavedState.h"
#include "elastos/droid/preference/MultiSelectListPreferenceSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CMultiSelectListPreferenceSavedState)
    , public MultiSelectListPreferenceSavedState
{
public:
    CAR_OBJECT_DECL()
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_CMULTISELECTLISTPREFERENCESAVEDSTATE_H__
