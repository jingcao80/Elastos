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

#include "elastos/droid/systemui/keyguard/CSlidingChallengeLayout.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include "R.h"

using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Utility::EIID_IProperty;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::Animation::EIID_IInterpolator;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::Widget::CScroller;
using Elastos::Core::CoreUtils;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CSlidingChallengeLayout::MyProperty::SetValue(
    /* [in] */ IInterface* obj,
    /* [in] */ Float value)
{
    AutoPtr<CSlidingChallengeLayout> view =
            (CSlidingChallengeLayout*)ISlidingChallengeLayout::Probe(obj);
    view->mHandleAlpha = value;
    return view->Invalidate();
}

ECode CSlidingChallengeLayout::MyProperty::Get(
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** rst)
{
    VALIDATE_NOT_NULL(rst)

    AutoPtr<CSlidingChallengeLayout> view =
            (CSlidingChallengeLayout*)ISlidingChallengeLayout::Probe(obj);
    AutoPtr<IFloat> _obj = CoreUtils::Convert(view->mHandleAlpha);
    *rst = TO_IINTERFACE(_obj);
    REFCOUNT_ADD(*rst)
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CSlidingChallengeLayout::MyInterpolator, Object, IInterpolator,
        ITimeInterpolator)

ECode CSlidingChallengeLayout::MyInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output)

    t -= 1.0f;
    *output = t * t * t * t * t + 1.0f;
    return NOERROR;
}

ECode CSlidingChallengeLayout::MyInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    *res = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CSlidingChallengeLayout::MyInterpolator2, Object, IInterpolator,
        ITimeInterpolator)

ECode CSlidingChallengeLayout::MyInterpolator2::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output)

    *output = t * t;
    return NOERROR;
}

ECode CSlidingChallengeLayout::MyInterpolator2::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    *res = FALSE;
    return NOERROR;
}

ECode CSlidingChallengeLayout::MyRunnable::Run()
{
    return mHost->CompleteChallengeScroll();
}

CAR_INTERFACE_IMPL(CSlidingChallengeLayout::MyOnClickListener,
        Object, IViewOnClickListener)

ECode CSlidingChallengeLayout::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->HideBouncer();
}

CAR_INTERFACE_IMPL(CSlidingChallengeLayout::MyOnClickListener2,
        Object, IViewOnClickListener)

ECode CSlidingChallengeLayout::MyOnClickListener2::OnClick(
    /* [in] */ IView* v)
{
    Boolean res;
    if (mHost->IsChallengeShowing(&res), !res) {
        mHost->ShowChallenge(TRUE);
    }
    return NOERROR;
}

ECode CSlidingChallengeLayout::MyAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return mHost->mScrimView->SetVisibility(VISIBLE);
}

ECode CSlidingChallengeLayout::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mHost->mScrimView->SetVisibility(GONE);
}

ECode CSlidingChallengeLayout::MyAnimatorListenerAdapter3::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mHost->OnFadeStart(mShow);
    return NOERROR;
}

ECode CSlidingChallengeLayout::MyAnimatorListenerAdapter3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->OnFadeEnd(mShow);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CSlidingChallengeLayout::SlidingChallengeLayoutLayoutParams,
        ViewGroup::MarginLayoutParams, ISlidingChallengeLayoutLayoutParams)

CSlidingChallengeLayout::SlidingChallengeLayoutLayoutParams::SlidingChallengeLayoutLayoutParams()
    : mChildType(CHILD_TYPE_NONE)
    , mMaxHeight(0)
{
}

ECode CSlidingChallengeLayout::SlidingChallengeLayoutLayoutParams::constructor()
{
    return constructor(MATCH_PARENT, WRAP_CONTENT);
}

ECode CSlidingChallengeLayout::SlidingChallengeLayoutLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return ViewGroup::MarginLayoutParams::constructor(width, height);
}

ECode CSlidingChallengeLayout::SlidingChallengeLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(source);
}

ECode CSlidingChallengeLayout::SlidingChallengeLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(source);
}

ECode CSlidingChallengeLayout::SlidingChallengeLayoutLayoutParams::constructor(
    /* [in] */ ISlidingChallengeLayoutLayoutParams* source)
{
    ViewGroup::MarginLayoutParams::constructor(IViewGroupLayoutParams::Probe(source));

    mChildType = ((SlidingChallengeLayoutLayoutParams*)source)->mChildType;
    return NOERROR;
}

ECode CSlidingChallengeLayout::SlidingChallengeLayoutLayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::MarginLayoutParams::constructor(c, attrs);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::SlidingChallengeLayout_Layout);
    AutoPtr<ITypedArray> a;
    c->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetInt32(R::styleable::SlidingChallengeLayout_Layout_layout_childType,
            CHILD_TYPE_NONE, &mChildType);
    a->GetDimensionPixelSize(
            R::styleable::SlidingChallengeLayout_Layout_layout_maxHeight, 0, &mMaxHeight);
    return a->Recycle();
}

const String CSlidingChallengeLayout::TAG("SlidingChallengeLayout");
const Boolean CSlidingChallengeLayout::DEBUG = IKeyguardConstants::DEBUG;

// The drag handle is measured in dp above & below the top edge of the
// challenge view; these parameters change based on whether the challenge
// is open or closed.
const Int32 CSlidingChallengeLayout::DRAG_HANDLE_CLOSED_ABOVE = 8; // dp
const Int32 CSlidingChallengeLayout::DRAG_HANDLE_CLOSED_BELOW = 0; // dp
const Int32 CSlidingChallengeLayout::DRAG_HANDLE_OPEN_ABOVE = 8; // dp
const Int32 CSlidingChallengeLayout::DRAG_HANDLE_OPEN_BELOW = 0; // dp

const Int32 CSlidingChallengeLayout::HANDLE_ANIMATE_DURATION = 250; // ms

const Int32 CSlidingChallengeLayout::MAX_SETTLE_DURATION = 600; // ms

const Int32 CSlidingChallengeLayout::INVALID_POINTER = -1;

AutoPtr<IProperty> CSlidingChallengeLayout::HANDLE_ALPHA = new MyProperty(String("handleAlpha"));

AutoPtr<IInterpolator> CSlidingChallengeLayout::sMotionInterpolator = new MyInterpolator();

AutoPtr<IInterpolator> CSlidingChallengeLayout::sHandleFadeInterpolator = new MyInterpolator2();

CAR_OBJECT_IMPL(CSlidingChallengeLayout)

CAR_INTERFACE_IMPL_2(CSlidingChallengeLayout, ViewGroup, IChallengeLayout, ISlidingChallengeLayout)

CSlidingChallengeLayout::CSlidingChallengeLayout()
    : mEdgeCaptured(FALSE)
    , mChallengeOffset(1.0f)
    , mChallengeShowing(TRUE)
    , mChallengeShowingTargetState(TRUE)
    , mWasChallengeShowing(TRUE)
    , mIsBouncing(FALSE)
    , mScrollState(0)
    , mEnableChallengeDragging(FALSE)
    , mActivePointerId(INVALID_POINTER)
    , mDragging(FALSE)
    , mBlockDrag(FALSE)
    , mMinVelocity(0)
    , mMaxVelocity(0)
    , mGestureStartX(0.0f)
    , mGestureStartY(0.0f)
    , mGestureStartChallengeBottom(0)
    , mDragHandleClosedBelow(0)
    , mDragHandleClosedAbove(0)
    , mDragHandleOpenBelow(0)
    , mDragHandleOpenAbove(0)
    , mDragHandleEdgeSlop(0)
    , mChallengeBottomBound(0)
    , mTouchSlop(0)
    , mTouchSlopSquare(0)
    , mHandleAlpha(0.0f)
    , mFrameAlpha(0.0f)
    , mFrameAnimationTarget(Elastos::Core::Math::FLOAT_MIN_VALUE)
    , mHasGlowpad(FALSE)
    , mChallengeInteractiveExternal(TRUE)
    , mChallengeInteractiveInternal(TRUE)
    , mHasLayout(FALSE)
{
    CRect::New((IRect**)&mInsets);

    mEndScrollRunnable = new MyRunnable(this);

    mScrimClickListener = new MyOnClickListener(this);

    mExpandChallengeClickListener = new MyOnClickListener2(this);
}

ECode CSlidingChallengeLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CSlidingChallengeLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CSlidingChallengeLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ViewGroup::constructor(context, attrs, defStyle);

    CScroller::New(context, sMotionInterpolator, (IScroller**)&mScroller);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(context, (IViewConfiguration**)&vc);

    vc->GetScaledMinimumFlingVelocity(&mMinVelocity);
    vc->GetScaledMaximumFlingVelocity(&mMaxVelocity);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::kg_edge_swipe_region_size, &mDragHandleEdgeSlop);

    vc->GetScaledTouchSlop(&mTouchSlop);
    mTouchSlopSquare = mTouchSlop * mTouchSlop;

    res->GetDisplayMetrics((IDisplayMetrics**)&mDisplayMetrics);
    Float density;
    mDisplayMetrics->GetDensity(&density);

    // top half of the lock icon, plus another 25% to be sure
    mDragHandleClosedAbove = (Int32)(DRAG_HANDLE_CLOSED_ABOVE * density + 0.5f);
    mDragHandleClosedBelow = (Int32)(DRAG_HANDLE_CLOSED_BELOW * density + 0.5f);
    mDragHandleOpenAbove = (Int32)(DRAG_HANDLE_OPEN_ABOVE * density + 0.5f);
    mDragHandleOpenBelow = (Int32)(DRAG_HANDLE_OPEN_BELOW * density + 0.5f);

    // how much space to account for in the handle when closed
    res->GetDimensionPixelSize(R::dimen::kg_widget_pager_bottom_padding, &mChallengeBottomBound);

    SetWillNotDraw(FALSE);
    return SetSystemUiVisibility(SYSTEM_UI_FLAG_LAYOUT_STABLE | SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);
}

ECode CSlidingChallengeLayout::SetEnableChallengeDragging(
    /* [in] */ Boolean enabled)
{
    mEnableChallengeDragging = enabled;
    return NOERROR;
}

ECode CSlidingChallengeLayout::SetInsets(
    /* [in] */ IRect* insets)
{
    return mInsets->Set(insets);
}

ECode CSlidingChallengeLayout::SetHandleAlpha(
    /* [in] */ Float alpha)
{
    if (mExpandChallengeView != NULL) {
        mExpandChallengeView->SetAlpha(alpha);
    }
    return NOERROR;
}

ECode CSlidingChallengeLayout::SetChallengeInteractive(
    /* [in] */ Boolean interactive)
{
    mChallengeInteractiveExternal = interactive;
    if (mExpandChallengeView != NULL) {
        mExpandChallengeView->SetEnabled(interactive);
    }
    return NOERROR;
}

ECode CSlidingChallengeLayout::AnimateHandle(
    /* [in] */ Boolean visible)
{
    if (mHandleAnimation != NULL) {
        IAnimator::Probe(mHandleAnimation)->Cancel();
        mHandleAnimation = NULL;
    }
    Float targetAlpha = visible ? 1.0f : 0.0f;
    if (targetAlpha == mHandleAlpha) {
        return NOERROR;
    }
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
    (*array)[0] = targetAlpha;
    helper->OfFloat(TO_IINTERFACE(this), HANDLE_ALPHA, array, (IObjectAnimator**)&mHandleAnimation);
    IAnimator::Probe(mHandleAnimation)->SetInterpolator(
            ITimeInterpolator::Probe(sHandleFadeInterpolator));
    IAnimator::Probe(mHandleAnimation)->SetDuration(HANDLE_ANIMATE_DURATION);
    return IAnimator::Probe(mHandleAnimation)->Start();
}

void CSlidingChallengeLayout::SendInitialListenerUpdates()
{
    if (mScrollListener != NULL) {
        Int32 challengeTop = 0;
        if (mChallengeView != NULL) {
            IView::Probe(mChallengeView)->GetTop(&challengeTop);
        }
        mScrollListener->OnScrollPositionChanged(mChallengeOffset, challengeTop);
        mScrollListener->OnScrollStateChanged(mScrollState);
    }
}

ECode CSlidingChallengeLayout::SetOnChallengeScrolledListener(
    /* [in] */ ISlidingChallengeLayoutOnChallengeScrolledListener* listener)
{
    mScrollListener = listener;
    if (mHasLayout) {
        SendInitialListenerUpdates();
    }
    return NOERROR;
}

ECode CSlidingChallengeLayout::SetOnBouncerStateChangedListener(
    /* [in] */ IChallengeLayoutOnBouncerStateChangedListener* listener)
{
    mBouncerListener = listener;
    return NOERROR;
}

ECode CSlidingChallengeLayout::OnAttachedToWindow()
{
    ViewGroup::OnAttachedToWindow();

    mHasLayout = FALSE;
    return NOERROR;
}

ECode CSlidingChallengeLayout::OnDetachedFromWindow()
{
    ViewGroup::OnDetachedFromWindow();

    Boolean res;
    RemoveCallbacks(mEndScrollRunnable, &res);
    mHasLayout = FALSE;
    return NOERROR;
}

ECode CSlidingChallengeLayout::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    if (mIsBouncing && TO_IINTERFACE(child) != TO_IINTERFACE(mChallengeView)) {
        // Clear out of the bouncer if the user tries to move focus outside of
        // the security challenge view.
        HideBouncer();
    }
    return ViewGroup::RequestChildFocus(child, focused);
}

ECode CSlidingChallengeLayout::DistanceInfluenceForSnapDuration(
    /* [in] */ Float f,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)

    f -= 0.5f; // center the values about 0.
    f *= 0.3f * Elastos::Core::Math::PI / 2.0f;
    *value = (Float)Elastos::Core::Math::Sin(f);
    return NOERROR;
}

ECode CSlidingChallengeLayout::SetScrollState(
    /* [in] */ Int32 state)
{
    if (mScrollState != state) {
        mScrollState = state;

        AnimateHandle(state == SCROLL_STATE_IDLE && !mChallengeShowing);
        if (mScrollListener != NULL) {
            mScrollListener->OnScrollStateChanged(state);
        }
    }
    return NOERROR;
}

ECode CSlidingChallengeLayout::CompleteChallengeScroll()
{
    SetChallengeShowing(mChallengeShowingTargetState);
    mChallengeOffset = mChallengeShowing ? 1.f : 0.f;
    SetScrollState(SCROLL_STATE_IDLE);
    mChallengeInteractiveInternal = TRUE;
    return IView::Probe(mChallengeView)->SetLayerType(LAYER_TYPE_NONE, NULL);
}

ECode CSlidingChallengeLayout::SetScrimView(
    /* [in] */ IView* scrim)
{
    if (mScrimView != NULL) {
        mScrimView->SetOnClickListener(NULL);
    }
    mScrimView = scrim;
    if (mScrimView != NULL) {
        mScrimView->SetVisibility(mIsBouncing ? VISIBLE : GONE);
        mScrimView->SetFocusable(TRUE);
        mScrimView->SetOnClickListener(mScrimClickListener);
    }
    return NOERROR;
}

ECode CSlidingChallengeLayout::AnimateChallengeTo(
    /* [in] */ Int32 y,
    /* [in] */ Int32 velocity)
{
    if (mChallengeView == NULL) {
        // Nothing to do.
        return NOERROR;
    }

    CancelTransitionsInProgress();

    mChallengeInteractiveInternal = FALSE;
    EnableHardwareLayerForChallengeView();
    Int32 sy;
    IView::Probe(mChallengeView)->GetBottom(&sy);
    Int32 dy = y - sy;
    if (dy == 0) {
        CompleteChallengeScroll();
        return NOERROR;
    }

    SetScrollState(SCROLL_STATE_SETTLING);

    Int32 childHeight;
    IView::Probe(mChallengeView)->GetHeight(&childHeight);
    Int32 halfHeight = childHeight / 2;
    Float distanceRatio = Elastos::Core::Math::Min(1.0f, 1.0f * Elastos::Core::Math::Abs(dy) / childHeight);

    Float tmp;
    DistanceInfluenceForSnapDuration(distanceRatio, &tmp);
    Float distance = halfHeight + halfHeight * tmp;

    Int32 duration = 0;
    velocity = Elastos::Core::Math::Abs(velocity);
    if (velocity > 0) {
        duration = 4 * Elastos::Core::Math::Round(1000 * Elastos::Core::Math::Abs(distance / velocity));
    }
    else {
        Float childDelta = (Float) Elastos::Core::Math::Abs(dy) / childHeight;
        duration = (Int32)((childDelta + 1) * 100);
    }
    duration = Elastos::Core::Math::Min(duration, MAX_SETTLE_DURATION);

    mScroller->StartScroll(0, sy, 0, dy, duration);
    return PostInvalidateOnAnimation();
}

void CSlidingChallengeLayout::SetChallengeShowing(
    /* [in] */ Boolean showChallenge)
{
    if (mChallengeShowing == showChallenge) {
        return;
    }
    mChallengeShowing = showChallenge;

    if (mExpandChallengeView == NULL || mChallengeView == NULL) {
        // These might not be here yet if we haven't been through layout.
        // If we haven't, the first layout pass will set everything up correctly
        // based on mChallengeShowing as set above.
        return;
    }

    AutoPtr<IAccessibilityManagerHelper> helper;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&helper);
    if (mChallengeShowing) {
        mExpandChallengeView->SetVisibility(IView::INVISIBLE);
        IView::Probe(mChallengeView)->SetVisibility(IView::VISIBLE);
        Boolean res;
        AutoPtr<IAccessibilityManager> manager;
        helper->GetInstance(mContext, (IAccessibilityManager**)&manager);
        if (manager->IsEnabled(&res), res) {
            Boolean res;
            IView::Probe(mChallengeView)->RequestAccessibilityFocus(&res);
            String expanded;
            mContext->GetString(R::string::keyguard_accessibility_unlock_area_expanded, &expanded);
            AutoPtr<ICharSequence> ccahr = CoreUtils::Convert(expanded);
            IView::Probe(mChallengeView)->AnnounceForAccessibility(ccahr);
        }
    }
    else {
        mExpandChallengeView->SetVisibility(IView::VISIBLE);
        IView::Probe(mChallengeView)->SetVisibility(IView::INVISIBLE);
        Boolean res;
        AutoPtr<IAccessibilityManager> manager;
        helper->GetInstance(mContext, (IAccessibilityManager**)&manager);
        if (manager->IsEnabled(&res), res) {
            mExpandChallengeView->RequestAccessibilityFocus(&res);
            String collapsed;
            mContext->GetString(R::string::keyguard_accessibility_unlock_area_collapsed, &collapsed);
            AutoPtr<ICharSequence> ccahr = CoreUtils::Convert(collapsed);
            IView::Probe(mChallengeView)->AnnounceForAccessibility(ccahr);
        }
    }
}

ECode CSlidingChallengeLayout::IsChallengeShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mChallengeShowing;
    return NOERROR;
}

ECode CSlidingChallengeLayout::IsChallengeOverlapping(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mChallengeShowing;
    return NOERROR;
}

ECode CSlidingChallengeLayout::IsBouncing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIsBouncing;
    return NOERROR;
}

ECode CSlidingChallengeLayout::GetBouncerAnimationDuration(
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration)

    *duration = HANDLE_ANIMATE_DURATION;
    return NOERROR;
}

ECode CSlidingChallengeLayout::ShowBouncer()
{
    if (mIsBouncing) return NOERROR;
    Int32 visibility;
    GetSystemUiVisibility(&visibility);
    SetSystemUiVisibility(visibility | STATUS_BAR_DISABLE_SEARCH);
    mWasChallengeShowing = mChallengeShowing;
    mIsBouncing = TRUE;
    ShowChallenge(TRUE);
    if (mScrimView != NULL) {
        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 1.0f;
        AutoPtr<IObjectAnimator> oa;
        helper->OfFloat(mScrimView, String("alpha"), array, (IObjectAnimator**)&oa);
        AutoPtr<IAnimator> anim = IAnimator::Probe(oa);
        anim->SetDuration(HANDLE_ANIMATE_DURATION);
        AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter(this);
        anim->AddListener(lis);
        anim->Start();
    }
    if (mChallengeView != NULL) {
        mChallengeView->ShowBouncer(HANDLE_ANIMATE_DURATION);
    }

    if (mBouncerListener != NULL) {
        mBouncerListener->OnBouncerStateChanged(TRUE);
    }
    return NOERROR;
}

ECode CSlidingChallengeLayout::HideBouncer()
{
    if (!mIsBouncing) return NOERROR;
    Int32 visibility;
    GetSystemUiVisibility(&visibility);
    SetSystemUiVisibility(visibility & ~STATUS_BAR_DISABLE_SEARCH);
    if (!mWasChallengeShowing) ShowChallenge(FALSE);
    mIsBouncing = FALSE;

    if (mScrimView != NULL) {
        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 0.0f;
        AutoPtr<IObjectAnimator> oa;
        helper->OfFloat(mScrimView, String("alpha"), array, (IObjectAnimator**)&oa);
        AutoPtr<IAnimator> anim = IAnimator::Probe(oa);
        anim->SetDuration(HANDLE_ANIMATE_DURATION);
        AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter2(this);
        anim->AddListener(lis);
        anim->Start();
    }
    if (mChallengeView != NULL) {
        mChallengeView->HideBouncer(HANDLE_ANIMATE_DURATION);
    }
    if (mBouncerListener != NULL) {
        mBouncerListener->OnBouncerStateChanged(FALSE);
    }
    return NOERROR;
}

Int32 CSlidingChallengeLayout::GetChallengeMargin(
    /* [in] */ Boolean expanded)
{
    return expanded && mHasGlowpad ? 0 : mDragHandleEdgeSlop;
}

Float CSlidingChallengeLayout::GetChallengeAlpha()
{
    Float x = mChallengeOffset - 1;
    return x * x * x + 1.f;
}

ECode CSlidingChallengeLayout::RequestDisallowInterceptTouchEvent(
    /* [in] */ Boolean allowIntercept)
{
    // We'll intercept whoever we feel like! ...as long as it isn't a challenge view.
    // If there are one or more pointers in the challenge view before we take over
    // touch events, onInterceptTouchEvent will set mBlockDrag.
    return NOERROR;
}

ECode CSlidingChallengeLayout::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> helper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
        helper->Obtain((IVelocityTracker**)&mVelocityTracker);
    }
    mVelocityTracker->AddMovement(ev);

    Int32 action;
    ev->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            ev->GetX(&mGestureStartX);
            ev->GetY(&mGestureStartY);
            mBlockDrag = FALSE;
            break;

        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
            ResetTouch();
            break;

        case IMotionEvent::ACTION_MOVE:
        {
            Int32 count;
            ev->GetPointerCount(&count);
            for (Int32 i = 0; i < count; i++) {
                Float x;
                ev->GetX(i, &x);
                Float y;
                ev->GetY(i, &y);
                if (!mIsBouncing && mActivePointerId == INVALID_POINTER
                            && ((CrossedDragHandle(x, y, mGestureStartY)
                                    && ShouldEnableChallengeDragging())
                                    || ((IsInChallengeView(x, y) &&
                                    mScrollState == SCROLL_STATE_SETTLING)))) {
                    ev->GetPointerId(i, &mActivePointerId);
                    mGestureStartX = x;
                    mGestureStartY = y;
                    mGestureStartChallengeBottom = GetChallengeBottom();
                    mDragging = TRUE;
                    EnableHardwareLayerForChallengeView();
                }
                else if (mChallengeShowing && IsInChallengeView(x, y)
                        && ShouldEnableChallengeDragging()) {
                    mBlockDrag = TRUE;
                }
            }
            break;
        }
    }

    if (mBlockDrag || IsChallengeInteractionBlocked()) {
        mActivePointerId = INVALID_POINTER;
        mDragging = FALSE;
    }

    *result = mDragging;
    return NOERROR;
}

Boolean CSlidingChallengeLayout::ShouldEnableChallengeDragging()
{
    return mEnableChallengeDragging || !mChallengeShowing;
}

Boolean CSlidingChallengeLayout::IsChallengeInteractionBlocked()
{
    return !mChallengeInteractiveExternal || !mChallengeInteractiveInternal;
}

void CSlidingChallengeLayout::ResetTouch()
{
    mVelocityTracker->Recycle();
    mVelocityTracker = NULL;
    mActivePointerId = INVALID_POINTER;
    mDragging = mBlockDrag = FALSE;
}

ECode CSlidingChallengeLayout::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> helper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
        helper->Obtain((IVelocityTracker**)&mVelocityTracker);
    }
    mVelocityTracker->AddMovement(ev);

    Int32 action;
    ev->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            mBlockDrag = FALSE;
            ev->GetX(&mGestureStartX);
            ev->GetY(&mGestureStartY);
            break;

        case IMotionEvent::ACTION_CANCEL:
            if (mDragging && !IsChallengeInteractionBlocked()) {
                ShowChallenge(0);
            }
            ResetTouch();
            break;

        case IMotionEvent::ACTION_POINTER_UP:
        {
            Int32 index;
            ev->GetActionIndex(&index);
            Int32 id;
            ev->GetPointerId(index, &id);
            if (mActivePointerId != id) {
                break;
            }
        }
        case IMotionEvent::ACTION_UP:
            if (mDragging && !IsChallengeInteractionBlocked()) {
                mVelocityTracker->ComputeCurrentVelocity(1000, mMaxVelocity);
                Float id;
                mVelocityTracker->GetYVelocity(mActivePointerId, &id);
                ShowChallenge((Int32)id);
            }
            ResetTouch();
            break;

        case IMotionEvent::ACTION_MOVE:
            if (!mDragging && !mBlockDrag && !mIsBouncing) {
                Int32 count;
                ev->GetPointerCount(&count);
                for (Int32 i = 0; i < count; i++) {
                    Float x;
                    ev->GetX(i, &x);
                    Float y;
                    ev->GetY(i, &y);

                    if ((IsInDragHandle(x, y) || CrossedDragHandle(x, y, mGestureStartY) ||
                            (IsInChallengeView(x, y) && mScrollState == SCROLL_STATE_SETTLING))
                            && mActivePointerId == INVALID_POINTER
                            && !IsChallengeInteractionBlocked()) {
                        mGestureStartX = x;
                        mGestureStartY = y;
                        ev->GetPointerId(i, &mActivePointerId);
                        mGestureStartChallengeBottom = GetChallengeBottom();
                        mDragging = TRUE;
                        EnableHardwareLayerForChallengeView();
                        break;
                    }
                }
            }
            // Not an else; this can be set above.
            if (mDragging) {
                // No-op if already in this state, but set it here in case we arrived
                // at this point from either intercept or the above.
                SetScrollState(SCROLL_STATE_DRAGGING);

                Int32 index;
                ev->FindPointerIndex(mActivePointerId, &index);
                if (index < 0) {
                    // Oops, bogus state. We lost some touch events somewhere.
                    // Just drop it with no velocity and let things settle.
                    ResetTouch();
                    ShowChallenge(0);
                    *result = TRUE;
                    return NOERROR;
                }
                Float y;
                ev->GetY(index, &y);
                Float pos = Elastos::Core::Math::Min(y - mGestureStartY,
                        (Float)(GetLayoutBottom() - mChallengeBottomBound));

                MoveChallengeTo(mGestureStartChallengeBottom + (Int32)pos);
            }
            break;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CSlidingChallengeLayout::DispatchTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 action;
    ev->GetActionMasked(&action);
    Boolean handled = FALSE;
    if (action == IMotionEvent::ACTION_DOWN) {
        // Defensive programming: if we didn't get the UP or CANCEL, reset anyway.
        mEdgeCaptured = FALSE;
    }
    if (mWidgetsView != NULL && !mIsBouncing && (mEdgeCaptured || IsEdgeSwipeBeginEvent(ev))) {
        // Normally we would need to do a lot of extra stuff here.
        // We can only get away with this because we haven't padded in
        // the widget pager or otherwise transformed it during layout.
        // We also don't support things like splitting MotionEvents.

        // We set handled to captured even if dispatch is returning false here so that
        // we don't send a different view a busted or incomplete event stream.
        Boolean res;
        mWidgetsView->DispatchTouchEvent(ev, &res);
        handled = mEdgeCaptured |= res;
    }

    if (!handled && !mEdgeCaptured) {
        ViewGroup::DispatchTouchEvent(ev, &handled);
    }

    if (action == IMotionEvent::ACTION_UP || action == IMotionEvent::ACTION_CANCEL) {
        mEdgeCaptured = FALSE;
    }

    *result = handled;
    return NOERROR;
}

Boolean CSlidingChallengeLayout::IsEdgeSwipeBeginEvent(
    /* [in] */ IMotionEvent* ev)
{
    Int32 mask;
    ev->GetActionMasked(&mask);
    if (mask != IMotionEvent::ACTION_DOWN) {
        return FALSE;
    }

    Float x;
    ev->GetX(&x);
    Int32 width;
    GetWidth(&width);
    return x < mDragHandleEdgeSlop || x >= width - mDragHandleEdgeSlop;
}

Int32 CSlidingChallengeLayout::GetDragHandleSizeAbove()
{
    Boolean res;
    IsChallengeShowing(&res);
    return res ? mDragHandleOpenAbove : mDragHandleClosedAbove;
}

Int32 CSlidingChallengeLayout::GetDragHandleSizeBelow()
{
    Boolean res;
    IsChallengeShowing(&res);
    return res ? mDragHandleOpenBelow : mDragHandleClosedBelow;
}

Boolean CSlidingChallengeLayout::IsInChallengeView(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return IsPointInView(x, y, IView::Probe(mChallengeView));
}

Boolean CSlidingChallengeLayout::IsInDragHandle(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return IsPointInView(x, y, IView::Probe(mExpandChallengeView));
}

Boolean CSlidingChallengeLayout::IsPointInView(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IView* view)
{
    if (view == NULL) {
        return FALSE;
    }
    Int32 left;
    view->GetLeft(&left);
    Int32 top;
    view->GetTop(&top);
    Int32 right;
    view->GetRight(&right);
    Int32 bottom;
    view->GetBottom(&bottom);
    return x >= left && y >= top && x < right && y < bottom;
}

Boolean CSlidingChallengeLayout::CrossedDragHandle(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float initialY)
{
    Int32 challengeTop;
    IView::Probe(mChallengeView)->GetTop(&challengeTop);

    Int32 width;
    GetWidth(&width);
    Boolean horizOk = x >= 0 && x < width;

    Int32 above = GetDragHandleSizeAbove();
    Int32 below = GetDragHandleSizeBelow();
    Boolean vertOk;
    if (mChallengeShowing) {
        vertOk = initialY < (challengeTop - above) &&
                y > challengeTop + below;
    }
    else {
        vertOk = initialY > challengeTop + below &&
                y < challengeTop - above;
    }
    return horizOk && vertOk;
}

Int32 CSlidingChallengeLayout::MakeChildMeasureSpec(
    /* [in] */ Int32 maxSize,
    /* [in] */ Int32 childDimen)
{
    Int32 mode;
    Int32 size;
    switch (childDimen) {
        case IViewGroupLayoutParams::WRAP_CONTENT:
            mode = MeasureSpec::AT_MOST;
            size = maxSize;
            break;
        case IViewGroupLayoutParams::MATCH_PARENT:
            mode = MeasureSpec::EXACTLY;
            size = maxSize;
            break;
        default:
            mode = MeasureSpec::EXACTLY;
            size = Elastos::Core::Math::Min(maxSize, childDimen);
            break;
    }
    return MeasureSpec::MakeMeasureSpec(size, mode);
}

ECode CSlidingChallengeLayout::OnMeasure(
    /* [in] */ Int32 widthSpec,
    /* [in] */ Int32 heightSpec)
{
    if (MeasureSpec::GetMode(widthSpec) != MeasureSpec::EXACTLY ||
            MeasureSpec::GetMode(heightSpec) != MeasureSpec::EXACTLY) {
        // throw new IllegalArgumentException(
        //         "SlidingChallengeLayout must be measured with an exact size");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 width = MeasureSpec::GetSize(widthSpec);
    Int32 height = MeasureSpec::GetSize(heightSpec);
    SetMeasuredDimension(width, height);

    Int32 itop;
    mInsets->GetTop(&itop);
    Int32 ibottom;
    mInsets->GetBottom(&ibottom);
    Int32 insetHeight = height - itop - ibottom;
    Int32 insetHeightSpec = MeasureSpec::MakeMeasureSpec(insetHeight,
            MeasureSpec::EXACTLY);

    // Find one and only one challenge view.
    AutoPtr<IView> oldChallengeView = IView::Probe(mChallengeView);
    AutoPtr<IView> oldExpandChallengeView = IView::Probe(mChallengeView);
    mChallengeView = NULL;
    mExpandChallengeView = NULL;
    Int32 count;
    GetChildCount(&count);

    // First iteration through the children finds special children and sets any associated
    // state.
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        AutoPtr<IViewGroupLayoutParams> p;
        child->GetLayoutParams((IViewGroupLayoutParams**)&p);
        AutoPtr<SlidingChallengeLayoutLayoutParams> lp =
                (SlidingChallengeLayoutLayoutParams*)ISlidingChallengeLayoutLayoutParams::Probe(p);
        if (lp->mChildType == ISlidingChallengeLayoutLayoutParams::CHILD_TYPE_CHALLENGE) {
            if (mChallengeView != NULL) {
                // throw new IllegalStateException(
                //         "There may only be one child with layout_isChallenge=\"true\"");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            if (IKeyguardSecurityContainer::Probe(child) == NULL) {
                        // throw new IllegalArgumentException(
                        //         "Challenge must be a KeyguardSecurityContainer");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mChallengeView = IKeyguardSecurityContainer::Probe(child);
            if (TO_IINTERFACE(mChallengeView) != TO_IINTERFACE(oldChallengeView)) {
                IView::Probe(mChallengeView)->SetVisibility(
                        mChallengeShowing ? VISIBLE : INVISIBLE);
            }
            // We're going to play silly games with the frame's background drawable later.
            if (!mHasLayout) {
                // Set up the margin correctly based on our content for the first run.
                AutoPtr<IView> view;
                child->FindViewById(R::id::keyguard_selector_view, (IView**)&view);
                mHasGlowpad = view != NULL;
                lp->mRightMargin = GetChallengeMargin(TRUE);
                lp->mLeftMargin = lp->mRightMargin;
            }
        }
        else if (lp->mChildType == ISlidingChallengeLayoutLayoutParams::CHILD_TYPE_EXPAND_CHALLENGE_HANDLE) {
            if (mExpandChallengeView != NULL) {
                // throw new IllegalStateException(
                //         "There may only be one child with layout_childType"
                //         + "=\"expandChallengeHandle\"");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mExpandChallengeView = child;
            if (mExpandChallengeView != oldExpandChallengeView) {
                mExpandChallengeView->SetVisibility(mChallengeShowing ? INVISIBLE : VISIBLE);
                mExpandChallengeView->SetOnClickListener(mExpandChallengeClickListener);
            }
        }
        else if (lp->mChildType == ISlidingChallengeLayoutLayoutParams::CHILD_TYPE_SCRIM) {
            SetScrimView(child);
        }
        else if (lp->mChildType == ISlidingChallengeLayoutLayoutParams::CHILD_TYPE_WIDGETS) {
            mWidgetsView = child;
        }
    }

    // We want to measure the challenge view first, since the KeyguardWidgetPager
    // needs to do things its measure pass that are dependent on the challenge view
    // having been measured.
    Int32 visibility;
    if (mChallengeView != NULL &&
            (IView::Probe(mChallengeView)->GetVisibility(&visibility), visibility) != IView::GONE) {
        // This one's a little funny. If the IME is present - reported in the form
        // of insets on the root view - we only give the challenge the space it would
        // have had if the IME wasn't there in order to keep the rest of the layout stable.
        // We base this on the layout_maxHeight on the challenge view. If it comes out
        // negative or zero, either we didn't have a maxHeight or we're totally out of space,
        // so give up and measure as if this rule weren't there.
        Int32 challengeHeightSpec = insetHeightSpec;
        AutoPtr<IView> root;
        GetRootView((IView**)&root);
        if (root != NULL) {
            AutoPtr<IViewGroupLayoutParams> p;
            IView::Probe(mChallengeView)->GetLayoutParams((IViewGroupLayoutParams**)&p);
            AutoPtr<SlidingChallengeLayoutLayoutParams> lp =
                (SlidingChallengeLayoutLayoutParams*)ISlidingChallengeLayoutLayoutParams::Probe(p);

            Int32 heightP;
            mDisplayMetrics->GetHeightPixels(&heightP);
            Int32 ptop;
            root->GetPaddingTop(&ptop);
            Int32 top;
            mInsets->GetTop(&top);
            Int32 windowHeight = heightP - ptop - top;

            Int32 diff = windowHeight - insetHeight;
            Int32 maxChallengeHeight = lp->mMaxHeight - diff;
            if (maxChallengeHeight > 0) {
                challengeHeightSpec = MakeChildMeasureSpec(maxChallengeHeight, lp->mHeight);
            }
        }
        MeasureChildWithMargins(IView::Probe(mChallengeView), widthSpec, 0,
                challengeHeightSpec, 0);
    }

    // Measure the rest of the children
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 visibility;
        child->GetVisibility(&visibility);
        if (visibility == GONE) {
            continue;
        }
        // Don't measure the challenge view twice!
        if (TO_IINTERFACE(child) == TO_IINTERFACE(mChallengeView)) continue;

        // Measure children. Widget frame measures special, so that we can ignore
        // insets for the IME.
        Int32 parentWidthSpec = widthSpec, parentHeightSpec = insetHeightSpec;

        AutoPtr<IViewGroupLayoutParams> p;
        child->GetLayoutParams((IViewGroupLayoutParams**)&p);
        AutoPtr<SlidingChallengeLayoutLayoutParams> lp =
            (SlidingChallengeLayoutLayoutParams*)ISlidingChallengeLayoutLayoutParams::Probe(p);

        if (lp->mChildType == ISlidingChallengeLayoutLayoutParams::CHILD_TYPE_WIDGETS) {
            AutoPtr<IView> root;
            GetRootView((IView**)&root);
            if (root != NULL) {
                // This calculation is super dodgy and relies on several assumptions.
                // Specifically that the root of the window will be padded in for insets
                // and that the window is LAYOUT_IN_SCREEN.
                Int32 windowWidth;
                mDisplayMetrics->GetWidthPixels(&windowWidth);

                Int32 heightP;
                mDisplayMetrics->GetHeightPixels(&heightP);
                Int32 ptop;
                root->GetPaddingTop(&ptop);
                Int32 top;
                mInsets->GetTop(&top);
                Int32 windowHeight = heightP - ptop - top;

                parentWidthSpec = MeasureSpec::MakeMeasureSpec(
                        windowWidth, MeasureSpec::EXACTLY);
                parentHeightSpec = MeasureSpec::MakeMeasureSpec(
                        windowHeight, MeasureSpec::EXACTLY);
            }
        }
        else if (lp->mChildType == ISlidingChallengeLayoutLayoutParams::CHILD_TYPE_SCRIM) {
            // Allow scrim views to extend into the insets
            parentWidthSpec = widthSpec;
            parentHeightSpec = heightSpec;
        }
        MeasureChildWithMargins(child, parentWidthSpec, 0, parentHeightSpec, 0);
    }
    return NOERROR;
}

ECode CSlidingChallengeLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 paddingLeft;
    GetPaddingLeft(&paddingLeft);
    Int32 paddingTop;
    GetPaddingTop(&paddingTop);
    Int32 paddingRight;
    GetPaddingRight(&paddingRight);
    Int32 paddingBottom;
    GetPaddingBottom(&paddingBottom);
    Int32 width = r - l;
    Int32 height = b - t;

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        Int32 visibility;
        if ((child->GetVisibility(&visibility), visibility) == GONE) continue;

        AutoPtr<IViewGroupLayoutParams> p;
        child->GetLayoutParams((IViewGroupLayoutParams**)&p);
        AutoPtr<SlidingChallengeLayoutLayoutParams> lp =
            (SlidingChallengeLayoutLayoutParams*)ISlidingChallengeLayoutLayoutParams::Probe(p);

        if (lp->mChildType == ISlidingChallengeLayoutLayoutParams::CHILD_TYPE_CHALLENGE) {
            // Challenge views pin to the bottom, offset by a portion of their height,
            // and center horizontally.
            Int32 center = (paddingLeft + width - paddingRight) / 2;
            Int32 childWidth;
            child->GetMeasuredWidth(&childWidth);
            Int32 childHeight;
            child->GetMeasuredHeight(&childHeight);
            Int32 left = center - childWidth / 2;
            Int32 _bottom;
            mInsets->GetBottom(&_bottom);
            Int32 layoutBottom = height - paddingBottom - lp->mBottomMargin - _bottom;
            // We use the top of the challenge view to position the handle, so
            // we never want less than the handle size showing at the bottom.
            Int32 bottom = layoutBottom + (Int32)((childHeight - mChallengeBottomBound)
                    * (1 - mChallengeOffset));
            child->SetAlpha(GetChallengeAlpha());
            child->Layout(left, bottom - childHeight, left + childWidth, bottom);
        }
        else if (lp->mChildType == ISlidingChallengeLayoutLayoutParams::CHILD_TYPE_EXPAND_CHALLENGE_HANDLE) {
            Int32 center = (paddingLeft + width - paddingRight) / 2;

            Int32 width;
            child->GetMeasuredWidth(&width);
            Int32 left = center - width / 2;
            Int32 right = left + width;
            Int32 _bottom;
            mInsets->GetBottom(&_bottom);
            Int32 bottom = height - paddingBottom - lp->mBottomMargin - _bottom;

            Int32 _height;
            child->GetMeasuredHeight(&_height);
            Int32 top = bottom - _height;
            child->Layout(left, top, right, bottom);
        }
        else if (lp->mChildType == ISlidingChallengeLayoutLayoutParams::CHILD_TYPE_SCRIM) {
            // Scrim views use the entire area, including padding & insets
            Int32 width;
            GetMeasuredWidth(&width);
            Int32 height;
            GetMeasuredHeight(&height);
            child->Layout(0, 0, width, height);
        }
        else {
            // Non-challenge views lay out from the upper left, layered.
            Int32 top;
            mInsets->GetTop(&top);
            Int32 width;
            child->GetMeasuredWidth(&width);
            Int32 height;
            child->GetMeasuredHeight(&height);
            child->Layout(paddingLeft + lp->mLeftMargin,
                    paddingTop + lp->mTopMargin + top,
                    paddingLeft + width,
                    paddingTop + height + top);
        }
    }

    if (!mHasLayout) {
        mHasLayout = TRUE;
    }
    return NOERROR;
}

ECode CSlidingChallengeLayout::Draw(
    /* [in] */ ICanvas* c)
{
    ViewGroup::Draw(c);
    if (DEBUG) {
        AutoPtr<IPaint> debugPaint;
        CPaint::New((IPaint**)&debugPaint);
        debugPaint->SetColor(0x40FF00CC);
        // show the isInDragHandle() rect

        Int32 top;
        IView::Probe(mChallengeView)->GetTop(&top);
        Int32 above = GetDragHandleSizeAbove();
        Int32 below = GetDragHandleSizeBelow();
        Int32 width;
        GetWidth(&width);
        c->DrawRect(mDragHandleEdgeSlop,
                top - above,
                width - mDragHandleEdgeSlop,
                top + below,
                debugPaint);
    }
    return NOERROR;
}

Boolean CSlidingChallengeLayout::OnRequestFocusInDescendants(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    // Focus security fileds before widgets.
    Boolean res;
    if (mChallengeView != NULL &&
            (IView::Probe(mChallengeView)->RequestFocus(direction,
            previouslyFocusedRect, &res), res)) {
        return TRUE;
    }
    return ViewGroup::OnRequestFocusInDescendants(direction, previouslyFocusedRect);
}

ECode CSlidingChallengeLayout::ComputeScroll()
{
    ViewGroup::ComputeScroll();

    Boolean res;
    if (mScroller->IsFinished(&res), !res) {
        if (mChallengeView == NULL) {
            // Can't scroll if the view is missing.
            Logger::E(TAG, "Challenge view missing in computeScroll");
            return mScroller->AbortAnimation();
        }

        mScroller->ComputeScrollOffset(&res);
        Int32 y;
        mScroller->GetCurrY(&y);
        MoveChallengeTo(y);

        if (mScroller->IsFinished(&res), res) {
            Post(mEndScrollRunnable, &res);
        }
    }
    return NOERROR;
}

void CSlidingChallengeLayout::CancelTransitionsInProgress()
{
    Boolean res;
    if (mScroller->IsFinished(&res), !res) {
        mScroller->AbortAnimation();
        CompleteChallengeScroll();
    }
    if (mFader != NULL) {
        IAnimator::Probe(mFader)->Cancel();
    }
}

ECode CSlidingChallengeLayout::FadeInChallenge()
{
    return FadeChallenge(TRUE);
}

ECode CSlidingChallengeLayout::FadeOutChallenge()
{
    return FadeChallenge(TRUE);
}

ECode CSlidingChallengeLayout::FadeChallenge(
    /* [in] */ Boolean show)
{
    if (mChallengeView != NULL) {

        CancelTransitionsInProgress();
        Float alpha = show ? 1.0f : 0.0f;
        Int32 duration = show ? CHALLENGE_FADE_IN_DURATION : CHALLENGE_FADE_OUT_DURATION;
        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = alpha;
        helper->OfFloat(mChallengeView, String("alpha"), array, (IObjectAnimator**)&mFader);

        AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter3(this, show);
        IAnimator::Probe(mFader)->AddListener(lis);
        IAnimator::Probe(mFader)->SetDuration(duration);
        IAnimator::Probe(mFader)->Start();
    }
    return NOERROR;
}

Int32 CSlidingChallengeLayout::GetMaxChallengeBottom()
{
    if (mChallengeView == NULL) return 0;
    Int32 layoutBottom = GetLayoutBottom();
    Int32 challengeHeight;
    IView::Probe(mChallengeView)->GetMeasuredHeight(&challengeHeight);

    return (layoutBottom + challengeHeight - mChallengeBottomBound);
}

Int32 CSlidingChallengeLayout::GetMinChallengeBottom()
{
    return GetLayoutBottom();
}

void CSlidingChallengeLayout::OnFadeStart(
    /* [in] */ Boolean show)
{
    mChallengeInteractiveInternal = FALSE;
    EnableHardwareLayerForChallengeView();

    if (show) {
        Int32 bottom = GetMinChallengeBottom();
        MoveChallengeTo(bottom);
    }

    SetScrollState(SCROLL_STATE_FADING);
}

void CSlidingChallengeLayout::EnableHardwareLayerForChallengeView()
{
    Boolean res;
    if (IView::Probe(mChallengeView)->IsHardwareAccelerated(&res), res) {
        IView::Probe(mChallengeView)->SetLayerType(LAYER_TYPE_HARDWARE, NULL);
    }
}

void CSlidingChallengeLayout::OnFadeEnd(
    /* [in] */ Boolean show)
{
    mChallengeInteractiveInternal = TRUE;
    SetChallengeShowing(show);

    if (!show) {
        Int32 bottom = GetMaxChallengeBottom();
        MoveChallengeTo(bottom);
    }

    IView::Probe(mChallengeView)->SetLayerType(LAYER_TYPE_NONE, NULL);
    mFader = NULL;
    SetScrollState(SCROLL_STATE_IDLE);
}

ECode CSlidingChallengeLayout::GetMaxChallengeTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top)

    if (mChallengeView == NULL) return 0;

    Int32 layoutBottom = GetLayoutBottom();
    Int32 challengeHeight;
    IView::Probe(mChallengeView)->GetMeasuredHeight(&challengeHeight);
    Int32 _top;
    mInsets->GetTop(&_top);
    *top = layoutBottom - challengeHeight - _top;
    return NOERROR;
}

Boolean CSlidingChallengeLayout::MoveChallengeTo(
    /* [in] */ Int32 bottom)
{
    if (mChallengeView == NULL || !mHasLayout) {
        return FALSE;
    }

    Int32 layoutBottom = GetLayoutBottom();
    Int32 challengeHeight;
    IView::Probe(mChallengeView)->GetHeight(&challengeHeight);

    Int32 minbottom = GetMinChallengeBottom();
    Int32 maxbottom = GetMaxChallengeBottom();
    bottom = Elastos::Core::Math::Max(minbottom,
            Elastos::Core::Math::Min(bottom, maxbottom));

    Float offset = 1.f - (Float) (bottom - layoutBottom) /
            (challengeHeight - mChallengeBottomBound);
    mChallengeOffset = offset;
    if (offset > 0 && !mChallengeShowing) {
        SetChallengeShowing(TRUE);
    }

    Int32 left;
    IView::Probe(mChallengeView)->GetLeft(&left);
    Int32 height;
    IView::Probe(mChallengeView)->GetHeight(&height);
    Int32 right;
    IView::Probe(mChallengeView)->GetRight(&right);
    IView::Probe(mChallengeView)->Layout(left,
            bottom - height, right, bottom);

    IView::Probe(mChallengeView)->SetAlpha(GetChallengeAlpha());
    if (mScrollListener != NULL) {
        Int32 top;
        IView::Probe(mChallengeView)->GetTop(&top);
        mScrollListener->OnScrollPositionChanged(offset, top);
    }
    PostInvalidateOnAnimation();
    return TRUE;
}

Int32 CSlidingChallengeLayout::GetLayoutBottom()
{
    Int32 bottomMargin = 0;
    if (mChallengeView != NULL) {
        AutoPtr<IViewGroupLayoutParams> p;
        IView::Probe(mChallengeView)->GetLayoutParams((IViewGroupLayoutParams**)&p);
        AutoPtr<SlidingChallengeLayoutLayoutParams> lp =
            (SlidingChallengeLayoutLayoutParams*)ISlidingChallengeLayoutLayoutParams::Probe(p);

        bottomMargin = lp->mBottomMargin;
    }

    Int32 height;
    GetMeasuredHeight(&height);
    Int32 bottom;
    GetPaddingBottom(&bottom);
    Int32 _bottom;
    mInsets->GetBottom(&_bottom);
    Int32 layoutBottom = height - bottom - bottomMargin - _bottom;
    return layoutBottom;
}

Int32 CSlidingChallengeLayout::GetChallengeBottom()
{
    if (mChallengeView == NULL) return 0;

    Int32 bottom;
    IView::Probe(mChallengeView)->GetBottom(&bottom);
    return bottom;
}

ECode CSlidingChallengeLayout::ShowChallenge(
    /* [in] */ Boolean show)
{
    ShowChallenge(show, 0);
    if (!show) {
        // Block any drags in progress so that callers can use this to disable dragging
        // for other touch interactions.
        mBlockDrag = TRUE;
    }
    return NOERROR;
}

void CSlidingChallengeLayout::ShowChallenge(
    /* [in] */ Int32 velocity)
{
    Boolean show = FALSE;
    if (Elastos::Core::Math::Abs(velocity) > mMinVelocity) {
        show = velocity < 0;
    }
    else {
        show = mChallengeOffset >= 0.5f;
    }
    ShowChallenge(show, velocity);
}

void CSlidingChallengeLayout::ShowChallenge(
    /* [in] */ Boolean show,
    /* [in] */ Int32 velocity)
{
    if (mChallengeView == NULL) {
        SetChallengeShowing(false);
        return;
    }

    if (mHasLayout) {
        mChallengeShowingTargetState = show;
        Int32 layoutBottom = GetLayoutBottom();

        Int32 height;
        IView::Probe(mChallengeView)->GetHeight(&height);
        AnimateChallengeTo(show ? layoutBottom :
                layoutBottom + height - mChallengeBottomBound, velocity);
    }
}

ECode CSlidingChallengeLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** p)
{
    VALIDATE_NOT_NULL(p);

    AutoPtr<IContext> context;
    AutoPtr<SlidingChallengeLayoutLayoutParams> params =
            new SlidingChallengeLayoutLayoutParams();
    params->constructor(context, attrs);
    *p = IViewGroupLayoutParams::Probe(params);
    REFCOUNT_ADD(*p)
    return NOERROR;
}

AutoPtr<IViewGroupLayoutParams> CSlidingChallengeLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    if(ISlidingChallengeLayoutLayoutParams::Probe(p) != NULL) {
        AutoPtr<SlidingChallengeLayoutLayoutParams> _params =
                new SlidingChallengeLayoutLayoutParams();
        _params->constructor(ISlidingChallengeLayoutLayoutParams::Probe(p));
        return IViewGroupLayoutParams::Probe(_params);
    }
    else {
        if(IViewGroupMarginLayoutParams::Probe(p) != NULL) {
            AutoPtr<SlidingChallengeLayoutLayoutParams> _params =
                    new SlidingChallengeLayoutLayoutParams();
            _params->constructor(IViewGroupMarginLayoutParams::Probe(p));
            return IViewGroupLayoutParams::Probe(_params);
        }
        else {
            AutoPtr<SlidingChallengeLayoutLayoutParams> _params =
                    new SlidingChallengeLayoutLayoutParams();
            _params->constructor(p);
            return IViewGroupLayoutParams::Probe(_params);
        }
    }
    return NULL;
}

ECode CSlidingChallengeLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** p)
{
    VALIDATE_NOT_NULL(p)

    AutoPtr<SlidingChallengeLayoutLayoutParams> _params =
            new SlidingChallengeLayoutLayoutParams();
    _params->constructor();
    *p = IViewGroupLayoutParams::Probe(_params);
    REFCOUNT_ADD(*p);
    return NOERROR;
}

Boolean CSlidingChallengeLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return ISlidingChallengeLayoutLayoutParams::Probe(p) != NULL;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
