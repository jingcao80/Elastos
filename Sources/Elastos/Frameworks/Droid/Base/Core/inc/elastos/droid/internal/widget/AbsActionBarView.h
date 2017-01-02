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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_ABSACTIONBARVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_ABSACTIONBARVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ViewGroup.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Internal::Widget::IAbsActionBarView;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::Widget::IActionMenuPresenter;
using Elastos::Droid::Widget::IActionMenuView;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class AbsActionBarView
    : public ViewGroup
    , public IAbsActionBarView
{
protected:
    class VisibilityAnimListener
        : public Object
        , public IAnimatorListener
    {
    public:
        CAR_INTERFACE_DECL()

        VisibilityAnimListener(
            /* [in] */ AbsActionBarView* owner);

        virtual CARAPI WithFinalVisibility(
            /* [in] */ Int32 visibility,
            /* [out] */ IAnimatorListener** result);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animation);

    public:
        Int32 mFinalVisibility;

    private:
        AbsActionBarView* mOwner;
        Boolean mCanceled;
    };

private:
    class InnerShowOverflowMenuRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerShowOverflowMenuRunnable(
            /* [in] */ AbsActionBarView* owner);

        CARAPI Run();

    private:
        AbsActionBarView* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    AbsActionBarView();

    ~AbsActionBarView();

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

    /**
      * Sets whether the bar should be split right now, no questions asked.
      * @param split true if the bar should split
      */
    virtual CARAPI SetSplitToolbar(
        /* [in] */ Boolean split);

    /**
      * Sets whether the bar should split if we enter a narrow screen configuration.
      * @param splitWhenNarrow true if the bar should check to split after a config change
      */
    virtual CARAPI SetSplitWhenNarrow(
        /* [in] */ Boolean splitWhenNarrow);

    virtual CARAPI SetContentHeight(
        /* [in] */ Int32 height);

    virtual CARAPI GetContentHeight(
        /* [out] */ Int32* result);

    virtual CARAPI SetSplitView(
        /* [in] */ IViewGroup* splitView);

    /**
      * @return Current visibility or if animating, the visibility being animated to.
      */
    virtual CARAPI GetAnimatedVisibility(
        /* [out] */ Int32* result);

    virtual CARAPI AnimateToVisibility(
        /* [in] */ Int32 visibility);

    // @Override
    CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    virtual CARAPI ShowOverflowMenu(
        /* [out] */ Boolean* result);

    virtual CARAPI PostShowOverflowMenu();

    virtual CARAPI HideOverflowMenu(
        /* [out] */ Boolean* result);

    virtual CARAPI IsOverflowMenuShowing(
        /* [out] */ Boolean* result);

    virtual CARAPI IsOverflowMenuShowPending(
        /* [out] */ Boolean* result);

    virtual CARAPI IsOverflowReserved(
        /* [out] */ Boolean* result);

    virtual CARAPI CanShowOverflowMenu(
        /* [out] */ Boolean* result);

    virtual CARAPI DismissPopupMenus();

protected:
    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    virtual CARAPI_(Int32) MeasureChildView(
        /* [in] */ IView* child,
        /* [in] */ Int32 availableWidth,
        /* [in] */ Int32 childSpecHeight,
        /* [in] */ Int32 spacing);

    static CARAPI_(Int32) Next(
        /* [in] */ Int32 x,
        /* [in] */ Int32 val,
        /* [in] */ Boolean isRtl);

    virtual CARAPI_(Int32) PositionChild(
        /* [in] */ IView* child,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 contentHeight,
        /* [in] */ Boolean reverse);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

protected:
    AutoPtr<VisibilityAnimListener> mVisAnimListener;
    IContext* mPopupContext;
    Boolean mHolderPopupContext;
    AutoPtr<IActionMenuView> mMenuView;
    AutoPtr<IActionMenuPresenter> mActionMenuPresenter;
    AutoPtr<IViewGroup> mSplitView;
    Boolean mSplitActionBar;
    Boolean mSplitWhenNarrow;
    Int32 mContentHeight;
    AutoPtr<IAnimator> mVisibilityAnim;

private:
    static const AutoPtr<ITimeInterpolator> sAlphaInterpolator;
    static const Int32 FADE_DURATION;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_ABSACTIONBARVIEW_H__

