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

#include "elastos/droid/settings/widget/CSwitchBar.h"
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

AutoPtr< ArrayOf<Int32> > CSwitchBar::MARGIN_ATTRIBUTES = InitMARGIN_ATTRIBUTES();

//===============================================================================
//                  CSwitchBar::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL_2(CSwitchBar::InnerListener, Object, IViewOnClickListener, ICompoundButtonOnCheckedChangeListener)

CSwitchBar::InnerListener::InnerListener(
    /* [in] */ CSwitchBar* host)
    : mHost(host)
{}

ECode CSwitchBar::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

ECode CSwitchBar::InnerListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnCheckedChanged(buttonView, isChecked);
}

//===============================================================================
//                  CSwitchBar::SavedState
//===============================================================================

CAR_INTERFACE_IMPL(CSwitchBar::SavedState, View::BaseSavedState, ISwitchBarSavedState);

CSwitchBar::SavedState::SavedState()
    : mChecked(FALSE)
    , mVisible(FALSE)
{}

ECode CSwitchBar::SavedState::constructor()
{
    return View::BaseSavedState::constructor();
}

ECode CSwitchBar::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return View::BaseSavedState::constructor(superState);
}

ECode CSwitchBar::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    View::BaseSavedState::ReadFromParcel(source);
    source->ReadBoolean(&mChecked);
    source->ReadBoolean(&mVisible);
    return NOERROR;
}

ECode CSwitchBar::SavedState::WriteToParcel(
    /* [in] */ IParcel* out)
{
    View::BaseSavedState::WriteToParcel(out);
    out->WriteBoolean(mChecked);
    out->WriteBoolean(mVisible);
    return NOERROR;
}

ECode CSwitchBar::SavedState::ToString(
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
//                  CSwitchBar::SwitchBarOnSwitchChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(CSwitchBar::SwitchBarOnSwitchChangeListener, Object, ISwitchBarOnSwitchChangeListener);

CSwitchBar::SwitchBarOnSwitchChangeListener::SwitchBarOnSwitchChangeListener(
    /* [in] */ CSwitchBar* host)
    : mHost(host)
{}

CSwitchBar::SwitchBarOnSwitchChangeListener::~SwitchBarOnSwitchChangeListener()
{}

ECode CSwitchBar::SwitchBarOnSwitchChangeListener::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    return mHost->SetTextViewLabel(isChecked);
}

//===============================================================================
//                  CSwitchBar
//===============================================================================

CAR_INTERFACE_IMPL(CSwitchBar, LinearLayout, ISwitchBar)

CAR_OBJECT_IMPL(CSwitchBar)

CSwitchBar::CSwitchBar()
{}

CSwitchBar::~CSwitchBar()
{}

ECode CSwitchBar::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CSwitchBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CSwitchBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CSwitchBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    CArrayList::New((IArrayList**)&mSwitchChangeListeners);
    mListener = new InnerListener(this);

    LinearLayout::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> tmp;
    inflater->Inflate(R::layout::switch_bar, this, (IView**)&tmp);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, MARGIN_ATTRIBUTES, (ITypedArray**)&a);
    Float value;
    a->GetDimension(0, 0, &value);
    Int32 switchBarMarginStart = (Int32)value;
    a->GetDimension(1, 0, &value);
    Int32 switchBarMarginEnd = (Int32)value;
    a->Recycle();

    AutoPtr<IView> textViewTmp;
    FindViewById(R::id::switch_text, (IView**)&textViewTmp);
    mTextView = ITextView::Probe(textViewTmp);
    mTextView->SetText(R::string::switch_off_text);
    AutoPtr<IViewGroupLayoutParams> params;
    textViewTmp->GetLayoutParams((IViewGroupLayoutParams**)&params);
    IViewGroupMarginLayoutParams* lp = IViewGroupMarginLayoutParams::Probe(params);
    lp->SetMarginStart(switchBarMarginStart);

    AutoPtr<IView> switchTmp;
    FindViewById(R::id::switch_widget, (IView**)&switchTmp);
    mSwitch = IToggleSwitch::Probe(switchTmp);
    // Prevent OnSaveInstanceState() to be called as we are managing the state of the Switch
    // on our own
    switchTmp->SetSaveEnabled(FALSE);
    params = NULL;
    switchTmp->GetLayoutParams((IViewGroupLayoutParams**)&params);
    lp = IViewGroupMarginLayoutParams::Probe(params);
    lp->SetMarginEnd(switchBarMarginEnd);

    AutoPtr<SwitchBarOnSwitchChangeListener> listener = new SwitchBarOnSwitchChangeListener(this);
    AddOnSwitchChangeListener(listener);

    SetOnClickListener(mListener);

    // Default is hide
    SetVisibility(IView::GONE);
    return NOERROR;
}

ECode CSwitchBar::SetTextViewLabel(
    /* [in] */ Boolean isChecked)
{
    return mTextView->SetText(isChecked ? R::string::switch_on_text : R::string::switch_off_text);
}

ECode CSwitchBar::SetChecked(
    /* [in] */ Boolean checked)
{
    SetTextViewLabel(checked);
    return ICheckable::Probe(mSwitch)->SetChecked(checked);
}

ECode CSwitchBar::SetCheckedInternal(
    /* [in] */ Boolean checked)
{
    SetTextViewLabel(checked);
    return ((CToggleSwitch*)mSwitch.Get())->SetCheckedInternal(checked);
}

ECode CSwitchBar::IsChecked(
    /* [out] */ Boolean* result)
{
    return ICheckable::Probe(mSwitch)->IsChecked(result);
}

ECode CSwitchBar::SetEnabled(
    /* [in] */ Boolean enabled)
{
    LinearLayout::SetEnabled(enabled);
    IView::Probe(mTextView)->SetEnabled(enabled);
    return IView::Probe(mSwitch)->SetEnabled(enabled);
}

AutoPtr<IToggleSwitch> CSwitchBar::GetSwitch()
{
    return mSwitch;
}

ECode CSwitchBar::Show()
{
    if (!IsShowing()) {
        SetVisibility(IView::VISIBLE);
        return ICompoundButton::Probe(mSwitch)->SetOnCheckedChangeListener(mListener);
    }
    return NOERROR;
}

ECode CSwitchBar::Hide()
{
    if (IsShowing()) {
        SetVisibility(IView::GONE);
        return ICompoundButton::Probe(mSwitch)->SetOnCheckedChangeListener(NULL);
    }
    return NOERROR;
}

Boolean CSwitchBar::IsShowing()
{
    Int32 visibility;
    GetVisibility(&visibility);
    return (visibility == IView::VISIBLE);
}

ECode CSwitchBar::OnClick(
    /* [in] */ IView* v)
{
    Boolean res;
    ICheckable::Probe(mSwitch)->IsChecked(&res);
    Boolean isChecked = !res;
    return SetChecked(isChecked);
}

ECode CSwitchBar::PropagateChecked(
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

ECode CSwitchBar::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return PropagateChecked(isChecked);
}

ECode CSwitchBar::AddOnSwitchChangeListener(
    /* [in] */ ISwitchBarOnSwitchChangeListener* listener)
{
    Boolean res;
    if (mSwitchChangeListeners->Contains(listener, &res), res) {
        Slogger::E("CSwitchBar", "Cannot add twice the same OnSwitchChangeListener");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Cannot add twice the same OnSwitchChangeListener");
    }
    return mSwitchChangeListeners->Add(listener);
}

ECode CSwitchBar::RemoveOnSwitchChangeListener(
    /* [in] */ ISwitchBarOnSwitchChangeListener* listener)
{
    Boolean res;
    if (mSwitchChangeListeners->Contains(listener, &res), !res) {
        Slogger::E("CSwitchBar", "Cannot remove OnSwitchChangeListener");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Cannot remove OnSwitchChangeListener");
    }
    return mSwitchChangeListeners->Remove(listener);
}

AutoPtr<IParcelable> CSwitchBar::OnSaveInstanceState()
{
    AutoPtr<IParcelable> superState = LinearLayout::OnSaveInstanceState();

    AutoPtr<ISwitchBarSavedState> ss;
    CSwitchBarSavedState::New(superState, (ISwitchBarSavedState**)&ss);
    AutoPtr<SavedState> _ss = (SavedState*)ss.Get();
    ICheckable::Probe(mSwitch)->IsChecked(&_ss->mChecked);
    _ss->mVisible = IsShowing();
    return IParcelable::Probe(ss);
}

ECode CSwitchBar::OnRestoreInstanceState(
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
    ICompoundButton::Probe(mSwitch)->SetOnCheckedChangeListener(ss->mVisible ? mListener : NULL);

    RequestLayout();
    return NOERROR;
}

} // namespace Widget
} // namespace Settings
} // namespace Droid
} // namespace Elastos
