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
