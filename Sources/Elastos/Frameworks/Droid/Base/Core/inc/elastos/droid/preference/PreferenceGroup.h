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

#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCEGROUP_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCEGROUP_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/preference/Preference.h"

using Elastos::Droid::Preference::IGenericInflaterParent;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Preference {

class ECO_PUBLIC PreferenceGroup
    : public Preference
    , public IGenericInflaterParent
    , public IPreferenceGroup
{
public:
    CAR_INTERFACE_DECL()

    PreferenceGroup();

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

    virtual CARAPI SetOrderingAsAdded(
        /* [in] */  Boolean orderingAsAdded);

    virtual CARAPI IsOrderingAsAdded(
        /* [out] */ Boolean* result);

    virtual CARAPI AddItemFromInflater(
        /* [in] */  IInterface* child);

    virtual CARAPI GetPreferenceCount(
        /* [out] */  Int32* count);

    virtual CARAPI GetPreference(
        /* [in] */  Int32 index,
        /* [out] */  IPreference** preference);

    virtual CARAPI AddPreference(
        /* [in] */  IPreference* preference,
        /* [out] */  Boolean* result);

    virtual CARAPI RemovePreference(
        /* [in] */  IPreference* preference,
        /* [out] */  Boolean* result);

    virtual CARAPI RemoveAll();

    virtual CARAPI FindPreference(
        /* [in] */  ICharSequence* key,
        /* [out] */  IPreference** preference);

    virtual CARAPI NotifyDependencyChange(
        /* [in] */ Boolean disableDependents);

protected:
    virtual CARAPI OnPrepareAddPreference(
        /* [in] */  IPreference* preference,
        /* [out] */  Boolean* result);

    virtual CARAPI IsOnSameScreenAsChildren(
        /* [out] */  Boolean* isOnSameScreenAsChildren);

    virtual CARAPI OnAttachedToActivity();

    virtual CARAPI OnPrepareForRemoval();

    virtual CARAPI SortPreferences();

    virtual CARAPI DispatchSaveInstanceState(
        /* [in] */ IBundle* container);

    virtual CARAPI DispatchRestoreInstanceState(
        /* [in] */ IBundle* container);

private:
    ECO_LOCAL CARAPI_(Boolean) RemovePreferenceInt(
        /* [in] */  IPreference* preference);

private:
    AutoPtr<IList> mPreferenceList;

    Boolean mOrderingAsAdded;

    Int32 mCurrentPreferenceOrder;

    Boolean mAttachedToActivity;
};

} // Preference
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_PREFERENCE_PREFERENCEGROUP_H__
