
#include "elastos/droid/settings/widget/SwitchBar.h"
#include "elastos/droid/settings/widget/CSwitchBarSavedState.h"
#include "elastos/droid/settings/widget/CToggleSwitch.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "../R.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Settings::Widget::CSwitchBarSavedState;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Widget {

static AutoPtr< ArrayOf<Int32> > InitMARGIN_ATTRIBUTES()
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(2);
    (*args)[0] = R::attr::switchBarMarginStart;
    (*args)[1] = R::attr::switchBarMarginEnd;
    return args;
}

AutoPtr< ArrayOf<Int32> > SwitchBar::MARGIN_ATTRIBUTES = InitMARGIN_ATTRIBUTES();

//===============================================================================
//                  SwitchBar::SavedState
//===============================================================================

CAR_INTERFACE_IMPL(SwitchBar::SavedState, View::BaseSavedState, ISwitchBarSavedState);

SwitchBar::SavedState::SavedState()
    : mChecked(FALSE)
    , mVisible(FALSE)
{}

ECode SwitchBar::SavedState::constructor()
{
    return View::BaseSavedState::constructor();
}

ECode SwitchBar::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return View::BaseSavedState::constructor(superState);
}

ECode SwitchBar::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    View::BaseSavedState::ReadFromParcel(source);
    source->ReadBoolean(&mChecked);
    source->ReadBoolean(&mVisible);
    return NOERROR;
}

ECode SwitchBar::SavedState::WriteToParcel(
    /* [in] */ IParcel* out)
{
    View::BaseSavedState::WriteToParcel(out);
    out->WriteBoolean(mChecked);
    out->WriteBoolean(mVisible);
    return NOERROR;
}

ECode SwitchBar::SavedState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder;
    builder += "SwitchBar.SavedState{";
    builder += StringUtils::ToHexString((Int32)this);
    builder += " checked=";
    builder += mChecked;
    builder += " visible=";
    builder += mVisible;
    builder += "}";
    *str = builder.ToString();
    return NOERROR;
}

//===============================================================================
//                  SwitchBar::SwitchBarOnSwitchChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(SwitchBar::SwitchBarOnSwitchChangeListener, Object, ISwitchBarOnSwitchChangeListener);

SwitchBar::SwitchBarOnSwitchChangeListener::SwitchBarOnSwitchChangeListener(
    /* [in] */ SwitchBar* host)
    : mHost(host)
{}

SwitchBar::SwitchBarOnSwitchChangeListener::~SwitchBarOnSwitchChangeListener()
{}

ECode SwitchBar::SwitchBarOnSwitchChangeListener::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    return mHost->SetTextViewLabel(isChecked);
}

//===============================================================================
//                  SwitchBar
//===============================================================================

CAR_INTERFACE_IMPL_3(SwitchBar, LinearLayout, ISwitchBar, ICompoundButtonOnCheckedChangeListener, IViewOnClickListener);

SwitchBar::SwitchBar()
{
    CArrayList::New((IArrayList**)&mSwitchChangeListeners);
}

SwitchBar::~SwitchBar()
{}

ECode SwitchBar::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode SwitchBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode SwitchBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode SwitchBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    LinearLayout::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::switch_bar, this, (IView**)&view);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, MARGIN_ATTRIBUTES, (ITypedArray**)&a);
    Float value;
    a->GetDimension(0, 0, &value);
    Int32 switchBarMarginStart = (Int32)value;
    a->GetDimension(1, 0, &value);
    Int32 switchBarMarginEnd = (Int32)value;
    a->Recycle();

    view = NULL;
    FindViewById(R::id::switch_text, (IView**)&view);
    mTextView = ITextView::Probe(view);
    mTextView->SetText(R::string::switch_off_text);
    AutoPtr<IViewGroupLayoutParams> params;
    IView::Probe(mTextView)->GetLayoutParams((IViewGroupLayoutParams**)&params);
    IViewGroupMarginLayoutParams* lp = IViewGroupMarginLayoutParams::Probe(params);
    lp->SetMarginStart(switchBarMarginStart);

    view = NULL;
    FindViewById(R::id::switch_widget, (IView**)&view);
    mSwitch = IToggleSwitch::Probe(view);
    // Prevent OnSaveInstanceState() to be called as we are managing the state of the Switch
    // on our own
    IView::Probe(mSwitch)->SetSaveEnabled(FALSE);
    params = NULL;
    IView::Probe(mSwitch)->GetLayoutParams((IViewGroupLayoutParams**)&params);
    lp = IViewGroupMarginLayoutParams::Probe(params);
    lp->SetMarginEnd(switchBarMarginEnd);

    AutoPtr<SwitchBarOnSwitchChangeListener> listener = new SwitchBarOnSwitchChangeListener(this);
    AddOnSwitchChangeListener(listener);

    SetOnClickListener(this);

    // Default is hide
    SetVisibility(IView::GONE);
    return NOERROR;
}

ECode SwitchBar::SetTextViewLabel(
    /* [in] */ Boolean isChecked)
{
    return mTextView->SetText(isChecked ? R::string::switch_on_text : R::string::switch_off_text);
}

ECode SwitchBar::SetChecked(
    /* [in] */ Boolean checked)
{
    SetTextViewLabel(checked);
    return ICheckable::Probe(mSwitch)->SetChecked(checked);
}

ECode SwitchBar::SetCheckedInternal(
    /* [in] */ Boolean checked)
{
    SetTextViewLabel(checked);
    return ((CToggleSwitch*)mSwitch.Get())->SetCheckedInternal(checked);
}

ECode SwitchBar::IsChecked(
    /* [out] */ Boolean* result)
{
    return ICheckable::Probe(mSwitch)->IsChecked(result);
}

ECode SwitchBar::SetEnabled(
    /* [in] */ Boolean enabled)
{
    LinearLayout::SetEnabled(enabled);
    IView::Probe(mTextView)->SetEnabled(enabled);
    return IView::Probe(mSwitch)->SetEnabled(enabled);
}

AutoPtr<IToggleSwitch> SwitchBar::GetSwitch()
{
    return mSwitch;
}

ECode SwitchBar::Show()
{
    if (!IsShowing()) {
        SetVisibility(IView::VISIBLE);
        return ICompoundButton::Probe(mSwitch)->SetOnCheckedChangeListener(this);
    }
    return NOERROR;
}

ECode SwitchBar::Hide()
{
    if (IsShowing()) {
        SetVisibility(IView::GONE);
        return ICompoundButton::Probe(mSwitch)->SetOnCheckedChangeListener(NULL);
    }
    return NOERROR;
}

Boolean SwitchBar::IsShowing()
{
    Int32 visibility;
    GetVisibility(&visibility);
    return (visibility == IView::VISIBLE);
}

ECode SwitchBar::OnClick(
    /* [in] */ IView* v)
{
    Boolean res;
    ICheckable::Probe(mSwitch)->IsChecked(&res);
    Boolean isChecked = !res;
    return SetChecked(isChecked);
}

ECode SwitchBar::PropagateChecked(
    /* [in] */ Boolean isChecked)
{
    Int32 count;
    mSwitchChangeListeners->GetSize(&count);
    for (Int32 n = 0; n < count; n++) {
        AutoPtr<IInterface> obj;
        mSwitchChangeListeners->Get(n, (IInterface**)&obj);
        ISwitchBarOnSwitchChangeListener::Probe(obj)->OnSwitchChanged(ISwitch::Probe(mSwitch), isChecked);
    }
    return NOERROR;
}

ECode SwitchBar::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return PropagateChecked(isChecked);
}

ECode SwitchBar::AddOnSwitchChangeListener(
    /* [in] */ ISwitchBarOnSwitchChangeListener* listener)
{
    Boolean res;
    if (mSwitchChangeListeners->Contains(listener, &res), res) {
        Slogger::E("SwitchBar", "Cannot add twice the same OnSwitchChangeListener");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Cannot add twice the same OnSwitchChangeListener");
    }
    return mSwitchChangeListeners->Add(listener);
}

ECode SwitchBar::RemoveOnSwitchChangeListener(
    /* [in] */ ISwitchBarOnSwitchChangeListener* listener)
{
    Boolean res;
    if (mSwitchChangeListeners->Contains(listener, &res), !res) {
        Slogger::E("SwitchBar", "Cannot remove OnSwitchChangeListener");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Cannot remove OnSwitchChangeListener");
    }
    return mSwitchChangeListeners->Remove(listener);
}

AutoPtr<IParcelable> SwitchBar::OnSaveInstanceState()
{
    AutoPtr<IParcelable> superState = LinearLayout::OnSaveInstanceState();

    AutoPtr<ISwitchBarSavedState> ss;
    CSwitchBarSavedState::New(superState, (ISwitchBarSavedState**)&ss);
    AutoPtr<SavedState> _ss = (SavedState*)ss.Get();
    ICheckable::Probe(mSwitch)->IsChecked(&_ss->mChecked);
    _ss->mVisible = IsShowing();
    return IParcelable::Probe(ss);
}

void SwitchBar::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<ISwitchBarSavedState> is = ISwitchBarSavedState::Probe(state);
    AutoPtr<SavedState> ss = (SavedState*)is.Get();
    AutoPtr<IParcelable> p;
    ss->GetSuperState((IParcelable**)&p);
    LinearLayout::OnRestoreInstanceState(p);

    ((CToggleSwitch*)mSwitch.Get())->SetCheckedInternal(ss->mChecked);
    SetTextViewLabel(ss->mChecked);
    SetVisibility(ss->mVisible ? IView::VISIBLE : IView::GONE);
    ICompoundButton::Probe(mSwitch)->SetOnCheckedChangeListener(ss->mVisible ? this : NULL);

    RequestLayout();
}

} // namespace Widget
} // namespace Settings
} // namespace Droid
} // namespace Elastos
