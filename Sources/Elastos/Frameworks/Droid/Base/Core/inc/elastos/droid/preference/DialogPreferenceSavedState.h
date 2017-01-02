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

#ifndef __ELASTOS_DROID_PREFERENCE_DIALOGPREFERENCESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_DIALOGPREFERENCESAVEDSTATE_H__

#include "Elastos.Droid.Os.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Preference {

 class DialogPreferenceSavedState
    : public PreferenceBaseSavedState
    , public IDialogPreferenceSavedState
{
public:
    CAR_INTERFACE_DECL()

    DialogPreferenceSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* superState);

    CARAPI GetIsDialogShowing(
        /* [out] */ Boolean* isShow);

    CARAPI SetIsDialogShowing(
        /* [in] */ Boolean isShow);

    CARAPI GetDialogBundle(
        /* [out] */ IBundle** bundle);

    CARAPI SetDialogBundle(
        /* [in] */ IBundle* bundle);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

private:
    Boolean mIsDialogShowing;
    AutoPtr<IBundle> mDialogBundle;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_DIALOGPREFERENCESAVEDSTATE_H__
