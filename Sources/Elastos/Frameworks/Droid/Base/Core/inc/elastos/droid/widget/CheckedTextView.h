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

#ifndef __ELASTOS_DROID_WIDGET_CHECKEDTEXTVIEW_H__
#define __ELASTOS_DROID_WIDGET_CHECKEDTEXTVIEW_H__

#include "elastos/droid/widget/TextView.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * An extension to TextView that supports the {@link android.widget.Checkable} interface.
 * This is useful when used in a {@link android.widget.ListView ListView} where the it's
 * {@link android.widget.ListView#setChoiceMode(Int32) setChoiceMode} has been set to
 * something other than {@link android.widget.ListView#CHOICE_MODE_NONE CHOICE_MODE_NONE}.
 *
 */
class CheckedTextView
    : public TextView
    , public ICheckable
    , public ICheckedTextView
{
public:
    CAR_INTERFACE_DECL()

    CheckedTextView();

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

    CARAPI Toggle();

    CARAPI IsChecked(
        /* [out] */ Boolean* isChecked);

    /**
     * <p>Changes the checked state of this text view.</p>
     *
     * @param checked true to check the text, false to uncheck it
     */
    CARAPI SetChecked(
        /* [in] */ Boolean checked);


    /**
     * Set the checkmark to a given Drawable, identified by its resourece id. This will be drawn
     * when {@link #isChecked()} is true.
     *
     * @param resid The Drawable to use for the checkmark.
     */
    CARAPI SetCheckMarkDrawable(
        /* [in] */ Int32 resid);

    /**
     * Set the checkmark to a given Drawable. This will be drawn when {@link #isChecked()} is true.
     *
     * @param d The Drawable to use for the checkmark.
     */
    CARAPI SetCheckMarkDrawable(
        /* [in] */ IDrawable* d);

    CARAPI GetCheckMarkDrawable(
        /* [out] */ IDrawable** drawable);

    CARAPI SetCheckMarkTintList(
        /* [in] */ IColorStateList* tint);

    CARAPI GetCheckMarkTintList(
        /* [out] */ IColorStateList** tint);

    CARAPI SetCheckMarkTintMode(
        /* [in] */ PorterDuffMode tintMode);

    CARAPI GetCheckMarkTintMode(
        /* [out] */ PorterDuffMode* tintMode);

    CARAPI JumpDrawablesToCurrentState();

    CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI OnCreateDrawableState(
        /* [in] */ Int32 extraSpace,
        /* [out] */ ArrayOf<Int32>** drawableState);

    virtual CARAPI DrawableStateChanged();

    virtual CARAPI OnRtlPropertiesChanged(
        /* [in] */ Int32 layoutDirection);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    //@Override
    CARAPI_(void) InternalSetPadding(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

private:
    CARAPI_(void) UpdatePadding();

    CARAPI_(void) SetBasePadding(
        /* [in] */ Boolean isLayoutRtl);

    CARAPI_(void) ApplyCheckMarkTint();

    CARAPI_(Boolean) IsCheckMarkAtStart();

private:
    Boolean mChecked;

    Int32 mCheckMarkResource;
    AutoPtr<IDrawable> mCheckMarkDrawable;
    AutoPtr<IColorStateList> mCheckMarkTintList;
    PorterDuffMode mCheckMarkTintMode;
    Boolean mHasCheckMarkTint;
    Boolean mHasCheckMarkTintMode;

    Int32 mBasePadding;
    Int32 mCheckMarkWidth;
    Int32 mCheckMarkGravity;

    Boolean mNeedRequestlayout;

    static AutoPtr<ArrayOf<Int32> > CHECKED_STATE_SET;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos
#endif
