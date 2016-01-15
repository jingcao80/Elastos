
#ifndef __ELASTOS_DROID_INTERNAL_PREFERENCE_CYESNOPREFERENCESAVESTATE_H__
#define __ELASTOS_DROID_INTERNAL_PREFERENCE_CYESNOPREFERENCESAVESTATE_H__

#include "_Elastos_Droid_Internal_Preference_CYesNoPreferenceSavedState.h"
#include "elastos/droid/internal/preference/YesNoPreference.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Preference {

CarClass(CYesNoPreferenceSavedState)
    , public YesNoPreference::SavedState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Preference
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_PREFERENCE_CYESNOPREFERENCESAVESTATE_H__
