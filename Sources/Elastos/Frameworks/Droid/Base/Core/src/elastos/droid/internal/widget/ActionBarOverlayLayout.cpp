
#include "elastos/droid/internal/widget/ActionBarContextView.h"
#include "elastos/droid/internal/widget/ActionBarOverlayLayout.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/CWindowInsets.h"
#include "elastos/droid/widget/COverScroller.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Internal::Widget::ActionBarContextView;
using Elastos::Droid::Internal::Widget::EIID_IActionBarOverlayLayout;
using Elastos::Droid::Internal::Widget::EIID_IDecorContentParent;
using Elastos::Droid::Os::Build;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::CWindowInsets;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::Widget::COverScroller;
using Elastos::Droid::Widget::IToolbar;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//=====================================================================
//                 ActionBarOverlayLayout::LayoutParams
//=====================================================================
ActionBarOverlayLayout::LayoutParams::LayoutParams()
{
}

ECode ActionBarOverlayLayout::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // super(c, attrs);

    return ViewGroup::MarginLayoutParams::constructor(c, attrs);
}

ECode ActionBarOverlayLayout::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    // ==================before translated======================
    // super(width, height);

    return ViewGroup::MarginLayoutParams::constructor(width, height);
}

ECode ActionBarOverlayLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    // ==================before translated======================
    // super(source);

    return ViewGroup::MarginLayoutParams::constructor(source);
}

ECode ActionBarOverlayLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    // ==================before translated======================
    // super(source);

    return ViewGroup::MarginLayoutParams::constructor(source);
}

//=====================================================================
//         ActionBarOverlayLayout::InnerAnimatorListenerAdapter
//=====================================================================
ActionBarOverlayLayout::InnerAnimatorListenerAdapter::InnerAnimatorListenerAdapter(
    /* [in] */ ActionBarOverlayLayout* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode ActionBarOverlayLayout::InnerAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    // ==================before translated======================
    // mCurrentActionBarTopAnimator = null;
    // mAnimatingForFling = false;

    mOwner->mCurrentActionBarTopAnimator = NULL;
    mOwner->mAnimatingForFling = FALSE;
    return NOERROR;
}

ECode ActionBarOverlayLayout::InnerAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    // ==================before translated======================
    // mCurrentActionBarTopAnimator = null;
    // mAnimatingForFling = false;

    mOwner->mCurrentActionBarTopAnimator = NULL;
    mOwner->mAnimatingForFling = FALSE;
    return NOERROR;
}

//=====================================================================
//        ActionBarOverlayLayout::InnerAnimatorListenerAdapter1
//=====================================================================
ActionBarOverlayLayout::InnerAnimatorListenerAdapter1::InnerAnimatorListenerAdapter1(
    /* [in] */ ActionBarOverlayLayout* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode ActionBarOverlayLayout::InnerAnimatorListenerAdapter1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    // ==================before translated======================
    // mCurrentActionBarBottomAnimator = null;
    // mAnimatingForFling = false;

    mOwner->mCurrentActionBarBottomAnimator = NULL;
    mOwner->mAnimatingForFling = FALSE;
    return NOERROR;
}

ECode ActionBarOverlayLayout::InnerAnimatorListenerAdapter1::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    // ==================before translated======================
    // mCurrentActionBarBottomAnimator = null;
    // mAnimatingForFling = false;

    mOwner->mCurrentActionBarBottomAnimator = NULL;
    mOwner->mAnimatingForFling = FALSE;
    return NOERROR;
}

//=====================================================================
//                ActionBarOverlayLayout::InnerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(ActionBarOverlayLayout::InnerRunnable, Object, IRunnable)

ActionBarOverlayLayout::InnerRunnable::InnerRunnable(
    /* [in] */ ActionBarOverlayLayout* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode ActionBarOverlayLayout::InnerRunnable::Run()
{
    // ==================before translated======================
    // haltActionBarHideOffsetAnimations();
    // mCurrentActionBarTopAnimator = mActionBarTop.animate().translationY(0)
    //         .setListener(mTopAnimatorListener);
    // if (mActionBarBottom != null && mActionBarBottom.getVisibility() != GONE) {
    //     mCurrentActionBarBottomAnimator = mActionBarBottom.animate().translationY(0)
    //             .setListener(mBottomAnimatorListener);
    // }

    mOwner->HaltActionBarHideOffsetAnimations();
    AutoPtr<IViewPropertyAnimator> propertyAnimatorTop;
    IView::Probe(mOwner->mActionBarTop)->Animate((IViewPropertyAnimator**)&propertyAnimatorTop);
    propertyAnimatorTop->TranslationY(0);
    propertyAnimatorTop->SetListener(mOwner->mTopAnimatorListener);
    mOwner->mCurrentActionBarTopAnimator = propertyAnimatorTop;
    Int32 visibility = 0;
    IView::Probe(mOwner->mActionBarBottom)->GetVisibility(&visibility);
    if (mOwner->mActionBarBottom != NULL && visibility != IView::GONE) {
        AutoPtr<IViewPropertyAnimator> propertyAnimatorBottom;
        IView::Probe(mOwner->mActionBarBottom)->Animate((IViewPropertyAnimator**)&propertyAnimatorBottom);
        propertyAnimatorBottom->TranslationY(0);
        propertyAnimatorBottom->SetListener(mOwner->mBottomAnimatorListener);
        mOwner->mCurrentActionBarBottomAnimator = propertyAnimatorBottom;
    }
    return NOERROR;
}

//=====================================================================
//                ActionBarOverlayLayout::InnerRunnable1
//=====================================================================
CAR_INTERFACE_IMPL(ActionBarOverlayLayout::InnerRunnable1, Object, IRunnable)

ActionBarOverlayLayout::InnerRunnable1::InnerRunnable1(
    /* [in] */ ActionBarOverlayLayout* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode ActionBarOverlayLayout::InnerRunnable1::Run()
{
    // ==================before translated======================
    // haltActionBarHideOffsetAnimations();
    // mCurrentActionBarTopAnimator = mActionBarTop.animate()
    //         .translationY(-mActionBarTop.getHeight())
    //         .setListener(mTopAnimatorListener);
    // if (mActionBarBottom != null && mActionBarBottom.getVisibility() != GONE) {
    //     mCurrentActionBarBottomAnimator = mActionBarBottom.animate()
    //             .translationY(mActionBarBottom.getHeight())
    //             .setListener(mBottomAnimatorListener);
    // }

    mOwner->HaltActionBarHideOffsetAnimations();
    AutoPtr<IViewPropertyAnimator> propertyAnimatorTop;
    IView::Probe(mOwner->mActionBarTop)->Animate((IViewPropertyAnimator**)&propertyAnimatorTop);
    Int32 height = 0;
    IView::Probe(mOwner->mActionBarTop)->GetHeight(&height);
    propertyAnimatorTop->TranslationY(-height);
    propertyAnimatorTop->SetListener(mOwner->mTopAnimatorListener);
    mOwner->mCurrentActionBarTopAnimator = propertyAnimatorTop;

    Int32 visibility = 0;
    IView::Probe(mOwner->mActionBarBottom)->GetVisibility(&visibility);
    if (mOwner->mActionBarBottom != NULL && visibility != IView::GONE) {
        AutoPtr<IViewPropertyAnimator> propertyAnimatorBottom;
        IView::Probe(mOwner->mActionBarBottom)->Animate((IViewPropertyAnimator**)&propertyAnimatorBottom);
        Int32 height = 0;
        IView::Probe(mOwner->mActionBarBottom)->GetHeight(&height);
        propertyAnimatorBottom->TranslationY(height);
        propertyAnimatorBottom->SetListener(mOwner->mBottomAnimatorListener);
        mOwner->mCurrentActionBarBottomAnimator = propertyAnimatorBottom;
    }
    return NOERROR;
}

//=====================================================================
//               ActionBarOverlayLayout::InnerIntProperty
//=====================================================================
ActionBarOverlayLayout::InnerIntProperty::InnerIntProperty()
    : Int32Property(String(""))
{
}

ECode ActionBarOverlayLayout::InnerIntProperty::SetValue(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 value)
{
    VALIDATE_NOT_NULL(object);
    // ==================before translated======================
    // object.setActionBarHideOffset(value);

    IObject* objTmp = IObject::Probe(object);
    ActionBarOverlayLayout* layout = (ActionBarOverlayLayout*)objTmp;
    assert(layout);
    return layout->SetActionBarHideOffset(value);
}

ECode ActionBarOverlayLayout::InnerIntProperty::Get(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(object);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return object.getActionBarHideOffset();

    IObject* objTmp = IObject::Probe(object);
    ActionBarOverlayLayout* layout = (ActionBarOverlayLayout*)objTmp;
    assert(layout);
    Int32 offset = 0;
    layout->GetActionBarHideOffset(&offset);
    AutoPtr<IInteger32> resTmp = CoreUtils::Convert(offset);
    *result = TO_IINTERFACE(resTmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                        ActionBarOverlayLayout
//=====================================================================
const AutoPtr<IProperty> ActionBarOverlayLayout::ACTION_BAR_HIDE_OFFSET = ActionBarOverlayLayout::InitActinoBarHideOffset();
AutoPtr< ArrayOf<Int32> > ActionBarOverlayLayout::ATTRS = ActionBarOverlayLayout::MiddleInitAttrs();
const String ActionBarOverlayLayout::TAG("ActionBarOverlayLayout");

CAR_INTERFACE_IMPL(ActionBarOverlayLayout, ViewGroup, IActionBarOverlayLayout)

ActionBarOverlayLayout::ActionBarOverlayLayout()
    : mActionBarHeight(0)
    , mWindowVisibility(0)
    , mIgnoreWindowContentOverlay(FALSE)
    , mOverlayMode(FALSE)
    , mHasNonEmbeddedTabs(FALSE)
    , mHideOnContentScroll(FALSE)
    , mAnimatingForFling(FALSE)
    , mHideOnContentScrollReference(0)
    , mLastSystemUiVisibility(0)
    , ACTION_BAR_ANIMATE_DELAY(0)
{
}

ECode ActionBarOverlayLayout::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // super(context);
    // init(context);

    ViewGroup::constructor(context);
    Init(context);
    return NOERROR;
}

ECode ActionBarOverlayLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // super(context, attrs);
    // init(context);

    ViewGroup::constructor(context, attrs);
    Init(context);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetActionBarVisibilityCallback(
    /* [in] */ IActionBarVisibilityCallback* cb)
{
    VALIDATE_NOT_NULL(cb);
    // ==================before translated======================
    // mActionBarVisibilityCallback = cb;
    // if (getWindowToken() != null) {
    //     // This is being initialized after being added to a window;
    //     // make sure to update all state now.
    //     mActionBarVisibilityCallback.onWindowVisibilityChanged(mWindowVisibility);
    //     if (mLastSystemUiVisibility != 0) {
    //         int newVis = mLastSystemUiVisibility;
    //         onWindowSystemUiVisibilityChanged(newVis);
    //         requestApplyInsets();
    //     }
    // }

    mActionBarVisibilityCallback = cb;
    AutoPtr<IBinder> token;
    GetWindowToken((IBinder**)&token);
    if (token != NULL) {
        // This is being initialized after being added to a window;
        // make sure to update all state now.
        mActionBarVisibilityCallback->OnWindowVisibilityChanged(mWindowVisibility);
        if (mLastSystemUiVisibility != 0) {
            Int32 newVis = mLastSystemUiVisibility;
            OnWindowSystemUiVisibilityChanged(newVis);
            RequestApplyInsets();
        }
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetOverlayMode(
    /* [in] */ Boolean overlayMode)
{
    // ==================before translated======================
    // mOverlayMode = overlayMode;
    //
    // /*
    //  * Drawing the window content overlay was broken before K so starting to draw it
    //  * again unexpectedly will cause artifacts in some apps. They should fix it.
    //  */
    // mIgnoreWindowContentOverlay = overlayMode &&
    //         getContext().getApplicationInfo().targetSdkVersion <
    //                 Build.VERSION_CODES.KITKAT;

    mOverlayMode = overlayMode;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IApplicationInfo> applicationInfo;
    context->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    Int32 sdkVersion = 0;
    applicationInfo->GetTargetSdkVersion(&sdkVersion);
    mIgnoreWindowContentOverlay = overlayMode && sdkVersion < Build::VERSION_CODES::KITKAT;
    return NOERROR;
}

ECode ActionBarOverlayLayout::IsInOverlayMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mOverlayMode;

    *result = mOverlayMode;
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetHasNonEmbeddedTabs(
    /* [in] */ Boolean hasNonEmbeddedTabs)
{
    // ==================before translated======================
    // mHasNonEmbeddedTabs = hasNonEmbeddedTabs;

    mHasNonEmbeddedTabs = hasNonEmbeddedTabs;
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetShowingForActionMode(
    /* [in] */ Boolean showing)
{
    // ==================before translated======================
    // if (showing) {
    //     // Here's a fun hack: if the status bar is currently being hidden,
    //     // and the application has asked for stable content insets, then
    //     // we will end up with the action mode action bar being shown
    //     // without the status bar, but moved below where the status bar
    //     // would be.  Not nice.  Trying to have this be positioned
    //     // correctly is not easy (basically we need yet *another* content
    //     // inset from the window manager to know where to put it), so
    //     // instead we will just temporarily force the status bar to be shown.
    //     if ((getWindowSystemUiVisibility() & (SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
    //             | SYSTEM_UI_FLAG_LAYOUT_STABLE))
    //             == (SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | SYSTEM_UI_FLAG_LAYOUT_STABLE)) {
    //         setDisabledSystemUiVisibility(SYSTEM_UI_FLAG_FULLSCREEN);
    //     }
    // } else {
    //     setDisabledSystemUiVisibility(0);
    // }

    if (showing) {
        Int32 visibility = 0;
        GetWindowSystemUiVisibility(&visibility);
        if ((visibility & (IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | IView::SYSTEM_UI_FLAG_LAYOUT_STABLE))
            == (IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | IView::SYSTEM_UI_FLAG_LAYOUT_STABLE)) {
            SetDisabledSystemUiVisibility(IView::SYSTEM_UI_FLAG_FULLSCREEN);
        }
    }
    else {
        SetDisabledSystemUiVisibility(0);
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnWindowSystemUiVisibilityChanged(
    /* [in] */ Int32 visible)
{
    // ==================before translated======================
    // super.onWindowSystemUiVisibilityChanged(visible);
    // pullChildren();
    // final int diff = mLastSystemUiVisibility ^ visible;
    // mLastSystemUiVisibility = visible;
    // final boolean barVisible = (visible & SYSTEM_UI_FLAG_FULLSCREEN) == 0;
    // final boolean stable = (visible & SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0;
    // if (mActionBarVisibilityCallback != null) {
    //     // We want the bar to be visible if it is not being hidden,
    //     // or the app has not turned on a stable UI mode (meaning they
    //     // are performing explicit layout around the action bar).
    //     mActionBarVisibilityCallback.enableContentAnimations(!stable);
    //     if (barVisible || !stable) mActionBarVisibilityCallback.showForSystem();
    //     else mActionBarVisibilityCallback.hideForSystem();
    // }
    // if ((diff & SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0) {
    //     if (mActionBarVisibilityCallback != null) {
    //         requestApplyInsets();
    //     }
    // }

    ViewGroup::OnWindowSystemUiVisibilityChanged(visible);
    PullChildren();
    Int32 diff = mLastSystemUiVisibility ^ visible;
    mLastSystemUiVisibility = visible;
    Boolean barVisible = (visible & SYSTEM_UI_FLAG_FULLSCREEN) == 0;
    Boolean stable = (visible & SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0;
    if (mActionBarVisibilityCallback != NULL) {
        mActionBarVisibilityCallback->EnableContentAnimations(!stable);
        if (barVisible || !stable)
            mActionBarVisibilityCallback->ShowForSystem();
        else
            mActionBarVisibilityCallback->HideForSystem();
    }
    if ((diff & SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0) {
        if (mActionBarVisibilityCallback != NULL) {
            RequestApplyInsets();
        }
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnApplyWindowInsets(
    /* [in] */ IWindowInsets* insets,
    /* [out] */ IWindowInsets** result)
{
    VALIDATE_NOT_NULL(insets);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // pullChildren();
    //
    // final int vis = getWindowSystemUiVisibility();
    // final boolean stable = (vis & SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0;
    // final Rect systemInsets = insets.getSystemWindowInsets();
    //
    // // The top and bottom action bars are always within the content area.
    // boolean changed = applyInsets(mActionBarTop, systemInsets, true, true, false, true);
    // if (mActionBarBottom != null) {
    //     changed |= applyInsets(mActionBarBottom, systemInsets, true, false, true, true);
    // }
    //
    // mBaseInnerInsets.set(systemInsets);
    // computeFitSystemWindows(mBaseInnerInsets, mBaseContentInsets);
    // if (!mLastBaseContentInsets.equals(mBaseContentInsets)) {
    //     changed = true;
    //     mLastBaseContentInsets.set(mBaseContentInsets);
    // }
    //
    // if (changed) {
    //     requestLayout();
    // }
    //
    // // We don't do any more at this point.  To correctly compute the content/inner
    // // insets in all cases, we need to know the measured size of the various action
    // // bar elements.  onApplyWindowInsets() happens before the measure pass, so we can't
    // // do that here.  Instead we will take this up in onMeasure().
    // return WindowInsets.CONSUMED;

    PullChildren();
    Int32 vis = 0;
    GetWindowSystemUiVisibility(&vis);
    // no used: Boolean stable = (vis & SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0;
    AutoPtr<IRect> systemInsets;
    insets->GetSystemWindowInsets((IRect**)&systemInsets);

    // The top and bottom action bars are always within the content area.
    Boolean changed = ApplyInsets(IView::Probe(mActionBarTop), systemInsets, TRUE, TRUE, FALSE, TRUE);
    if (mActionBarBottom != NULL) {
        changed |= ApplyInsets(IView::Probe(mActionBarBottom), systemInsets, TRUE, FALSE, TRUE, TRUE);
    }

    mBaseInnerInsets->Set(systemInsets);
    ComputeFitSystemWindows(mBaseInnerInsets, mBaseContentInsets);
    Boolean isEquals = FALSE;
    mLastBaseContentInsets->Equals(mBaseContentInsets, &isEquals);
    if (!isEquals) {
        changed = TRUE;
        mLastBaseContentInsets->Set(mBaseContentInsets);
    }

    if (changed) {
        RequestLayout();
    }

    // We don't do any more at this point.  To correctly compute the content/inner
    // insets in all cases, we need to know the measured size of the various action
    // bar elements.  onApplyWindowInsets() happens before the measure pass, so we can't
    // do that here.  Instead we will take this up in onMeasure().
    *result = CWindowInsets::CONSUMED;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarOverlayLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(attrs);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new LayoutParams(getContext(), attrs);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    CViewGroupLayoutParams::New(context, attrs, result);
    return NOERROR;
}

ECode ActionBarOverlayLayout::Draw(
    /* [in] */ ICanvas* c)
{
    VALIDATE_NOT_NULL(c);
    // ==================before translated======================
    // super.draw(c);
    // if (mWindowContentOverlay != null && !mIgnoreWindowContentOverlay) {
    //     final int top = mActionBarTop.getVisibility() == VISIBLE ?
    //             (int) (mActionBarTop.getBottom() + mActionBarTop.getTranslationY() + 0.5f) : 0;
    //     mWindowContentOverlay.setBounds(0, top, getWidth(),
    //             top + mWindowContentOverlay.getIntrinsicHeight());
    //     mWindowContentOverlay.draw(c);
    // }

    ViewGroup::Draw(c);
    if (mWindowContentOverlay != NULL && !mIgnoreWindowContentOverlay) {
        Int32 visibility = 0;
        IView::Probe(mActionBarTop)->GetVisibility(&visibility);
        Int32 bottom = 0;
        Float transY = 0;
        IView::Probe(mActionBarTop)->GetBottom(&bottom);
        IView::Probe(mActionBarTop)->GetTranslationY(&transY);
        Int32 top = visibility == VISIBLE ? (Int32)(bottom + transY + 0.5f) : 0;

        Int32 width = 0, intrinsHeight = 0;
        GetWidth(&width);
        mWindowContentOverlay->GetIntrinsicHeight(&intrinsHeight);
        mWindowContentOverlay->SetBounds(0, top, width, top + intrinsHeight);
        mWindowContentOverlay->Draw(c);
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::ShouldDelayChildPressedState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnStartNestedScroll(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 axes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(child);
    VALIDATE_NOT_NULL(target);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if ((axes & SCROLL_AXIS_VERTICAL) == 0 || mActionBarTop.getVisibility() != VISIBLE) {
    //     return false;
    // }
    // return mHideOnContentScroll;

    Int32 visibility = 0;
    IView::Probe(mActionBarTop)->GetVisibility(&visibility);
    if ((axes & SCROLL_AXIS_VERTICAL) == 0 || visibility != VISIBLE) {
        *result = FALSE;
        return NOERROR;
    }
    *result = mHideOnContentScroll;
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnNestedScrollAccepted(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 axes)
{
    VALIDATE_NOT_NULL(child);
    VALIDATE_NOT_NULL(target);
    // ==================before translated======================
    // super.onNestedScrollAccepted(child, target, axes);
    // mHideOnContentScrollReference = getActionBarHideOffset();
    // haltActionBarHideOffsetAnimations();
    // if (mActionBarVisibilityCallback != null) {
    //     mActionBarVisibilityCallback.onContentScrollStarted();
    // }

    ViewGroup::OnNestedScrollAccepted(child, target, axes);
    GetActionBarHideOffset(&mHideOnContentScrollReference);
    HaltActionBarHideOffsetAnimations();
    if (mActionBarVisibilityCallback != NULL) {
        mActionBarVisibilityCallback->OnContentScrollStarted();
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnNestedScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dxConsumed,
    /* [in] */ Int32 dyConsumed,
    /* [in] */ Int32 dxUnconsumed,
    /* [in] */ Int32 dyUnconsumed)
{
    VALIDATE_NOT_NULL(target);
    // ==================before translated======================
    // mHideOnContentScrollReference += dyConsumed;
    // setActionBarHideOffset(mHideOnContentScrollReference);

    mHideOnContentScrollReference += dyConsumed;
    SetActionBarHideOffset(mHideOnContentScrollReference);
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnStopNestedScroll(
    /* [in] */ IView* target)
{
    VALIDATE_NOT_NULL(target);
    // ==================before translated======================
    // super.onStopNestedScroll(target);
    // if (mHideOnContentScroll && !mAnimatingForFling) {
    //     if (mHideOnContentScrollReference <= mActionBarTop.getHeight()) {
    //         postRemoveActionBarHideOffset();
    //     } else {
    //         postAddActionBarHideOffset();
    //     }
    // }
    // if (mActionBarVisibilityCallback != null) {
    //     mActionBarVisibilityCallback.onContentScrollStopped();
    // }

    ViewGroup::OnStopNestedScroll(target);
    if (mHideOnContentScroll && !mAnimatingForFling) {
        Int32 height = 0;
        IView::Probe(mActionBarTop)->GetHeight(&height);
        if (mHideOnContentScrollReference <= height) {
            PostRemoveActionBarHideOffset();
        }
        else {
            PostAddActionBarHideOffset();
        }
    }
    if (mActionBarVisibilityCallback != NULL) {
        mActionBarVisibilityCallback->OnContentScrollStopped();
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnNestedFling(
    /* [in] */ IView* target,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [in] */ Boolean consumed,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(target);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (!mHideOnContentScroll || !consumed) {
    //     return false;
    // }
    // if (shouldHideActionBarOnFling(velocityX, velocityY)) {
    //     addActionBarHideOffset();
    // } else {
    //     removeActionBarHideOffset();
    // }
    // mAnimatingForFling = true;
    // return true;

    if (!mHideOnContentScroll || !consumed) {
        *result = FALSE;
        return NOERROR;
    }
    if (ShouldHideActionBarOnFling(velocityX, velocityY)) {
        AddActionBarHideOffset();
    }
    else {
        RemoveActionBarHideOffset();
    }
    *result = TRUE;
    return NOERROR;
}

ECode ActionBarOverlayLayout::PullChildren()
{
    // ==================before translated======================
    // if (mContent == null) {
    //     mContent = findViewById(com.android.internal.R.id.content);
    //     mActionBarTop = (ActionBarContainer) findViewById(
    //             com.android.internal.R.id.action_bar_container);
    //     mDecorToolbar = getDecorToolbar(findViewById(com.android.internal.R.id.action_bar));
    //     mActionBarBottom = (ActionBarContainer) findViewById(
    //             com.android.internal.R.id.split_action_bar);
    // }

    if (mContent == NULL) {
        FindViewById(R::id::content, (IView**)&mContent);
        AutoPtr<IView> viewTmp;
        FindViewById(R::id::action_bar_container, (IView**)&viewTmp);
        mActionBarTop = IActionBarContainer::Probe(viewTmp);

        AutoPtr<IView> viewTmp1;
        FindViewById(R::id::action_bar, (IView**)&viewTmp1);
        GetDecorToolbar(viewTmp1, (IDecorToolbar**)&mDecorToolbar);

        AutoPtr<IView> viewTmp2;
        FindViewById(R::id::split_action_bar, (IView**)&viewTmp2);
        mActionBarBottom = IActionBarContainer::Probe(viewTmp2);
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetHideOnContentScrollEnabled(
    /* [in] */ Boolean hideOnContentScroll)
{
    // ==================before translated======================
    // if (hideOnContentScroll != mHideOnContentScroll) {
    //     mHideOnContentScroll = hideOnContentScroll;
    //     if (!hideOnContentScroll) {
    //         stopNestedScroll();
    //         haltActionBarHideOffsetAnimations();
    //         setActionBarHideOffset(0);
    //     }
    // }

    if (hideOnContentScroll != mHideOnContentScroll) {
        mHideOnContentScroll = hideOnContentScroll;
        if (!hideOnContentScroll) {
            StopNestedScroll();
            HaltActionBarHideOffsetAnimations();
            SetActionBarHideOffset(0);
        }
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::IsHideOnContentScrollEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHideOnContentScroll;

    *result = mHideOnContentScroll;
    return NOERROR;
}

ECode ActionBarOverlayLayout::GetActionBarHideOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mActionBarTop != null ? -((int) mActionBarTop.getTranslationY()) : 0;

    Float transY = 0;
    IView::Probe(mActionBarTop)->GetTranslationY(&transY);
    *result = mActionBarTop != NULL ? -(Int32)transY : 0;
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetActionBarHideOffset(
    /* [in] */ Int32 offset)
{
    // ==================before translated======================
    // haltActionBarHideOffsetAnimations();
    // final int topHeight = mActionBarTop.getHeight();
    // offset = Math.max(0, Math.min(offset, topHeight));
    // mActionBarTop.setTranslationY(-offset);
    // if (mActionBarBottom != null && mActionBarBottom.getVisibility() != GONE) {
    //     // Match the hide offset proportionally for a split bar
    //     final float fOffset = (float) offset / topHeight;
    //     final int bOffset = (int) (mActionBarBottom.getHeight() * fOffset);
    //     mActionBarBottom.setTranslationY(bOffset);
    // }

    HaltActionBarHideOffsetAnimations();
    Int32 topHeight = 0;
    IView::Probe(mActionBarTop)->GetHeight(&topHeight);
    offset = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(offset, topHeight));
    IView::Probe(mActionBarTop)->SetTranslationY(-offset);
    Int32 visibility = 0;
    IView::Probe(mActionBarBottom)->GetVisibility(&visibility);
    Float fOffset = 0.0f;
    Int32 bOffset = 0;
    Int32 height = 0;
    if (mActionBarBottom != NULL && visibility != IView::GONE) {
        // Match the hide offset proportionally for a split bar
        fOffset = (Float)offset / topHeight;
        IView::Probe(mActionBarBottom)->GetHeight(&height);
        bOffset = (Int32)(height * fOffset);
        IView::Probe(mActionBarBottom)->SetTranslationY(bOffset);
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetWindowCallback(
    /* [in] */ IWindowCallback* cb)
{
    VALIDATE_NOT_NULL(cb);
    // ==================before translated======================
    // pullChildren();
    // mDecorToolbar.setWindowCallback(cb);

    PullChildren();
    mDecorToolbar->SetWindowCallback(cb);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetWindowTitle(
    /* [in] */ ICharSequence* title)
{
    VALIDATE_NOT_NULL(title);
    // ==================before translated======================
    // pullChildren();
    // mDecorToolbar.setWindowTitle(title);

    PullChildren();
    mDecorToolbar->SetWindowTitle(title);
    return NOERROR;
}

ECode ActionBarOverlayLayout::GetTitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // pullChildren();
    // return mDecorToolbar.getTitle();

    PullChildren();
    return mDecorToolbar->GetTitle(result);
}

ECode ActionBarOverlayLayout::InitFeature(
    /* [in] */ Int32 windowFeature)
{
    // ==================before translated======================
    // pullChildren();
    // switch (windowFeature) {
    //     case Window.FEATURE_PROGRESS:
    //         mDecorToolbar.initProgress();
    //         break;
    //     case Window.FEATURE_INDETERMINATE_PROGRESS:
    //         mDecorToolbar.initIndeterminateProgress();
    //         break;
    //     case Window.FEATURE_ACTION_BAR_OVERLAY:
    //         setOverlayMode(true);
    //         break;
    // }

    PullChildren();
    switch (windowFeature) {
        case IWindow::FEATURE_PROGRESS:
            mDecorToolbar->InitProgress();
            break;
        case IWindow::FEATURE_INDETERMINATE_PROGRESS:
            mDecorToolbar->InitIndeterminateProgress();
            break;
        case IWindow::FEATURE_ACTION_BAR_OVERLAY:
            SetOverlayMode(TRUE);
            break;
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetUiOptions(
    /* [in] */ Int32 uiOptions)
{
    // ==================before translated======================
    // boolean splitActionBar = false;
    // final boolean splitWhenNarrow =
    //         (uiOptions & ActivityInfo.UIOPTION_SPLIT_ACTION_BAR_WHEN_NARROW) != 0;
    // if (splitWhenNarrow) {
    //     splitActionBar = getContext().getResources().getBoolean(
    //             com.android.internal.R.bool.split_action_bar_is_narrow);
    // }
    // if (splitActionBar) {
    //     pullChildren();
    //     if (mActionBarBottom != null && mDecorToolbar.canSplit()) {
    //         mDecorToolbar.setSplitView(mActionBarBottom);
    //         mDecorToolbar.setSplitToolbar(splitActionBar);
    //         mDecorToolbar.setSplitWhenNarrow(splitWhenNarrow);
    //
    //         final ActionBarContextView cab = (ActionBarContextView) findViewById(
    //                 com.android.internal.R.id.action_context_bar);
    //         cab.setSplitView(mActionBarBottom);
    //         cab.setSplitToolbar(splitActionBar);
    //         cab.setSplitWhenNarrow(splitWhenNarrow);
    //     } else if (splitActionBar) {
    //         Log.e(TAG, "Requested split action bar with " +
    //                 "incompatible window decor! Ignoring request.");
    //     }
    // }

    Boolean splitActionBar = FALSE;
    Boolean splitWhenNarrow = (uiOptions & IActivityInfo::UIOPTION_SPLIT_ACTION_BAR_WHEN_NARROW) != 0;
    if (splitWhenNarrow) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        resources->GetBoolean(R::bool_::split_action_bar_is_narrow, &splitActionBar);
    }
    if (splitActionBar) {
        PullChildren();
        Boolean canSplit = FALSE;
        mDecorToolbar->CanSplit(&canSplit);
        if (mActionBarBottom != NULL && canSplit) {
            mDecorToolbar->SetSplitView(IViewGroup::Probe(mActionBarBottom));
            mDecorToolbar->SetSplitToolbar(splitActionBar);
            mDecorToolbar->SetSplitWhenNarrow(splitWhenNarrow);

            AutoPtr<IView> viewTmp;
            FindViewById(R::id::action_context_bar, (IView**)&viewTmp);
            ActionBarContextView* cab = (ActionBarContextView*)viewTmp.Get();

            cab->SetSplitView(IViewGroup::Probe(mActionBarBottom));
            cab->SetSplitToolbar(splitActionBar);
            cab->SetSplitWhenNarrow(splitWhenNarrow);
        }
        else if (splitActionBar) {
            Logger::E(TAG, "Requested split action bar with incompatible window decor! Ignoring request.");
        }
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::HasIcon(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // pullChildren();
    // return mDecorToolbar.hasIcon();

    PullChildren();
    mDecorToolbar->HasIcon(result);
    return NOERROR;
}

ECode ActionBarOverlayLayout::HasLogo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // pullChildren();
    // return mDecorToolbar.hasLogo();

    PullChildren();
    mDecorToolbar->HasLogo(result);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetIcon(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // pullChildren();
    // mDecorToolbar.setIcon(resId);

    PullChildren();
    mDecorToolbar->SetIcon(resId);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetIcon(
    /* [in] */ IDrawable* d)
{
    VALIDATE_NOT_NULL(d);
    // ==================before translated======================
    // pullChildren();
    // mDecorToolbar.setIcon(d);

    PullChildren();
    mDecorToolbar->SetIcon(d);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetLogo(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // pullChildren();
    // mDecorToolbar.setLogo(resId);

    PullChildren();
    mDecorToolbar->SetLogo(resId);
    return NOERROR;
}

ECode ActionBarOverlayLayout::CanShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // pullChildren();
    // return mDecorToolbar.canShowOverflowMenu();

    PullChildren();
    return mDecorToolbar->CanShowOverflowMenu(result);
}

ECode ActionBarOverlayLayout::IsOverflowMenuShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // pullChildren();
    // return mDecorToolbar.isOverflowMenuShowing();

    PullChildren();
    return mDecorToolbar->IsOverflowMenuShowing(result);
}

ECode ActionBarOverlayLayout::IsOverflowMenuShowPending(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // pullChildren();
    // return mDecorToolbar.isOverflowMenuShowPending();

    PullChildren();
    return mDecorToolbar->IsOverflowMenuShowPending(result);
}

ECode ActionBarOverlayLayout::ShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // pullChildren();
    // return mDecorToolbar.showOverflowMenu();

    PullChildren();
    return mDecorToolbar->ShowOverflowMenu(result);
}

ECode ActionBarOverlayLayout::HideOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // pullChildren();
    // return mDecorToolbar.hideOverflowMenu();

    PullChildren();
    return mDecorToolbar->HideOverflowMenu(result);
}

ECode ActionBarOverlayLayout::SetMenuPrepared()
{
    // ==================before translated======================
    // pullChildren();
    // mDecorToolbar.setMenuPrepared();

    PullChildren();
    mDecorToolbar->SetMenuPrepared();
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuPresenterCallback* cb)
{
    VALIDATE_NOT_NULL(menu);
    // ==================before translated======================
    // pullChildren();
    // mDecorToolbar.setMenu(menu, cb);

    PullChildren();
    mDecorToolbar->SetMenu(menu, cb);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SaveToolbarHierarchyState(
    /* [in] */ ISparseArray* toolbarStates)
{
    VALIDATE_NOT_NULL(toolbarStates);
    // ==================before translated======================
    // pullChildren();
    // mDecorToolbar.saveHierarchyState(toolbarStates);

    PullChildren();
    mDecorToolbar->SaveHierarchyState(toolbarStates);
    return NOERROR;
}

ECode ActionBarOverlayLayout::RestoreToolbarHierarchyState(
    /* [in] */ ISparseArray* toolbarStates)
{
    VALIDATE_NOT_NULL(toolbarStates);
    // ==================before translated======================
    // pullChildren();
    // mDecorToolbar.restoreHierarchyState(toolbarStates);

    PullChildren();
    mDecorToolbar->RestoreHierarchyState(toolbarStates);
    return NOERROR;
}

ECode ActionBarOverlayLayout::DismissPopups()
{
    // ==================before translated======================
    // pullChildren();
    // mDecorToolbar.dismissPopupMenus();

    PullChildren();
    mDecorToolbar->DismissPopupMenus();
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnDetachedFromWindow()
{
    // ==================before translated======================
    // super.onDetachedFromWindow();
    // haltActionBarHideOffsetAnimations();

    ViewGroup::OnDetachedFromWindow();
    HaltActionBarHideOffsetAnimations();
    return NOERROR;
}

void ActionBarOverlayLayout::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // ==================before translated======================
    // super.onConfigurationChanged(newConfig);
    // init(getContext());
    // requestApplyInsets();

    ViewGroup::OnConfigurationChanged(newConfig);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Init(context);
    RequestApplyInsets();
}

void ActionBarOverlayLayout::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    // ==================before translated======================
    // super.onWindowVisibilityChanged(visibility);
    // mWindowVisibility = visibility;
    // if (mActionBarVisibilityCallback != null) {
    //     mActionBarVisibilityCallback.onWindowVisibilityChanged(visibility);
    // }

    ViewGroup::OnWindowVisibilityChanged(visibility);
    mWindowVisibility = visibility;
    if (mActionBarVisibilityCallback != NULL) {
        mActionBarVisibilityCallback->OnWindowVisibilityChanged(visibility);
    }
}

ECode ActionBarOverlayLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);

    CViewGroupLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::MATCH_PARENT, result);
    return NOERROR;
}

ECode ActionBarOverlayLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p,
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(p);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new LayoutParams(p);

    CViewGroupLayoutParams::New(p, result);
    return NOERROR;
}

Boolean ActionBarOverlayLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    // ==================before translated======================
    // return p instanceof LayoutParams;

    return TRUE;
}

void ActionBarOverlayLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // ==================before translated======================
    // pullChildren();
    //
    // int maxHeight = 0;
    // int maxWidth = 0;
    // int childState = 0;
    //
    // int topInset = 0;
    // int bottomInset = 0;
    //
    // measureChildWithMargins(mActionBarTop, widthMeasureSpec, 0, heightMeasureSpec, 0);
    // LayoutParams lp = (LayoutParams) mActionBarTop.getLayoutParams();
    // maxWidth = Math.max(maxWidth,
    //         mActionBarTop.getMeasuredWidth() + lp.leftMargin + lp.rightMargin);
    // maxHeight = Math.max(maxHeight,
    //         mActionBarTop.getMeasuredHeight() + lp.topMargin + lp.bottomMargin);
    // childState = combineMeasuredStates(childState, mActionBarTop.getMeasuredState());
    //
    // // xlarge screen layout doesn't have bottom action bar.
    // if (mActionBarBottom != null) {
    //     measureChildWithMargins(mActionBarBottom, widthMeasureSpec, 0, heightMeasureSpec, 0);
    //     lp = (LayoutParams) mActionBarBottom.getLayoutParams();
    //     maxWidth = Math.max(maxWidth,
    //             mActionBarBottom.getMeasuredWidth() + lp.leftMargin + lp.rightMargin);
    //     maxHeight = Math.max(maxHeight,
    //             mActionBarBottom.getMeasuredHeight() + lp.topMargin + lp.bottomMargin);
    //     childState = combineMeasuredStates(childState, mActionBarBottom.getMeasuredState());
    // }
    //
    // final int vis = getWindowSystemUiVisibility();
    // final boolean stable = (vis & SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0;
    //
    // if (stable) {
    //     // This is the standard space needed for the action bar.  For stable measurement,
    //     // we can't depend on the size currently reported by it -- this must remain constant.
    //     topInset = mActionBarHeight;
    //     if (mHasNonEmbeddedTabs) {
    //         final View tabs = mActionBarTop.getTabContainer();
    //         if (tabs != null) {
    //             // If tabs are not embedded, increase space on top to account for them.
    //             topInset += mActionBarHeight;
    //         }
    //     }
    // } else if (mActionBarTop.getVisibility() != GONE) {
    //     // This is the space needed on top of the window for all of the action bar
    //     // and tabs.
    //     topInset = mActionBarTop.getMeasuredHeight();
    // }
    //
    // if (mDecorToolbar.isSplit()) {
    //     // If action bar is split, adjust bottom insets for it.
    //     if (mActionBarBottom != null) {
    //         if (stable) {
    //             bottomInset = mActionBarHeight;
    //         } else {
    //             bottomInset = mActionBarBottom.getMeasuredHeight();
    //         }
    //     }
    // }
    //
    // // If the window has not requested system UI layout flags, we need to
    // // make sure its content is not being covered by system UI...  though it
    // // will still be covered by the action bar if they have requested it to
    // // overlay.
    // mContentInsets.set(mBaseContentInsets);
    // mInnerInsets.set(mBaseInnerInsets);
    // if (!mOverlayMode && !stable) {
    //     mContentInsets.top += topInset;
    //     mContentInsets.bottom += bottomInset;
    // } else {
    //     mInnerInsets.top += topInset;
    //     mInnerInsets.bottom += bottomInset;
    // }
    // applyInsets(mContent, mContentInsets, true, true, true, true);
    //
    // if (!mLastInnerInsets.equals(mInnerInsets)) {
    //     // If the inner insets have changed, we need to dispatch this down to
    //     // the app's fitSystemWindows().  We do this before measuring the content
    //     // view to keep the same semantics as the normal fitSystemWindows() call.
    //     mLastInnerInsets.set(mInnerInsets);
    //     mContent.dispatchApplyWindowInsets(new WindowInsets(mInnerInsets));
    // }
    //
    // measureChildWithMargins(mContent, widthMeasureSpec, 0, heightMeasureSpec, 0);
    // lp = (LayoutParams) mContent.getLayoutParams();
    // maxWidth = Math.max(maxWidth,
    //         mContent.getMeasuredWidth() + lp.leftMargin + lp.rightMargin);
    // maxHeight = Math.max(maxHeight,
    //         mContent.getMeasuredHeight() + lp.topMargin + lp.bottomMargin);
    // childState = combineMeasuredStates(childState, mContent.getMeasuredState());
    //
    // // Account for padding too
    // maxWidth += getPaddingLeft() + getPaddingRight();
    // maxHeight += getPaddingTop() + getPaddingBottom();
    //
    // // Check against our minimum height and width
    // maxHeight = Math.max(maxHeight, getSuggestedMinimumHeight());
    // maxWidth = Math.max(maxWidth, getSuggestedMinimumWidth());
    //
    // setMeasuredDimension(resolveSizeAndState(maxWidth, widthMeasureSpec, childState),
    //         resolveSizeAndState(maxHeight, heightMeasureSpec,
    //                 childState << MEASURED_HEIGHT_STATE_SHIFT));

    PullChildren();

    Int32 maxHeight = 0;
    Int32 maxWidth = 0;
    Int32 childState = 0;
    Int32 topInset = 0;
    Int32 bottomInset = 0;

    MeasureChildWithMargins(IView::Probe(mActionBarTop), widthMeasureSpec, 0, heightMeasureSpec, 0);
    AutoPtr<IViewGroupLayoutParams> lp;
    IView::Probe(mActionBarTop)->GetLayoutParams((IViewGroupLayoutParams**)&lp);

    Int32 measureWidth = 0, measureHeight = 0, measureState = 0;
    Int32 leftMargin = 0, rightMargin = 0, topMargin = 0, bottomMargin = 0;
    IView::Probe(mActionBarTop)->GetMeasuredWidth(&measureWidth);
    IView::Probe(mActionBarTop)->GetMeasuredHeight(&measureHeight);
    IView::Probe(mActionBarTop)->GetMeasuredState(&measureState);
    IViewGroupMarginLayoutParams::Probe(lp)->GetLeftMargin(&leftMargin);
    IViewGroupMarginLayoutParams::Probe(lp)->GetRightMargin(&rightMargin);
    IViewGroupMarginLayoutParams::Probe(lp)->GetTopMargin(&topMargin);
    IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&bottomMargin);
    maxWidth = Elastos::Core::Math::Max(maxWidth, measureWidth + leftMargin + rightMargin);
    maxHeight = Elastos::Core::Math::Max(maxHeight, measureHeight + topMargin + bottomMargin);
    childState = CombineMeasuredStates(childState, measureState);

    // xlarge screen layout doesn't have bottom action bar.
    if (mActionBarBottom != NULL) {
        MeasureChildWithMargins(IView::Probe(mActionBarBottom), widthMeasureSpec, 0, heightMeasureSpec, 0);

        lp = NULL;
        IView::Probe(mActionBarBottom)->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        IView::Probe(mActionBarBottom)->GetMeasuredWidth(&measureWidth);
        IView::Probe(mActionBarBottom)->GetMeasuredHeight(&measureHeight);
        IView::Probe(mActionBarBottom)->GetMeasuredState(&measureState);
        IViewGroupMarginLayoutParams::Probe(lp)->GetLeftMargin(&leftMargin);
        IViewGroupMarginLayoutParams::Probe(lp)->GetRightMargin(&rightMargin);
        IViewGroupMarginLayoutParams::Probe(lp)->GetTopMargin(&topMargin);
        IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&bottomMargin);

        maxWidth = Elastos::Core::Math::Max(maxWidth, measureWidth + leftMargin + rightMargin);
        maxHeight = Elastos::Core::Math::Max(maxHeight, measureHeight + topMargin + bottomMargin);
        childState = CombineMeasuredStates(childState, measureState);
    }

    Int32 vis = 0;
    GetWindowSystemUiVisibility(&vis);
    Boolean stable = (vis & SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0;
    Int32 visibility = 0;
    IView::Probe(mActionBarTop)->GetVisibility(&visibility);

    if (stable) {
        // This is the standard space needed for the action bar.  For stable measurement,
        // we can't depend on the size currently reported by it -- this must remain constant.
        topInset = mActionBarHeight;
        if (mHasNonEmbeddedTabs) {
            AutoPtr<IView> tabs;
            mActionBarTop->GetTabContainer((IView**)&tabs);
            if (tabs != NULL) {
                // If tabs are not embedded, increase space on top to account for them.
                topInset += mActionBarHeight;
            }
        }
    }
    else if (visibility != IView::GONE) {
        // This is the space needed on top of the window for all of the action bar
        // and tabs.
        IView::Probe(mActionBarTop)->GetMeasuredHeight(&topInset);
    }

    Boolean isSplit = FALSE;
    mDecorToolbar->IsSplit(&isSplit);
    if (isSplit) {
        // If action bar is split, adjust bottom insets for it.
        if (mActionBarBottom != NULL) {
            if (stable) {
                bottomInset = mActionBarHeight;
            }
            else {
                IView::Probe(mActionBarBottom)->GetMeasuredHeight(&bottomInset);
            }
        }
    }

    // If the window has not requested system UI layout flags, we need to
    // make sure its content is not being covered by system UI...  though it
    // will still be covered by the action bar if they have requested it to
    // overlay.
    mContentInsets->Set(mBaseContentInsets);
    mInnerInsets->Set(mBaseInnerInsets);
    if (!mOverlayMode && !stable) {
        Int32 top = 0, bottom = 0;
        mContentInsets->GetTop(&top);
        mContentInsets->GetBottom(&bottom);
        top += topInset;
        bottom += bottomInset;
        mContentInsets->SetTop(top);
        mContentInsets->SetBottom(bottom);
    }
    else {
        Int32 top = 0, bottom = 0;
        mInnerInsets->GetTop(&top);
        mInnerInsets->GetBottom(&bottom);
        top += topInset;
        bottom += bottomInset;
        mInnerInsets->SetTop(top);
        mInnerInsets->SetBottom(bottom);
    }
    ApplyInsets(mContent, mContentInsets, TRUE, TRUE, TRUE, TRUE);

    Boolean isEquals = FALSE;
    mLastInnerInsets->Equals(mInnerInsets, &isEquals);
    if (!isEquals) {
        // If the inner insets have changed, we need to dispatch this down to
        // the app's fitSystemWindows().  We do this before measuring the content
        // view to keep the same semantics as the normal fitSystemWindows() call.
        mLastInnerInsets->Set(mInnerInsets);

        AutoPtr<IWindowInsets> windowInsets;
        CWindowInsets::New(mInnerInsets, (IWindowInsets**)&windowInsets);
        AutoPtr<IWindowInsets> windowInsetsTmp;
        mContent->DispatchApplyWindowInsets(windowInsets, (IWindowInsets**)&windowInsetsTmp);
    }

    MeasureChildWithMargins(mContent, widthMeasureSpec, 0, heightMeasureSpec, 0);

    lp = NULL;
    mContent->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    mContent->GetMeasuredWidth(&measureWidth);
    mContent->GetMeasuredHeight(&measureHeight);
    mContent->GetMeasuredState(&measureState);
    IViewGroupMarginLayoutParams::Probe(lp)->GetLeftMargin(&leftMargin);
    IViewGroupMarginLayoutParams::Probe(lp)->GetRightMargin(&rightMargin);
    IViewGroupMarginLayoutParams::Probe(lp)->GetTopMargin(&topMargin);
    IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&bottomMargin);

    maxWidth = Elastos::Core::Math::Max(maxWidth, measureWidth + leftMargin + rightMargin);
    maxHeight = Elastos::Core::Math::Max(maxHeight, measureHeight + topMargin + bottomMargin);
    childState = CombineMeasuredStates(childState, measureState);

    // Account for padding too
    Int32 paddingLeft = 0, paddingRight = 0, paddingTop = 0, paddingBottom = 0;
    GetPaddingLeft(&paddingLeft);
    GetPaddingRight(&paddingRight);
    GetPaddingTop(&paddingTop);
    GetPaddingBottom(&paddingBottom);
    maxWidth += paddingLeft + paddingRight;
    maxHeight += paddingTop + paddingBottom;

    // Check against our minimum height and width
    maxHeight = Elastos::Core::Math::Max(maxHeight, GetSuggestedMinimumHeight());
    maxWidth = Elastos::Core::Math::Max(maxWidth, GetSuggestedMinimumWidth());

    SetMeasuredDimension(ResolveSizeAndState(maxWidth, widthMeasureSpec, childState),
            ResolveSizeAndState(maxHeight, heightMeasureSpec, childState << MEASURED_HEIGHT_STATE_SHIFT));
}

ECode ActionBarOverlayLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    // ==================before translated======================
    // final int count = getChildCount();
    //
    // final int parentLeft = getPaddingLeft();
    // final int parentRight = right - left - getPaddingRight();
    //
    // final int parentTop = getPaddingTop();
    // final int parentBottom = bottom - top - getPaddingBottom();
    //
    // for (int i = 0; i < count; i++) {
    //     final View child = getChildAt(i);
    //     if (child.getVisibility() != GONE) {
    //         final LayoutParams lp = (LayoutParams) child.getLayoutParams();
    //
    //         final int width = child.getMeasuredWidth();
    //         final int height = child.getMeasuredHeight();
    //
    //         int childLeft = parentLeft + lp.leftMargin;
    //         int childTop;
    //         if (child == mActionBarBottom) {
    //             childTop = parentBottom - height - lp.bottomMargin;
    //         } else {
    //             childTop = parentTop + lp.topMargin;
    //         }
    //
    //         child.layout(childLeft, childTop, childLeft + width, childTop + height);
    //     }
    // }

    Int32 count = 0;
    GetChildCount(&count);

    Int32 parentLeft = 0, parentRight = 0;
    GetPaddingLeft(&parentLeft);
    GetPaddingRight(&parentRight);
    parentRight = right - left - parentRight;

    Int32 parentTop = 0, parentBottom = 0;
    GetPaddingTop(&parentTop);
    GetPaddingBottom(&parentBottom);
    parentBottom = bottom - top - parentBottom;

    Int32 visibility = 0;
    Int32 width = 0, height = 0;
    Int32 childLeft = 0, childTop = 0;
    Int32 leftMargin = 0, bottomMargin = 0, topMargin = 0;
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->GetVisibility(&visibility);
        if (visibility != IView::GONE) {
            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);

            child->GetMeasuredWidth(&width);
            child->GetMeasuredHeight(&height);

            IViewGroupMarginLayoutParams::Probe(lp)->GetLeftMargin(&leftMargin);
            childLeft = parentLeft + leftMargin;
            IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&bottomMargin);
            if (TO_IINTERFACE(child) == TO_IINTERFACE(mActionBarBottom)) {
                childTop = parentBottom - height - bottomMargin;
            }
            else {
                IViewGroupMarginLayoutParams::Probe(lp)->GetTopMargin(&topMargin);
                childTop = parentTop + topMargin;
            }

            child->Layout(childLeft, childTop, childLeft + width, childTop + height);
        }
    }
    return NOERROR;
}

AutoPtr< ArrayOf<Int32> > ActionBarOverlayLayout::MiddleInitAttrs()
{
    // ==================before translated======================
    // int[] result = new int [] {
    //              com.android.internal.R.attr.actionBarSize,
    //              com.android.internal.R.attr.windowContentOverlay
    //      };

    AutoPtr< ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(2);
    result->Set(0, R::attr::actionBarSize);
    result->Set(1, R::attr::windowContentOverlay);
    return result;
}

void ActionBarOverlayLayout::Init(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // TypedArray ta = getContext().getTheme().obtainStyledAttributes(ATTRS);
    // mActionBarHeight = ta.getDimensionPixelSize(0, 0);
    // mWindowContentOverlay = ta.getDrawable(1);
    // setWillNotDraw(mWindowContentOverlay == null);
    // ta.recycle();
    //
    // mIgnoreWindowContentOverlay = context.getApplicationInfo().targetSdkVersion <
    //         Build.VERSION_CODES.KITKAT;
    //
    // mFlingEstimator = new OverScroller(context);

    AutoPtr<IContext> contextTmp;
    GetContext((IContext**)&contextTmp);
    AutoPtr<IResourcesTheme> theme;
    contextTmp->GetTheme((IResourcesTheme**)&theme);
    AutoPtr<ITypedArray> ta;
    theme->ObtainStyledAttributes(ATTRS, (ITypedArray**)&ta);
    ta->GetDimensionPixelSize(0, 0, &mActionBarHeight);
    ta->GetDrawable(1, (IDrawable**)&mWindowContentOverlay);
    SetWillNotDraw(mWindowContentOverlay == NULL);
    ta->Recycle();

    AutoPtr<IApplicationInfo> applicationInfo;
    context->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    Int32 sdkVersion = 0;
    applicationInfo->GetTargetSdkVersion(&sdkVersion);
    mIgnoreWindowContentOverlay = sdkVersion < Build::VERSION_CODES::KITKAT;
    COverScroller::New(context, (IOverScroller**)&mFlingEstimator);
}

Boolean ActionBarOverlayLayout::ApplyInsets(
    /* [in] */ IView* view,
    /* [in] */ IRect* insets,
    /* [in] */ Boolean left,
    /* [in] */ Boolean top,
    /* [in] */ Boolean bottom,
    /* [in] */ Boolean right)
{
    // ==================before translated======================
    // boolean changed = false;
    // LayoutParams lp = (LayoutParams)view.getLayoutParams();
    // if (left && lp.leftMargin != insets.left) {
    //     changed = true;
    //     lp.leftMargin = insets.left;
    // }
    // if (top && lp.topMargin != insets.top) {
    //     changed = true;
    //     lp.topMargin = insets.top;
    // }
    // if (right && lp.rightMargin != insets.right) {
    //     changed = true;
    //     lp.rightMargin = insets.right;
    // }
    // if (bottom && lp.bottomMargin != insets.bottom) {
    //     changed = true;
    //     lp.bottomMargin = insets.bottom;
    // }
    // return changed;

    Boolean changed = FALSE;
    AutoPtr<IViewGroupLayoutParams> lp;
    view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    Int32 lpValue = 0, rValue = 0;

    IViewGroupMarginLayoutParams::Probe(lp)->GetLeftMargin(&lpValue);
    insets->GetLeft(&rValue);
    if (left && lpValue != rValue) {
        changed = TRUE;
        IViewGroupMarginLayoutParams::Probe(lp)->SetLeftMargin(rValue);
    }

    IViewGroupMarginLayoutParams::Probe(lp)->GetTopMargin(&lpValue);
    insets->GetTop(&rValue);
    if (top && lpValue != rValue) {
        changed = TRUE;
        IViewGroupMarginLayoutParams::Probe(lp)->SetTopMargin(rValue);
    }

    IViewGroupMarginLayoutParams::Probe(lp)->GetRightMargin(&lpValue);
    insets->GetRight(&rValue);
    if (right && lpValue != rValue) {
        changed = TRUE;
        IViewGroupMarginLayoutParams::Probe(lp)->SetRightMargin(rValue);
    }

    IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&lpValue);
    insets->GetBottom(&rValue);
    if (bottom && lpValue != rValue) {
        changed = TRUE;
        IViewGroupMarginLayoutParams::Probe(lp)->SetBottomMargin(rValue);
    }
    return changed;
}

ECode ActionBarOverlayLayout::GetDecorToolbar(
    /* [in] */ IView* view,
    /* [out] */ IDecorToolbar** result)
{
    VALIDATE_NOT_NULL(view);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (view instanceof DecorToolbar) {
    //     return (DecorToolbar) view;
    // } else if (view instanceof Toolbar) {
    //     return ((Toolbar) view).getWrapper();
    // } else {
    //     throw new IllegalStateException("Can't make a decor toolbar out of " +
    //             view.getClass().getSimpleName());
    // }

    AutoPtr<IDecorToolbar> decorToolbar = IDecorToolbar::Probe(view);
    IToolbar* toolbar = IToolbar::Probe(view);
    if (decorToolbar != NULL) {
        *result = decorToolbar;
        REFCOUNT_ADD(*result);
    }
    else if (toolbar) {
        return toolbar->GetWrapper(result);
    }
    else {
        //throw new IllegalStateException("Can't make a decor toolbar out of " +
        //        view.getClass().getSimpleName());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

void ActionBarOverlayLayout::HaltActionBarHideOffsetAnimations()
{
    // ==================before translated======================
    // removeCallbacks(mRemoveActionBarHideOffset);
    // removeCallbacks(mAddActionBarHideOffset);
    // if (mCurrentActionBarTopAnimator != null) {
    //     mCurrentActionBarTopAnimator.cancel();
    // }
    // if (mCurrentActionBarBottomAnimator != null) {
    //     mCurrentActionBarBottomAnimator.cancel();
    // }

    Boolean resTmp;
    RemoveCallbacks(mRemoveActionBarHideOffset, &resTmp);
    RemoveCallbacks(mAddActionBarHideOffset, &resTmp);
    if (mCurrentActionBarTopAnimator != NULL) {
        mCurrentActionBarTopAnimator->Cancel();
    }
    if (mCurrentActionBarBottomAnimator != NULL) {
        mCurrentActionBarBottomAnimator->Cancel();
    }
}

void ActionBarOverlayLayout::PostRemoveActionBarHideOffset()
{
    // ==================before translated======================
    // haltActionBarHideOffsetAnimations();
    // postDelayed(mRemoveActionBarHideOffset, ACTION_BAR_ANIMATE_DELAY);

    HaltActionBarHideOffsetAnimations();
    Boolean resTmp;
    PostDelayed(mRemoveActionBarHideOffset, ACTION_BAR_ANIMATE_DELAY, &resTmp);
}

void ActionBarOverlayLayout::PostAddActionBarHideOffset()
{
    // ==================before translated======================
    // haltActionBarHideOffsetAnimations();
    // postDelayed(mAddActionBarHideOffset, ACTION_BAR_ANIMATE_DELAY);

    HaltActionBarHideOffsetAnimations();
    Boolean resTmp;
    PostDelayed(mAddActionBarHideOffset, ACTION_BAR_ANIMATE_DELAY, &resTmp);
}

void ActionBarOverlayLayout::RemoveActionBarHideOffset()
{
    // ==================before translated======================
    // haltActionBarHideOffsetAnimations();
    // mRemoveActionBarHideOffset.run();

    HaltActionBarHideOffsetAnimations();
    mRemoveActionBarHideOffset->Run();
}

void ActionBarOverlayLayout::AddActionBarHideOffset()
{
    // ==================before translated======================
    // haltActionBarHideOffsetAnimations();
    // mAddActionBarHideOffset.run();

    HaltActionBarHideOffsetAnimations();
    mAddActionBarHideOffset->Run();
}

Boolean ActionBarOverlayLayout::ShouldHideActionBarOnFling(
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY)
{
    // ==================before translated======================
    // mFlingEstimator.fling(0, 0, 0, (int) velocityY, 0, 0, Integer.MIN_VALUE, Integer.MAX_VALUE);
    // final int finalY = mFlingEstimator.getFinalY();
    // return finalY > mActionBarTop.getHeight();

    mFlingEstimator->Fling(0, 0, 0, (Int32)velocityY, 0, 0, Elastos::Core::Math::INT32_MIN_VALUE, Elastos::Core::Math::INT32_MAX_VALUE);
    Int32 finalY = 0;
    mFlingEstimator->GetFinalY(&finalY);
    Int32 height = 0;
    IView::Probe(mActionBarTop)->GetHeight(&height);
    return finalY > height;
}

AutoPtr<IProperty> ActionBarOverlayLayout::InitActinoBarHideOffset()
{
    AutoPtr<IProperty> result = new InnerIntProperty();
    return result;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos


