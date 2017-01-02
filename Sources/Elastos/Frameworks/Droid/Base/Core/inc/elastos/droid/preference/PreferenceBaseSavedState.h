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

#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCEBASESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCEBASESAVEDSTATE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/view/AbsSavedState.h"

using Elastos::Droid::Preference::IPreferenceBaseSavedState;
using Elastos::Droid::View::AbsSavedState;

namespace Elastos {
namespace Droid {
namespace Preference {

class PreferenceBaseSavedState
    : public AbsSavedState
    , public IPreferenceBaseSavedState
{
public:
    CAR_INTERFACE_DECL()

    PreferenceBaseSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* source);

    virtual CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    virtual CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    virtual CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);
};

} // Preference
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_PREFERENCE_PREFERENCEBASESAVEDSTATE_H__
