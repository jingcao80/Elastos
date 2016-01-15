
#include "elastos/droid/preference/DialogPreferenceSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(DialogPreferenceSavedState, PreferenceBaseSavedState, IDialogPreferenceSavedState)

DialogPreferenceSavedState::DialogPreferenceSavedState()
    : mIsDialogShowing(FALSE)
{}

ECode DialogPreferenceSavedState::constructor()
{
    return PreferenceBaseSavedState::constructor();
}

ECode DialogPreferenceSavedState::constructor(
    /* [in] */ IParcelable* source)
{
    return PreferenceBaseSavedState::constructor(source);
}

ECode DialogPreferenceSavedState::GetIsDialogShowing(
    /* [out] */ Boolean* isShow)
{
    VALIDATE_NOT_NULL(isShow)
    *isShow = mIsDialogShowing;
    return NOERROR;
}

ECode DialogPreferenceSavedState::SetIsDialogShowing(
    /* [in] */ Boolean isShow)
{
    mIsDialogShowing = isShow;
    return NOERROR;
}

ECode DialogPreferenceSavedState::GetDialogBundle(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    *bundle = mDialogBundle;
    REFCOUNT_ADD(*bundle)
    return NOERROR;
}

ECode DialogPreferenceSavedState::SetDialogBundle(
    /* [in] */ IBundle* bundle)
{
    mDialogBundle = bundle;
    return NOERROR;
}

ECode DialogPreferenceSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PreferenceBaseSavedState::ReadFromParcel(source);
    source->ReadBoolean(&mIsDialogShowing);
    mDialogBundle = NULL;
    source->ReadInterfacePtr((Handle32*)(IBundle**)&mDialogBundle);
    return NOERROR;
}

ECode DialogPreferenceSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PreferenceBaseSavedState::WriteToParcel(dest);
    dest->WriteBoolean(mIsDialogShowing);
    dest->WriteInterfacePtr(mDialogBundle);
    return NOERROR;
}

ECode DialogPreferenceSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    return PreferenceBaseSavedState::GetSuperState(superState);
}


} // Preference
} // Droid
} // Elastos
