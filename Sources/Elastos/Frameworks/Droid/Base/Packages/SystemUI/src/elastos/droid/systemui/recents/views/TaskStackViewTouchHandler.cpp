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

#include "elastos/droid/systemui/recents/views/TaskStackView.h"
#include "elastos/droid/systemui/recents/Constants.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

static const String TAG("TaskStackViewTouchHandler");

CAR_INTERFACE_IMPL(TaskStackViewTouchHandler::CallBack, Object, ISwipeHelperCallback)

ECode TaskStackViewTouchHandler::CallBack::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** view)
{
    return mHost->GetChildAtPosition(ev, view);
}

ECode TaskStackViewTouchHandler::CallBack::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    return mHost->CanChildBeDismissed(v, result);
}

ECode TaskStackViewTouchHandler::CallBack::OnBeginDrag(
    /* [in] */ IView* v)
{
    return mHost->OnBeginDrag(v);
}

ECode TaskStackViewTouchHandler::CallBack::OnSwipeChanged(
    /* [in] */ IView* v,
    /* [in] */ Float delta)
{
    return mHost->OnSwipeChanged(v, delta);
}

ECode TaskStackViewTouchHandler::CallBack::OnChildDismissed(
    /* [in] */ IView* v)
{
    return mHost->OnChildDismissed(v);
}

ECode TaskStackViewTouchHandler::CallBack::OnSnapBackCompleted(
    /* [in] */ IView* v)
{
    return mHost->OnSnapBackCompleted(v);
}

ECode TaskStackViewTouchHandler::CallBack::OnDragCancelled(
    /* [in] */ IView* v)
{
    return mHost->OnDragCancelled(v);
}

Int32 TaskStackViewTouchHandler::INACTIVE_POINTER_ID = -1;

TaskStackViewTouchHandler::TaskStackViewTouchHandler(
    /* [in] */ IContext* context,
    /* [in] */ TaskStackView* sv,
    /* [in] */ RecentsConfiguration* config,
    /* [in] */ TaskStackViewScroller* scroller)
    : mIsScrolling(FALSE)
    , mInitialP(0.0f)
    , mLastP(0.0f)
    , mTotalPMotion(0.0f)
    , mInitialMotionX(0)
    , mInitialMotionY(0)
    , mLastMotionX(0)
    , mLastMotionY(0)
    , mActivePointerId(INACTIVE_POINTER_ID)
    , mInterceptedBySwipeHelper(FALSE)
{
    AutoPtr<IViewConfigurationHelper> vcHelper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&vcHelper);
    AutoPtr<IViewConfiguration> configuration;
    vcHelper->Get(context, (IViewConfiguration**)&configuration);
    configuration->GetScaledMinimumFlingVelocity(&mMinimumVelocity);
    configuration->GetScaledMaximumFlingVelocity(&mMaximumVelocity);
    configuration->GetScaledTouchSlop(&mScrollTouchSlop);
    Int32 pagingTouchSlop;
    configuration->GetScaledPagingTouchSlop(&pagingTouchSlop);
    mPagingTouchSlop = pagingTouchSlop;
    mScroller = scroller;
    mConfig = config;

    sv->GetWeakReference((IWeakReference**)&mWeakTaskStackView);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float densityScale;
    dm->GetDensity(&densityScale);
    mCallback = new CallBack(this);
    mSwipeHelper = new SwipeHelper(SwipeHelper::X, mCallback, densityScale, mPagingTouchSlop);
    mSwipeHelper->SetMinAlpha(1.0f);
}

AutoPtr<TaskStackView> TaskStackViewTouchHandler::GetTaskStackView()
{
    AutoPtr<ITaskStackView> tsv;
    mWeakTaskStackView->Resolve(EIID_ITaskStackView, (IInterface**)&tsv);
    if (tsv != NULL) {
        AutoPtr<TaskStackView> statckView = (TaskStackView*)tsv.Get();
        return statckView;
    }
    return NULL;
}

/** Velocity tracker helpers */
void TaskStackViewTouchHandler::InitOrResetVelocityTracker()
{
    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> vtHelper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&vtHelper);
        vtHelper->Obtain((IVelocityTracker**)&mVelocityTracker);
    }
    else {
        mVelocityTracker->Clear();
    }
}

void TaskStackViewTouchHandler::InitVelocityTrackerIfNotExists()
{
    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> vtHelper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&vtHelper);
        vtHelper->Obtain((IVelocityTracker**)&mVelocityTracker);
    }
}

void TaskStackViewTouchHandler::RecycleVelocityTracker()
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
}

/** Returns the view at the specified coordinates */
AutoPtr<TaskView> TaskStackViewTouchHandler::FindViewAtPoint(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    AutoPtr<TaskStackView> statckView = GetTaskStackView();
    Int32 childCount;
    statckView->GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> view;
        statckView->GetChildAt(i, (IView**)&view);
        TaskView* tv = (TaskView*)ITaskView::Probe(view);
        Int32 visibility;
        tv->GetVisibility(&visibility);
        if (visibility == IView::VISIBLE) {
            if (statckView->IsTransformedTouchPointInView(x, y, tv)) {
                return tv;
            }
        }
    }
    return NULL;
}

/** Constructs a simulated motion event for the current stack scroll. */
AutoPtr<IMotionEvent> TaskStackViewTouchHandler::CreateMotionEventForStackScroll(
    /* [in] */ IMotionEvent* ev)
{
    AutoPtr<IMotionEventHelper> meHelper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&meHelper);
    AutoPtr<IMotionEvent> pev;
    meHelper->ObtainNoHistory(ev, (IMotionEvent**)&pev);
    pev->SetLocation(0, mScroller->ProgressToScrollRange(mScroller->GetStackScroll()));
    return pev;
}

/** Touch preprocessing for handling below */
Boolean TaskStackViewTouchHandler::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    AutoPtr<TaskStackView> stackView = GetTaskStackView();
    if (stackView == NULL) {
        return FALSE;
    }

    // Return early if we have no children
    Int32 childCount;
    stackView->GetChildCount(&childCount);
    Boolean hasChildren = (childCount > 0);
    if (!hasChildren) {
        return FALSE;
    }

    // Pass through to swipe helper if we are swiping
    mInterceptedBySwipeHelper = mSwipeHelper->OnInterceptTouchEvent(ev);
    if (mInterceptedBySwipeHelper) {
        return TRUE;
    }

    Boolean isRunning;
    Boolean wasScrolling = mScroller->IsScrolling() || (mScroller->mScrollAnimator != NULL
        && (IAnimator::Probe(mScroller->mScrollAnimator)->IsRunning(&isRunning), isRunning));
    Int32 action;
    ev->GetAction(&action);
    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_DOWN: {
            // Save the touch down info
            Float x, y;
            ev->GetX(&x);
            ev->GetY(&y);
            mInitialMotionX = mLastMotionX = (Int32)x;
            mInitialMotionY = mLastMotionY = (Int32)y;
            mInitialP = mLastP = stackView->mLayoutAlgorithm->ScreenYToCurveProgress(mLastMotionY);
            ev->GetPointerId(0, &mActivePointerId);
            mActiveTaskView = FindViewAtPoint(mLastMotionX, mLastMotionY);
            // Stop the current scroll if it is still flinging
            mScroller->StopScroller();
            mScroller->StopBoundScrollAnimation();
            // Initialize the velocity tracker
            InitOrResetVelocityTracker();
            mVelocityTracker->AddMovement(CreateMotionEventForStackScroll(ev));
            // Check if the scroller is finished yet
            mIsScrolling = mScroller->IsScrolling();
            break;
        }
        case IMotionEvent::ACTION_MOVE: {
            if (mActivePointerId == INACTIVE_POINTER_ID) break;

            Int32 activePointerIndex;
            ev->FindPointerIndex(mActivePointerId, &activePointerIndex);
            Float px, py;
            ev->GetX(activePointerIndex, &px);
            ev->GetY(activePointerIndex, &py);
            Int32 y = (Int32)py;
            Int32 x = (Int32)px;
            if (Elastos::Core::Math::Abs(y - mInitialMotionY) > mScrollTouchSlop) {
                // Save the touch move info
                mIsScrolling = TRUE;
                // Initialize the velocity tracker if necessary
                InitVelocityTrackerIfNotExists();
                mVelocityTracker->AddMovement(CreateMotionEventForStackScroll(ev));
                // Disallow parents from intercepting touch events
                AutoPtr<IViewParent> parent;
                stackView->GetParent((IViewParent**)&parent);
                if (parent != NULL) {
                    parent->RequestDisallowInterceptTouchEvent(TRUE);
                }
            }

            mLastMotionX = x;
            mLastMotionY = y;
            mLastP = stackView->mLayoutAlgorithm->ScreenYToCurveProgress(mLastMotionY);
            break;
        }
        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP: {
            // Animate the scroll back if we've cancelled
            mScroller->AnimateBoundScroll();
            // Reset the drag state and the velocity tracker
            mIsScrolling = FALSE;
            mActivePointerId = INACTIVE_POINTER_ID;
            mActiveTaskView = NULL;
            mTotalPMotion = 0;
            RecycleVelocityTracker();
            break;
        }
    }

    return wasScrolling || mIsScrolling;
}

/** Handles touch events once we have intercepted them */
Boolean TaskStackViewTouchHandler::OnTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    AutoPtr<TaskStackView> stackView = GetTaskStackView();

    Int32 childCount;
    stackView->GetChildCount(&childCount);
    // Short circuit if we have no children
    Boolean hasChildren = (childCount > 0);
    if (!hasChildren) {
        return FALSE;
    }

    // Pass through to swipe helper if we are swiping
    if (mInterceptedBySwipeHelper && mSwipeHelper->OnTouchEvent(ev)) {
        return TRUE;
    }

    // Update the velocity tracker
    InitVelocityTrackerIfNotExists();

    Int32 action;
    ev->GetAction(&action);
    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_DOWN: {
            // Save the touch down info
            Float x, y;
            ev->GetX(&x);
            ev->GetY(&y);
            mInitialMotionX = mLastMotionX = (Int32)x;
            mInitialMotionY = mLastMotionY = (Int32)y;
            mInitialP = mLastP = stackView->mLayoutAlgorithm->ScreenYToCurveProgress(mLastMotionY);
            ev->GetPointerId(0, &mActivePointerId);
            mActiveTaskView = FindViewAtPoint(mLastMotionX, mLastMotionY);
            // Stop the current scroll if it is still flinging
            mScroller->StopScroller();
            mScroller->StopBoundScrollAnimation();
            // Initialize the velocity tracker
            InitOrResetVelocityTracker();
            mVelocityTracker->AddMovement(CreateMotionEventForStackScroll(ev));
            // Disallow parents from intercepting touch events
            AutoPtr<IViewParent> parent;
            stackView->GetParent((IViewParent**)&parent);
            if (parent != NULL) {
                parent->RequestDisallowInterceptTouchEvent(TRUE);
            }
            break;
        }
        case IMotionEvent::ACTION_POINTER_DOWN: {
            Int32 index;
            ev->GetActionIndex(&index);
            ev->GetPointerId(index, &mActivePointerId);
            Float x, y;
            ev->GetX(index, &x);
            ev->GetY(index, &y);
            mLastMotionX = (Int32)x;
            mLastMotionY = (Int32)y;
            mLastP = stackView->mLayoutAlgorithm->ScreenYToCurveProgress(mLastMotionY);
            break;
        }
        case IMotionEvent::ACTION_MOVE: {
            if (mActivePointerId == INACTIVE_POINTER_ID) break;

            Int32 activePointerIndex;
            ev->FindPointerIndex(mActivePointerId, &activePointerIndex);
            Float px, py;
            ev->GetX(activePointerIndex, &px);
            ev->GetY(activePointerIndex, &py);
            Int32 x = (Int32)px;
            Int32 y = (Int32)py;
            Int32 yTotal = Elastos::Core::Math::Abs(y - mInitialMotionY);
            Float curP = stackView->mLayoutAlgorithm->ScreenYToCurveProgress(y);
            Float deltaP = mLastP - curP;
            if (!mIsScrolling) {
                if (yTotal > mScrollTouchSlop) {
                    mIsScrolling = TRUE;
                    // Initialize the velocity tracker
                    InitOrResetVelocityTracker();
                    mVelocityTracker->AddMovement(CreateMotionEventForStackScroll(ev));
                    // Disallow parents from intercepting touch events
                    AutoPtr<IViewParent> parent;
                    stackView->GetParent((IViewParent**)&parent);
                    if (parent != NULL) {
                        parent->RequestDisallowInterceptTouchEvent(TRUE);
                    }
                }
            }
            if (mIsScrolling) {
                Float curStackScroll = mScroller->GetStackScroll();
                Float overScrollAmount = mScroller->GetScrollAmountOutOfBounds(curStackScroll + deltaP);
                if (Elastos::Core::Math::Compare(overScrollAmount, 0.0f) != 0) {
                    // Bound the overscroll to a fixed amount, and inversely scale the y-movement
                    // relative to how close we are to the max overscroll
                    Float maxOverScroll = mConfig->mTaskStackOverscrollPct;
                    deltaP *= (1.0f - (Elastos::Core::Math::Min(maxOverScroll, overScrollAmount)
                            / maxOverScroll));
                }
                mScroller->SetStackScroll(curStackScroll + deltaP);
                if (mScroller->IsScrollOutOfBounds()) {
                    mVelocityTracker->Clear();
                }
                else {
                    mVelocityTracker->AddMovement(CreateMotionEventForStackScroll(ev));
                }
            }
            mLastMotionX = x;
            mLastMotionY = y;
            mLastP = stackView->mLayoutAlgorithm->ScreenYToCurveProgress(mLastMotionY);
            mTotalPMotion += Elastos::Core::Math::Abs(deltaP);
            break;
        }
        case IMotionEvent::ACTION_UP: {
            AutoPtr<IVelocityTracker> velocityTracker = mVelocityTracker;
            velocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
            Float yVelocity;
            velocityTracker->GetYVelocity(mActivePointerId, &yVelocity);
            Int32 velocity = (Int32)yVelocity;
            if (mIsScrolling && (Elastos::Core::Math::Abs(velocity) > mMinimumVelocity)) {
                Int32 overscrollRange = (Int32) (Elastos::Core::Math::Min(1.0f,
                        Elastos::Core::Math::Abs((Float) velocity / mMaximumVelocity)) *
                        Constants::Values::TaskStackView::TaskStackOverscrollRange);
                // Fling scroll
                mScroller->mScroller->Fling(0, mScroller->ProgressToScrollRange(mScroller->GetStackScroll()),
                        0, velocity,
                        0, 0,
                        mScroller->ProgressToScrollRange(stackView->mLayoutAlgorithm->mMinScrollP),
                        mScroller->ProgressToScrollRange(stackView->mLayoutAlgorithm->mMaxScrollP),
                        0, overscrollRange);
                // Invalidate to kick off computeScroll
                stackView->Invalidate();
            }
            else if (mScroller->IsScrollOutOfBounds()) {
                // Animate the scroll back into bounds
                mScroller->AnimateBoundScroll();
            }

            mActivePointerId = INACTIVE_POINTER_ID;
            mIsScrolling = FALSE;
            mTotalPMotion = 0;
            RecycleVelocityTracker();
            break;
        }
        case IMotionEvent::ACTION_POINTER_UP: {
            Int32 pointerIndex;
            ev->GetActionIndex(&pointerIndex);
            Int32 pointerId;
            ev->GetPointerId(pointerIndex, &pointerId);
            if (pointerId == mActivePointerId) {
                // Select a new active pointer id and reset the motion state
                Int32 newPointerIndex = (pointerIndex == 0) ? 1 : 0;
                ev->GetPointerId(newPointerIndex, &mActivePointerId);
                Float x, y;
                ev->GetX(newPointerIndex, &x);
                ev->GetY(newPointerIndex, &y);
                mLastMotionX = (Int32)x;
                mLastMotionY = (Int32)y;
                mLastP = stackView->mLayoutAlgorithm->ScreenYToCurveProgress(mLastMotionY);
                mVelocityTracker->Clear();
            }
            break;
        }
        case IMotionEvent::ACTION_CANCEL: {
            if (mScroller->IsScrollOutOfBounds()) {
                // Animate the scroll back into bounds
                mScroller->AnimateBoundScroll();
            }
            mActivePointerId = INACTIVE_POINTER_ID;
            mIsScrolling = FALSE;
            mTotalPMotion = 0;
            RecycleVelocityTracker();
            break;
        }
    }
    return TRUE;
}

/**** SwipeHelper Implementation ****/

ECode TaskStackViewTouchHandler::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    Float x, y;
    ev->GetX(&x);
    ev->GetY(&y);
    AutoPtr<TaskView> taskView = FindViewAtPoint((Int32)x, (Int32)y);
    *view = taskView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode TaskStackViewTouchHandler::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode TaskStackViewTouchHandler::OnBeginDrag(
    /* [in] */ IView* v)
{
    AutoPtr<TaskStackView> stackView = GetTaskStackView();
    AutoPtr<TaskView> tv = (TaskView*)ITaskView::Probe(v);
    // Disable clipping with the stack while we are swiping
    tv->SetClipViewInStack(FALSE);
    // Disallow touch events from this task view
    tv->SetTouchEnabled(FALSE);
    // Hide the footer
    tv->AnimateFooterVisibility(FALSE, stackView->mConfig->mTaskViewLockToAppShortAnimDuration);
    // Disallow parents from intercepting touch events
    AutoPtr<IViewParent> parent;
    stackView->GetParent((IViewParent**)&parent);
    if (parent != NULL) {
        parent->RequestDisallowInterceptTouchEvent(TRUE);
    }
    return NOERROR;
}

ECode TaskStackViewTouchHandler::OnSwipeChanged(
    /* [in] */ IView* v,
    /* [in] */ Float delta)
{
    // Do nothing
    return NOERROR;
}

ECode TaskStackViewTouchHandler::OnChildDismissed(
    /* [in] */ IView* v)
{
    AutoPtr<TaskView> tv = (TaskView*)ITaskView::Probe(v);
    // Re-enable clipping with the stack (we will reuse this view)
    tv->SetClipViewInStack(TRUE);
    // Re-enable touch events from this task view
    tv->SetTouchEnabled(TRUE);
    // Remove the task view from the stack
    AutoPtr<TaskStackView> tsv = GetTaskStackView();
    if (tsv != NULL) {
        tsv->OnTaskViewDismissed(tv);
    }
    else {
        Logger::I(TAG, "waring: TaskStackView has been released.");
    }
    return NOERROR;
}

ECode TaskStackViewTouchHandler::OnSnapBackCompleted(
    /* [in] */ IView* v)
{
    AutoPtr<TaskView> tv = (TaskView*)ITaskView::Probe(v);
    // Re-enable clipping with the stack
    tv->SetClipViewInStack(TRUE);
    // Re-enable touch events from this task view
    tv->SetTouchEnabled(TRUE);
    // Restore the footer
    tv->AnimateFooterVisibility(TRUE, GetTaskStackView()->mConfig->mTaskViewLockToAppShortAnimDuration);
    return NOERROR;
}

ECode TaskStackViewTouchHandler::OnDragCancelled(
    /* [in] */ IView* v)
{
    // Do nothing
    return NOERROR;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
