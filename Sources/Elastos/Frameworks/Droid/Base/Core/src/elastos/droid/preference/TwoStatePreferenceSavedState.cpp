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

