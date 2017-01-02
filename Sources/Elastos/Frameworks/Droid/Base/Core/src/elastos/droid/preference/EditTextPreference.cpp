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

#include "Elastos.Droid.View.h"
#include "elastos/droid/preference/CEditTextPreferenceSavedState.h"
#include "elastos/droid/preference/EditTextPreference.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/widget/CEditText.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Preference::IEditTextPreferenceSavedState;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IAbsSavedState;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::CEditText;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Preference::CEditTextPreferenceSavedState;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(EditTextPreference, DialogPreference, IEditTextPreference)

EditTextPreference::EditTextPreference()
{}

EditTextPreference::~EditTextPreference()
{}

ECode EditTextPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(DialogPreference::constructor(context, attrs, defStyleAttr, defStyleRes));
    CEditText::New(context, attrs, (IEditText**)&mEditText);

    // Give it an ID so it can be saved/restored
    IView::Probe(mEditText)->SetId(R::id::edit);

    /*
     * The preference framework and view framework both have an 'enabled'
     * attribute. Most likely, the 'enabled' specified in this XML is for
     * the preference framework, but it was also given to the view framework.
     * We reset the enabled state.
     */
    IView::Probe(mEditText)->SetEnabled(TRUE);
    return NOERROR;
}

ECode EditTextPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode EditTextPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::editTextPreferenceStyle);
}

ECode EditTextPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode EditTextPreference::SetText(
    /* [in] */ const String& text)
{
    Boolean wasBlocking;
    ShouldDisableDependents(&wasBlocking);

    mText = text;

    Boolean isSuccess;
    PersistString(text, &isSuccess);

    Boolean isBlocking;
    if (ShouldDisableDependents(&isBlocking), isBlocking != wasBlocking) {
        NotifyDependencyChange(isBlocking);
    }

    return NOERROR;
}

ECode EditTextPreference::GetText(
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text)
    *text = mText;
    return NOERROR;
}

ECode EditTextPreference::OnBindDialogView(
    /* [in] */ IView* view)
{
    FAIL_RETURN(DialogPreference::OnBindDialogView(view))

    AutoPtr<IEditText> editText = mEditText;
    AutoPtr<ICharSequence> cs;
    CString::New(mText, (ICharSequence**)&cs);
    ITextView::Probe(editText)->SetText(cs);

    AutoPtr<IViewParent> oldParent;
    IView::Probe(editText)->GetParent((IViewParent**)&oldParent);
    if (IView::Probe(oldParent) != view) {
        if (oldParent != NULL) {
            AutoPtr<IViewGroup> vGroup = IViewGroup::Probe(oldParent);
            vGroup->RemoveViewInLayout(IView::Probe(editText));
        }
        OnAddEditTextToDialogView(view, editText);
    }

    return NOERROR;
}

ECode EditTextPreference::OnAddEditTextToDialogView(
    /* [in] */ IView* dialogView,
    /* [in] */ IEditText* editText)
{
    AutoPtr<IView> temp;
    dialogView->FindViewById(R::id::edittext_container, (IView**)&temp);
    AutoPtr<IViewGroup> container = IViewGroup::Probe(temp);
    if (container != NULL) {
        container->AddView(IView::Probe(editText), IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::WRAP_CONTENT);
    }

    return NOERROR;
}

ECode EditTextPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    FAIL_RETURN(DialogPreference::OnDialogClosed(positiveResult))

    if (positiveResult) {
        AutoPtr<ICharSequence> cs;
        ITextView::Probe(mEditText)->GetText((ICharSequence**)&cs);
        Boolean isSuccess;
        if (CallChangeListener(cs, &isSuccess), isSuccess) {
            String value;
            cs->ToString(&value);
            SetText(value);
        }
    }
    return NOERROR;
}

ECode EditTextPreference::OnGetDefaultValue(
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

ECode EditTextPreference::OnSetInitialValue(
    /* [in] */ Boolean restoreValue,
    /* [in] */ IInterface* defaultValue)
{
    String text;
    if (restoreValue) {
        GetPersistedString(mText, &text);
    }
    else {
        AutoPtr<ICharSequence> cs = ICharSequence::Probe(defaultValue);
        cs->ToString(&text);
    }

    return SetText(text);
}

ECode EditTextPreference::ShouldDisableDependents(
    /* [out] */ Boolean* shouldDisableDependents)
{
    VALIDATE_NOT_NULL(shouldDisableDependents)

    Boolean superShould;
    DialogPreference::ShouldDisableDependents(&superShould);
    *shouldDisableDependents = TextUtils::IsEmpty(mText) || superShould;
    return NOERROR;
}

ECode EditTextPreference::GetEditText(
    /* [out] */ IEditText** editText)
{
    VALIDATE_NOT_NULL(editText)
    *editText = mEditText;
    REFCOUNT_ADD(*editText)
    return NOERROR;
}

ECode EditTextPreference::NeedInputMethod(
    /* [out] */ Boolean* isNeed)
{
    VALIDATE_NOT_NULL(isNeed)
    // We want the input method to show, if possible, when dialog is displayed
    *isNeed = TRUE;
    return NOERROR;
}

ECode EditTextPreference::OnSaveInstanceState(
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

    AutoPtr<IEditTextPreferenceSavedState> myState;
    CEditTextPreferenceSavedState::New(superState, (IEditTextPreferenceSavedState**)&myState);
    String str;
    GetText(&str);
    myState->SetText(&str);
    *state = IParcelable::Probe(myState);
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode EditTextPreference::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (state == NULL || IEditTextPreferenceSavedState::Probe(state) == NULL) {
        // Didn't save state for us in onSaveInstanceState
        return DialogPreference::OnRestoreInstanceState(state);
    }

    AutoPtr<IEditTextPreferenceSavedState> myState = IEditTextPreferenceSavedState::Probe(state);
    AutoPtr<IParcelable> superParcel;
    IAbsSavedState::Probe(myState)->GetSuperState((IParcelable**)&superParcel);
    DialogPreference::OnRestoreInstanceState(superParcel);
    String str;
    myState->GetText(&str);
    SetText(str);

    return NOERROR;
}

}
}
}
