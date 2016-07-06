
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/preference/CTwoStatePreferenceSavedState.h"
#include "elastos/droid/preference/TwoStatePreference.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::R;
using Elastos::Core::CBoolean;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(TwoStatePreference, Preference, ITwoStatePreference)

TwoStatePreference::TwoStatePreference()
    : mChecked(FALSE)
    , mCheckedSet(FALSE)
    , mDisableDependentsState(FALSE)
{}

ECode TwoStatePreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return Preference::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode TwoStatePreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return Preference::constructor(context, attrs, defStyleAttr, 0);
}

ECode TwoStatePreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Preference::constructor(context, attrs, 0, 0);
}

ECode TwoStatePreference::constructor(
    /* [in] */ IContext* context)
{
    return Preference::constructor(context, NULL, 0, 0);
}

ECode TwoStatePreference::OnClick()
{
    Preference::OnClick();

    Boolean isChecked;
    IsChecked(&isChecked);
    Boolean newValue = !isChecked;

    AutoPtr<IBoolean> interfaceV;
    CBoolean::New(newValue, (IBoolean**)&interfaceV);
    Boolean callResult;
    if (CallChangeListener(interfaceV, &callResult), callResult) {
        SetChecked(newValue);
    }
    return NOERROR;
}

ECode TwoStatePreference::SetChecked(
    /* [in] */ Boolean checked)
{
    // Always persist/notify the first time; don't assume the field's default of false.
    Boolean changed = mChecked != checked;
    if (changed || !mCheckedSet) {
        mChecked = checked;
        mCheckedSet = TRUE;
        Boolean isPersist;
        PersistBoolean(checked, &isPersist);
        if (changed) {
            Boolean shouldDisableDependents;
            ShouldDisableDependents(&shouldDisableDependents);
            NotifyDependencyChange(shouldDisableDependents);
            NotifyChanged();
        }
    }

    return NOERROR;
}

ECode TwoStatePreference::IsChecked(
    /* [out] */ Boolean* isChecked)
{
    VALIDATE_NOT_NULL(isChecked)
    *isChecked = mChecked;
    return NOERROR;
}

ECode TwoStatePreference::ShouldDisableDependents(
    /* [out] */ Boolean* shouldDisableDependents)
{
    VALIDATE_NOT_NULL(shouldDisableDependents)

    Boolean shouldDisable = mDisableDependentsState ? mChecked : !mChecked;
    Boolean superShouldDisableDependents;
    *shouldDisableDependents = shouldDisable ||
            (Preference::ShouldDisableDependents(&superShouldDisableDependents), superShouldDisableDependents);

    return NOERROR;
}

ECode TwoStatePreference::SetSummaryOn(
    /* [in] */ ICharSequence* summary)
{
    mSummaryOn = summary;
    Boolean isChecked;
    if (IsChecked(&isChecked), isChecked) {
        NotifyChanged();
    }
    return NOERROR;
}

ECode TwoStatePreference::SetSummaryOn(
    /* [in] */ Int32 summaryResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String summaryStr;
    context->GetString(summaryResId, &summaryStr);
    AutoPtr<ICharSequence> summary;
    CString::New(summaryStr, (ICharSequence**)&summary);
    return SetSummaryOn(summary);
}

ECode TwoStatePreference::GetSummaryOn(
    /* [out] */ ICharSequence** summary)
{
    VALIDATE_NOT_NULL(summary)
    *summary = mSummaryOn;
    REFCOUNT_ADD(*summary)
    return NOERROR;
}


ECode TwoStatePreference::SetSummaryOff(
    /* [in] */ ICharSequence* summary)
{
    mSummaryOff = summary;
    Boolean isChecked;
    if (IsChecked(&isChecked), !isChecked) {
        NotifyChanged();
    }
    return NOERROR;
}

ECode TwoStatePreference::SetSummaryOff(
    /* [in] */ Int32 summaryResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String summaryStr;
    context->GetString(summaryResId, &summaryStr);
    AutoPtr<ICharSequence> summary;
    CString::New(summaryStr, (ICharSequence**)&summary);
    return SetSummaryOff(summary);
}

ECode TwoStatePreference::GetSummaryOff(
    /* [in] */ ICharSequence** summary)
{
    VALIDATE_NOT_NULL(summary)
    *summary = mSummaryOff;
    REFCOUNT_ADD(*summary)
    return NOERROR;
}

ECode TwoStatePreference::GetDisableDependentsState(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state)
    *state = mDisableDependentsState;
    return NOERROR;
}

ECode TwoStatePreference::SetDisableDependentsState(
    /* [in] */ Boolean disableDependentsState)
{
    mDisableDependentsState = disableDependentsState;
    return NOERROR;
}


ECode TwoStatePreference::OnGetDefaultValue(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 index,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    Boolean retVal;
    a->GetBoolean(index, FALSE, &retVal);
    AutoPtr<IBoolean> retObj;
    CBoolean::New(retVal, (IBoolean**)&retObj);
    *value = (IInterface*)retObj;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode TwoStatePreference::OnSetInitialValue(
    /* [in] */ Boolean restorePersistedValue,
    /* [in] */ IInterface* defaultValue)
{
    Boolean value = FALSE;
    if (restorePersistedValue) {
        GetPersistedBoolean(mChecked, &value);
    }
    else {
        AutoPtr<IBoolean> defaultVal = IBoolean::Probe(defaultVal);
        if(defaultVal != NULL){
            defaultVal->GetValue(&value);
        }
    }
    SetChecked(value);
    return NOERROR;
}

ECode TwoStatePreference::SyncSummaryView(
    /* [in] */ IView* view)
{
    // Sync the summary view
    AutoPtr<IView> v;
    view->FindViewById(R::id::summary, (IView**)&v);
    ITextView* summaryView = ITextView::Probe(v);
    if (summaryView != NULL) {
        Boolean useDefaultSummary = TRUE;
        if (mChecked && !TextUtils::IsEmpty(mSummaryOn)) {
            summaryView->SetText(mSummaryOn);
            useDefaultSummary = FALSE;
        }
        else if (!mChecked && !TextUtils::IsEmpty(mSummaryOff)) {
            summaryView->SetText(mSummaryOff);
            useDefaultSummary = FALSE;
        }

        if (useDefaultSummary) {
            AutoPtr<ICharSequence> summary;
            GetSummary((ICharSequence**)&summary);
            if (!TextUtils::IsEmpty(summary)) {
                summaryView->SetText(summary);
                useDefaultSummary = FALSE;
            }
        }

        Int32 newVisibility = IView::GONE;
        if (!useDefaultSummary) {
            // Someone has written to it
            newVisibility = IView::VISIBLE;
        }

        AutoPtr<IView> vv = IView::Probe(summaryView);
        Int32 visibility;
        if (vv->GetVisibility(&visibility), newVisibility != visibility) {
            vv->SetVisibility(newVisibility);
        }
    }
    return NOERROR;
}

ECode TwoStatePreference::OnSaveInstanceState(
    /* [out] */ IParcelable** state)
{
    VALIDATE_NOT_NULL(state)
    AutoPtr<IParcelable> superState;
    Preference::OnSaveInstanceState((IParcelable**)&superState);
    Boolean isPersistent;
    if (IsPersistent(&isPersistent), isPersistent) {
        // No need to save instance state since it's persistent
        *state = superState;
        REFCOUNT_ADD(*state)
        return NOERROR;
    }

    AutoPtr<ITwoStatePreferenceSavedState> myState;
    CTwoStatePreferenceSavedState::New(superState, (ITwoStatePreferenceSavedState**)&myState);
    Boolean isChecked;
    IsChecked(&isChecked);
    myState->SetChecked(isChecked);
    *state = IParcelable::Probe(myState);
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode TwoStatePreference::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (state == NULL || ITwoStatePreferenceSavedState::Probe(state) == NULL) {
        // Didn't save state for us in onSaveInstanceState
        Preference::OnRestoreInstanceState(state);
        return NOERROR;
    }
    AutoPtr<ITwoStatePreferenceSavedState> myState = ITwoStatePreferenceSavedState::Probe(state);
    AutoPtr<IParcelable> superState;
    IPreferenceBaseSavedState::Probe(myState)->GetSuperState((IParcelable**)&superState);
    Preference::OnRestoreInstanceState(superState);
    Boolean isChecked;
    myState->IsChecked(&isChecked);
    SetChecked(isChecked);
    return NOERROR;
}

}
}
}
