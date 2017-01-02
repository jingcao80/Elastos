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

#ifndef __ELASTOS_DROID_WIDGET_TOGGLEBUTTON_H__
#define __ELASTOS_DROID_WIDGET_TOGGLEBUTTON_H__

#include "elastos/droid/widget/CompoundButton.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;

namespace Elastos {
namespace Droid {
namespace Widget {

class ToggleButton
    : public CompoundButton
    , public IToggleButton
{
public:
    CAR_INTERFACE_DECL()

    ToggleButton();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    //@Override
    virtual CARAPI SetChecked(
        /* [in] */ Boolean checked);

    virtual CARAPI GetTextOn(
        /* [out] */ ICharSequence** on);

    virtual CARAPI SetTextOn(
        /* [in] */ ICharSequence* textOn);

    virtual CARAPI GetTextOff(
        /* [out] */ ICharSequence** off);

    virtual CARAPI SetTextOff(
        /* [in] */ ICharSequence* textOff);

    //@Override
    virtual CARAPI SetBackgroundDrawable(
        /* [in] */ IDrawable* d);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    //@Override
    virtual CARAPI OnFinishInflate();

    //@Override
    virtual CARAPI DrawableStateChanged();

private:
    CARAPI_(void) UpdateReferenceToIndicatorDrawable(
        /* [in] */ IDrawable* backgroundDrawable);

    CARAPI_(void) SyncTextState();

private:
    AutoPtr<ICharSequence> mTextOn;
    AutoPtr<ICharSequence> mTextOff;

    AutoPtr<IDrawable> mIndicatorDrawable;

    static const Int32 NO_ALPHA;
    Float mDisabledAlpha;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_TOGGLEBUTTON_H__
