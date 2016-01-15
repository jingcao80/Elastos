
#include "elastos/droid/preference/ListPreference.h"
#include "elastos/droid/preference/CListPreferenceSavedState.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IAbsSavedState;

namespace Elastos {
namespace Droid {
namespace Preference {

/////////////////////////////////////////////////////
// ListPreference::DialogInterfaceOnClickListener
/////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(ListPreference::DialogInterfaceOnClickListener, Object,IDialogInterfaceOnClickListener)

ListPreference::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ ListPreference* host)
    : mHost(host)
{}

ECode ListPreference::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->mClickedDialogEntryIndex = which;

    /*
     * Clicking on an item simulates the positive button
     * click, and dismisses the dialog.
     */
    mHost->OnClick(dialog, IDialogInterface::BUTTON_POSITIVE);
    dialog->Dismiss();
    return NOERROR;
}

/////////////////////////////////////////////////////
// ListPreference
/////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(ListPreference, DialogPreference, IListPreference)

ListPreference::ListPreference()
    : mClickedDialogEntryIndex(0)
    , mValueSet(FALSE)
{}

ECode ListPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(DialogPreference::constructor(context, attrs, defStyleAttr, defStyleRes));

    AutoPtr< ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::ListPreference),
            ArraySize(R::styleable::ListPreference));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    a->GetTextArray(R::styleable::ListPreference_entries, (ArrayOf<ICharSequence*>**)&mEntries);
    a->GetTextArray(R::styleable::ListPreference_entryValues, (ArrayOf<ICharSequence*>**)&mEntryValues);
    a->Recycle();

    /* Retrieve the Preference summary attribute since it's private
     * in the Preference class.
     */
    a = NULL;
    attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::Preference),
            ArraySize(R::styleable::Preference));
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    a->GetString(R::styleable::Preference_summary, &mSummary);
    a->Recycle();
    return NOERROR;
}

ECode ListPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ListPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::dialogPreferenceStyle);
}

ECode ListPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ListPreference::SetEntries(
    /* [in] */ ArrayOf<ICharSequence*>* entries)
{
    mEntries = entries;
    return NOERROR;
}

ECode ListPreference::SetEntries(
    /* [in] */ Int32 entriesResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    resources->GetTextArray(entriesResId, (ArrayOf<ICharSequence*>**)&entries);
    return SetEntries(entries);
}

ECode ListPreference::GetEntries(
    /* [out, callee] */ ArrayOf<ICharSequence*>** entries)
{
    VALIDATE_NOT_NULL(entries)
    *entries = mEntries;
    REFCOUNT_ADD(*entries);
    return NOERROR;
}

ECode ListPreference::SetEntryValues(
    /* [in] */ ArrayOf<ICharSequence*>* entryValues)
{
    mEntryValues = entryValues;
    return NOERROR;
}

ECode ListPreference::SetEntryValues(
    /* [in] */ Int32 entryValuesResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    resources->GetTextArray(entryValuesResId, (ArrayOf<ICharSequence*>**)&entries);
    return SetEntryValues(entries);
}

ECode ListPreference::GetEntryValues(
    /* [out, callee] */ ArrayOf<ICharSequence*>** entryValues)
{
    VALIDATE_NOT_NULL(entryValues)
    *entryValues = mEntryValues;
    REFCOUNT_ADD(*entryValues);
    return NOERROR;
}

ECode ListPreference::SetValue(
    /* [in] */ const String& value)
{
    // Always persist/notify the first time.
    Boolean changed = !TextUtils::Equals(mValue, value);
    if (changed || !mValueSet) {
        mValue = value;
        mValueSet = TRUE;
        Boolean isPersistString = FALSE;
        PersistString(value, &isPersistString);
        if (changed) {
            NotifyChanged();
        }
    }
    return NOERROR;
}

ECode ListPreference::GetSummary(
    /* [out] */ ICharSequence** summary)
{
    VALIDATE_NOT_NULL(summary)

    AutoPtr<ICharSequence> entry;
    GetEntry((ICharSequence**)&entry);
    if (mSummary.IsNull() || entry == NULL) {
        return DialogPreference::GetSummary(summary);
    }

    String str;
    str.AppendFormat(mSummary.string(), entry.Get());
    return CString::New(str, summary);
}

ECode ListPreference::SetSummary(
    /* [in] */ ICharSequence* summary)
{
    FAIL_RETURN(DialogPreference::SetSummary(summary))
    if (summary == NULL && !mSummary.IsNull()) {
        mSummary = String(NULL);
    }
    else if (summary != NULL) {
        String str;
        summary->ToString(&str);
        if (!str.Equals(mSummary)) {
            mSummary = str;
        }
    }

    return NOERROR;
}

ECode ListPreference::SetValueIndex(
    /* [in] */ Int32 index)
{
    if (mEntryValues != NULL) {
        AutoPtr<ICharSequence> cstr = (*mEntryValues)[index];
        String str;
        cstr->ToString(&str);
        SetValue(str);
    }
    return NOERROR;
}

ECode ListPreference::GetValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mValue;
    return NOERROR;
}

ECode ListPreference::GetEntry(
    /* [out] */ ICharSequence** entry)
{
    Int32 index = GetValueIndex();
    if (index >= 0 && mEntries != NULL) {
        *entry = (*mEntries)[index];
        REFCOUNT_ADD(*entry)
    }
    else {
        *entry = NULL;
    }
    return NOERROR;
}

ECode ListPreference::FindIndexOfValue(
    /* [in] */ const String& value,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    if (!value.IsNull() && mEntryValues != NULL) {
        for (Int32 i = mEntryValues->GetLength() - 1; i >= 0; i--) {
            AutoPtr<ICharSequence> cstr = (*mEntryValues)[i];
            String str;
            cstr->ToString(&str);
            if (str.Equals(value)) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

Int32 ListPreference::GetValueIndex()
{
    Int32 index;
    FindIndexOfValue(mValue, &index);
    return index;
}

ECode ListPreference::OnPrepareDialogBuilder(
    /* [in] */ IAlertDialogBuilder* builder)
{
    FAIL_RETURN(DialogPreference::OnPrepareDialogBuilder(builder))

    if (mEntries == NULL || mEntryValues == NULL) {
        Logger::E("ListPreference", "ListPreference requires an entries array and an entryValues array.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mClickedDialogEntryIndex = GetValueIndex();
    AutoPtr<IDialogInterfaceOnClickListener> listener = new DialogInterfaceOnClickListener(this);
    builder->SetSingleChoiceItems(mEntries, mClickedDialogEntryIndex, listener);

    /*
     * The typical interaction for list-based dialogs is to have
     * click-on-an-item dismiss the dialog instead of the user having to
     * press 'Ok'.
     */
    builder->SetPositiveButton(NULL, NULL);
    return NOERROR;
}

ECode ListPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    DialogPreference::OnDialogClosed(positiveResult);

    if (positiveResult && mClickedDialogEntryIndex >= 0 && mEntryValues != NULL) {
        AutoPtr<ICharSequence> cstr = (*mEntryValues)[mClickedDialogEntryIndex];
        Boolean isSuccess;
        if (CallChangeListener(cstr, &isSuccess), isSuccess) {
            String value;
            cstr->ToString(&value);
            SetValue(value);
        }
    }
    return NOERROR;
}

ECode ListPreference::OnGetDefaultValue(
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

ECode ListPreference::OnSetInitialValue(
    /* [in] */ Boolean restoreValue,
    /* [in] */ IInterface* defaultValue)
{
    String value;
    if (restoreValue) {
        GetPersistedString(mValue, &value);
    }
    else {
        ICharSequence::Probe(defaultValue)->ToString(&value);
    }
    return SetValue(value);
}

ECode ListPreference::OnSaveInstanceState(
    /* [out] */ IParcelable** parcel)
{
    VALIDATE_NOT_NULL(parcel)

    AutoPtr<IParcelable> superState;
    DialogPreference::OnSaveInstanceState((IParcelable**)&superState);
    Boolean isPersistent;
    IsPersistent(&isPersistent);
    if (isPersistent) {
        // No need to save instance state since it's persistent
        *parcel = superState;
        REFCOUNT_ADD(*parcel)
        return NOERROR;
    }

    AutoPtr<IListPreferenceSavedState> myState;
    CListPreferenceSavedState::New(superState, (IListPreferenceSavedState**)&myState);
    String value;
    GetValue(&value);
    myState->SetValue(&value);
    *parcel = IParcelable::Probe(myState);
    REFCOUNT_ADD(*parcel)
    return NOERROR;
}

ECode ListPreference::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (state == NULL || IListPreferenceSavedState::Probe(state) == NULL) {
        // Didn't save state for us in onSaveInstanceState
        return DialogPreference::OnRestoreInstanceState(state);
    }

    AutoPtr<IListPreferenceSavedState> myState = IListPreferenceSavedState::Probe(state);
    AutoPtr<IParcelable> superState;
    IAbsSavedState::Probe(myState)->GetSuperState((IParcelable**)&superState);
    DialogPreference::OnRestoreInstanceState(superState);
    String value;
    myState->GetValue(&value);
    SetValue(value);
    return NOERROR;
}

}
}
}
