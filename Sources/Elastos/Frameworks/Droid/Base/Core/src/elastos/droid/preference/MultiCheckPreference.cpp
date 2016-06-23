#include "elastos/droid/preference/MultiCheckPreference.h"
#include "elastos/droid/preference/CMultiCheckPreferenceSavedState.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::EIID_IDialogInterfaceOnMultiChoiceClickListener;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IAbsSavedState;
using Elastos::Droid::R;
using Elastos::Core::CArrayOf;
using Elastos::Core::CBoolean;
using Elastos::Core::CString;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::IArrayOf;
using Elastos::Core::IBoolean;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Preference {

//===============================
// MultiCheckPreference::MultiChoiceClickListener
//===============================

CAR_INTERFACE_IMPL(MultiCheckPreference::MultiChoiceClickListener, Object, IDialogInterfaceOnMultiChoiceClickListener)

MultiCheckPreference::MultiChoiceClickListener::MultiChoiceClickListener(
    /* [in] */ MultiCheckPreference* host)
    : mHost(host)
{
}

ECode MultiCheckPreference::MultiChoiceClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which,
    /* [in] */ Boolean isChecked)
{
    (*mHost->mSetValues)[which] = isChecked;
    return NOERROR;
}

//================
// MultiCheckPreference
//================

CAR_INTERFACE_IMPL(MultiCheckPreference, DialogPreference, IMultiCheckPreference)

MultiCheckPreference::MultiCheckPreference()
{
}

ECode MultiCheckPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(DialogPreference::constructor(context, attrs, defStyleAttr, defStyleRes));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ListPreference);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    mEntries = NULL;
    a->GetTextArray(R::styleable::ListPreference_entries, (ArrayOf<ICharSequence*>**)&mEntries);
    if (mEntries != NULL) {
        SetEntries(mEntries);
    }
    AutoPtr< ArrayOf<ICharSequence*> > textArray;
    a->GetTextArray(R::styleable::ListPreference_entryValues, (ArrayOf<ICharSequence*>**)&textArray);
    SetEntryValuesCS(textArray);
    a->Recycle();

    /* Retrieve the Preference summary attribute since it's private
     * in the Preference class.
     */
    a = NULL;
    attrIds = TO_ATTRS_ARRAYOF(R::styleable::Preference);
    context->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);
    a->GetString(R::styleable::Preference_summary, &mSummary);
    a->Recycle();
    return NOERROR;
}

ECode MultiCheckPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
     return constructor(context, attrs, defStyleAttr, 0);
}

ECode MultiCheckPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::dialogPreferenceStyle, 0);
}

ECode MultiCheckPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, R::attr::dialogPreferenceStyle, 0);
}

ECode MultiCheckPreference::SetEntries(
    /* [in] */ ArrayOf<ICharSequence*>* entries)
{
    mEntries = entries;
    mSetValues = ArrayOf<Boolean>::Alloc(entries->GetLength());
    mOrigValues = ArrayOf<Boolean>::Alloc(entries->GetLength());
    return NOERROR;
}

ECode MultiCheckPreference::SetEntries(
    /* [in] */ Int32 entriesResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    resources->GetTextArray(entriesResId, (ArrayOf<ICharSequence*>**)&entries);
    SetEntries(entries);
    return NOERROR;
}

ECode MultiCheckPreference::GetEntries(
    /* [out, callee] */ ArrayOf<ICharSequence*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEntries;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode MultiCheckPreference::SetEntryValues(
    /* [in] */ ArrayOf<String>* entryValues)
{
    mEntryValues = entryValues;
    for (Int32 i = 0; i < mSetValues->GetLength(); ++i) {
        (*mSetValues)[i] = FALSE;
        (*mOrigValues)[i] = FALSE;
    }
    return NOERROR;
}

ECode MultiCheckPreference::SetEntryValues(
    /* [in] */ Int32 entryValuesResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    resources->GetTextArray(entryValuesResId, (ArrayOf<ICharSequence*>**)&entries);
    SetEntryValuesCS(entries);
    return NOERROR;
}

void MultiCheckPreference::SetEntryValuesCS(
    /* [in] */ ArrayOf<ICharSequence*>* values)
{
    SetValues(NULL);
    if (values != NULL) {
        mEntryValues = ArrayOf<String>::Alloc(values->GetLength());
        for (Int32 i = 0; i < values->GetLength(); i++) {
            ((*values)[i])->ToString(&((*mEntryValues)[i]));
        }
    }
}

ECode MultiCheckPreference::GetEntryValues(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEntryValues;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode MultiCheckPreference::GetValue(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = (*mSetValues)[index];
    return NOERROR;
}

ECode MultiCheckPreference::SetValue(
    /* [in] */ Int32 index,
    /* [in] */ Boolean state)
{
    (*mSetValues)[index] = state;
    return NOERROR;
}

ECode MultiCheckPreference::SetValues(
    /* [in] */ ArrayOf<Boolean>* values)
{
    if (mSetValues != NULL) {
        for (Int32 i = 0; i < mSetValues->GetLength(); i++) {
            (*mSetValues)[i] = FALSE;
            (*mOrigValues)[i] = FALSE;
        }
        if (values != NULL) {
            mSetValues->Copy(values, values->GetLength() < mSetValues->GetLength() ? values->GetLength() : mSetValues->GetLength());
        }
    }
    return NOERROR;
}

ECode MultiCheckPreference::GetSummary(
    /* [out] */ ICharSequence** summary)
{
    VALIDATE_NOT_NULL(summary)
    if (mSummary.IsNull()) {
        return DialogPreference::GetSummary(summary);
    }
    else {
        return CString::New(mSummary, summary);
    }
}

ECode MultiCheckPreference::SetSummary(
    /* [in] */ ICharSequence* summary)
{
    FAIL_RETURN(DialogPreference::SetSummary(summary))
    if (summary == NULL && !mSummary.IsNull()) {
        mSummary = String(NULL);
    }
    else if (summary != NULL) {
        String str;
        summary->ToString(&str);
        if (!str.Equals(mSummary)){
            mSummary = str;
        }
    }

    return NOERROR;
}

ECode MultiCheckPreference::GetValues(
    /* [out, callee] */ ArrayOf<Boolean>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSetValues;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode MultiCheckPreference::FindIndexOfValue(
    /* [in] */ const String& value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (!value.IsNull() && mEntryValues != NULL) {
        for (Int32 i = mEntryValues->GetLength() - 1; i >= 0; i--) {
            if (((*mEntryValues)[i]).Equals(value)) {
                *result = i;
                return NOERROR;
            }
        }
    }
    *result = -1;
    return NOERROR;
}

ECode MultiCheckPreference::OnPrepareDialogBuilder(
    /* [in] */ IAlertDialogBuilder* builder)
{
    FAIL_RETURN(DialogPreference::OnPrepareDialogBuilder(builder))
    if (mEntries == NULL || mEntryValues == NULL) {
        Logger::E("MultiCheckPreference", "ListPreference requires an entries array and an entryValues array.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mOrigValues->Copy(mSetValues);
    AutoPtr<IDialogInterfaceOnMultiChoiceClickListener> listener = new MultiChoiceClickListener(this);
    return builder->SetMultiChoiceItems(mEntries, mSetValues, listener);
}

ECode MultiCheckPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    FAIL_RETURN(DialogPreference::OnDialogClosed(positiveResult))

    if (positiveResult) {
        AutoPtr< ArrayOf<Boolean> > values;
        GetValues((ArrayOf<Boolean>**)&values);
        AutoPtr<IArrayOf> array;
        CArrayOf::New(EIID_IBoolean, values->GetLength(), (IArrayOf**)&array);
        for (Int32 i = 0; i < values->GetLength(); ++i) {
            AutoPtr<IBoolean> b;
            CBoolean::New((*values)[i], (IBoolean**)&b);
            array->Set(i, b);
        }
        Boolean result;
        if (CallChangeListener(array, &result), result) {
            return NOERROR;
        }
    }
    mSetValues->Copy(mOrigValues, mSetValues->GetLength());

    return NOERROR;
}

ECode MultiCheckPreference::OnGetDefaultValue(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 index,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    String str;
    a->GetString(index, &str);
    AutoPtr<ICharSequence> cs;
    CString::New(str, (ICharSequence**)&cs);
    *value = (IInterface*)cs;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode MultiCheckPreference::OnSetInitialValue(
    /* [in] */ Boolean restorePersistedValue,
    /* [in] */ IInterface* defaultValue)
{
    return NOERROR;
}

ECode MultiCheckPreference::OnSaveInstanceState(
    /* [out] */ IParcelable** state)
{
    VALIDATE_NOT_NULL(state)

    AutoPtr<IParcelable> superState;
    DialogPreference::OnSaveInstanceState((IParcelable**)&superState);
    Boolean isPersistent;
    if (IsPersistent(&isPersistent), isPersistent) {
        // No need to save instance state since it's persistent
        *state = superState;
        REFCOUNT_ADD(*state)
        return NOERROR;
    }

    AutoPtr<IMultiCheckPreferenceSavedState> myState;
    CMultiCheckPreferenceSavedState::New(superState, (IMultiCheckPreferenceSavedState**)&myState);
    AutoPtr< ArrayOf<Boolean> > values;
    GetValues((ArrayOf<Boolean>**)&values);
    myState->SetValues(values);
    *state = IParcelable::Probe(myState);
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode MultiCheckPreference::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (state == NULL || IMultiCheckPreferenceSavedState::Probe(state) == NULL) {
        // Didn't save state for us in onSaveInstanceState
        return DialogPreference::OnRestoreInstanceState(state);
    }

    AutoPtr<IMultiCheckPreferenceSavedState> myState = IMultiCheckPreferenceSavedState::Probe(state);
    AutoPtr<IParcelable> superParcel;
    IAbsSavedState::Probe(myState)->GetSuperState((IParcelable**)&superParcel);
    DialogPreference::OnRestoreInstanceState(superParcel);
    AutoPtr< ArrayOf<Boolean> > values;
    myState->GetValues((ArrayOf<Boolean>**)&values);
    SetValues(values);
    return NOERROR;
}

}
}
}
