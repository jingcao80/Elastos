
#include "Elastos.Droid.Os.h"
#include "elastos/droid/preference/PreferenceScreenSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(PreferenceScreenSavedState, PreferenceBaseSavedState, IPreferenceBaseSavedState)

PreferenceScreenSavedState::PreferenceScreenSavedState()
    : mIsDialogShowing(FALSE)
{}

ECode PreferenceScreenSavedState::constructor()
{
    return NOERROR;
}

ECode PreferenceScreenSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return PreferenceBaseSavedState::constructor(superState);
}

ECode PreferenceScreenSavedState::IsDialogShowing(
    /* [out] */ Boolean* isDialogShowing)
{
    VALIDATE_NOT_NULL(isDialogShowing)
    *isDialogShowing = mIsDialogShowing;
    return NOERROR;
}

ECode PreferenceScreenSavedState::SetDialogShowing(
    /* [in] */ Boolean isDialogShowing)
{
    mIsDialogShowing = isDialogShowing;
    return NOERROR;
}

ECode PreferenceScreenSavedState::SetDialogBundle(
    /* [in] */ IBundle* b)
{
    mDialogBundle = b;
    return NOERROR;
}

ECode PreferenceScreenSavedState::GetDialogBundle(
    /* [out] */ IBundle** b)
{
    VALIDATE_NOT_NULL(b)
    *b = mDialogBundle;
    REFCOUNT_ADD(*b)
    return NOERROR;
}

ECode PreferenceScreenSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PreferenceBaseSavedState::WriteToParcel(dest);
    dest->WriteBoolean(mIsDialogShowing);
    dest->WriteInterfacePtr(mDialogBundle);
    return NOERROR;
}

ECode PreferenceScreenSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PreferenceBaseSavedState::ReadFromParcel(source);
    source->ReadBoolean(&mIsDialogShowing);
    mDialogBundle = NULL;
    source->ReadInterfacePtr((Handle32*)(IBundle**)&mDialogBundle);
    return NOERROR;
}

ECode PreferenceScreenSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    return PreferenceBaseSavedState::GetSuperState(superState);
}

} // Preference
} // Droid
} // Elastos
