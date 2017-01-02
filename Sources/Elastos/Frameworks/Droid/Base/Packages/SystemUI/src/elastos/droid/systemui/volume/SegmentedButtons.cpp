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

#include "elastos/droid/systemui/volume/SegmentedButtons.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/systemui/volume/Interaction.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Graphics::CTypefaceHelper;
using Elastos::Droid::Graphics::ITypefaceHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

//==========================================
// SegmentedButtons::MyClick
//==========================================

CAR_INTERFACE_IMPL(SegmentedButtons::MyClick, Object, IViewOnClickListener)

SegmentedButtons::MyClick::MyClick(
    /* [in] */ SegmentedButtons* host)
    : mHost(host)
{}

ECode SegmentedButtons::MyClick::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IInterface> obj;
    v->GetTag((IInterface**)&obj);
    mHost->SetSelectedValue(obj);
    return NOERROR;
}

//==========================================
// SegmentedButtons::MyInteractionCallback
//==========================================

CAR_INTERFACE_IMPL(SegmentedButtons::MyInteractionCallback, Object, IInteractionCallback)

SegmentedButtons::MyInteractionCallback::MyInteractionCallback(
    /* [in] */ SegmentedButtons* host)
    : mHost(host)
{}

ECode SegmentedButtons::MyInteractionCallback::OnInteraction()
{
    mHost->FireInteraction();
    return NOERROR;
}

//==========================================
// SegmentedButtons
//==========================================

static AutoPtr<ITypeface> InitMEDIUM()
{
    AutoPtr<ITypefaceHelper> th;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&th);
    AutoPtr<ITypeface> tf;
    th->Create(String("sans-serif-medium"), ITypeface::NORMAL, (ITypeface**)&tf);
    return tf;
}

static AutoPtr<ITypeface> InitBOLD()
{
    AutoPtr<ITypefaceHelper> th;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&th);
    AutoPtr<ITypeface> tf;
    th->Create(String("sans-serif"), ITypeface::BOLD, (ITypeface**)&tf);
    return tf;
}

const AutoPtr<ITypeface> SegmentedButtons::MEDIUM = InitMEDIUM();
const AutoPtr<ITypeface> SegmentedButtons::BOLD = InitBOLD();
const Int32 SegmentedButtons::LABEL_RES_KEY = R::id::label;

CAR_INTERFACE_IMPL(SegmentedButtons, LinearLayout, ISegmentedButtons)

SegmentedButtons::SegmentedButtons()
{
}

ECode SegmentedButtons::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);

    mClick = new MyClick(this);
    LayoutInflater::From(mContext, (ILayoutInflater**)&mInflater);
    SetOrientation(ILinearLayout::HORIZONTAL);
    return NOERROR;
}

ECode SegmentedButtons::SetCallback(
    /* [in] */ ISegmentedButtonsCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode SegmentedButtons::GetSelectedValue(
    /* [out]] */ IInterface** sv)
{
    VALIDATE_NOT_NULL(sv)
    *sv = mSelectedValue;
    REFCOUNT_ADD(*sv)
    return NOERROR;
}

ECode SegmentedButtons::SetSelectedValue(
    /* [in] */ IInterface* value)
{
    if (Object::Equals(value, mSelectedValue.Get())) return E_NULL_POINTER_EXCEPTION;
    mSelectedValue = value;
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        AutoPtr<IInterface> tag;
        v->GetTag((IInterface**)&tag);
        Boolean selected = Object::Equals(mSelectedValue, tag);
        v->SetSelected(selected);
        ITextView::Probe(v)->SetTypeface(selected ? BOLD : MEDIUM);
    }
    FireOnSelected();
    return NOERROR;
}

ECode SegmentedButtons::AddButton(
    /* [in] */ Int32 labelResId,
    /* [in] */ IInterface* value)
{
    AutoPtr<IView> view;
    mInflater->Inflate(R::layout::segmented_button, this, FALSE, (IView**)&view);

    AutoPtr<IInteger32> i = CoreUtils::Convert(labelResId);
    view->SetTag(LABEL_RES_KEY, i);
    ITextView::Probe(view)->SetText(labelResId);

    AutoPtr<IViewGroupLayoutParams> lp;
    view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    Int32 count;
    GetChildCount(&count);
    if (count == 0) {
        IViewGroupMarginLayoutParams::Probe(lp)->SetRightMargin(0);
        IViewGroupMarginLayoutParams::Probe(lp)->SetLeftMargin(0); // first button has no margin
    }
    view->SetLayoutParams(lp);
    AddView(view);
    view->SetTag(value);
    view->SetOnClickListener(mClick);
    AutoPtr<MyInteractionCallback> icb = new MyInteractionCallback(this);
    Interaction::Register(view, icb);
    return NOERROR;
}

ECode SegmentedButtons::UpdateLocale()
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> c;
        GetChildAt(i, (IView**)&c);
        AutoPtr<IInterface> tag;
        c->GetTag(LABEL_RES_KEY, (IInterface**)&tag);
        AutoPtr<IInteger32> i32 = IInteger32::Probe(tag);
        Int32 labelResId;
        i32->GetValue(&labelResId);
        ITextView::Probe(c)->SetText(labelResId);
    }
    return NOERROR;
}

void SegmentedButtons::FireOnSelected()
{
    if (mCallback != NULL) {
        mCallback->OnSelected(mSelectedValue);
    }
}

void SegmentedButtons::FireInteraction()
{
    if (mCallback != NULL) {
        IInteractionCallback::Probe(mCallback)->OnInteraction();
    }
}

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos