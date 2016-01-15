
#include "elastos/droid/preference/MultiSelectListPreferenceSavedState.h"

using Elastos::Utility::CHashSet;

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(MultiSelectListPreferenceSavedState, PreferenceBaseSavedState, IMultiSelectListPreferenceSavedState)

MultiSelectListPreferenceSavedState::MultiSelectListPreferenceSavedState()
{
}

ECode MultiSelectListPreferenceSavedState::constructor()
{
    return PreferenceBaseSavedState::constructor();
}

ECode MultiSelectListPreferenceSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return PreferenceBaseSavedState::constructor(superState);
}

ECode MultiSelectListPreferenceSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PreferenceBaseSavedState::ReadFromParcel(source);
    mValues = NULL;
    source->ReadInterfacePtr((Handle32*)(ISet**)&mValues);
    return NOERROR;
}

ECode MultiSelectListPreferenceSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PreferenceBaseSavedState::WriteToParcel(dest);
    dest->WriteInterfacePtr(mValues);
    return NOERROR;
}

ECode MultiSelectListPreferenceSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    return PreferenceBaseSavedState::GetSuperState(superState);
}

ECode MultiSelectListPreferenceSavedState::GetValues(
    /* [out] */ ISet** values)
{
    VALIDATE_NOT_NULL(values)
    *values = mValues;
    REFCOUNT_ADD(*values)
    return NOERROR;
}

ECode MultiSelectListPreferenceSavedState::SetValues(
    /* [in] */ ISet* values)
{
    mValues = values;
    return NOERROR;
}

} // Preference
} // Droid
} // Elastos
