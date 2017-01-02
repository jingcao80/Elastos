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

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/ToggleButton.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Graphics::Drawable::ILayerDrawable;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::R;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 ToggleButton::NO_ALPHA = 0xFF;

CAR_INTERFACE_IMPL(ToggleButton, CompoundButton, IToggleButton);

ToggleButton::ToggleButton()
    : mDisabledAlpha(0)
{}

ECode ToggleButton::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ToggleButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::buttonStyleToggle);
}

ECode ToggleButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ToggleButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    CompoundButton::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ToggleButton);
    AutoPtr<ITypedArray> a;
    ASSERT_SUCCEEDED(context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    a->GetText(R::styleable::ToggleButton_textOn, (ICharSequence**)&mTextOn);
    a->GetText(R::styleable::ToggleButton_textOff, (ICharSequence**)&mTextOff);
    a->GetFloat(R::styleable::ToggleButton_disabledAlpha, 0.5f, &mDisabledAlpha);
    SyncTextState();
    a->Recycle();
    return NOERROR;
}

ECode ToggleButton::SetChecked(
    /* [in] */ Boolean checked)
{
    CompoundButton::SetChecked(checked);
    SyncTextState();

    return NOERROR;
}

void ToggleButton::SyncTextState()
{
    Boolean checked;
    IsChecked(&checked);
    if (checked && mTextOn != NULL) {
        SetText(mTextOn);
    }
    else if (!checked && mTextOff != NULL) {
        SetText(mTextOff);
    }
}

ECode ToggleButton::GetTextOn(
    /* [out] */ ICharSequence** on)
{
    VALIDATE_NOT_NULL(on);
    *on = mTextOn;
    REFCOUNT_ADD(*on);
    return NOERROR;
}

ECode ToggleButton::SetTextOn(
    /* [in] */ ICharSequence* textOn)
{
    mTextOn = textOn;

    return NOERROR;
}

ECode ToggleButton::GetTextOff(
    /* [out] */ ICharSequence** off)
{
    VALIDATE_NOT_NULL(off);
    *off = mTextOff;
    REFCOUNT_ADD(*off);
    return NOERROR;
}

ECode ToggleButton::SetTextOff(
    /* [in] */ ICharSequence* textOff)
{
    mTextOff = textOff;

    return NOERROR;
}

ECode ToggleButton::OnFinishInflate()
{
    CompoundButton::OnFinishInflate();

    AutoPtr<IDrawable> drawable;
    GetBackground((IDrawable**)&drawable);
    UpdateReferenceToIndicatorDrawable(drawable);

    return NOERROR;
}

ECode ToggleButton::SetBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    CompoundButton::SetBackgroundDrawable(d);

    UpdateReferenceToIndicatorDrawable(d);

    return NOERROR;
}

void ToggleButton::UpdateReferenceToIndicatorDrawable(
    /* [in] */ IDrawable* backgroundDrawable)
{
    AutoPtr<ILayerDrawable> layerDrawable = ILayerDrawable::Probe(backgroundDrawable);
    if (layerDrawable) {
        layerDrawable->FindDrawableByLayerId(
                R::id::toggle, (IDrawable**)&mIndicatorDrawable);
    }
    else {
        mIndicatorDrawable = NULL;
    }
}

ECode ToggleButton::DrawableStateChanged()
{
    CompoundButton::DrawableStateChanged();

    if (mIndicatorDrawable != NULL) {
        Boolean enabled = FALSE;
        IsEnabled(&enabled);
        mIndicatorDrawable->SetAlpha(enabled ? NO_ALPHA : (Int32)(NO_ALPHA * mDisabledAlpha));
    }

    return NOERROR;
}

ECode ToggleButton::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    CompoundButton::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> s;
    CString::New(String("CToggleButton"), (ICharSequence**)&s);
    IAccessibilityRecord::Probe(event)->SetClassName(s);
    return NOERROR;
}

ECode ToggleButton::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    CompoundButton::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> s;
    CString::New(String("CToggleButton"), (ICharSequence**)&s);
    info->SetClassName(s);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
