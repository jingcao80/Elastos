
#include "elastos/droid/preference/SeekBarPreference.h"
#include "elastos/droid/preference/CSeekBarPreferenceSavedState.h"
#include "elastos/droid/R.h"

using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::IAbsSeekBar;
using Elastos::Droid::Widget::IProgressBar;

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(SeekBarPreference, Preference, ISeekBarOnSeekBarChangeListener)

SeekBarPreference::SeekBarPreference()
    : mProgress(0)
    , mMax(0)
    , mTrackingTouch(FALSE)
{
}

ECode SeekBarPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(Preference::constructor(context, attrs, defStyleAttr, defStyleRes));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ProgressBar);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    Int32 max;
    a->GetInt32(R::styleable::ProgressBar_max, mMax, &max);
    SetMax(max);
    a->Recycle();

    a = NULL;
    attrIds = TO_ATTRS_ARRAYOF(R::styleable::SeekBarPreference);
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    Int32 layoutResId;
    a->GetResourceId(
            R::styleable::SeekBarPreference_layout,
            R::layout::preference_widget_seekbar, &layoutResId);
    a->Recycle();
    SetLayoutResource(layoutResId);
    return NOERROR;
}

ECode SeekBarPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode SeekBarPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::seekBarPreferenceStyle, 0);
}

ECode SeekBarPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, R::attr::seekBarPreferenceStyle, 0);
}

ECode SeekBarPreference::OnBindView(
    /* [in] */ IView* view)
{
    FAIL_RETURN(Preference::OnBindView(view))
    AutoPtr<IView> tempView;
    view->FindViewById(R::id::seekbar, (IView**)&tempView);
    ISeekBar* seekBar = ISeekBar::Probe(tempView);
    seekBar->SetOnSeekBarChangeListener(this);
    IProgressBar::Probe(seekBar)->SetMax(mMax);
    IProgressBar::Probe(seekBar)->SetProgress(mProgress);
    Boolean isEnabled;
    IsEnabled(&isEnabled);
    tempView->SetEnabled(isEnabled);
    return NOERROR;
}

ECode SeekBarPreference::GetSummary(
    /* [out] */ ICharSequence** summary)
{
    VALIDATE_NOT_NULL(summary)
    *summary = NULL;
    return NOERROR;
}

ECode SeekBarPreference::OnSetInitialValue(
    /* [in] */ Boolean restoreValue,
    /* [in] */ IInterface* defaultValue)
{
    Int32 progress;
    if (restoreValue) {
        GetPersistedInt32(mProgress, &progress);
    }
    else {
        IInteger32::Probe(defaultValue)->GetValue(&progress);
    }
    SetProgress(progress);
    return NOERROR;
}

ECode SeekBarPreference::OnGetDefaultValue(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 index,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    Int32 lvalue;
    a->GetInt32(index, 0, &lvalue);
    AutoPtr<IInteger32> integer;
    CInteger32::New(lvalue, (IInteger32**)&integer);
    *value = integer;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode SeekBarPreference::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 action, progress;
    GetProgress(&progress);
    if (event->GetAction(&action), action != IKeyEvent::ACTION_UP) {
        if (keyCode == IKeyEvent::KEYCODE_PLUS || keyCode == IKeyEvent::KEYCODE_EQUALS) {
            SetProgress(progress + 1);
            *result = TRUE;
            return NOERROR;
        }
        if (keyCode == IKeyEvent::KEYCODE_MINUS) {
            SetProgress(progress - 1);
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode SeekBarPreference::SetMax(
    /* [in] */ Int32 max)
{
    if (max != mMax) {
        mMax = max;
        NotifyChanged();
    }
    return NOERROR;
}

ECode SeekBarPreference::SetProgress(
    /* [in] */ Int32 progress)
{
    return SetProgress(progress, TRUE);
}

ECode SeekBarPreference::SetProgress(
    /* [in] */ Int32 progress,
    /* [in] */ Boolean notifyChanged)
{
    if (progress > mMax) {
        progress = mMax;
    }
    if (progress < 0) {
        progress = 0;
    }
    if (progress != mProgress) {
        mProgress = progress;
        Boolean result;
        PersistInt32(progress, &result);
        if (notifyChanged) {
            NotifyChanged();
        }
    }
    return NOERROR;
}

ECode SeekBarPreference::GetProgress(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProgress;
    return NOERROR;
}

ECode SeekBarPreference::SyncProgress(
    /* [in] */ ISeekBar* seekBar)
{
    Int32 progress;
    IProgressBar::Probe(seekBar)->GetProgress(&progress);
    if (progress != mProgress) {
        AutoPtr<IInteger32> iprogress;
        CInteger32::New(progress, (IInteger32**)&iprogress);
        Boolean result;
        if (CallChangeListener(iprogress, &result), result) {
            SetProgress(progress, FALSE);
        }
        else {
            IProgressBar::Probe(seekBar)->SetProgress(mProgress);
        }
    }
    return NOERROR;
}

ECode SeekBarPreference::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    if (fromUser && !mTrackingTouch) {
        SyncProgress(seekBar);
    }
    return NOERROR;
}

ECode SeekBarPreference::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    mTrackingTouch = TRUE;
    return NOERROR;
}

ECode SeekBarPreference::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    mTrackingTouch = FALSE;
    Int32 progress;
    if (IProgressBar::Probe(seekBar)->GetProgress(&progress), progress != mProgress) {
        SyncProgress(seekBar);
    }
    return NOERROR;
}

ECode SeekBarPreference::OnSaveInstanceState(
    /* [out] */ IParcelable** state)
{
    VALIDATE_NOT_NULL(state)

    /*
     * Suppose a client uses this preference type without persisting. We
     * must save the instance state so it is able to, for example, survive
     * orientation changes.
     */

    AutoPtr<IParcelable> superState;
    Preference::OnSaveInstanceState(state);
    Boolean isPersistent;
    if (IsPersistent(&isPersistent), isPersistent) {
        // No need to save instance state since it's persistent
        *state = superState;
        REFCOUNT_ADD(*state)
        return NOERROR;
    }

    // Save the instance state
    AutoPtr<ISeekBarPreferenceSavedState> myState;
    CSeekBarPreferenceSavedState::New(superState, (ISeekBarPreferenceSavedState**)&myState);
    myState->SetProgress(mProgress);
    myState->SetMax(mMax);
    *state = IParcelable::Probe(myState);
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode SeekBarPreference::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (ISeekBarPreferenceSavedState::Probe(state) == NULL) {
        // Didn't save state for us in onSaveInstanceState
        return OnRestoreInstanceState(state);
    }

    // Restore the instance state
    AutoPtr<ISeekBarPreferenceSavedState> myState = ISeekBarPreferenceSavedState::Probe(state);
    AutoPtr<IParcelable> superParcel;
    IPreferenceBaseSavedState::Probe(myState)->GetSuperState((IParcelable**)&superParcel);
    Preference::OnRestoreInstanceState(superParcel);
    myState->GetProgress(&mProgress);
    myState->GetMax(&mMax);
    NotifyChanged();

    return NOERROR;
}

}
}
}

