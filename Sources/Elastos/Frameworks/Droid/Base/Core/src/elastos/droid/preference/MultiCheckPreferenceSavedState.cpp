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

#include "elastos/droid/preference/MultiCheckPreferenceSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(MultiCheckPreferenceSavedState, PreferenceBaseSavedState, IMultiCheckPreferenceSavedState)

MultiCheckPreferenceSavedState::MultiCheckPreferenceSavedState()
{
}

ECode MultiCheckPreferenceSavedState::constructor()
{
    return PreferenceBaseSavedState::constructor();
}

ECode MultiCheckPreferenceSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return PreferenceBaseSavedState::constructor(superState);
}

ECode MultiCheckPreferenceSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PreferenceBaseSavedState::ReadFromParcel(source);
    mValues = NULL;
    source->ReadArrayOf((Handle32*)(&mValues));
    return NOERROR;
}

ECode MultiCheckPreferenceSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PreferenceBaseSavedState::WriteToParcel(dest);
    dest->WriteArrayOf((Handle32)mValues.Get());
    return NOERROR;
}

ECode MultiCheckPreferenceSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    return PreferenceBaseSavedState::GetSuperState(superState);
}

ECode MultiCheckPreferenceSavedState::GetValues(
    /* [out, callee] */ ArrayOf<Boolean>** values)
{
    VALIDATE_NOT_NULL(values)
    *values = mValues;
    REFCOUNT_ADD(*values)
    return NOERROR;
}

ECode MultiCheckPreferenceSavedState::SetValues(
    /* [in] */ ArrayOf<Boolean>* values)
{
    mValues = values;
    return NOERROR;
}

} // Preference
} // Droid
} // Elastos
