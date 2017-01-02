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

#include "elastos/droid/internal/preference/YesNoPreference.h"
#include "elastos/droid/internal/preference/CYesNoPreferenceSavedState.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::IBoolean;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Preference {

CAR_INTERFACE_IMPL(YesNoPreference::SavedState, PreferenceBaseSavedState, IYesNoPreferenceSavedState);

ECode YesNoPreference::SavedState::constructor()
{
    return NOERROR;
}

ECode YesNoPreference::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return PreferenceBaseSavedState::constructor(superState);
}

ECode YesNoPreference::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PreferenceBaseSavedState::ReadFromParcel(source);
    source->ReadBoolean(&mWasPositiveResult);
    return NOERROR;
}

ECode YesNoPreference::SavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PreferenceBaseSavedState::WriteToParcel(dest);
    dest->WriteBoolean(mWasPositiveResult);
    return NOERROR;
}

CAR_INTERFACE_IMPL(YesNoPreference, DialogPreference, IYesNoPreference);

YesNoPreference::YesNoPreference()
    : mWasPositiveResult(FALSE)
{
}

ECode YesNoPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return DialogPreference::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode YesNoPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode YesNoPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::yesNoPreferenceStyle);
}

ECode YesNoPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode YesNoPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    FAIL_RETURN(DialogPreference::OnDialogClosed(positiveResult));

    Boolean res;
    if (CallChangeListener(CoreUtils::Convert(positiveResult), &res), res) {
        SetValue(positiveResult);
    }
    return NOERROR;
}

ECode YesNoPreference::SetValue(
    /* [in] */ Boolean value)
{
    mWasPositiveResult = value;

    Boolean res;
    PersistBoolean(value, &res);

    NotifyDependencyChange(!value);
    return NOERROR;
}

ECode YesNoPreference::GetValue(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mWasPositiveResult;
    return NOERROR;
}

ECode YesNoPreference::OnGetDefaultValue(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 index,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    Boolean b;
    a->GetBoolean(index, FALSE, &b);
    AutoPtr<IBoolean> temp = CoreUtils::Convert(b);
    *value = temp;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode YesNoPreference::OnSetInitialValue(
    /* [in] */ Boolean restorePersistedValue,
    /* [in] */ IInterface* defaultValue)
{
    Boolean value;
    if (restorePersistedValue)
        GetPersistedBoolean(mWasPositiveResult, &value);
    else
        IBoolean::Probe(defaultValue)->GetValue(&value);
    return SetValue(value);
}

ECode YesNoPreference::ShouldDisableDependents(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean res;
    *result = !mWasPositiveResult ||
        (DialogPreference::ShouldDisableDependents(&res), res);
    return NOERROR;
}

ECode YesNoPreference::OnSaveInstanceState(
    /* [out] */ IParcelable** state)
{
    AutoPtr<IParcelable> superState;
    FAIL_RETURN(DialogPreference::OnSaveInstanceState((IParcelable**)&superState));
    Boolean isPersistent;
    IsPersistent(&isPersistent);
    if (isPersistent) {
        // No need to save instance state since it's persistent
        *state = superState;
        REFCOUNT_ADD(*state)
        return NOERROR;
    }

    AutoPtr<CYesNoPreferenceSavedState> myState;
    CYesNoPreferenceSavedState::NewByFriend(superState, (CYesNoPreferenceSavedState**)&myState);
    GetValue(&myState->mWasPositiveResult);
    *state = myState;
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode YesNoPreference::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (!IYesNoPreferenceSavedState::Probe(state)) {
        // Didn't save state for us in onSaveInstanceState
        return DialogPreference::OnRestoreInstanceState(state);
    }

    SavedState* myState = (SavedState*)IYesNoPreferenceSavedState::Probe(state);
    AutoPtr<IParcelable> superState;
    myState->GetSuperState((IParcelable**)&superState);
    FAIL_RETURN(DialogPreference::OnRestoreInstanceState(superState))
    return SetValue(myState->mWasPositiveResult);
}

} // namespace Preference
} // namespace Internal
} // namespace Droid
} // namespace Elastos
