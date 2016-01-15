
#include "elastos/droid/preference/ListPreferenceSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(ListPreferenceSavedState, PreferenceBaseSavedState, IListPreferenceSavedState)

ListPreferenceSavedState::ListPreferenceSavedState()
{
}

ECode ListPreferenceSavedState::constructor()
{
    PreferenceBaseSavedState::constructor();
    return NOERROR;
}

ECode ListPreferenceSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return PreferenceBaseSavedState::constructor(superState);
}

ECode ListPreferenceSavedState::GetValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mValue;
    return NOERROR;
}

ECode ListPreferenceSavedState::SetValue(
    /* [in] */ String* value)
{
    mValue = *value;
    return NOERROR;
}

ECode ListPreferenceSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PreferenceBaseSavedState::ReadFromParcel(source);
    source->ReadString(&mValue);
    return NOERROR;
}

ECode ListPreferenceSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PreferenceBaseSavedState::WriteToParcel(dest);
    dest->WriteString(mValue);
    return NOERROR;
}

ECode ListPreferenceSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    return PreferenceBaseSavedState::GetSuperState(superState);
}

} // Preference
} // Droid
} // Elastos
