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

#include "elastos/droid/systemui/statusbar/phone/PanelView.h"
#include "elastos/droid/systemui/statusbar/phone/VelocityTrackerFactory.h"
#include "elastos/droid/systemui/statusbar/phone/CBounceInterpolator.h"
#include "elastos/droid/systemui/doze/DozeLog.h"
#include "../R.h"
#include <elastos/droid/R.h>
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::SystemUI::Doze::DozeLog;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Core::IFloat;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

static const String TAG("PanelView");

//===============================================================================
// PanelView::PeekRunnable
//===============================================================================
PanelView::PeekRunnable::PeekRunnable(
    /* [in] */ PanelView* host)
    : mHost(host)
{}

ECode PanelView::PeekRunnable::Run()
{
    mHost->mPeekPending = FALSE;
    mHost->RunPeekAnimation();
    return NOERROR;
}

//===============================================================================
// PanelView::FlingCollapseRunnable
//===============================================================================
PanelView::FlingCollapseRunnable::FlingCollapseRunnable(
    /* [in] */ PanelView* host)
    : mHost(host)
{}

ECode PanelView::FlingCollapseRunnable::Run()
{
    mHost->Fling(0, FALSE /* expand */);
    return NOERROR;
}

//===============================================================================
// PanelView::PostCollapseRunnable
//===============================================================================
PanelView::PostCollapseRunnable::PostCollapseRunnable(
    /* [in] */ PanelView* host)
    : mHost(host)
{}

ECode PanelView::PostCollapseRunnable::Run()
{
    mHost->Collapse(FALSE /* delayed */);
    return NOERROR;
}

//===============================================================================
// PanelView::PeekAnimatorListenerAdapter::ListenerRunnable
//===============================================================================
PanelView::PeekAnimatorListenerAdapter::ListenerRunnable::ListenerRunnable(
    /* [in] */ PanelView* host)
    : mHost(host)
{}

ECode PanelView::PeekAnimatorListenerAdapter::ListenerRunnable::Run()
{
    mHost->Collapse(FALSE /* delayed */);
    return NOERROR;
}

//===============================================================================
// PanelView::PeekAnimatorListenerAdapter
//===============================================================================
PanelView::PeekAnimatorListenerAdapter::PeekAnimatorListenerAdapter(
    /* [in] */ PanelView* host)
    : mHost(host)
    , mCancelled(FALSE)
{}

ECode PanelView::PeekAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCancelled = TRUE;
    return NOERROR;
}

ECode PanelView::PeekAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mPeekAnimator = NULL;
    if (mHost->mCollapseAfterPeek && !mCancelled) {
        AutoPtr<ListenerRunnable> listener = new ListenerRunnable(mHost);
        mHost->PostOnAnimation(listener);
    }
    mHost->mCollapseAfterPeek = FALSE;
    return NOERROR;
}

//===============================================================================
// PanelView::FlingAnimatorListenerAdapter
//===============================================================================
PanelView::FlingAnimatorListenerAdapter::FlingAnimatorListenerAdapter(
    /* [in] */ PanelView* host,
    /* [in] */ Boolean clearAllExpandHack)
    : mHost(host)
    , mCancelled(FALSE)
    , mClearAllExpandHack(clearAllExpandHack)
{}

ECode PanelView::FlingAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCancelled = TRUE;
    return NOERROR;
}

ECode PanelView::FlingAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mClearAllExpandHack && !mCancelled) {
        mHost->SetExpandedHeightInternal(mHost->GetMaxPanelHeight());
    }
    mHost->mHeightAnimator = NULL;
    if (!mCancelled) {
        mHost->NotifyExpandingFinished();
    }
    return NOERROR;
}

//===============================================================================
// PanelView::OnGlobalLayoutListener
//===============================================================================
CAR_INTERFACE_IMPL(PanelView::OnGlobalLayoutListener, Object, IOnGlobalLayoutListener)

PanelView::OnGlobalLayoutListener::OnGlobalLayoutListener(
    /* [in] */ PanelView* host)
    : mHost(host)
{}

ECode PanelView::OnGlobalLayoutListener::OnGlobalLayout()
{
    AutoPtr<IStatusBarWindowView> sbwv;
    mHost->mStatusBar->GetStatusBarWindow((IStatusBarWindowView**)&sbwv);
    Int32 sh = 0, bh = 0;
    IView::Probe(sbwv)->GetHeight(&sh);
    mHost->mStatusBar->GetStatusBarHeight(&bh);
    if (sh != bh) {
        AutoPtr<IViewTreeObserver> vto;
        mHost->GetViewTreeObserver((IViewTreeObserver**)&vto);
        vto->RemoveOnGlobalLayoutListener(this);
        mHost->SetExpandedFraction(1.f);
        mHost->mInstantExpanding = FALSE;
    }
    return NOERROR;
}

//===============================================================================
// PanelView::UnlockHintAnimationEndRunnable
//===============================================================================
PanelView::UnlockHintAnimationEndRunnable::UnlockHintAnimationEndRunnable(
    /* [in] */ PanelView* host)
    : mHost(host)
{}

ECode PanelView::UnlockHintAnimationEndRunnable::Run()
{
    mHost->NotifyExpandingFinished();
    mHost->mStatusBar->OnHintFinished();
    mHost->mHintAnimationRunning = FALSE;
    return NOERROR;
}

//===============================================================================
// PanelView::HeightAnimatorPhase1ListenerAdapter
//===============================================================================
PanelView::HeightAnimatorPhase1ListenerAdapter::HeightAnimatorPhase1ListenerAdapter(
    /* [in] */ PanelView* host,
    /* [in] */ IRunnable* onAnimationFinished)
    : mHost(host)
    , mCancelled(FALSE)
    , mOnAnimationFinished(onAnimationFinished)
{}

ECode PanelView::HeightAnimatorPhase1ListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCancelled = TRUE;
    return NOERROR;
}

ECode PanelView::HeightAnimatorPhase1ListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mCancelled) {
        mHost->mHeightAnimator = NULL;
        mOnAnimationFinished->Run();
    }
    else {
        mHost->StartUnlockHintAnimationPhase2(mOnAnimationFinished);
    }
    return NOERROR;
}

//===============================================================================
// PanelView::IndicationViewAnimationFinishedRunnable
//===============================================================================
PanelView::IndicationViewAnimationFinishedRunnable::IndicationViewAnimationFinishedRunnable(
    /* [in] */ PanelView* host)
    : mHost(host)
{}

ECode PanelView::IndicationViewAnimationFinishedRunnable::Run()
{
    AutoPtr<IView> view;
    mHost->mKeyguardBottomArea->GetIndicationView((IView**)&view);

    AutoPtr<IViewPropertyAnimator> vpa;
    view->Animate((IViewPropertyAnimator**)&vpa);
    vpa->Y(mHost->mOriginalIndicationY);
    vpa->SetDuration(450);
    vpa->SetInterpolator(mHost->mBounceInterpolator);
    vpa->Start();
    return NOERROR;
}

//===============================================================================
// PanelView::HeightAnimatorPhase2ListenerAdapter
//===============================================================================
PanelView::HeightAnimatorPhase2ListenerAdapter::HeightAnimatorPhase2ListenerAdapter(
    /* [in] */ PanelView* host,
    /* [in] */ IRunnable* onAnimationFinished)
    : mHost(host)
    , mOnAnimationFinished(onAnimationFinished)
{}

ECode PanelView::HeightAnimatorPhase2ListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mHeightAnimator = NULL;
    mOnAnimationFinished->Run();
    return NOERROR;
}

//===============================================================================
// PanelView::HeightAnimatorUpdateListener
//===============================================================================
CAR_INTERFACE_IMPL(PanelView::HeightAnimatorUpdateListener, Object, IAnimatorUpdateListener)

PanelView::HeightAnimatorUpdateListener::HeightAnimatorUpdateListener(
    /* [in] */ PanelView* host)
    : mHost(host)
{}

ECode PanelView::HeightAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    Float fv = 0;
    IFloat::Probe(obj)->GetValue(&fv);
    mHost->SetExpandedHeightInternal(fv);
    return NOERROR;
}

//===============================================================================
// PanelView
//===============================================================================
const Boolean PanelView::DEBUG = FALSE;

CAR_INTERFACE_IMPL(PanelView, FrameLayout, IPanelView)

void PanelView::OnExpandingFinished()
{
    mClosing = FALSE;
    mBar->OnExpandingFinished();
}

void PanelView::OnExpandingStarted()
{
}

void PanelView::NotifyExpandingStarted()
{
    if (!mExpanding) {
        mExpanding = TRUE;
        OnExpandingStarted();
    }
}

void PanelView::NotifyExpandingFinished()
{
    if (mExpanding) {
        mExpanding = FALSE;
        OnExpandingFinished();
    }
}

void PanelView::SchedulePeek()
{
    mPeekPending = TRUE;
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    Int32 timeout = 0;
    helper->GetTapTimeout(&timeout);
    PostOnAnimationDelayed(mPeekRunnable, timeout);
    NotifyBarPanelExpansionChanged();
}

void PanelView::RunPeekAnimation()
{
    mPeekHeight = GetPeekHeight();
    if (DEBUG) Logger::V(TAG, "%s peek to height=%.1f", (mViewName != NULL ? (mViewName + String(": ")).string() : "") , mPeekHeight);
    if (mHeightAnimator != NULL) {
        return;
    }

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);

    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(1);
    (*fvs)[0] = mPeekHeight;
    helper->OfFloat(TO_IINTERFACE(this), String("expandedHeight"), fvs, (IObjectAnimator**)&mPeekAnimator);
    IAnimator* a = IAnimator::Probe(mPeekAnimator);
    a->SetDuration(250);
    a->SetInterpolator(mLinearOutSlowInInterpolator);
    AutoPtr<PeekAnimatorListenerAdapter> listener = new PeekAnimatorListenerAdapter(this);
    a->AddListener(listener);
    NotifyExpandingStarted();
    a->Start();
    mJustPeeked = TRUE;
}

PanelView::PanelView()
    : mExpandedHeight(0)
    , mTracking(FALSE)
    , mTouchSlop(0)
    , mHintAnimationRunning(FALSE)
    , mPeekHeight(0)
    , mHintDistance(0)
    , mEdgeTapAreaWidth(0)
    , mInitialOffsetOnTouch(0)
    , mExpandedFraction(0)
    , mPanelClosedOnDown(FALSE)
    , mHasLayoutedSinceDown(FALSE)
    , mUpdateFlingVelocity(0)
    , mUpdateFlingOnLayout(FALSE)
    , mPeekTouching(FALSE)
    , mJustPeeked(FALSE)
    , mClosing(FALSE)
    , mTouchSlopExceeded(FALSE)
    , mTrackingPointer(0)
    , mOverExpandedBeforeFling(FALSE)
    , mOriginalIndicationY(0)
    , mTouchAboveFalsingThreshold(FALSE)
    , mUnlockFalsingThreshold(0)
    , mInstantExpanding(FALSE)
    , mInitialTouchY(0)
    , mInitialTouchX(0)
    , mTouchDisabled(FALSE)
    , mPeekPending(FALSE)
    , mCollapseAfterPeek(FALSE)
    , mExpanding(FALSE)
    , mGestureWaitForTouchSlop(FALSE)
{
}

ECode PanelView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);

    mFlingCollapseRunnable = new FlingCollapseRunnable(this);
    mPeekRunnable = new PeekRunnable(this);
    mPostCollapseRunnable = new PostCollapseRunnable(this);

    mFlingAnimationUtils = new FlingAnimationUtils(context, 0.6f);
    AutoPtr<IInterpolator> ip;
    AnimationUtils::LoadInterpolator(context, Elastos::Droid::R::interpolator::fast_out_slow_in, (IInterpolator**)&ip);
    mFastOutSlowInInterpolator = ITimeInterpolator::Probe(ip);
    ip = NULL;
    AnimationUtils::LoadInterpolator(context, Elastos::Droid::R::interpolator::linear_out_slow_in, (IInterpolator**)&ip);
    mLinearOutSlowInInterpolator = ITimeInterpolator::Probe(ip);
    CBounceInterpolator::New((ITimeInterpolator**)&mBounceInterpolator);
    return NOERROR;
}

void PanelView::LoadDimens()
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration;
    helper->Get(ctx, (IViewConfiguration**)&configuration);
    configuration->GetScaledTouchSlop(&mTouchSlop);
    res->GetDimension(R::dimen::hint_move_distance, &mHintDistance);
    res->GetDimensionPixelSize(R::dimen::edge_tap_area_width, &mEdgeTapAreaWidth);
    res->GetDimensionPixelSize(R::dimen::unlock_falsing_threshold, &mUnlockFalsingThreshold);
}

void PanelView::TrackMovement(
    /* [in] */ IMotionEvent* event)
{
    // Add movement to velocity tracker using raw screen X and Y coordinates instead
    // of window coordinates because the window frame may be moving at the same time.
    Float fv1 = 0, fv2 = 0;
    Float deltaX = (event->GetRawX(&fv1), fv1) - (event->GetX(&fv2), fv2);
    Float deltaY = (event->GetRawY(&fv1), fv1) - (event->GetY(&fv2), fv2);
    event->OffsetLocation(deltaX, deltaY);
    if (mVelocityTracker != NULL) mVelocityTracker->AddMovement(event);
    event->OffsetLocation(-deltaX, -deltaY);
}

ECode PanelView::SetTouchDisabled(
    /* [in] */ Boolean disabled)
{
    mTouchDisabled = disabled;
    return NOERROR;
}

ECode PanelView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mInstantExpanding || mTouchDisabled) {
        *result = FALSE;
        return NOERROR;
    }

    /*
     * We capture touch events here and update the expand height here in case according to
     * the users fingers. This also handles multi-touch.
     *
     * If the user just clicks shortly, we give him a quick peek of the shade.
     *
     * Flinging is also enabled in order to open or close the shade.
     */

    Int32 pointerIndex = 0;
    event->FindPointerIndex(mTrackingPointer, &pointerIndex);
    if (pointerIndex < 0) {
        pointerIndex = 0;
        event->GetPointerId(pointerIndex, &mTrackingPointer);
    }
    Float fv1 = 0;
    Float x, y;
    event->GetY(pointerIndex, &y);
    event->GetX(pointerIndex, &x);

    Int32 value = 0, masked = 0;
    if ((event->GetActionMasked(&masked), masked) == IMotionEvent::ACTION_DOWN) {
        mGestureWaitForTouchSlop = mExpandedHeight == 0.f;
    }
    Boolean waitForTouchSlop = HasConflictingGestures() || mGestureWaitForTouchSlop;

    switch (masked) {
        case IMotionEvent::ACTION_DOWN:
            mInitialTouchY = y;
            mInitialTouchX = x;
            mInitialOffsetOnTouch = mExpandedHeight;
            mTouchSlopExceeded = FALSE;
            mJustPeeked = FALSE;
            mPanelClosedOnDown = mExpandedHeight == 0.0f;
            mHasLayoutedSinceDown = FALSE;
            mUpdateFlingOnLayout = FALSE;
            mPeekTouching = mPanelClosedOnDown;
            mTouchAboveFalsingThreshold = FALSE;
            if (mVelocityTracker == NULL) {
                InitVelocityTracker();
            }
            TrackMovement(event);
            if (!waitForTouchSlop || (mHeightAnimator != NULL && !mHintAnimationRunning) ||
                    mPeekPending || mPeekAnimator != NULL) {
                if (mHeightAnimator != NULL) {
                    IAnimator::Probe(mHeightAnimator)->Cancel(); // end any outstanding animations
                }
                CancelPeek();
                mTouchSlopExceeded = (mHeightAnimator != NULL && !mHintAnimationRunning)
                        || mPeekPending || mPeekAnimator != NULL;
                OnTrackingStarted();
            }
            if (mExpandedHeight == 0) {
                SchedulePeek();
            }
            break;

        case IMotionEvent::ACTION_POINTER_UP: {
            event->GetActionIndex(&value);
            Int32 upPointer = 0;
            event->GetPointerId(value, &upPointer);
            if (mTrackingPointer == upPointer) {
                // gesture is ongoing, find a new pointer to track
                const Int32 newIndex = (event->GetPointerId(0, &value), value) != upPointer ? 0 : 1;
                Float newX, newY;
                event->GetY(newIndex, &newY);
                event->GetX(newIndex, &newX);
                event->GetPointerId(newIndex, &mTrackingPointer);
                mInitialOffsetOnTouch = mExpandedHeight;
                mInitialTouchY = newY;
                mInitialTouchX = newX;
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            Float h = y - mInitialTouchY;

            // If the panel was collapsed when touching, we only need to check for the
            // y-component of the gesture, as we have no conflicting horizontal gesture.
            if (Elastos::Core::Math::Abs(h) > mTouchSlop
                    && (Elastos::Core::Math::Abs(h) > Elastos::Core::Math::Abs(x - mInitialTouchX)
                            || mInitialOffsetOnTouch == 0.f)) {
                mTouchSlopExceeded = TRUE;
                if (waitForTouchSlop && !mTracking) {
                    if (!mJustPeeked) {
                        mInitialOffsetOnTouch = mExpandedHeight;
                        mInitialTouchX = x;
                        mInitialTouchY = y;
                        h = 0;
                    }
                    if (mHeightAnimator != NULL) {
                        IAnimator::Probe(mHeightAnimator)->Cancel(); // end any outstanding animations
                    }
                    Boolean tmp = FALSE;
                    RemoveCallbacks(mPeekRunnable, &tmp);
                    mPeekPending = FALSE;
                    OnTrackingStarted();
                }
            }
            const Float newHeight = Elastos::Core::Math::Max((Float)0, h + mInitialOffsetOnTouch);
            if (newHeight > mPeekHeight) {
                if (mPeekAnimator != NULL) {
                    IAnimator::Probe(mPeekAnimator)->Cancel();
                }
                mJustPeeked = FALSE;
            }
            if (-h >= GetFalsingThreshold()) {
                mTouchAboveFalsingThreshold = TRUE;
            }
            if (!mJustPeeked && (!waitForTouchSlop || mTracking) && !IsTrackingBlocked()) {
                SetExpandedHeightInternal(newHeight);
            }

            TrackMovement(event);
            break;
        }

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL: {
            mTrackingPointer = -1;
            TrackMovement(event);
            if ((mTracking && mTouchSlopExceeded)
                    || Elastos::Core::Math::Abs(x - mInitialTouchX) > mTouchSlop
                    || Elastos::Core::Math::Abs(y - mInitialTouchY) > mTouchSlop
                    || masked == IMotionEvent::ACTION_CANCEL) {
                Float vel = 0.f;
                Float vectorVel = 0.f;
                if (mVelocityTracker != NULL) {
                    mVelocityTracker->ComputeCurrentVelocity(1000);
                    mVelocityTracker->GetYVelocity(&vel);
                    mVelocityTracker->GetXVelocity(&fv1);
                    vectorVel = (Float) Elastos::Core::Math::Hypot(fv1, vel);
                }
                Boolean expand = FlingExpands(vel, vectorVel);
                OnTrackingStopped(expand);
                Boolean t1 = FALSE, t2 = FALSE;
                DozeLog::TraceFling(expand, mTouchAboveFalsingThreshold,
                        (mStatusBar->IsFalsingThresholdNeeded(&t1), t1),
                        (mStatusBar->IsScreenOnComingFromTouch(&t2), t2));
                Fling(vel, expand);
                mUpdateFlingOnLayout = expand && mPanelClosedOnDown && !mHasLayoutedSinceDown;
                if (mUpdateFlingOnLayout) {
                    mUpdateFlingVelocity = vel;
                }
            }
            else {
                Boolean expands = OnEmptySpaceClick(mInitialTouchX);
                OnTrackingStopped(expands);
            }

            if (mVelocityTracker != NULL) {
                mVelocityTracker->Recycle();
                mVelocityTracker = NULL;
            }
            mPeekTouching = FALSE;
            break;
        }
    }
    *result = !waitForTouchSlop || mTracking;
    return NOERROR;
}

Int32 PanelView::GetFalsingThreshold()
{
    Boolean tmp = FALSE;
    mStatusBar->IsScreenOnComingFromTouch(&tmp);
    Float factor = tmp ? 1.5f : 1.0f;
    return (Int32) (mUnlockFalsingThreshold * factor);
}

void PanelView::OnTrackingStopped(
    /* [in] */ Boolean expand)
{
    mTracking = FALSE;
    mBar->OnTrackingStopped(this, expand);
}

void PanelView::OnTrackingStarted()
{
    mClosing = FALSE;
    mTracking = TRUE;
    mCollapseAfterPeek = FALSE;
    mBar->OnTrackingStarted(this);
    NotifyExpandingStarted();
}

ECode PanelView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mInstantExpanding) {
        *result = FALSE;
        return NOERROR;
    }

    /*
     * If the user drags anywhere inside the panel we intercept it if he moves his finger
     * upwards. This allows closing the shade from anywhere inside the panel.
     *
     * We only do this if the current content is scrolled to the bottom,
     * i.e IsScrolledToBottom() is TRUE and therefore there is no conflicting scrolling gesture
     * possible.
     */
    Int32 pointerIndex = 0;
    event->FindPointerIndex(mTrackingPointer, &pointerIndex);
    if (pointerIndex < 0) {
        pointerIndex = 0;
        event->GetPointerId(pointerIndex, &mTrackingPointer);
    }
    Float x, y;
    event->GetX(pointerIndex, &x);
    event->GetY(pointerIndex, &y);
    Boolean scrolledToBottom = IsScrolledToBottom();

    Int32 masked = 0;
    event->GetActionMasked(&masked);
    switch (masked) {
        case IMotionEvent::ACTION_DOWN:
            mStatusBar->UserActivity();
            if ((mHeightAnimator != NULL && !mHintAnimationRunning) ||
                    mPeekPending || mPeekAnimator != NULL) {
                if (mHeightAnimator != NULL) {
                    IAnimator::Probe(mHeightAnimator)->Cancel(); // end any outstanding animations
                }
                CancelPeek();
                mTouchSlopExceeded = TRUE;
                *result = TRUE;
                return NOERROR;
            }
            mInitialTouchY = y;
            mInitialTouchX = x;
            mTouchSlopExceeded = FALSE;
            mJustPeeked = FALSE;
            mPanelClosedOnDown = mExpandedHeight == 0.0f;
            mHasLayoutedSinceDown = FALSE;
            mUpdateFlingOnLayout = FALSE;
            mTouchAboveFalsingThreshold = FALSE;
            InitVelocityTracker();
            TrackMovement(event);
            break;
        case IMotionEvent::ACTION_POINTER_UP: {
            Int32 v = 0;
            event->GetActionIndex(&v);
            Int32 upPointer = 0;
            event->GetPointerId(v, &upPointer);
            if (mTrackingPointer == upPointer) {
                // gesture is ongoing, find a new pointer to track
                event->GetPointerId(0, &v);
                const Int32 newIndex = v != upPointer ? 0 : 1;
                event->GetPointerId(newIndex, &mTrackingPointer);
                event->GetX(newIndex, &mInitialTouchX);
                event->GetY(newIndex, &mInitialTouchY);
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            const Float h = y - mInitialTouchY;
            TrackMovement(event);
            if (scrolledToBottom) {
                if (h < -mTouchSlop && h < - Elastos::Core::Math::Abs(x - mInitialTouchX)) {
                    if (mHeightAnimator != NULL) {
                        IAnimator::Probe(mHeightAnimator)->Cancel();
                    }
                    mInitialOffsetOnTouch = mExpandedHeight;
                    mInitialTouchY = y;
                    mInitialTouchX = x;
                    mTracking = TRUE;
                    mTouchSlopExceeded = TRUE;
                    OnTrackingStarted();
                    *result = TRUE;
                    return NOERROR;
                }
            }
            break;
        }
        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
            break;
    }
    *result = FALSE;
    return NOERROR;
}

void PanelView::InitVelocityTracker()
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
    }
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mVelocityTracker = VelocityTrackerFactory::Obtain(ctx);
}

Boolean PanelView::IsScrolledToBottom()
{
    return TRUE;
}

Float PanelView::GetContentHeight()
{
    return mExpandedHeight;
}

ECode PanelView::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    LoadDimens();
    return NOERROR;
}

ECode PanelView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    FrameLayout::OnConfigurationChanged(newConfig);
    LoadDimens();
    return NOERROR;
}

Boolean PanelView::FlingExpands(
    /* [in] */ Float vel,
    /* [in] */ Float vectorVel)
{
    if (IsBelowFalsingThreshold()) {
        return TRUE;
    }
    if (Elastos::Core::Math::Abs(vectorVel) < mFlingAnimationUtils->GetMinVelocityPxPerSecond()) {
        Float fv = 0;
        GetExpandedFraction(&fv);
        return fv > 0.5f;
    }

    return vel > 0;
}

Boolean PanelView::IsBelowFalsingThreshold()
{
    Boolean tmp = FALSE;
    return !mTouchAboveFalsingThreshold && (mStatusBar->IsFalsingThresholdNeeded(&tmp), tmp);
}

void PanelView::Fling(
    /* [in] */ Float vel,
    /* [in] */ Boolean expand)
{
    CancelPeek();
    Float target = expand ? GetMaxPanelHeight() : 0.0f;

    // Hack to make the expand transition look nice when clear all button is visible - we make
    // the animation only to the last notification, and then jump to the maximum panel height so
    // clear all just fades in and the decelerating motion is towards the last notification.
    const Boolean clearAllExpandHack = expand && FullyExpandedClearAllVisible()
            && mExpandedHeight < GetMaxPanelHeight() - GetClearAllHeight()
            && !IsClearAllVisible();
    if (clearAllExpandHack) {
        target = GetMaxPanelHeight() - GetClearAllHeight();
    }
    if (target == mExpandedHeight || (GetOverExpansionAmount() > 0.f && expand)) {
        NotifyExpandingFinished();
        return;
    }
    mOverExpandedBeforeFling = GetOverExpansionAmount() > 0.f;
    AutoPtr<IValueAnimator> animator = CreateHeightAnimator(target);
    IAnimator* fa = IAnimator::Probe(animator);

    Int32 h = 0;
    GetHeight(&h);
    if (expand) {
        Boolean belowFalsingThreshold = IsBelowFalsingThreshold();
        if (belowFalsingThreshold) {
            vel = 0;
        }
        mFlingAnimationUtils->Apply(fa, mExpandedHeight, target, vel, h);
        if (belowFalsingThreshold) {
            fa->SetDuration(350);
        }
    }
    else {
        mFlingAnimationUtils->ApplyDismissing(fa, mExpandedHeight, target, vel, h);

        // Make it shorter if we run a canned animation
        if (vel == 0) {
            Int64 d = 0;
            animator->GetDuration(&d);
            fa->SetDuration((Int64)
                    (d * GetCannedFlingDurationFactor()));
        }
    }

    AutoPtr<FlingAnimatorListenerAdapter> listener = new FlingAnimatorListenerAdapter(this, clearAllExpandHack);
    fa->AddListener(listener);
    mHeightAnimator = animator;
    fa->Start();
}

ECode PanelView::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 id = 0;
    GetId(&id);
    res->GetResourceName(id, &mViewName);
    return NOERROR;
}

ECode PanelView::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mViewName;
    return NOERROR;
}

ECode PanelView::SetExpandedHeight(
    /* [in] */ Float height)
{
    if (DEBUG) Logger::V(TAG, "%s setExpandedHeight(%.1f)", (mViewName != NULL ? (mViewName + String(": ")).string() : "")
        , height);
    SetExpandedHeightInternal(height + GetOverExpansionPixels());
    return NOERROR;
}

ECode PanelView::OnLayout (
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    FrameLayout::OnLayout(changed, left, top, right, bottom);
    RequestPanelHeightUpdate();
    mHasLayoutedSinceDown = TRUE;
    if (mUpdateFlingOnLayout) {
        AbortAnimations();
        Fling(mUpdateFlingVelocity, TRUE);
        mUpdateFlingOnLayout = FALSE;
    }
    return NOERROR;
}

void PanelView::RequestPanelHeightUpdate()
{
    Float currentMaxPanelHeight = GetMaxPanelHeight();

    // If the user isn't actively poking us, let's update the height
    if ((!mTracking || IsTrackingBlocked())
            && mHeightAnimator == NULL
            && mExpandedHeight > 0
            && currentMaxPanelHeight != mExpandedHeight
            && !mPeekPending
            && mPeekAnimator == NULL
            && !mPeekTouching) {
        SetExpandedHeight(currentMaxPanelHeight);
    }
}

ECode PanelView::SetExpandedHeightInternal(
    /* [in] */ Float h)
{
    Float fhWithoutOverExpansion = GetMaxPanelHeight() - GetOverExpansionAmount();
    if (mHeightAnimator == NULL) {
        Float overExpansionPixels = Elastos::Core::Math::Max((Float)0, h - fhWithoutOverExpansion);
        if (GetOverExpansionPixels() != overExpansionPixels && mTracking) {
            SetOverExpansion(overExpansionPixels, TRUE /* isPixels */);
        }
        mExpandedHeight = Elastos::Core::Math::Min(h, fhWithoutOverExpansion) + GetOverExpansionAmount();
    }
    else {
        mExpandedHeight = h;
        if (mOverExpandedBeforeFling) {
            SetOverExpansion(Elastos::Core::Math::Max((Float)0, h - fhWithoutOverExpansion), FALSE /* isPixels */);
        }
    }

    mExpandedHeight = Elastos::Core::Math::Max((Float)0, mExpandedHeight);
    OnHeightUpdated(mExpandedHeight);
    mExpandedFraction = Elastos::Core::Math::Min(1.f, fhWithoutOverExpansion == 0
            ? 0 : mExpandedHeight / fhWithoutOverExpansion);
    NotifyBarPanelExpansionChanged();
    return NOERROR;
}

ECode PanelView::SetExpandedFraction(
    /* [in] */ Float frac)
{
    SetExpandedHeight(GetMaxPanelHeight() * frac);
    return NOERROR;
}

ECode PanelView::GetExpandedHeight(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mExpandedHeight;
    return NOERROR;
}

ECode PanelView::GetExpandedFraction(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mExpandedFraction;
    return NOERROR;
}

ECode PanelView::IsFullyExpanded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mExpandedHeight >= GetMaxPanelHeight();
    return NOERROR;
}

ECode PanelView::IsFullyCollapsed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mExpandedHeight <= 0;
    return NOERROR;
}

ECode PanelView::IsCollapsing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mClosing;
    return NOERROR;
}

ECode PanelView::IsTracking(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTracking;
    return NOERROR;
}

ECode PanelView::SetBar(
    /* [in] */ IPanelBar* panelBar)
{
    mBar = panelBar;
    return NOERROR;
}

ECode PanelView::Collapse(
    /* [in] */ Boolean delayed)
{
    if (DEBUG) Logger::V(TAG, "%s collapse: %s", (mViewName != NULL ? (mViewName + String(": ")).string() : "")
            , TO_CSTR(this));

    Boolean tmp = FALSE;
    if (mPeekPending || mPeekAnimator != NULL) {
        mCollapseAfterPeek = TRUE;
        if (mPeekPending) {

            // We know that the whole gesture is just a peek triggered by a simple click, so
            // better start it now.
            Boolean tmp = FALSE;
            RemoveCallbacks(mPeekRunnable, &tmp);
            mPeekRunnable->Run();
        }
    }
    else if ((IsFullyCollapsed(&tmp), !tmp) && !mTracking && !mClosing) {
        if (mHeightAnimator != NULL) {
            IAnimator::Probe(mHeightAnimator)->Cancel();
        }
        mClosing = TRUE;
        NotifyExpandingStarted();
        if (delayed) {
            PostDelayed(mFlingCollapseRunnable, 120, &tmp);
        }
        else {
            Fling(0, FALSE /* expand */);
        }
    }
    return NOERROR;
}

ECode PanelView::Expand()
{
    if (DEBUG) Logger::V(TAG, "%s expand: %s", (mViewName != NULL ? (mViewName + String(": ")).string() : ""), TO_CSTR(this));
    Boolean tmp = FALSE;
    if (IsFullyCollapsed(&tmp), tmp) {
        mBar->StartOpeningPanel(this);
        NotifyExpandingStarted();
        Fling(0, TRUE /* expand */);
    }
    else if (DEBUG) {
        if (DEBUG) Logger::V(TAG, "skipping expansion: is expanded");
    }
    return NOERROR;
}

ECode PanelView::CancelPeek()
{
    if (mPeekAnimator != NULL) {
        IAnimator::Probe(mPeekAnimator)->Cancel();
    }
    Boolean tmp = FALSE;
    RemoveCallbacks(mPeekRunnable, &tmp);
    mPeekPending = FALSE;

    // When peeking, we already tell mBar that we expanded ourselves. Make sure that we also
    // notify mBar that we might have closed ourselves.
    NotifyBarPanelExpansionChanged();
    return NOERROR;
}

ECode PanelView::InstantExpand()
{
    mInstantExpanding = TRUE;
    mUpdateFlingOnLayout = FALSE;
    AbortAnimations();
    CancelPeek();
    if (mTracking) {
        OnTrackingStopped(TRUE /* expands */); // The panel is expanded after this call.
    }
    if (mExpanding) {
        NotifyExpandingFinished();
    }
    SetVisibility(IView::VISIBLE);

    // Wait for window manager to pickup the change, so we know the maximum height of the panel
    // then.
    AutoPtr<IViewTreeObserver> vto;
    GetViewTreeObserver((IViewTreeObserver**)&vto);
    AutoPtr<OnGlobalLayoutListener> listener = new OnGlobalLayoutListener(this);
    vto->AddOnGlobalLayoutListener(listener);

    // Make sure a layout really happens.
    RequestLayout();
    return NOERROR;
}

ECode PanelView::InstantCollapse()
{
    AbortAnimations();
    SetExpandedFraction(0.f);
    if (mExpanding) {
        NotifyExpandingFinished();
    }
    return NOERROR;
}

void PanelView::AbortAnimations()
{
    CancelPeek();
    if (mHeightAnimator != NULL) {
        IAnimator::Probe(mHeightAnimator)->Cancel();
    }
    Boolean tmp = FALSE;
    RemoveCallbacks(mPostCollapseRunnable, &tmp);
    RemoveCallbacks(mFlingCollapseRunnable, &tmp);
}

void PanelView::StartUnlockHintAnimation()
{
    // We don't need to hint the user if an animation is already running or the user is changing
    // the expansion.
    if (mHeightAnimator != NULL || mTracking) {
        return;
    }
    CancelPeek();
    NotifyExpandingStarted();
    AutoPtr<UnlockHintAnimationEndRunnable> run = new UnlockHintAnimationEndRunnable(this);
    StartUnlockHintAnimationPhase1(run);
    mStatusBar->OnUnlockHintStarted();
    mHintAnimationRunning = TRUE;
}

void PanelView::StartUnlockHintAnimationPhase1(
    /* [in] */ IRunnable* onAnimationFinished)
{
    Float target = Elastos::Core::Math::Max((Float)0, GetMaxPanelHeight() - mHintDistance);
    AutoPtr<IValueAnimator> animator = CreateHeightAnimator(target);
    IAnimator* ani = IAnimator::Probe(animator);
    ani->SetDuration(250);
    ani->SetInterpolator(mFastOutSlowInInterpolator);
    AutoPtr<HeightAnimatorPhase1ListenerAdapter> listener
        = new HeightAnimatorPhase1ListenerAdapter(this, onAnimationFinished);
    ani->AddListener(listener);
    ani->Start();
    mHeightAnimator = animator;

    AutoPtr<IView> view;
    mKeyguardBottomArea->GetIndicationView((IView**)&view);
    view->GetY(&mOriginalIndicationY);

    AutoPtr<IViewPropertyAnimator> vpa;
    view->Animate((IViewPropertyAnimator**)&vpa);
    vpa->Y(mOriginalIndicationY - mHintDistance);
    vpa->SetDuration(250);
    vpa->SetInterpolator(mFastOutSlowInInterpolator);
    AutoPtr<IndicationViewAnimationFinishedRunnable> run = new IndicationViewAnimationFinishedRunnable(this);
    vpa->WithEndAction(run);
    vpa->Start();
}

void PanelView::StartUnlockHintAnimationPhase2(
    /* [in] */ IRunnable* onAnimationFinished)
{
    AutoPtr<IValueAnimator> animator = CreateHeightAnimator(GetMaxPanelHeight());
    IAnimator* ani = IAnimator::Probe(animator);
    ani->SetDuration(450);
    ani->SetInterpolator(mBounceInterpolator);
    AutoPtr<HeightAnimatorPhase2ListenerAdapter> listener =
        new HeightAnimatorPhase2ListenerAdapter(this, onAnimationFinished);
    ani->AddListener(listener);
    ani->Start();
    mHeightAnimator = animator;
}

AutoPtr<IValueAnimator> PanelView::CreateHeightAnimator(
    /* [in] */ Float targetHeight)
{
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> animator;
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = mExpandedHeight;
    (*fvs)[1] = targetHeight;
    helper->OfFloat(fvs, (IValueAnimator**)&animator);
    AutoPtr<HeightAnimatorUpdateListener> listener = new HeightAnimatorUpdateListener(this);
    animator->AddUpdateListener(listener);
    return animator;
}

void PanelView::NotifyBarPanelExpansionChanged()
{
    mBar->PanelExpansionChanged(this, mExpandedFraction, mExpandedFraction > 0.f || mPeekPending
            || mPeekAnimator != NULL);
}

Boolean PanelView::OnEmptySpaceClick(
    /* [in] */ Float x)
{
    if (mHintAnimationRunning) {
        return TRUE;
    }

    Int32 state = 0, w = 0;
    if (x < mEdgeTapAreaWidth
            && (mStatusBar->GetBarState(&state), state) == IStatusBarState::KEYGUARD) {
        OnEdgeClicked(FALSE /* right */);
        return TRUE;
    }
    else if (x > (GetWidth(&w), w) - mEdgeTapAreaWidth
            && (mStatusBar->GetBarState(&state), state) == IStatusBarState::KEYGUARD) {
        OnEdgeClicked(TRUE /* right */);
        return TRUE;
    }

    return OnMiddleClicked();
}

Boolean PanelView::OnMiddleClicked()
{
    Int32 state = 0;
    mStatusBar->GetBarState(&state);
    switch (state) {
        case IStatusBarState::KEYGUARD:
            if (!IsDozing()) {
                StartUnlockHintAnimation();
            }
            return TRUE;
        case IStatusBarState::SHADE_LOCKED:
            mStatusBar->GoToKeyguard();
            return TRUE;
        case IStatusBarState::SHADE: {
            // This gets called in the middle of the touch handling, where the state is still
            // that we are tracking the panel. Collapse the panel after this is done.
            Boolean tmp = FALSE;
            Post(mPostCollapseRunnable, &tmp);
            return FALSE;
        }
        default:
            return TRUE;
    }
}

ECode PanelView::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Float eh = 0;
    GetExpandedHeight(&eh);
    Boolean tmp = FALSE;
    String format;
    format.AppendFormat("[PanelView(%s): expandedHeight=%f maxPanelHeight=%d closing=%s" \
             " tracking=%s justPeeked=%s peekAnim=%s%s timeAnim=%s%s touchDisabled=%s]"
            , /*this.getClass().getSimpleName()*/"TODO: PanelView"
            , eh
            , GetMaxPanelHeight()
            , mClosing ? "T" : "f"
            , mTracking ? "T" : "f"
            , mJustPeeked ? "T" : "f"
            , TO_CSTR(mPeekAnimator)
            , ((mPeekAnimator != NULL && (IAnimator::Probe(mPeekAnimator)->IsStarted(&tmp), tmp))? " (started)" : "")
            , TO_CSTR(mHeightAnimator)
            , ((mHeightAnimator != NULL && (IAnimator::Probe(mHeightAnimator)->IsStarted(&tmp), tmp)) ? " (started)" : "")
            , mTouchDisabled ? "T" : "f"
    );

    pw->Println(format);
    return NOERROR;
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
