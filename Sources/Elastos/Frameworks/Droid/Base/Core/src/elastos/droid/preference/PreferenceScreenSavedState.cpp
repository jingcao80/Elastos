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
    source->ReadInterfacePtr((HANDLE*)(IBundle**)&mDialogBundle);
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
