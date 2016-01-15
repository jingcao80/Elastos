
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/preference/MultiSelectListPreference.h"
#include "elastos/droid/preference/CMultiSelectListPreferenceSavedState.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CString;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnMultiChoiceClickListener;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Preference {

//================================
// MultiSelectListPreference::MultiChoiceClickListener
//================================

CAR_INTERFACE_IMPL(MultiSelectListPreference::MultiChoiceClickListener, Object, IDialogInterfaceOnMultiChoiceClickListener)

MultiSelectListPreference::MultiChoiceClickListener::MultiChoiceClickListener(
    /* [in] */ MultiSelectListPreference* host)
    : mHost(host)
{}

ECode MultiSelectListPreference::MultiChoiceClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which,
    /* [in] */ Boolean isChecked)
{
    String entryValue;
    ((*mHost->mEntryValues)[which])->ToString(&entryValue);
    if (isChecked) {
        mHost->mNewValues.Insert(entryValue);
        mHost->mPreferenceChanged |= TRUE;
        return NOERROR;
    }
    else {
        Set<String>::Iterator itr = mHost->mNewValues.Find(entryValue);
        if (itr !=  mHost->mNewValues.End()) {
            mHost->mNewValues.Erase(itr);
            mHost->mPreferenceChanged |= TRUE;
            return NOERROR;
        }
        mHost->mPreferenceChanged |= FALSE;
        return NOERROR;
    }
}

//====================
// MultiSelectListPreference
//====================

CAR_INTERFACE_IMPL(MultiSelectListPreference, DialogPreference, IMultiSelectListPreference)

MultiSelectListPreference::MultiSelectListPreference()
    : mPreferenceChanged(FALSE)
{}

ECode MultiSelectListPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(DialogPreference::constructor(context, attrs, defStyleAttr, defStyleRes));
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::MultiSelectListPreference),
            ArraySize(R::styleable::MultiSelectListPreference));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    a->GetTextArray(R::styleable::MultiSelectListPreference_entries, (ArrayOf<ICharSequence*>**)&mEntries);
    a->GetTextArray(R::styleable::MultiSelectListPreference_entryValues, (ArrayOf<ICharSequence*>**)&mEntryValues);
    a->Recycle();
    return NOERROR;
}

ECode MultiSelectListPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode MultiSelectListPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::dialogPreferenceStyle);
}

ECode MultiSelectListPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode MultiSelectListPreference::SetEntries(
    /* [in] */ ArrayOf<ICharSequence*>* entries)
{
    mEntries = entries;
    return NOERROR;
}

ECode MultiSelectListPreference::SetEntries(
    /* [in] */ Int32 entriesResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    resources->GetTextArray(entriesResId, (ArrayOf<ICharSequence*>**)&entries);
    return NOERROR;
}

ECode MultiSelectListPreference::GetEntries(
    /* [out, callee] */ ArrayOf<ICharSequence*>** entries)
{
    VALIDATE_NOT_NULL(entries)
    *entries = mEntries;
    REFCOUNT_ADD(*entries)
    return NOERROR;
}

ECode MultiSelectListPreference::SetEntryValues(
    /* [in] */ ArrayOf<ICharSequence*>* entryValues)
{
    mEntryValues = entryValues;
    return NOERROR;
}

ECode MultiSelectListPreference::SetEntryValues(
    /* [in] */ Int32 entryValuesResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<ArrayOf<ICharSequence*> > entries;
    resources->GetTextArray(entryValuesResId, (ArrayOf<ICharSequence*>**)&entries);
    return SetEntryValues(entries);
}

ECode MultiSelectListPreference::GetEntryValues(
    /* [out, callee] */ ArrayOf<ICharSequence*>** entryValues)
{
    VALIDATE_NOT_NULL(entryValues)
    *entryValues = mEntryValues;
    REFCOUNT_ADD(*entryValues)
    return NOERROR;
}

ECode MultiSelectListPreference::SetValues(
    /* [in] */ ISet* values)
{
    mValues.Clear();
    AutoPtr< ArrayOf<IInterface*> > valuesArray;
    values->ToArray((ArrayOf<IInterface*>**)&valuesArray);
    for (Int32 i = 0; i < valuesArray->GetLength(); ++i) {
        AutoPtr<ICharSequence> cs = ICharSequence::Probe((*valuesArray)[i]);
        String str;
        cs->ToString(&str);
        mValues.Insert(str);
    }

    Boolean result;
    return PersistStringSet(values, &result);
}

ECode MultiSelectListPreference::GetValues(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISet> v;
    CHashSet::New((ISet**)&v);
    Set<String>::Iterator it = mValues.Begin();
    for (; it != mValues.End(); ++it) {
        AutoPtr<ICharSequence> cs;
        CString::New(*it, (ICharSequence**)&cs);
        Boolean isSuccess;
        v->Add(cs, &isSuccess);
    }
    *result = v;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode MultiSelectListPreference::FindIndexOfValue(
    /* [in] */ const String& value,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    if (!value.IsNull() && mEntryValues != NULL) {
        for (Int32 i = mEntryValues->GetLength() - 1; i >= 0; i--) {
            String entryValue;
            ((*mEntryValues)[i])->ToString(&entryValue);
            if (entryValue.Equals(value)) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode MultiSelectListPreference::OnPrepareDialogBuilder(
    /* [in] */ IAlertDialogBuilder* builder)
{
    FAIL_RETURN(DialogPreference::OnPrepareDialogBuilder(builder))
    if (mEntries == NULL || mEntryValues == NULL) {
        Logger::E("MultiSelectListPreference", "MultiSelectListPreference requires an entries array and an entryValues array.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr< ArrayOf<Boolean> > checkedItems = GetSelectedItems();

    AutoPtr<IDialogInterfaceOnMultiChoiceClickListener> listener = new MultiChoiceClickListener(this);
    builder->SetMultiChoiceItems(mEntries, checkedItems, listener);

    mNewValues.Clear();
    mNewValues.Insert(mValues.Begin(), mValues.End());
    return NOERROR;
}

AutoPtr< ArrayOf<Boolean> > MultiSelectListPreference::GetSelectedItems()
{
    AutoPtr< ArrayOf<ICharSequence*> > entries = mEntryValues;
    Int32 entryCount = mEntryValues->GetLength();
    Set<String> values(mValues);
    AutoPtr< ArrayOf<Boolean> > result = ArrayOf<Boolean>::Alloc(entryCount);

    for (Int32 i = 0; i < entryCount; i++) {
        String entryValue;
        ((*entries)[i])->ToString(&entryValue);
        Set<String>::Iterator itr = values.Find(entryValue);
        (*result)[i] = itr != values.End() ? TRUE : FALSE;
    }
    return result;
}

ECode MultiSelectListPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    FAIL_RETURN(DialogPreference::OnDialogClosed(positiveResult))
    if (positiveResult && mPreferenceChanged) {
        AutoPtr<ISet> values;
        CHashSet::New((ISet**)&values);
        Set<String>::Iterator it = mNewValues.Begin();
        for (; it != mNewValues.End(); ++it) {
            AutoPtr<ICharSequence> cs;
            CString::New(*it, (ICharSequence**)&cs);
            Boolean isSuccess;
            values->Add(cs, &isSuccess);
        }
        Boolean result;
        if (CallChangeListener(values, &result), result) {
            SetValues(values);
        }
    }
    mPreferenceChanged = FALSE;
    return NOERROR;
}

ECode MultiSelectListPreference::OnGetDefaultValue(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 index,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    AutoPtr< ArrayOf<ICharSequence*> > defaultValues;
    a->GetTextArray(index, (ArrayOf<ICharSequence*>**)&defaultValues);
    Int32 valueCount = defaultValues->GetLength();
    AutoPtr<ISet> result;
    CHashSet::New((ISet**)&result);
    for (Int32 i = 0; i < valueCount; i++) {
        Boolean isSuccess;
        result->Add((*defaultValues)[i], &isSuccess);
    }
    *value = result;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode MultiSelectListPreference::OnSetInitialValue(
    /* [in] */ Boolean restoreValue,
    /* [in] */ IInterface* defaultValue)
{
    AutoPtr<ISet> values;
    if (restoreValue) {
        AutoPtr<ISet> valuesSet;
        CHashSet::New((ISet**)&valuesSet);
        Set<String>::Iterator it = mValues.Begin();
        for (; it != mValues.End(); ++it) {
            AutoPtr<ICharSequence> cs;
            CString::New(*it, (ICharSequence**)&cs);
            Boolean isSuccess;
            valuesSet->Add(cs, &isSuccess);
        }
        GetPersistedStringSet(valuesSet, (ISet**)&values);
    }
    else {
        values = ISet::Probe(defaultValue);
    }
    return SetValues(values);
}

ECode MultiSelectListPreference::OnSaveInstanceState(
    /* [out] */ IParcelable** state)
{
    VALIDATE_NOT_NULL(state)

    AutoPtr<IParcelable> superState;
    DialogPreference::OnSaveInstanceState((IParcelable**)&superState);
    Boolean isPersistent;
    if (IsPersistent(&isPersistent), isPersistent) {
        // No need to save instance state
        *state = superState;
        REFCOUNT_ADD(*state)
        return NOERROR;
    }

    AutoPtr<IMultiSelectListPreferenceSavedState> myState;
    CMultiSelectListPreferenceSavedState::New(superState, (IMultiSelectListPreferenceSavedState**)&myState);
    AutoPtr<ISet> values;
    GetValues((ISet**)&values);
    myState->SetValues(values);
    *state = IParcelable::Probe(myState);
    REFCOUNT_ADD(*state)
    return NOERROR;
}

}
}
}
