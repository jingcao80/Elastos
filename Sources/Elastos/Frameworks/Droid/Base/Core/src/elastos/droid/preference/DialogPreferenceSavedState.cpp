//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    source->ReadInterfacePtr((HANDLE*)(IBundle**)&mDialogBundle);
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
