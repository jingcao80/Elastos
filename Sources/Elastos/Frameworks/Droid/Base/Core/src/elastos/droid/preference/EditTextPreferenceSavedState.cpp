
#include "elastos/droid/preference/EditTextPreferenceSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(EditTextPreferenceSavedState, PreferenceBaseSavedState, IEditTextPreferenceSavedState)

EditTextPreferenceSavedState::EditTextPreferenceSavedState()
{
}

ECode EditTextPreferenceSavedState::constructor()
{
    return PreferenceBaseSavedState::constructor();
}

ECode EditTextPreferenceSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return PreferenceBaseSavedState::constructor(superState);
}

ECode EditTextPreferenceSavedState::GetText(
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text)
    *text = mText;
    return NOERROR;
}

ECode EditTextPreferenceSavedState::SetText(
    /* [in] */ String* text)
{
    mText = *text;
    return NOERROR;
}

ECode EditTextPreferenceSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PreferenceBaseSavedState::ReadFromParcel(source);
    source->ReadString(&mText);
    return NOERROR;
}

ECode EditTextPreferenceSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PreferenceBaseSavedState::WriteToParcel(dest);
    dest->WriteString(mText);
    return NOERROR;
}

ECode EditTextPreferenceSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    return PreferenceBaseSavedState::GetSuperState(superState);
}

} // Preference
} // Droid
} // Elastos
