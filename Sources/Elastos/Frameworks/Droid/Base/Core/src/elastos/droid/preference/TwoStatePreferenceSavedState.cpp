
#include "elastos/droid/preference/TwoStatePreferenceSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(TwoStatePreferenceSavedState, PreferenceBaseSavedState, ITwoStatePreferenceSavedState)

TwoStatePreferenceSavedState::TwoStatePreferenceSavedState()
    : mChecked(FALSE)
{}

ECode TwoStatePreferenceSavedState::constructor()
{
    PreferenceBaseSavedState::constructor();
    return NOERROR;
}

ECode TwoStatePreferenceSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return PreferenceBaseSavedState::constructor(superState);
}

ECode TwoStatePreferenceSavedState::IsChecked(
    /* [out] */ Boolean* isChecked)
{
    VALIDATE_NOT_NULL(isChecked)
    *isChecked = mChecked;
    return NOERROR;
}

ECode TwoStatePreferenceSavedState::SetChecked(
    /* [in] */ Boolean checked)
{
    mChecked = checked;
    return NOERROR;
}

ECode TwoStatePreferenceSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PreferenceBaseSavedState::ReadFromParcel(source);
    source->ReadBoolean(&mChecked);
    return NOERROR;
}

ECode TwoStatePreferenceSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PreferenceBaseSavedState::WriteToParcel(dest);
    dest->WriteBoolean(mChecked);
    return NOERROR;
}

ECode TwoStatePreferenceSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    return PreferenceBaseSavedState::GetSuperState(superState);
}

}
}
}

