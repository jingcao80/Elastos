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

#ifndef __ELASTOS_DROID_PREFERENCE_LISTPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_LISTPREFERENCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/DialogPreference.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::R;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Preference {

/**
 * A {@link Preference} that displays a list of entries as
* a dialog.
* <p>
* This preference will store a string into the SharedPreferences. This string will be the value
* from the {@link #setEntryValues(CharSequence[])} array.
 *
* @attr ref android.R.styleable#ListPreference_entries
* @attr ref android.R.styleable#ListPreference_entryValues
 */
class ECO_PUBLIC ListPreference
    : public DialogPreference
    , public IListPreference
{
private:
    class DialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        DialogInterfaceOnClickListener(
            /* [in] */ ListPreference* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        ListPreference* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ListPreference();

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

    CARAPI SetValue(
        /* [in] */ const String& value);

    //@Override
    CARAPI GetSummary(
        /* [out] */ ICharSequence** summary);

    //@Override
    CARAPI SetSummary(
        /* [in] */ ICharSequence* summary);

    CARAPI SetValueIndex(
        /* [in] */ Int32 index);

    CARAPI GetValue(
        /* [out] */ String* value);

    CARAPI GetEntry(
        /* [out] */ ICharSequence** entry);

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

    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    CARAPI_(Int32) GetValueIndex();

private:
    AutoPtr< ArrayOf<ICharSequence*> > mEntries;
    AutoPtr< ArrayOf<ICharSequence*> >  mEntryValues;
    String mValue;
    String mSummary;
    Int32 mClickedDialogEntryIndex;
    Boolean mValueSet;
};

}
}
}
#endif // __ELASTOS_DROID_PREFERENCE_LISTPREFERENCE_H__