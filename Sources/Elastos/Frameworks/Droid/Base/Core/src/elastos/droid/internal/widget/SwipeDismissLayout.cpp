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

#include "elastos/droid/internal/widget/SwipeDismissLayout.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/animation/CAccelerateInterpolator.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
//#include "elastos/droid/view/CVelocityTrackerHelper.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Widget::EIID_ISwipeDismissLayout;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
//using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//=====================================================================
//                          SwipeDismissLayout
//=====================================================================
const String SwipeDismissLayout::TAG("SwipeDismissLayout");
const Float SwipeDismissLayout::DISMISS_MIN_DRAG_WIDTH_RATIO = .33f;

CAR_INTERFACE_IMPL(SwipeDismissLayout, FrameLayout, ISwipeDismissLayout)

SwipeDismissLayout::SwipeDismissLayout()
    : mSlop(0)
    , mMinFlingVelocity(0)
    , mMaxFlingVelocity(0)
    , mAnimationTime(0)
    , mActiveTouchId(0)
    , mDownX(0.0f)
    , mDownY(0.0f)
    , mSwiping(FALSE)
    , mDismissed(FALSE)
    , mDiscardIntercept(FALSE)
    , mTranslationX(0.0f)
    , mLastX(0.0f)
{
}

ECode SwipeDismissLayout::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // super(context);
    // init(context);

    FrameLayout::constructor(context);
    Init(context);
    return NOERROR;
}

ECode SwipeDismissLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // super(context, attrs);
    // init(context);

    FrameLayout::constructor(context, attrs);
    Init(context);
    return NOERROR;
}

ECode SwipeDismissLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    // ==================before translated======================
    // super(context, attrs, defStyle);
    // init(context);

    FrameLayout::constructor(context, attrs, defStyle);
    Init(context);
    return NOERROR;
}

ECode SwipeDismissLayout::SetOnDismissedListener(
    /* [in] */ IOnDismissedListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // mDismissedListener = listener;

    mDismissedListener = listener;
    return NOERROR;
}

ECode SwipeDismissLayout::SetOnSwipeProgressChangedListener(
    /* [in] */ IOnSwipeProgressChangedListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // mProgressListener = listener;

    mProgressListener = listener;
    return NOERROR;
}

ECode SwipeDismissLayout::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(ev);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // offset because the view is translated during swipe
    // ev.offsetLocation(mTranslationX, 0);
    //
    // switch (ev.getActionMasked()) {
    //     case MotionEvent.ACTION_DOWN:
    //         resetMembers();
    //         mDownX = ev.getRawX();
    //         mDownY = ev.getRawY();
    //         mActiveTouchId = ev.getPointerId(0);
    //         mVelocityTracker = VelocityTracker.obtain();
    //         mVelocityTracker.addMovement(ev);
    //         break;
    //
    //     case MotionEvent.ACTION_POINTER_DOWN:
    //         int actionIndex = ev.getActionIndex();
    //         mActiveTouchId = ev.getPointerId(actionIndex);
    //         break;
    //     case MotionEvent.ACTION_POINTER_UP:
    //         actionIndex = ev.getActionIndex();
    //         int pointerId = ev.getPointerId(actionIndex);
    //         if (pointerId == mActiveTouchId) {
    //             // This was our active pointer going up. Choose a new active pointer.
    //             int newActionIndex = actionIndex == 0 ? 1 : 0;
    //             mActiveTouchId = ev.getPointerId(newActionIndex);
    //         }
    //         break;
    //
    //     case MotionEvent.ACTION_CANCEL:
    //     case MotionEvent.ACTION_UP:
    //         resetMembers();
    //         break;
    //
    //     case MotionEvent.ACTION_MOVE:
    //         if (mVelocityTracker == null || mDiscardIntercept) {
    //             break;
    //         }
    //
    //         int pointerIndex = ev.findPointerIndex(mActiveTouchId);
    //         if (pointerIndex == -1) {
    //             Log.e(TAG, "Invalid pointer index: ignoring.");
    //             mDiscardIntercept = true;
    //             break;
    //         }
    //         float dx = ev.getRawX() - mDownX;
    //         float x = ev.getX(pointerIndex);
    //         float y = ev.getY(pointerIndex);
    //         if (dx != 0 && canScroll(this, false, dx, x, y)) {
    //             mDiscardIntercept = true;
    //             break;
    //         }
    //         updateSwiping(ev);
    //         break;
    // }
    //
    // return !mDiscardIntercept && mSwiping;

    assert(0);
    Int32 actionMasked = 0;
    ev->GetActionMasked(&actionMasked);
    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN:
            {
                ResetMembers();
                ev->GetRawX(&mDownX);
                ev->GetRawY(&mDownY);
                ev->GetPointerId(0, &mActiveTouchId);

                AutoPtr<IVelocityTrackerHelper> helper;
                //CVelocityTrackerHelper::AcquireSingelton((IVelocityTrackerHelper**)&helper);
                helper->Obtain((IVelocityTracker**)&mVelocityTracker);
                mVelocityTracker->AddMovement(ev);
            }
            break;
        case IMotionEvent::ACTION_POINTER_DOWN:
            {
                Int32 actionIndex;
                ev->GetActionIndex(&actionIndex);
                ev->GetPointerId(actionIndex, &mActiveTouchId);
            }
            break;
        case IMotionEvent::ACTION_POINTER_UP:
            {
                Int32 actionIndex;
                ev->GetActionIndex(&actionIndex);
                Int32 pointerId;
                ev->GetPointerId(actionIndex, &pointerId);
                if (pointerId == mActiveTouchId) {
                    // This was our active pointer going up. Choose a new active pointer.
                    Int32 newActionIndex = actionIndex == 0 ? 1 : 0;
                    ev->GetPointerId(newActionIndex, &mActiveTouchId);
                }
            }
            break;
        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
            ResetMembers();
            break;
        case IMotionEvent::ACTION_MOVE:
            {
                if (mVelocityTracker == NULL || mDiscardIntercept) {
                    break;
                }

                Int32 pointerIndex;
                ev->FindPointerIndex(mActiveTouchId, &pointerIndex);
                if (pointerIndex == -1) {
                    Logger::E(TAG, "Invalid pointer index: ignoring.");
                    mDiscardIntercept = TRUE;
                    break;
                }
                Float rawX;
                ev->GetRawX(&rawX);
                Float dx = rawX - mDownX;
                Float x, y;
                ev->GetX(pointerIndex, &x);
                ev->GetY(pointerIndex, &y);
                if (dx != 0 && CanScroll(this, FALSE, dx, x, y)) {
                    mDiscardIntercept = TRUE;
                    break;
                }
                UpdateSwiping(ev);
            }
            break;
    }

    *result = !mDiscardIntercept && mSwiping;
    return NOERROR;
}

ECode SwipeDismissLayout::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(ev);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mVelocityTracker == null) {
    //     return super.onTouchEvent(ev);
    // }
    // switch (ev.getActionMasked()) {
    //     case MotionEvent.ACTION_UP:
    //         updateDismiss(ev);
    //         if (mDismissed) {
    //             dismiss();
    //         } else if (mSwiping) {
    //             cancel();
    //         }
    //         resetMembers();
    //         break;
    //
    //     case MotionEvent.ACTION_CANCEL:
    //         cancel();
    //         resetMembers();
    //         break;
    //
    //     case MotionEvent.ACTION_MOVE:
    //         mVelocityTracker.addMovement(ev);
    //         mLastX = ev.getRawX();
    //         updateSwiping(ev);
    //         if (mSwiping) {
    //             setProgress(ev.getRawX() - mDownX);
    //             break;
    //         }
    // }
    // return true;

    if (mVelocityTracker == NULL) {
        return FrameLayout::OnTouchEvent(ev, result);
    }

    Int32 actionMask = 0;
    ev->GetActionMasked(&actionMask);
    switch (actionMask) {
        case IMotionEvent::ACTION_UP:
            {
                UpdateDismiss(ev);
                if (mDismissed) {
                    Dismiss();
                }
                else if (mSwiping) {
                    Cancel();
                }
                ResetMembers();
            }
            break;
        case IMotionEvent::ACTION_CANCEL:
            {
                Cancel();
                ResetMembers();
            }
            break;
        case IMotionEvent::ACTION_MOVE:
            {
                mVelocityTracker->AddMovement(ev);
                ev->GetRawX(&mLastX);
                UpdateSwiping(ev);
                if (mSwiping) {
                    SetProgress(mLastX - mDownX);
                    break;
                }
            }
            break;
    }
    *result = true;
    return NOERROR;
}

void SwipeDismissLayout::Cancel()
{
    // ==================before translated======================
    // if (mProgressListener != null) {
    //     mProgressListener.onSwipeCancelled(this);
    // }

    if (mProgressListener != NULL) {
        mProgressListener->OnSwipeCancelled(this);
    }
}

Boolean SwipeDismissLayout::CanScroll(
    /* [in] */ IView* v,
    /* [in] */ Boolean checkV,
    /* [in] */ Float dx,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    // ==================before translated======================
    // if (v instanceof ViewGroup) {
    //     final ViewGroup group = (ViewGroup) v;
    //     final int scrollX = v.getScrollX();
    //     final int scrollY = v.getScrollY();
    //     final int count = group.getChildCount();
    //     for (int i = count - 1; i >= 0; i--) {
    //         final View child = group.getChildAt(i);
    //         if (x + scrollX >= child.getLeft() && x + scrollX < child.getRight() &&
    //                 y + scrollY >= child.getTop() && y + scrollY < child.getBottom() &&
    //                 canScroll(child, true, dx, x + scrollX - child.getLeft(),
    //                         y + scrollY - child.getTop())) {
    //             return true;
    //         }
    //     }
    // }
    //
    // return checkV && v.canScrollHorizontally((int) -dx);

    IViewGroup* group = IViewGroup::Probe(v);
    if (group) {
        Int32 scrollX = 0.0f, scrollY = 0.0f;
        v->GetScrollX(&scrollX);
        v->GetScrollY(&scrollY);

        Int32 count = 0;
        group->GetChildCount(&count);

        Int32 left, right, top, bottom;
        for (Int32 i = count - 1; i >= 0; --i) {
            AutoPtr<IView> child;
            group->GetChildAt(i, (IView**)&child);

            child->GetLeft(&left);
            child->GetRight(&right);
            child->GetTop(&top);
            child->GetBottom(&bottom);
            if (x + scrollX >= left && x + scrollX < right &&
                    y + scrollY >= top && y + scrollY < bottom &&
                    CanScroll(child, TRUE, dx, x + scrollX - left, y + scrollY - top)) {
                return TRUE;
            }
        }
    }

    Boolean resTmp = FALSE;
    v->CanScrollHorizontally((Int32)-dx, &resTmp);
    return checkV && resTmp;
}

void SwipeDismissLayout::Init(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // ViewConfiguration vc = ViewConfiguration.get(getContext());
    // mSlop = vc.getScaledTouchSlop();
    // mMinFlingVelocity = vc.getScaledMinimumFlingVelocity();
    // mMaxFlingVelocity = vc.getScaledMaximumFlingVelocity();
    // mAnimationTime = getContext().getResources().getInteger(
    //         android.R.integer.config_shortAnimTime);
    // mCancelInterpolator = new DecelerateInterpolator(1.5f);
    // mDismissInterpolator = new AccelerateInterpolator(1.5f);

    AutoPtr<IContext> contextTmp;
    GetContext((IContext**)&contextTmp);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(contextTmp, (IViewConfiguration**)&vc);
    vc->GetScaledTouchSlop(&mSlop);
    vc->GetScaledMinimumFlingVelocity(&mMinFlingVelocity);
    vc->GetScaledMaximumFlingVelocity(&mMaxFlingVelocity);

    AutoPtr<IResources> resoutces;
    contextTmp->GetResources((IResources**)&resoutces);
    Int32 intTmp = 0;
    resoutces->GetInteger(R::integer::config_shortAnimTime, &intTmp);
    mAnimationTime = intTmp;

    AutoPtr<IDecelerateInterpolator> interpolatorTmp;
    CDecelerateInterpolator::New(1.5f, (IDecelerateInterpolator**)&interpolatorTmp);
    mCancelInterpolator = ITimeInterpolator::Probe(interpolatorTmp);

    AutoPtr<IAccelerateInterpolator> interpolatorTmp1;
    CAccelerateInterpolator::New(1.5f, (IAccelerateInterpolator**)&interpolatorTmp1);
    mDismissInterpolator = ITimeInterpolator::Probe(interpolatorTmp1);
}

void SwipeDismissLayout::SetProgress(
    /* [in] */ Float deltaX)
{
    // ==================before translated======================
    // mTranslationX = deltaX;
    // if (mProgressListener != null && deltaX >= 0)  {
    //     mProgressListener.onSwipeProgressChanged(this, deltaX / getWidth(), deltaX);
    // }

    mTranslationX = deltaX;
    if (mProgressListener != NULL && deltaX >= 0)  {
        Int32 width = 0;
        GetWidth(&width);
        mProgressListener->OnSwipeProgressChanged(this, deltaX / width, deltaX);
    }
}

void SwipeDismissLayout::Dismiss()
{
    // ==================before translated======================
    // if (mDismissedListener != null) {
    //     mDismissedListener.onDismissed(this);
    // }

    if (mDismissedListener != NULL) {
        mDismissedListener->OnDismissed(this);
    }
}

void SwipeDismissLayout::ResetMembers()
{
    // ==================before translated======================
    // if (mVelocityTracker != null) {
    //     mVelocityTracker.recycle();
    // }
    // mVelocityTracker = null;
    // mTranslationX = 0;
    // mDownX = 0;
    // mDownY = 0;
    // mSwiping = false;
    // mDismissed = false;
    // mDiscardIntercept = false;

    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
    }
    mVelocityTracker = NULL;
    mTranslationX = 0;
    mDownX = 0;
    mDownY = 0;
    mSwiping = FALSE;
    mDismissed = FALSE;
    mDiscardIntercept = FALSE;
}

void SwipeDismissLayout::UpdateSwiping(
    /* [in] */ IMotionEvent* ev)
{
    // ==================before translated======================
    // if (!mSwiping) {
    //     float deltaX = ev.getRawX() - mDownX;
    //     float deltaY = ev.getRawY() - mDownY;
    //     if ((deltaX * deltaX) + (deltaY * deltaY) > mSlop * mSlop) {
    //         mSwiping = deltaX > mSlop * 2 && Math.abs(deltaY) < mSlop * 2;
    //     } else {
    //         mSwiping = false;
    //     }
    // }

    if (!mSwiping) {
        Float rawX, rawY;
        ev->GetRawX(&rawX);
        ev->GetRawY(&rawY);
        Float deltaX = rawX - mDownX;
        Float deltaY = rawY - mDownY;
        if ((deltaX * deltaX) + (deltaY * deltaY) > mSlop * mSlop) {
            mSwiping = deltaX > mSlop * 2 && Elastos::Core::Math::Abs(deltaY) < mSlop * 2;
        }
        else {
            mSwiping = FALSE;
        }
    }
}

void SwipeDismissLayout::UpdateDismiss(
    /* [in] */ IMotionEvent* ev)
{
    // ==================before translated======================
    // float deltaX = ev.getRawX() - mDownX;
    // if (!mDismissed) {
    //     mVelocityTracker.addMovement(ev);
    //     mVelocityTracker.computeCurrentVelocity(1000);
    //
    //     if (deltaX > (getWidth() * DISMISS_MIN_DRAG_WIDTH_RATIO) &&
    //             ev.getRawX() >= mLastX) {
    //         mDismissed = true;
    //     }
    // }
    // // Check if the user tried to undo this.
    // if (mDismissed && mSwiping) {
    //     // Check if the user's finger is actually back
    //     if (deltaX < (getWidth() * DISMISS_MIN_DRAG_WIDTH_RATIO)) {
    //         mDismissed = false;
    //     }
    // }

    Float rawX;
    ev->GetRawX(&rawX);
    Float deltaX = rawX - mDownX;

    Int32 width;
    GetWidth(&width);
    if (!mDismissed) {
        mVelocityTracker->AddMovement(ev);
        mVelocityTracker->ComputeCurrentVelocity(1000);
        if (deltaX > (width * DISMISS_MIN_DRAG_WIDTH_RATIO) && rawX >= mLastX) {
            mDismissed = TRUE;
        }
    }
    // Check if the user tried to undo this.
    if (mDismissed && mSwiping) {
        // Check if the user's finger is actually back
        if (deltaX < (width * DISMISS_MIN_DRAG_WIDTH_RATIO)) {
            mDismissed = FALSE;
        }
    }
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos


