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

#ifndef __ELASTOS_DROID_PREFERENCE_VOLUMEPREFERENCESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_VOLUMEPREFERENCESAVEDSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

class VolumePreferenceSavedState
    : public PreferenceBaseSavedState
    , public IVolumePreferenceSavedState
{
public:
    CAR_INTERFACE_DECL()

    VolumePreferenceSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* superState);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

    CARAPI GetVolumeStore(
        /* [out] */ IVolumePreferenceVolumeStore** vs);

    CARAPI SetVolumeStore(
        /* [in] */ IVolumePreferenceVolumeStore* vs);
private:
    AutoPtr<IVolumePreferenceVolumeStore> mVolumeStore;
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_VOLUMEPREFERENCESAVEDSTATE_H__
