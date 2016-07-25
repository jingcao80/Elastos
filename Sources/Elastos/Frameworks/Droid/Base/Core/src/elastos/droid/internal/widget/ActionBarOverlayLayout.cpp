
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/internal/widget/CActionBarContextView.h"
#include "elastos/droid/internal/widget/ActionBarOverlayLayout.h"
#include "elastos/droid/internal/widget/CActionBarOverlayLayoutLayoutParams.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/view/CWindowInsets.h"
#include "elastos/droid/widget/COverScroller.h"
#include "elastos/droid/R.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::Build;
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

static AutoPtr< ArrayOf<Int32> > InitAttrs()
{
    AutoPtr< ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(2);
    (*result)[0] = R::attr::actionBarSize;
    (*result)[1] = R::attr::windowContentOverlay;
    return result;
}
const AutoPtr<IProperty> ActionBarOverlayLayout::ACTION_BAR_HIDE_OFFSET = new InnerIntProperty();

AutoPtr< ArrayOf<Int32> > ActionBarOverlayLayout::ATTRS = InitAttrs();
const String ActionBarOverlayLayout::TAG("ActionBarOverlayLayout");

//=====================================================================
//                 ActionBarOverlayLayout::ActionBarOverlayLayoutLayoutParams
//=====================================================================
CAR_INTERFACE_IMPL(ActionBarOverlayLayout::ActionBarOverlayLayoutLayoutParams, ViewGroup::MarginLayoutParams, IActionBarOverlayLayoutLayoutParams)

ActionBarOverlayLayout::ActionBarOverlayLayoutLayoutParams::ActionBarOverlayLayoutLayoutParams()
{
}

ECode ActionBarOverlayLayout::ActionBarOverlayLayoutLayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    return ViewGroup::MarginLayoutParams::constructor(c, attrs);
}

ECode ActionBarOverlayLayout::ActionBarOverlayLayoutLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return ViewGroup::MarginLayoutParams::constructor(width, height);
}

ECode ActionBarOverlayLayout::ActionBarOverlayLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(source);
}

ECode ActionBarOverlayLayout::ActionBarOverlayLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(source);
}

//=====================================================================
//         ActionBarOverlayLayout::InnerAnimatorListenerAdapter
//=====================================================================
ActionBarOverlayLayout::InnerAnimatorListenerAdapter::InnerAnimatorListenerAdapter(
    /* [in] */ ActionBarOverlayLayout* owner)
    : mOwner(owner)
{
}

ECode ActionBarOverlayLayout::InnerAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mOwner->mCurrentActionBarTopAnimator = NULL;
    mOwner->mAnimatingForFling = FALSE;
    return NOERROR;
}

ECode ActionBarOverlayLayout::InnerAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
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
{}

ECode ActionBarOverlayLayout::InnerAnimatorListenerAdapter1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mOwner->mCurrentActionBarBottomAnimator = NULL;
    mOwner->mAnimatingForFling = FALSE;
    return NOERROR;
}

ECode ActionBarOverlayLayout::InnerAnimatorListenerAdapter1::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
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
{}

ECode ActionBarOverlayLayout::InnerRunnable::Run()
{
    mOwner->HaltActionBarHideOffsetAnimations();
    AutoPtr<IViewPropertyAnimator> propertyAnimatorTop;
    IView::Probe(mOwner->mActionBarTop)->Animate((IViewPropertyAnimator**)&propertyAnimatorTop);
    propertyAnimatorTop->TranslationY(0);
    propertyAnimatorTop->SetListener(mOwner->mTopAnimatorListener);
    mOwner->mCurrentActionBarTopAnimator = propertyAnimatorTop;
    Int32 visibility = 0;
    if (mOwner->mActionBarBottom != NULL) {
        IView* barBottom = IView::Probe(mOwner->mActionBarBottom);
        if (barBottom->GetVisibility(&visibility), visibility != IView::GONE) {
            AutoPtr<IViewPropertyAnimator> propertyAnimatorBottom;
            barBottom->Animate((IViewPropertyAnimator**)&propertyAnimatorBottom);
            propertyAnimatorBottom->TranslationY(0);
            propertyAnimatorBottom->SetListener(mOwner->mBottomAnimatorListener);
            mOwner->mCurrentActionBarBottomAnimator = propertyAnimatorBottom;
        }
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
{}

ECode ActionBarOverlayLayout::InnerRunnable1::Run()
{
    mOwner->HaltActionBarHideOffsetAnimations();
    IView* barTop = IView::Probe(mOwner->mActionBarTop);
    AutoPtr<IViewPropertyAnimator> propertyAnimatorTop;
    barTop->Animate((IViewPropertyAnimator**)&propertyAnimatorTop);
    Int32 height = 0;
    barTop->GetHeight(&height);
    propertyAnimatorTop->TranslationY(-height);
    propertyAnimatorTop->SetListener(mOwner->mTopAnimatorListener);
    mOwner->mCurrentActionBarTopAnimator = propertyAnimatorTop;
    Int32 visibility = 0;
    if (mOwner->mActionBarBottom != NULL) {
        IView* barBottom = IView::Probe(mOwner->mActionBarBottom);
        if (barBottom->GetVisibility(&visibility), visibility != IView::GONE) {
            AutoPtr<IViewPropertyAnimator> propertyAnimatorBottom;
            barBottom->Animate((IViewPropertyAnimator**)&propertyAnimatorBottom);
            barBottom->GetHeight(&height);
            propertyAnimatorBottom->TranslationY(height);
            propertyAnimatorBottom->SetListener(mOwner->mBottomAnimatorListener);
            mOwner->mCurrentActionBarBottomAnimator = propertyAnimatorBottom;
        }
    }

    return NOERROR;
}

//=====================================================================
//               ActionBarOverlayLayout::InnerIntProperty
//=====================================================================
ActionBarOverlayLayout::InnerIntProperty::InnerIntProperty()
    : Int32Property(String("actionBarHideOffset"))
{
}

ECode ActionBarOverlayLayout::InnerIntProperty::SetValue(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 value)
{
    ActionBarOverlayLayout* layout = (ActionBarOverlayLayout*)IObject::Probe(object);
    return layout->SetActionBarHideOffset(value);
}

ECode ActionBarOverlayLayout::InnerIntProperty::Get(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    ActionBarOverlayLayout* layout = (ActionBarOverlayLayout*)IObject::Probe(object);
    Int32 offset = 0;
    layout->GetActionBarHideOffset(&offset);
    AutoPtr<IInteger32> offsetObj = CoreUtils::Convert(offset);
    *result = offsetObj;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                        ActionBarOverlayLayout
//=====================================================================

CAR_INTERFACE_IMPL_2(ActionBarOverlayLayout, ViewGroup, IDecorContentParent, IActionBarOverlayLayout)

ActionBarOverlayLayout::ActionBarOverlayLayout()
    : mActionBarHeight(0)
    , mWindowVisibility(IView::VISIBLE)
    , mIgnoreWindowContentOverlay(FALSE)
    , mOverlayMode(FALSE)
    , mHasNonEmbeddedTabs(FALSE)
    , mHideOnContentScroll(FALSE)
    , mAnimatingForFling(FALSE)
    , mHideOnContentScrollReference(0)
    , mLastSystemUiVisibility(0)
    , ACTION_BAR_ANIMATE_DELAY(600)
{
    CRect::New((IRect**)&mBaseContentInsets);
    CRect::New((IRect**)&mLastBaseContentInsets);
    CRect::New((IRect**)&mContentInsets);
    CRect::New((IRect**)&mBaseInnerInsets);
    CRect::New((IRect**)&mInnerInsets);
    CRect::New((IRect**)&mLastInnerInsets);

    mTopAnimatorListener = new InnerAnimatorListenerAdapter(this);
    mBottomAnimatorListener = new InnerAnimatorListenerAdapter1(this);

    mRemoveActionBarHideOffset = new InnerRunnable(this);
    mAddActionBarHideOffset = new InnerRunnable1(this);
}

ECode ActionBarOverlayLayout::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(ViewGroup::constructor(context));
    Init(context);
    return NOERROR;
}

ECode ActionBarOverlayLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(ViewGroup::constructor(context, attrs));
    Init(context);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetActionBarVisibilityCallback(
    /* [in] */ IActionBarVisibilityCallback* cb)
{
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
    mOverlayMode = overlayMode;

    /*
     * Drawing the window content overlay was broken before K so starting to draw it
     * again unexpectedly will cause artifacts in some apps. They should fix it.
     */
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
    *result = mOverlayMode;
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetHasNonEmbeddedTabs(
    /* [in] */ Boolean hasNonEmbeddedTabs)
{
    mHasNonEmbeddedTabs = hasNonEmbeddedTabs;
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetShowingForActionMode(
    /* [in] */ Boolean showing)
{
    if (showing) {
        // Here's a fun hack: if the status bar is currently being hidden,
        // and the application has asked for stable content insets, then
        // we will end up with the action mode action bar being shown
        // without the status bar, but moved below where the status bar
        // would be.  Not nice.  Trying to have this be positioned
        // correctly is not easy (basically we need yet *another* content
        // inset from the window manager to know where to put it), so
        // instead we will just temporarily force the status bar to be shown.
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
    FAIL_RETURN(ViewGroup::OnWindowSystemUiVisibilityChanged(visible));
    PullChildren();
    Int32 diff = mLastSystemUiVisibility ^ visible;
    mLastSystemUiVisibility = visible;
    Boolean barVisible = (visible & SYSTEM_UI_FLAG_FULLSCREEN) == 0;
    Boolean stable = (visible & SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0;
    if (mActionBarVisibilityCallback != NULL) {
        mActionBarVisibilityCallback->EnableContentAnimations(!stable);
        if (barVisible || !stable) {
            mActionBarVisibilityCallback->ShowForSystem();
        }
        else {
            mActionBarVisibilityCallback->HideForSystem();
        }
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
    VALIDATE_NOT_NULL(result)

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
    AutoPtr<IWindowInsets> rect = CWindowInsets::GetCONSUMED();
    *result = rect;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ActionBarOverlayLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    return CActionBarOverlayLayoutLayoutParams::New(context, attrs, result);
}

ECode ActionBarOverlayLayout::Draw(
    /* [in] */ ICanvas* c)
{
    ViewGroup::Draw(c);
    if (mWindowContentOverlay != NULL && !mIgnoreWindowContentOverlay) {
        IView* barTop = IView::Probe(mActionBarTop);
        Int32 visibility = 0;
        barTop->GetVisibility(&visibility);
        Int32 bottom = 0;
        Float transY = 0;
        barTop->GetBottom(&bottom);
        barTop->GetTranslationY(&transY);
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
    *result = FALSE;
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnStartNestedScroll(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 axes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 visibility = 0;
    if ((axes & SCROLL_AXIS_VERTICAL) == 0 ||
        (IView::Probe(mActionBarTop)->GetVisibility(&visibility), visibility != VISIBLE)) {
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
    FAIL_RETURN(ViewGroup::OnNestedScrollAccepted(child, target, axes));
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
    mHideOnContentScrollReference += dyConsumed;
    SetActionBarHideOffset(mHideOnContentScrollReference);
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnStopNestedScroll(
    /* [in] */ IView* target)
{
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
    VALIDATE_NOT_NULL(result);
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
    mAnimatingForFling = TRUE;
    *result = TRUE;
    return NOERROR;
}

ECode ActionBarOverlayLayout::PullChildren()
{
    if (mContent == NULL) {
        FindViewById(R::id::content, (IView**)&mContent);
        AutoPtr<IView> viewTmp;
        FindViewById(R::id::action_bar_container, (IView**)&viewTmp);
        mActionBarTop = IActionBarContainer::Probe(viewTmp);
        viewTmp = NULL;
        FindViewById(R::id::action_bar, (IView**)&viewTmp);
        mDecorToolbar = NULL;
        GetDecorToolbar(viewTmp, (IDecorToolbar**)&mDecorToolbar);
        viewTmp = NULL;
        FindViewById(R::id::split_action_bar, (IView**)&viewTmp);
        mActionBarBottom = IActionBarContainer::Probe(viewTmp);
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetHideOnContentScrollEnabled(
    /* [in] */ Boolean hideOnContentScroll)
{
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
    *result = mHideOnContentScroll;
    return NOERROR;
}

ECode ActionBarOverlayLayout::GetActionBarHideOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (mActionBarTop != NULL) {
        Float transY = 0;
        IView::Probe(mActionBarTop)->GetTranslationY(&transY);
        *result = -(Int32)transY;
    }

    return NOERROR;
}

ECode ActionBarOverlayLayout::SetActionBarHideOffset(
    /* [in] */ Int32 offset)
{
    using Elastos::Core::Math;
    HaltActionBarHideOffsetAnimations();
    IView* barTop = IView::Probe(mActionBarTop);
    Int32 topHeight = 0;
    barTop->GetHeight(&topHeight);
    offset = Math::Max(0, Math::Min(offset, topHeight));
    barTop->SetTranslationY(-offset);
    Int32 visibility = 0;
    if (mActionBarBottom != NULL) {
        IView* barBottom = IView::Probe(mActionBarBottom);
        if (barBottom->GetVisibility(&visibility), visibility != IView::GONE) {
            // Match the hide offset proportionally for a split bar
            Float fOffset = (Float)offset / topHeight;
            Int32 height = 0;
            barBottom->GetHeight(&height);
            Int32 bOffset = (Int32)(height * fOffset);
            barBottom->SetTranslationY(bOffset);
        }
    }
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetWindowCallback(
    /* [in] */ IWindowCallback* cb)
{
    PullChildren();
    mDecorToolbar->SetWindowCallback(cb);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetWindowTitle(
    /* [in] */ ICharSequence* title)
{
    PullChildren();
    mDecorToolbar->SetWindowTitle(title);
    return NOERROR;
}

ECode ActionBarOverlayLayout::GetTitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    PullChildren();
    return mDecorToolbar->GetTitle(result);
}

ECode ActionBarOverlayLayout::InitFeature(
    /* [in] */ Int32 windowFeature)
{
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
    Boolean splitActionBar = FALSE;
    Boolean splitWhenNarrow =
            (uiOptions & IActivityInfo::UIOPTION_SPLIT_ACTION_BAR_WHEN_NARROW) != 0;
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
        if (mActionBarBottom != NULL && (mDecorToolbar->CanSplit(&canSplit), canSplit)) {
            IViewGroup* abb = IViewGroup::Probe(mActionBarBottom);
            mDecorToolbar->SetSplitView(abb);
            mDecorToolbar->SetSplitToolbar(splitActionBar);
            mDecorToolbar->SetSplitWhenNarrow(splitWhenNarrow);

            AutoPtr<IView> viewTmp;
            FindViewById(R::id::action_context_bar, (IView**)&viewTmp);
            CActionBarContextView* cab = (CActionBarContextView*)viewTmp.Get();
            cab->SetSplitView(abb);
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
    PullChildren();
    mDecorToolbar->HasIcon(result);
    return NOERROR;
}

ECode ActionBarOverlayLayout::HasLogo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    PullChildren();
    mDecorToolbar->HasLogo(result);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetIcon(
    /* [in] */ Int32 resId)
{
    PullChildren();
    mDecorToolbar->SetIcon(resId);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetIcon(
    /* [in] */ IDrawable* d)
{
    PullChildren();
    mDecorToolbar->SetIcon(d);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetLogo(
    /* [in] */ Int32 resId)
{
    PullChildren();
    mDecorToolbar->SetLogo(resId);
    return NOERROR;
}

ECode ActionBarOverlayLayout::CanShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    PullChildren();
    return mDecorToolbar->CanShowOverflowMenu(result);
}

ECode ActionBarOverlayLayout::IsOverflowMenuShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    PullChildren();
    return mDecorToolbar->IsOverflowMenuShowing(result);
}

ECode ActionBarOverlayLayout::IsOverflowMenuShowPending(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    PullChildren();
    return mDecorToolbar->IsOverflowMenuShowPending(result);
}

ECode ActionBarOverlayLayout::ShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    PullChildren();
    return mDecorToolbar->ShowOverflowMenu(result);
}

ECode ActionBarOverlayLayout::HideOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    PullChildren();
    return mDecorToolbar->HideOverflowMenu(result);
}

ECode ActionBarOverlayLayout::SetMenuPrepared()
{
    PullChildren();
    mDecorToolbar->SetMenuPrepared();
    return NOERROR;
}

ECode ActionBarOverlayLayout::SetMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuPresenterCallback* cb)
{
    PullChildren();
    mDecorToolbar->SetMenu(menu, cb);
    return NOERROR;
}

ECode ActionBarOverlayLayout::SaveToolbarHierarchyState(
    /* [in] */ ISparseArray* toolbarStates)
{
    PullChildren();
    mDecorToolbar->SaveHierarchyState(toolbarStates);
    return NOERROR;
}

ECode ActionBarOverlayLayout::RestoreToolbarHierarchyState(
    /* [in] */ ISparseArray* toolbarStates)
{
    PullChildren();
    mDecorToolbar->RestoreHierarchyState(toolbarStates);
    return NOERROR;
}

ECode ActionBarOverlayLayout::DismissPopups()
{
    PullChildren();
    mDecorToolbar->DismissPopupMenus();
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnDetachedFromWindow()
{
    ViewGroup::OnDetachedFromWindow();
    HaltActionBarHideOffsetAnimations();
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    ViewGroup::OnConfigurationChanged(newConfig);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Init(context);
    RequestApplyInsets();
    return NOERROR;
}

void ActionBarOverlayLayout::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
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
    return CActionBarOverlayLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::MATCH_PARENT, result);
}

ECode ActionBarOverlayLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p,
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);
    return CActionBarOverlayLayoutLayoutParams::New(p, result);
}

Boolean ActionBarOverlayLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return IActionBarOverlayLayoutLayoutParams::Probe(p) != NULL;
}

ECode ActionBarOverlayLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    PullChildren();

    Int32 maxHeight = 0;
    Int32 maxWidth = 0;
    Int32 childState = 0;

    Int32 topInset = 0;
    Int32 bottomInset = 0;

    IView* barTop = IView::Probe(mActionBarTop);
    IView* barBottom = IView::Probe(mActionBarBottom);

    MeasureChildWithMargins(barTop, widthMeasureSpec, 0, heightMeasureSpec, 0);
    AutoPtr<IViewGroupLayoutParams> lp;
    barTop->GetLayoutParams((IViewGroupLayoutParams**)&lp);

    using Elastos::Core::Math;

    Int32 measureWidth = 0, measureHeight = 0, measureState = 0;
    Int32 leftMargin = 0, rightMargin = 0, topMargin = 0, bottomMargin = 0;
    barTop->GetMeasuredWidth(&measureWidth);
    barTop->GetMeasuredHeight(&measureHeight);
    barTop->GetMeasuredState(&measureState);
    IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin);
    maxWidth = Math::Max(maxWidth, measureWidth + leftMargin + rightMargin);
    maxHeight = Math::Max(maxHeight, measureHeight + topMargin + bottomMargin);
    childState = CombineMeasuredStates(childState, measureState);

    // xlarge screen layout doesn't have bottom action bar.
    if (mActionBarBottom != NULL) {
        MeasureChildWithMargins(barBottom, widthMeasureSpec, 0, heightMeasureSpec, 0);

        lp = NULL;
        barBottom->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        barBottom->GetMeasuredWidth(&measureWidth);
        barBottom->GetMeasuredHeight(&measureHeight);
        barBottom->GetMeasuredState(&measureState);
        IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin);

        maxWidth = Math::Max(maxWidth, measureWidth + leftMargin + rightMargin);
        maxHeight = Math::Max(maxHeight, measureHeight + topMargin + bottomMargin);
        childState = CombineMeasuredStates(childState, measureState);
    }

    Int32 vis = 0;
    GetWindowSystemUiVisibility(&vis);
    Boolean stable = (vis & SYSTEM_UI_FLAG_LAYOUT_STABLE) != 0;

    Int32 visibility = 0;
    barTop->GetVisibility(&visibility);

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
        barTop->GetMeasuredHeight(&topInset);
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
                barBottom->GetMeasuredHeight(&bottomInset);
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
    IViewGroupMarginLayoutParams::Probe(lp)->GetMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin);

    maxWidth = Math::Max(maxWidth, measureWidth + leftMargin + rightMargin);
    maxHeight = Math::Max(maxHeight, measureHeight + topMargin + bottomMargin);
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
    maxHeight = Math::Max(maxHeight, GetSuggestedMinimumHeight());
    maxWidth = Math::Max(maxWidth, GetSuggestedMinimumWidth());

    SetMeasuredDimension(ResolveSizeAndState(maxWidth, widthMeasureSpec, childState),
            ResolveSizeAndState(maxHeight, heightMeasureSpec, childState << MEASURED_HEIGHT_STATE_SHIFT));
    return NOERROR;
}

ECode ActionBarOverlayLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
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
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 visibility = 0;
        child->GetVisibility(&visibility);
        if (visibility != IView::GONE) {
            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);

            Int32 width = 0, height = 0;
            child->GetMeasuredWidth(&width);
            child->GetMeasuredHeight(&height);

            IViewGroupMarginLayoutParams* vgmlp = IViewGroupMarginLayoutParams::Probe(lp);

            Int32 leftMargin = 0;
            vgmlp->GetLeftMargin(&leftMargin);
            Int32 childLeft = parentLeft + leftMargin;
            Int32 childTop;
            if (child.Get() == IView::Probe(mActionBarBottom)) {
                Int32 bottomMargin = 0;
                vgmlp->GetBottomMargin(&bottomMargin);
                childTop = parentBottom - height - bottomMargin;
            }
            else {
                Int32 topMargin = 0;
                vgmlp->GetTopMargin(&topMargin);
                childTop = parentTop + topMargin;
            }

            child->Layout(childLeft, childTop, childLeft + width, childTop + height);
        }
    }
    return NOERROR;
}

void ActionBarOverlayLayout::Init(
    /* [in] */ IContext* context)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResourcesTheme> theme;
    ctx->GetTheme((IResourcesTheme**)&theme);
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
    Boolean changed = FALSE;
    AutoPtr<IViewGroupLayoutParams> lp;
    view->GetLayoutParams((IViewGroupLayoutParams**)&lp);

    IViewGroupMarginLayoutParams* vgmlp = IViewGroupMarginLayoutParams::Probe(lp);

    Int32 lpValue = 0, rValue = 0;
    vgmlp->GetLeftMargin(&lpValue);
    insets->GetLeft(&rValue);
    if (left && lpValue != rValue) {
        changed = TRUE;
        vgmlp->SetLeftMargin(rValue);
    }
    vgmlp->GetTopMargin(&lpValue);
    insets->GetTop(&rValue);
    if (top && lpValue != rValue) {
        changed = TRUE;
        vgmlp->SetTopMargin(rValue);
    }
    vgmlp->GetRightMargin(&lpValue);
    insets->GetRight(&rValue);
    if (right && lpValue != rValue) {
        changed = TRUE;
        vgmlp->SetRightMargin(rValue);
    }
    vgmlp->GetBottomMargin(&lpValue);
    insets->GetBottom(&rValue);
    if (bottom && lpValue != rValue) {
        changed = TRUE;
        vgmlp->SetBottomMargin(rValue);
    }
    return changed;
}

ECode ActionBarOverlayLayout::GetDecorToolbar(
    /* [in] */ IView* view,
    /* [out] */ IDecorToolbar** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (IDecorToolbar::Probe(view) != NULL) {
        *result = IDecorToolbar::Probe(view);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (IToolbar::Probe(view) != NULL) {
        return IToolbar::Probe(view)->GetWrapper(result);
    }
    else {
        //throw new IllegalStateException("Can't make a decor toolbar out of " +
        //        view.getClass().getSimpleName());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

void ActionBarOverlayLayout::HaltActionBarHideOffsetAnimations()
{
    Boolean res;
    RemoveCallbacks(mRemoveActionBarHideOffset, &res);
    RemoveCallbacks(mAddActionBarHideOffset, &res);
    if (mCurrentActionBarTopAnimator != NULL) {
        mCurrentActionBarTopAnimator->Cancel();
    }
    if (mCurrentActionBarBottomAnimator != NULL) {
        mCurrentActionBarBottomAnimator->Cancel();
    }
}

void ActionBarOverlayLayout::PostRemoveActionBarHideOffset()
{
    HaltActionBarHideOffsetAnimations();
    Boolean res;
    PostDelayed(mRemoveActionBarHideOffset, ACTION_BAR_ANIMATE_DELAY, &res);
}

void ActionBarOverlayLayout::PostAddActionBarHideOffset()
{
    HaltActionBarHideOffsetAnimations();
    Boolean res;
    PostDelayed(mAddActionBarHideOffset, ACTION_BAR_ANIMATE_DELAY, &res);
}

void ActionBarOverlayLayout::RemoveActionBarHideOffset()
{
    HaltActionBarHideOffsetAnimations();
    mRemoveActionBarHideOffset->Run();
}

void ActionBarOverlayLayout::AddActionBarHideOffset()
{
    HaltActionBarHideOffsetAnimations();
    mAddActionBarHideOffset->Run();
}

Boolean ActionBarOverlayLayout::ShouldHideActionBarOnFling(
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY)
{
    using Elastos::Core::Math;
    mFlingEstimator->Fling(0, 0, 0, (Int32)velocityY, 0, 0,
            Math::INT32_MIN_VALUE, Math::INT32_MAX_VALUE);
    Int32 finalY = 0;
    mFlingEstimator->GetFinalY(&finalY);
    Int32 height = 0;
    IView::Probe(mActionBarTop)->GetHeight(&height);
    return finalY > height;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos