
#ifndef __ELASTOS_DROID_PREFERENCE_CMULTICHECKPREFERENCESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_CMULTICHECKPREFERENCESAVEDSTATE_H__

#include "_Elastos_Droid_Preference_CMultiCheckPreferenceSavedState.h"
#include "elastos/droid/preference/MultiCheckPreferenceSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CMultiCheckPreferenceSavedState)
    , public MultiCheckPreferenceSavedState
{
public:
    CAR_OBJECT_DECL()
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_CMULTICHECKPREFERENCESAVEDSTATE_H__
