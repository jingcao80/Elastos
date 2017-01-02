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

#ifndef __ELASTOS_DROID_INTERNAL_PREFERENCE_YESNOPREFERENCE_H__
#define __ELASTOS_DROID_INTERNAL_PREFERENCE_YESNOPREFERENCE_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/preference/DialogPreference.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

using Elastos::Droid::Preference::DialogPreference;
using Elastos::Droid::Preference::PreferenceBaseSavedState;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Preference {

class YesNoPreference
    : public DialogPreference
    , public IYesNoPreference
{
public:
    class SavedState
        : public PreferenceBaseSavedState
        , public IYesNoPreferenceSavedState
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IParcelable* superState);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

    public:
        Boolean mWasPositiveResult;
    };

public:
    YesNoPreference();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Sets the value of this preference, and saves it to the persistent store
     * if required.
     *
     * @param value The value of the preference.
     */
    CARAPI SetValue(
        /* [in] */ Boolean value);

    /**
     * Gets the value of this preference.
     *
     * @return The value of the preference.
     */
    CARAPI GetValue(
        /* [out] */ Boolean* value);

    // @Override
    CARAPI ShouldDisableDependents(
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

    // @Override
    CARAPI OnGetDefaultValue(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 index,
        /* [out] */ IInterface** value);

    // @Override
    CARAPI OnSetInitialValue(
        /* [in] */ Boolean restorePersistedValue,
        /* [in] */ IInterface* defaultValue);

    // @Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** state);

    // @Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    Boolean mWasPositiveResult;
};

} // namespace Preference
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_PREFERENCE_YESNOPREFERENCE_H__
