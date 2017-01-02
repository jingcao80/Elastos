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

#include "elastos/droid/internal/widget/AutoScrollHelper.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/view/animation/CAccelerateInterpolator.h"
#include "elastos/droid/view/MotionEvent.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Internal::Widget::EIID_IAutoScrollHelper;
using Elastos::Droid::Internal::Widget::EIID_IAbsListViewAutoScroller;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::MotionEvent;
using Elastos::Droid::View::ViewConfiguration;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const Float AutoScrollHelper::NO_MAX = Elastos::Core::Math::FLOAT_MAX_VALUE;

const Float AutoScrollHelper::DEFAULT_MAXIMUM_EDGE = NO_MAX;

const Int32 AutoScrollHelper::DEFAULT_ACTIVATION_DELAY = ViewConfiguration::GetTapTimeout();

//==============================================================================
//          AutoScrollHelper::ScrollAnimationRunnable
//==============================================================================

AutoScrollHelper::ScrollAnimationRunnable::ScrollAnimationRunnable(
    /* [in] */ AutoScrollHelper* host)
    : mHost(host)
{}

AutoScrollHelper::ScrollAnimationRunnable::~ScrollAnimationRunnable()
{}

ECode AutoScrollHelper::ScrollAnimationRunnable::Run()
{
    if (!mHost->mAnimating) {
        return NOERROR;
    }

    if (mHost->mNeedsReset) {
        mHost->mNeedsReset = FALSE;
        mHost->mScroller->Start();
    }

    AutoPtr<ClampedScroller> scroller = mHost->mScroller;
    if (scroller->IsFinished() || !mHost->ShouldAnimate()) {
        mHost->mAnimating = FALSE;
        return NOERROR;
    }

    if (mHost->mNeedsCancel) {
        mHost->mNeedsCancel = FALSE;
        mHost->CancelTargetTouch();
    }

    scroller->ComputeScrollDelta();

    const Int32 deltaX = scroller->GetDeltaX();
    const Int32 deltaY = scroller->GetDeltaY();
    mHost->ScrollTargetBy(deltaX, deltaY);

    // Keep going until the scroller has permanently stopped.
    mHost->mTarget->PostOnAnimation(this);
    return NOERROR;
}

//==============================================================================
//          AutoScrollHelper::ClampedScroller
//==============================================================================

AutoScrollHelper::ClampedScroller::ClampedScroller(
    /* [in] */ AutoScrollHelper* host)
    : mHost(host)
{
    mStartTime = Elastos::Core::Math::INT64_MIN_VALUE;
    mStopTime = -1;
    mDeltaTime = 0;
    mDeltaX = 0;
    mDeltaY = 0;
}

AutoScrollHelper::ClampedScroller::~ClampedScroller()
{}

void AutoScrollHelper::ClampedScroller::SetRampUpDuration(
    /* [in] */ Int32 durationMillis)
{
    mRampUpDuration = durationMillis;
}

void AutoScrollHelper::ClampedScroller::SetRampDownDuration(
    /* [in] */ Int32 durationMillis)
{
    mRampDownDuration = durationMillis;
}

void AutoScrollHelper::ClampedScroller::Start()
{
    AnimationUtils::CurrentAnimationTimeMillis(&mStartTime);
    mStopTime = -1;
    mDeltaTime = mStartTime;
    mStopValue = 0.5f;
    mDeltaX = 0;
    mDeltaY = 0;
}

void AutoScrollHelper::ClampedScroller::RequestStop()
{
    Int64 currentTime;
    AnimationUtils::CurrentAnimationTimeMillis(&currentTime);
    mEffectiveRampDown = mHost->Constrain((Int32) (currentTime - mStartTime), 0, mRampDownDuration);
    mStopValue = GetValueAt(currentTime);
    mStopTime = currentTime;
}

Boolean AutoScrollHelper::ClampedScroller::IsFinished()
{
    Int64 currentTime;
    AnimationUtils::CurrentAnimationTimeMillis(&currentTime);
    return mStopTime > 0
            && currentTime > mStopTime + mEffectiveRampDown;
}

Float AutoScrollHelper::ClampedScroller::GetValueAt(
    /* [in] */ Int64 currentTime)
{
    if (currentTime < mStartTime) {
        return 0.f;
    }
    else if (mStopTime < 0 || currentTime < mStopTime) {
        const Int64 elapsedSinceStart = currentTime - mStartTime;
        return 0.5f * mHost->Constrain(elapsedSinceStart / (Float) mRampUpDuration, 0.0, 1.0);
    }
    else {
        const Int64 elapsedSinceEnd = currentTime - mStopTime;
        return (1 - mStopValue) + mStopValue
                * mHost->Constrain(elapsedSinceEnd / (Float) mEffectiveRampDown, 0.0, 1.0);
    }
}

Float AutoScrollHelper::ClampedScroller::InterpolateValue(
    /* [in] */ Float value)
{
    return -4 * value * value + 4 * value;
}

ECode AutoScrollHelper::ClampedScroller::ComputeScrollDelta()
{
    if (mDeltaTime == 0) {
        Logger::E("AutoScrollHelper::ClampedScroller", "Cannot compute scroll delta before calling start()");
        return E_RUNTIME_EXCEPTION;
    }

    Int64 currentTime;
    AnimationUtils::CurrentAnimationTimeMillis(&currentTime);
    const Float value = GetValueAt(currentTime);
    const Float scale = InterpolateValue(value);
    const Int64 elapsedSinceDelta = currentTime - mDeltaTime;

    mDeltaTime = currentTime;
    mDeltaX = (Int32) (elapsedSinceDelta * scale * mTargetVelocityX);
    mDeltaY = (Int32) (elapsedSinceDelta * scale * mTargetVelocityY);

    return NOERROR;
}

void AutoScrollHelper::ClampedScroller::SetTargetVelocity(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mTargetVelocityX = x;
    mTargetVelocityY = y;
}

Int32 AutoScrollHelper::ClampedScroller::GetHorizontalDirection()
{
    return (Int32) (mTargetVelocityX / Elastos::Core::Math::Abs(mTargetVelocityX));
}

Int32 AutoScrollHelper::ClampedScroller::GetVerticalDirection()
{
    return (Int32) (mTargetVelocityY / Elastos::Core::Math::Abs(mTargetVelocityY));
}

Int32 AutoScrollHelper::ClampedScroller::GetDeltaX()
{
    return mDeltaX;
}

Int32 AutoScrollHelper::ClampedScroller::GetDeltaY()
{
    return mDeltaY;
}

//==============================================================================
//          AutoScrollHelper
//==============================================================================

CAR_INTERFACE_IMPL_2(AutoScrollHelper, Object, IAutoScrollHelper, IViewOnTouchListener);

AutoScrollHelper::AutoScrollHelper()
{
    mScroller = new ClampedScroller(this);

    CAccelerateInterpolator::New((IInterpolator**)&mEdgeInterpolator);

    mRelativeEdges = ArrayOf<Float>::Alloc(2);
    (*mRelativeEdges)[0] = RELATIVE_UNSPECIFIED;
    (*mRelativeEdges)[1] = RELATIVE_UNSPECIFIED;

    mMaximumEdges = ArrayOf<Float>::Alloc(2);
    (*mMaximumEdges)[0] = NO_MAX;
    (*mMaximumEdges)[1] = NO_MAX;

    mRelativeVelocity = ArrayOf<Float>::Alloc(2);
    (*mRelativeVelocity)[0] = RELATIVE_UNSPECIFIED;
    (*mRelativeVelocity)[1] = RELATIVE_UNSPECIFIED;

    mMinimumVelocity = ArrayOf<Float>::Alloc(2);
    (*mMinimumVelocity)[0] = NO_MIN;
    (*mMinimumVelocity)[1] = NO_MIN;

    mMaximumVelocity = ArrayOf<Float>::Alloc(2);
    (*mMaximumVelocity)[0] = NO_MAX;
    (*mMaximumVelocity)[1] = NO_MAX;
}

AutoScrollHelper::~AutoScrollHelper()
{}

ECode AutoScrollHelper::constructor(
    /* [in] */ IView* target)
{
    mTarget = target;

    AutoPtr<IDisplayMetrics> metrics;
    CResources::GetSystem()->GetDisplayMetrics((IDisplayMetrics**)&metrics);

    Float density;
    metrics->GetDensity(&density);

    const Int32 maxVelocity = (Int32) (DEFAULT_MAXIMUM_VELOCITY_DIPS * density + 0.5f);
    const Int32 minVelocity = (Int32) (DEFAULT_MINIMUM_VELOCITY_DIPS * density + 0.5f);
    SetMaximumVelocity(maxVelocity, maxVelocity);
    SetMinimumVelocity(minVelocity, minVelocity);

    SetEdgeType(DEFAULT_EDGE_TYPE);
    SetMaximumEdges(DEFAULT_MAXIMUM_EDGE, DEFAULT_MAXIMUM_EDGE);
    SetRelativeEdges(DEFAULT_RELATIVE_EDGE, DEFAULT_RELATIVE_EDGE);
    SetRelativeVelocity(DEFAULT_RELATIVE_VELOCITY, DEFAULT_RELATIVE_VELOCITY);
    SetActivationDelay(DEFAULT_ACTIVATION_DELAY);
    SetRampUpDuration(DEFAULT_RAMP_UP_DURATION);
    SetRampDownDuration(DEFAULT_RAMP_DOWN_DURATION);

    return NOERROR;
}

ECode AutoScrollHelper::SetEnabled(
    /* [in] */ Boolean enabled)
{
    if (mEnabled && !enabled) {
        RequestStop();
    }

    mEnabled = enabled;
    return NOERROR;
}

ECode AutoScrollHelper::IsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mEnabled;
    return NOERROR;
}

ECode AutoScrollHelper::SetExclusive(
    /* [in] */ Boolean exclusive)
{
    mExclusive = exclusive;
    return NOERROR;
}

ECode AutoScrollHelper::IsExclusive(
    /* [out] */ Boolean* isExclusive)
{
    VALIDATE_NOT_NULL(isExclusive);
    *isExclusive = mExclusive;
    return NOERROR;
}

ECode AutoScrollHelper::SetMaximumVelocity(
    /* [in] */ Float horizontalMax,
    /* [in] */ Float verticalMax)
{
    (*mMaximumVelocity)[HORIZONTAL] = horizontalMax / 1000.f;
    (*mMaximumVelocity)[VERTICAL] = verticalMax / 1000.f;

    return NOERROR;
}

ECode AutoScrollHelper::SetMinimumVelocity(
    /* [in] */ Float horizontalMin,
    /* [in] */ Float verticalMin)
{
    (*mMinimumVelocity)[HORIZONTAL] = horizontalMin / 1000.f;
    (*mMinimumVelocity)[VERTICAL] = verticalMin / 1000.f;
    return NOERROR;
}

ECode AutoScrollHelper::SetRelativeVelocity(
    /* [in] */ Float horizontal,
    /* [in] */ Float vertical)
{
    (*mRelativeVelocity)[HORIZONTAL] = horizontal / 1000.f;
    (*mRelativeVelocity)[VERTICAL] = vertical / 1000.f;
    return NOERROR;
}

ECode AutoScrollHelper::SetEdgeType(
    /* [in] */ Int32 type)
{
    mEdgeType = type;
    return NOERROR;
}

ECode AutoScrollHelper::SetRelativeEdges(
    /* [in] */ Float horizontal,
    /* [in] */ Float vertical)
{
    (*mRelativeEdges)[HORIZONTAL] = horizontal;
    (*mRelativeEdges)[VERTICAL] = vertical;
    return NOERROR;
}

ECode AutoScrollHelper::SetMaximumEdges(
    /* [in] */ Float horizontalMax,
    /* [in] */ Float verticalMax)
{
    (*mMaximumEdges)[HORIZONTAL] = horizontalMax;
    (*mMaximumEdges)[VERTICAL] = verticalMax;
    return NOERROR;
}

ECode AutoScrollHelper::SetActivationDelay(
    /* [in] */ Int32 delayMillis)
{
    mActivationDelay = delayMillis;
    return NOERROR;
}

ECode AutoScrollHelper::SetRampUpDuration(
    /* [in] */ Int32 durationMillis)
{
    mScroller->SetRampUpDuration(durationMillis);
    return NOERROR;
}

ECode AutoScrollHelper::SetRampDownDuration(
    /* [in] */ Int32 durationMillis)
{
    mScroller->SetRampDownDuration(durationMillis);
    return NOERROR;
}

ECode AutoScrollHelper::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (!mEnabled) {
        return NOERROR;
    }

    Int32 action;
    event->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            mNeedsCancel = TRUE;
            mAlreadyDelayed = FALSE;
            // $FALL-THROUGH$
        case IMotionEvent::ACTION_MOVE: {
            Float x, y;
            event->GetX(&x);
            event->GetY(&y);
            Int32 width, height;
            v->GetWidth(&width);
            v->GetHeight(&height);
            Int32 width1, height1;
            mTarget->GetWidth(&width1);
            mTarget->GetHeight(&height1);
            const Float xTargetVelocity = ComputeTargetVelocity(HORIZONTAL, x, width, width1);
            const Float yTargetVelocity = ComputeTargetVelocity(VERTICAL, y, height, height1);
            mScroller->SetTargetVelocity(xTargetVelocity, yTargetVelocity);

            // If the auto scroller was not previously active, but it should
            // be, then update the state and start animations.
            if (!mAnimating && ShouldAnimate()) {
                StartAnimating();
            }
            break;
        }
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            RequestStop();
            break;
    }

    *result = mExclusive && mAnimating;
    return NOERROR;
}

Boolean AutoScrollHelper::ShouldAnimate()
{
    AutoPtr<ClampedScroller> scroller = mScroller;
    const Int32 verticalDirection = scroller->GetVerticalDirection();
    const Int32 horizontalDirection = scroller->GetHorizontalDirection();

    Boolean res1, res2;
    CanTargetScrollVertically(verticalDirection, &res1);
    CanTargetScrollHorizontally(horizontalDirection, &res2);

    return (verticalDirection != 0 && res1) || (horizontalDirection != 0 && res2);
}

void AutoScrollHelper::StartAnimating()
{
    if (mRunnable == NULL) {
        mRunnable = new ScrollAnimationRunnable(this);
    }

    mAnimating = TRUE;
    mNeedsReset = TRUE;

    if (!mAlreadyDelayed && mActivationDelay > 0) {
        mTarget->PostOnAnimationDelayed(mRunnable, mActivationDelay);
    }
    else {
        mRunnable->Run();
    }

    // If we start animating again before the user lifts their finger, we
    // already know it's not a tap and don't need an activation delay.
    mAlreadyDelayed = TRUE;
}

void AutoScrollHelper::RequestStop()
{
    if (mNeedsReset) {
        // The animation has been posted, but hasn't run yet. Manually
        // stopping animation will prevent it from running.
        mAnimating = FALSE;
    }
    else {
        mScroller->RequestStop();
    }
}

Float AutoScrollHelper::ComputeTargetVelocity(
    /* [in] */ Int32 direction,
    /* [in] */ Float coordinate,
    /* [in] */ Float srcSize,
    /* [in] */ Float dstSize)
{
    const Float relativeEdge = (*mRelativeEdges)[direction];
    const Float maximumEdge = (*mMaximumEdges)[direction];
    const Float value = GetEdgeValue(relativeEdge, srcSize, maximumEdge, coordinate);
    if (value == 0) {
        // The edge in this direction is not activated.
        return 0;
    }

    const Float relativeVelocity = (*mRelativeVelocity)[direction];
    const Float minimumVelocity = (*mMinimumVelocity)[direction];
    const Float maximumVelocity = (*mMaximumVelocity)[direction];
    const Float targetVelocity = relativeVelocity * dstSize;

    // Target velocity is adjusted for interpolated edge position, then
    // clamped to the minimum and maximum values. Later, this value will be
    // adjusted for time-based acceleration.
    if (value > 0) {
        return Constrain(value * targetVelocity, minimumVelocity, maximumVelocity);
    }
    else {
        return -Constrain(-value * targetVelocity, minimumVelocity, maximumVelocity);
    }
}

Float AutoScrollHelper::GetEdgeValue(
    /* [in] */ Float relativeValue,
    /* [in] */ Float size,
    /* [in] */ Float maxValue,
    /* [in] */ Float current)
{
    // For now, leading and trailing edges are always the same size.
    const Float edgeSize = Constrain(relativeValue * size, NO_MIN, maxValue);
    const Float valueLeading = ConstrainEdgeValue(current, edgeSize);
    const Float valueTrailing = ConstrainEdgeValue(size - current, edgeSize);
    const Float value = (valueTrailing - valueLeading);
    AutoPtr<ITimeInterpolator> polator = ITimeInterpolator::Probe(mEdgeInterpolator);
    Float interpolated;
    if (value < 0) {
        Float data;
        polator->GetInterpolation(-value, &data);
        interpolated = -data;
    }
    else if (value > 0) {
        polator->GetInterpolation(value, &interpolated);
    }
    else {
        return 0;
    }

    return Constrain(interpolated, -1.0, 1.0);
}

Float AutoScrollHelper::ConstrainEdgeValue(
    /* [in] */ Float current,
    /* [in] */ Float leading)
{
    if (leading == 0) {
        return 0;
    }

    switch (mEdgeType) {
        case EDGE_TYPE_INSIDE:
        case EDGE_TYPE_INSIDE_EXTEND:
            if (current < leading) {
                if (current >= 0) {
                    // Movement up to the edge is scaled.
                    return 1.f - current / leading;
                }
                else if (mAnimating && (mEdgeType == EDGE_TYPE_INSIDE_EXTEND)) {
                    // Movement beyond the edge is always maximum.
                    return 1.f;
                }
            }
            break;
        case EDGE_TYPE_OUTSIDE:
            if (current < 0) {
                // Movement beyond the edge is scaled.
                return current / -leading;
            }
            break;
    }

    return 0;
}

Int32 AutoScrollHelper::Constrain(
    /* [in] */ Int32 value,
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    if (value > max) {
        return max;
    }
    else if (value < min) {
        return min;
    }
    else {
        return value;
    }
}

Float AutoScrollHelper::Constrain(
    /* [in] */ Float value,
    /* [in] */ Float min,
    /* [in] */ Float max)
{
    if (value > max) {
        return max;
    }
    else if (value < min) {
        return min;
    }
    else {
        return value;
    }
}

void AutoScrollHelper::CancelTargetTouch()
{
    const Int64 eventTime = SystemClock::GetUptimeMillis();
    AutoPtr<IMotionEvent> cancel;
    MotionEvent::Obtain(eventTime, eventTime, IMotionEvent::ACTION_CANCEL,
            0, 0, 0, (IMotionEvent**)&cancel);
    Boolean res;
    mTarget->OnTouchEvent(cancel, &res);
    IInputEvent::Probe(cancel)->Recycle();
}

//==============================================================================
//          AbsListViewAutoScroller
//==============================================================================

CAR_INTERFACE_IMPL(AbsListViewAutoScroller, AutoScrollHelper, IAbsListViewAutoScroller);

AbsListViewAutoScroller::AbsListViewAutoScroller()
{}

AbsListViewAutoScroller::~AbsListViewAutoScroller()
{}

ECode AbsListViewAutoScroller::constructor(
    /* [in] */ IAbsListView* target)
{
    AutoScrollHelper::constructor(IView::Probe(target));
    mTarget = target;
    return NOERROR;
}

ECode AbsListViewAutoScroller::ScrollTargetBy(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY)
{
    return mTarget->ScrollListBy(deltaY);
}

ECode AbsListViewAutoScroller::CanTargetScrollHorizontally(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // List do not scroll horizontally.
    *res = FALSE;
    return NOERROR;
}

ECode AbsListViewAutoScroller::CanTargetScrollVertically(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    AutoPtr<IAbsListView> target = mTarget;
    Int32 itemCount;
    IAdapterView::Probe(target)->GetCount(&itemCount);
    if (itemCount == 0) {
        return NOERROR;
    }

    Int32 childCount;
    IViewGroup::Probe(target)->GetChildCount(&childCount);
    Int32 firstPosition;
    IAdapterView::Probe(target)->GetFirstVisiblePosition(&firstPosition);
    const Int32 lastPosition = firstPosition + childCount;

    if (direction > 0) {
        // Are we already showing the entire last item?
        if (lastPosition >= itemCount) {
            AutoPtr<IView> lastView;
            IViewGroup::Probe(target)->GetChildAt(childCount - 1, (IView**)&lastView);
            Int32 bottom, height;
            lastView->GetBottom(&bottom);
            IView::Probe(target)->GetHeight(&height);
            if (bottom <= height) {
                return NOERROR;
            }
        }
    }
    else if (direction < 0) {
        // Are we already showing the entire first item?
        if (firstPosition <= 0) {
            AutoPtr<IView> firstView;
            IViewGroup::Probe(target)->GetChildAt(0, (IView**)&firstView);
            Int32 top;
            firstView->GetTop(&top);
            if (top >= 0) {
                return NOERROR;
            }
        }
    }
    else {
        // The behavior for direction 0 is undefined and we can return
        // whatever we want.
        return NOERROR;
    }

    *res = TRUE;
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
