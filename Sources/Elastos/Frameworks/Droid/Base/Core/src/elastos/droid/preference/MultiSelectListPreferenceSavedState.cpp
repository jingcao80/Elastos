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
