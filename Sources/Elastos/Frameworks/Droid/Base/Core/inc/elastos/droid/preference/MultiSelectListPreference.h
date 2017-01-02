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

#ifndef __ELASTOS_DROID_PREFERENCE_MULTISELECTLISTPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_MULTISELECTLISTPREFERENCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/DialogPreference.h"
#include <elastos/utility/etl/Set.h>

using Elastos::Utility::ISet;
using Elastos::Utility::Etl::Set;
using Elastos::Droid::Content::IDialogInterfaceOnMultiChoiceClickListener;

namespace Elastos {
namespace Droid {
namespace Preference {

/**
 * A {@link Preference} that displays a list of entries as
* a dialog.
* <p>
* This preference will store a set of strings into the SharedPreferences.
* This set will contain one or more values from the
* {@link #setEntryValues(CharSequence[])} array.
 *
* @attr ref android.R.styleable#MultiSelectListPreference_entries
* @attr ref android.R.styleable#MultiSelectListPreference_entryValues
 */
class MultiSelectListPreference
    : public DialogPreference
    , public IMultiSelectListPreference
{
private:
    class MultiChoiceClickListener
        : public Object
        , public IDialogInterfaceOnMultiChoiceClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MultiChoiceClickListener(
            /* [in] */ MultiSelectListPreference* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which,
            /* [in] */ Boolean isChecked);

    private:
        MultiSelectListPreference* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    MultiSelectListPreference();

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

    CARAPI SetEntries(
        /* [in] */ ArrayOf<ICharSequence*>* entries);

    CARAPI SetEntries(
        /* [in] */ Int32 entriesResId);

    CARAPI GetEntries(
        /* [out, callee] */ ArrayOf<ICharSequence*>** entries);

    CARAPI SetEntryValues(
        /* [in] */ ArrayOf<ICharSequence*>* entryValues);

    CARAPI SetEntryValues(
        /* [in] */ Int32 entryValuesResId);

    CARAPI GetEntryValues(
        /* [out, callee] */ ArrayOf<ICharSequence*>** entryValues);

    CARAPI SetValues(
        /* [in] */ ISet* values);

    CARAPI GetValues(
        /* [out] */ ISet** values);

    CARAPI FindIndexOfValue(
        /* [in] */ const String& value,
        /* [out] */ Int32* index);

protected:
    CARAPI OnPrepareDialogBuilder(
        /* [in] */ IAlertDialogBuilder* builder);

    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

    CARAPI OnGetDefaultValue(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 index,
        /* [out] */ IInterface** value);

    CARAPI OnSetInitialValue(
        /* [in] */ Boolean restoreValue,
        /* [in] */ IInterface* defaultValue);

    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** state);

private:
    CARAPI_(AutoPtr< ArrayOf<Boolean> >) GetSelectedItems();

private:
    AutoPtr<ArrayOf<ICharSequence*> > mEntries;
    AutoPtr<ArrayOf<ICharSequence*> > mEntryValues;
    Set<String> mValues;
    Set<String> mNewValues;
    Boolean mPreferenceChanged;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_MULTISELECTLISTPREFERENCE_H__
