
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(PreferenceBaseSavedState, AbsSavedState, IPreferenceBaseSavedState)

PreferenceBaseSavedState::PreferenceBaseSavedState()
{
}

ECode PreferenceBaseSavedState::constructor()
{
    return NOERROR;
}

ECode PreferenceBaseSavedState::constructor(
    /* [in] */ IParcelable* source)
{
    return AbsSavedState::constructor(source);
}

ECode PreferenceBaseSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return AbsSavedState::ReadFromParcel(source);
}

ECode PreferenceBaseSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return AbsSavedState::WriteToParcel(dest);
}

ECode PreferenceBaseSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    VALIDATE_NOT_NULL(superState)
    return AbsSavedState::GetSuperState(superState);
}

} // Preference
} // Droid
} // Elastos
