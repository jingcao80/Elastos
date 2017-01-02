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

#include "elastos/droid/preference/VolumePreferenceSavedState.h"
#include "elastos/droid/preference/CVolumePreferenceVolumnStore.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(VolumePreferenceSavedState, PreferenceBaseSavedState, IVolumePreferenceSavedState)

VolumePreferenceSavedState::VolumePreferenceSavedState()
{
    CVolumePreferenceVolumnStore::New((IVolumePreferenceVolumeStore**)&mVolumeStore);
}

ECode VolumePreferenceSavedState::constructor()
{
    return PreferenceBaseSavedState::constructor();
}

ECode VolumePreferenceSavedState::constructor(
    /* [in] */ IParcelable* source)
{
    return PreferenceBaseSavedState::constructor(source);
}

ECode VolumePreferenceSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PreferenceBaseSavedState::ReadFromParcel(source);
    Int32 volumn, originalVolume;
    source->ReadInt32(&volumn);
    source->ReadInt32(&originalVolume);
    mVolumeStore->SetVolume(volumn);
    mVolumeStore->SetOriginalVolume(originalVolume);
    return NOERROR;
}

ECode VolumePreferenceSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PreferenceBaseSavedState::WriteToParcel(dest);
    Int32 volumn, originalVolume;
    mVolumeStore->GetVolume(&volumn);
    mVolumeStore->GetOriginalVolume(&originalVolume);
    dest->WriteInt32(volumn);
    dest->WriteInt32(originalVolume);
    return NOERROR;
}

ECode VolumePreferenceSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    return PreferenceBaseSavedState::GetSuperState(superState);
}

ECode VolumePreferenceSavedState::GetVolumeStore(
     /* [out] */ IVolumePreferenceVolumeStore** vs)
{
    VALIDATE_NOT_NULL(vs)
    *vs = mVolumeStore;
    REFCOUNT_ADD(*vs)
    return NOERROR;
}

ECode VolumePreferenceSavedState::SetVolumeStore(
     /* [in] */ IVolumePreferenceVolumeStore* vs)
{
    mVolumeStore = vs;
    return NOERROR;
}

} // Preference
} // Droid
} // Elastos
