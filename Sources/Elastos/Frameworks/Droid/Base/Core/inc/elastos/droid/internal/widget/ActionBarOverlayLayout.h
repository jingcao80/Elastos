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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_ACTIONBAROVERLAYLAYOUT_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_ACTIONBAROVERLAYLAYOUT_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/utility/Int32Property.h"
#include "elastos/droid/view/ViewGroup.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::View::Menu::IMenuPresenterCallback;
using Elastos::Droid::Internal::Widget::IActionBarContainer;
using Elastos::Droid::Internal::Widget::IActionBarOverlayLayout;
using Elastos::Droid::Internal::Widget::IActionBarVisibilityCallback;
using Elastos::Droid::Internal::Widget::IDecorContentParent;
using Elastos::Droid::Internal::Widget::IDecorToolbar;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Int32Property;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::Widget::IOverScroller;
using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
  * Special layout for the containing of an overlay action bar (and its
  * content) to correctly handle fitting system windows when the content
  * has request that its layout ignore them.
  */
class ActionBarOverlayLayout
    : public ViewGroup
    , public IDecorContentParent
    , public IActionBarOverlayLayout
{
public:
    class ActionBarOverlayLayoutLayoutParams
        : public ViewGroup::MarginLayoutParams
        , public IActionBarOverlayLayoutLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()
    };

private:
    class TopAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TopAnimatorListenerAdapter(
            /* [in] */ ActionBarOverlayLayout* owner);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        ActionBarOverlayLayout* mOwner;
    };

    class BottomAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        BottomAnimatorListenerAdapter(
            /* [in] */ ActionBarOverlayLayout* owner);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        ActionBarOverlayLayout* mOwner;
    };

    class InnerRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRunnable(
            /* [in] */ ActionBarOverlayLayout* owner);

        virtual CARAPI Run();

    private:
        ActionBarOverlayLayout* mOwner;
    };

    class InnerRunnable1
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRunnable1(
            /* [in] */ ActionBarOverlayLayout* owner);

        virtual CARAPI Run();

    private:
        ActionBarOverlayLayout* mOwner;
    };

    class InnerIntProperty
        : public Int32Property
    {
    public:
        InnerIntProperty();

        // @Override
        CARAPI SetValue(
            /* [in] */ IInterface* object,
            /* [in] */ Int32 value);

        // @Override
        CARAPI Get(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** result);
    };

public:
    CAR_INTERFACE_DECL()

    ActionBarOverlayLayout();

    ~ActionBarOverlayLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI SetActionBarVisibilityCallback(
        /* [in] */ IActionBarVisibilityCallback* cb);

    virtual CARAPI SetOverlayMode(
        /* [in] */ Boolean overlayMode);

    virtual CARAPI IsInOverlayMode(
        /* [out] */ Boolean* result);

    virtual CARAPI SetHasNonEmbeddedTabs(
        /* [in] */ Boolean hasNonEmbeddedTabs);

    virtual CARAPI SetShowingForActionMode(
        /* [in] */ Boolean showing);

    // @Override
    CARAPI OnWindowSystemUiVisibilityChanged(
        /* [in] */ Int32 visible);

    // @Override
    CARAPI OnApplyWindowInsets(
        /* [in] */ IWindowInsets* insets,
        /* [out] */ IWindowInsets** result);

    // @Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** result);

    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* c);

    // @Override
    CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnStartNestedScroll(
        /* [in] */ IView* child,
        /* [in] */ IView* target,
        /* [in] */ Int32 axes,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnNestedScrollAccepted(
        /* [in] */ IView* child,
        /* [in] */ IView* target,
        /* [in] */ Int32 axes);

    // @Override
    CARAPI OnNestedScroll(
        /* [in] */ IView* target,
        /* [in] */ Int32 dxConsumed,
        /* [in] */ Int32 dyConsumed,
        /* [in] */ Int32 dxUnconsumed,
        /* [in] */ Int32 dyUnconsumed);

    // @Override
    CARAPI OnStopNestedScroll(
        /* [in] */ IView* target);

    // @Override
    CARAPI OnNestedFling(
        /* [in] */ IView* target,
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [in] */ Boolean consumed,
        /* [out] */ Boolean* result);

    virtual CARAPI PullChildren();

    virtual CARAPI SetHideOnContentScrollEnabled(
        /* [in] */ Boolean hideOnContentScroll);

    virtual CARAPI IsHideOnContentScrollEnabled(
        /* [out] */ Boolean* result);

    virtual CARAPI GetActionBarHideOffset(
        /* [out] */ Int32* result);

    virtual CARAPI SetActionBarHideOffset(
        /* [in] */ Int32 offset);

    // @Override
    CARAPI SetWindowCallback(
        /* [in] */ IWindowCallback* cb);

    // @Override
    CARAPI SetWindowTitle(
        /* [in] */ ICharSequence* title);

    // @Override
    CARAPI GetTitle(
        /* [out] */ ICharSequence** result);

    // @Override
    CARAPI InitFeature(
        /* [in] */ Int32 windowFeature);

    // @Override
    CARAPI SetUiOptions(
        /* [in] */ Int32 uiOptions);

    // @Override
    CARAPI HasIcon(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HasLogo(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetIcon(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetIcon(
        /* [in] */ IDrawable* d);

    // @Override
    CARAPI SetLogo(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI CanShowOverflowMenu(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsOverflowMenuShowing(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsOverflowMenuShowPending(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ShowOverflowMenu(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HideOverflowMenu(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetMenuPrepared();

    // @Override
    CARAPI SetMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuPresenterCallback* cb);

    // @Override
    CARAPI SaveToolbarHierarchyState(
        /* [in] */ ISparseArray* toolbarStates);

    // @Override
    CARAPI RestoreToolbarHierarchyState(
        /* [in] */ ISparseArray* toolbarStates);

    // @Override
    CARAPI DismissPopups();

protected:
    // @Override
    CARAPI OnDetachedFromWindow();

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    // @Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

    // @Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    // @Override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

private:
    CARAPI_(void) Init(
        /* [in] */ IContext* context);

    CARAPI_(Boolean) ApplyInsets(
        /* [in] */ IView* view,
        /* [in] */ IRect* insets,
        /* [in] */ Boolean left,
        /* [in] */ Boolean top,
        /* [in] */ Boolean bottom,
        /* [in] */ Boolean right);

    CARAPI GetDecorToolbar(
        /* [in] */ IView* view,
        /* [out] */ IDecorToolbar** result);

    CARAPI_(void) HaltActionBarHideOffsetAnimations();

    CARAPI_(void) PostRemoveActionBarHideOffset();

    CARAPI_(void) PostAddActionBarHideOffset();

    CARAPI_(void) RemoveActionBarHideOffset();

    CARAPI_(void) AddActionBarHideOffset();

    CARAPI_(Boolean) ShouldHideActionBarOnFling(
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY);

public:
    static const AutoPtr<IProperty> ACTION_BAR_HIDE_OFFSET;
    static AutoPtr< ArrayOf<Int32> > ATTRS;

private:
    static const String TAG;
    Int32 mActionBarHeight;
    Int32 mWindowVisibility;
    AutoPtr<IView> mContent;
    AutoPtr<IActionBarContainer> mActionBarBottom;
    AutoPtr<IActionBarContainer> mActionBarTop;
    AutoPtr<IDecorToolbar> mDecorToolbar;
    AutoPtr<IDrawable> mWindowContentOverlay;
    Boolean mIgnoreWindowContentOverlay;
    Boolean mOverlayMode;
    Boolean mHasNonEmbeddedTabs;
    Boolean mHideOnContentScroll;
    Boolean mAnimatingForFling;
    Int32 mHideOnContentScrollReference;
    Int32 mLastSystemUiVisibility;
    /*const*/ AutoPtr<IRect> mBaseContentInsets;
    /*const*/ AutoPtr<IRect> mLastBaseContentInsets;
    /*const*/ AutoPtr<IRect> mContentInsets;
    /*const*/ AutoPtr<IRect> mBaseInnerInsets;
    /*const*/ AutoPtr<IRect> mInnerInsets;
    /*const*/ AutoPtr<IRect> mLastInnerInsets;
    AutoPtr<IActionBarVisibilityCallback> mActionBarVisibilityCallback;
    /*const*/ Int32 ACTION_BAR_ANIMATE_DELAY;
    // ms
    AutoPtr<IOverScroller> mFlingEstimator;
    AutoPtr<IViewPropertyAnimator> mCurrentActionBarTopAnimator;
    AutoPtr<IViewPropertyAnimator> mCurrentActionBarBottomAnimator;
    /*const*/ AutoPtr<IAnimatorListener> mTopAnimatorListener;
    /*const*/ AutoPtr<IAnimatorListener> mBottomAnimatorListener;
    /*const*/ AutoPtr<IRunnable> mRemoveActionBarHideOffset;
    /*const*/ AutoPtr<IRunnable> mAddActionBarHideOffset;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_ACTIONBAROVERLAYLAYOUT_H__