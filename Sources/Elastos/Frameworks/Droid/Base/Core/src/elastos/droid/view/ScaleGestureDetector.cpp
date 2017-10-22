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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ScaleGestureDetector.h"
#include "elastos/droid/view/InputEventConsistencyVerifier.h"
#include "elastos/droid/view/CGestureDetector.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace View {

const String ScaleGestureDetector::TAG("CScaleGestureDetector");
const Int64 ScaleGestureDetector::TOUCH_STABILIZE_TIME = 128; // ms
const Int32 ScaleGestureDetector::DOUBLE_TAP_MODE_NONE = 0;
const Int32 ScaleGestureDetector::DOUBLE_TAP_MODE_IN_PROGRESS = 1;
const Float ScaleGestureDetector::SCALE_FACTOR = .5f;

/* ScaleGestureDetector::SimpleOnScaleGestureListener */
CAR_INTERFACE_IMPL(ScaleGestureDetector::SimpleOnScaleGestureListener, Object, IScaleGestureDetectorOnScaleGestureListener);

ScaleGestureDetector::SimpleOnScaleGestureListener::SimpleOnScaleGestureListener()
{}

ScaleGestureDetector::SimpleOnScaleGestureListener::~SimpleOnScaleGestureListener()
{}

ECode ScaleGestureDetector::SimpleOnScaleGestureListener::constructor()
{
    return NOERROR;
}

ECode ScaleGestureDetector::SimpleOnScaleGestureListener::OnScale(
    /* [in ]*/ IScaleGestureDetector* detector,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode ScaleGestureDetector::SimpleOnScaleGestureListener::OnScaleBegin(
    /* [in ]*/ IScaleGestureDetector* detector,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = TRUE;
    return NOERROR;
}

ECode ScaleGestureDetector::SimpleOnScaleGestureListener::OnScaleEnd(
    /* [in ]*/ IScaleGestureDetector* detector)
{
    // Intentionally empty
    return NOERROR;
}

/*ScaleGestureDetector::MySimpleOnGestureListener*/

ScaleGestureDetector::MySimpleOnGestureListener::MySimpleOnGestureListener(
    /* [in] */ ScaleGestureDetector* host)
    : mHost(host)
{}

ScaleGestureDetector::MySimpleOnGestureListener::~MySimpleOnGestureListener()
{}

ECode ScaleGestureDetector::MySimpleOnGestureListener::constructor()
{
    return NOERROR;
}

ECode ScaleGestureDetector::MySimpleOnGestureListener::OnDoubleTap(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // Double tap: start watching for a swipe
    mHost->mDoubleTapEvent = e;
    mHost->mDoubleTapMode = DOUBLE_TAP_MODE_IN_PROGRESS;
    *res = TRUE;
    return NOERROR;
}

/*ScaleGestureDetector*/
CAR_INTERFACE_IMPL(ScaleGestureDetector, Object, IScaleGestureDetector);

ScaleGestureDetector::ScaleGestureDetector()
    : mFocusX(0)
    , mFocusY(0)
    , mQuickScaleEnabled(FALSE)
    , mCurrSpan(0)
    , mPrevSpan(0)
    , mInitialSpan(0)
    , mCurrSpanX(0)
    , mCurrSpanY(0)
    , mPrevSpanX(0)
    , mPrevSpanY(0)
    , mCurrTime(0)
    , mPrevTime(0)
    , mInProgress(FALSE)
    , mSpanSlop(0)
    , mMinSpan(0)
    , mTouchUpper(Elastos::Core::Math::FLOAT_NAN)
    , mTouchLower(Elastos::Core::Math::FLOAT_NAN)
    , mTouchHistoryLastAccepted(Elastos::Core::Math::FLOAT_NAN)
    , mTouchHistoryDirection(0)
    , mTouchHistoryLastAcceptedTime(0)
    , mTouchMinMajor(0)
    , mDoubleTapMode(DOUBLE_TAP_MODE_NONE)
    , mEventBeforeOrAboveStartingGestureEvent(FALSE)
{
}

ScaleGestureDetector::~ScaleGestureDetector()
{}

ECode ScaleGestureDetector::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IScaleGestureDetectorOnScaleGestureListener* listener)
{
    return constructor(context, listener, NULL);
}

ECode ScaleGestureDetector::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IScaleGestureDetectorOnScaleGestureListener* listener,
    /* [in] */ IHandler* handler)
{
    assert(context != NULL);

    mInputEventConsistencyVerifier =
            InputEventConsistencyVerifier::IsInstrumentationEnabled() ?
                    new InputEventConsistencyVerifier(TO_IINTERFACE(this), 0) : NULL;

    mContext = context;
    mListener = listener;

    AutoPtr<IViewConfiguration> vc = ViewConfiguration::Get(context);
    vc->GetScaledTouchSlop(&mSpanSlop);
    mSpanSlop *= 2;

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::config_minScalingTouchMajor, &mTouchMinMajor);
    res->GetDimensionPixelSize(R::dimen::config_minScalingSpan, &mMinSpan);
    mHandler = handler;
    // Quick scale is enabled by default after JB_MR2
    AutoPtr<IApplicationInfo> info;
    context->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 sdkVersion;
    info->GetTargetSdkVersion(&sdkVersion);
    if (sdkVersion > Build::VERSION_CODES::JELLY_BEAN_MR2) {
        SetQuickScaleEnabled(TRUE);
    }

    return NOERROR;
}

void ScaleGestureDetector::AddTouchHistory(
    /* [in] */ IMotionEvent* ev)
{
    using Elastos::Core::Math;

    Int64 currentTime = SystemClock::GetUptimeMillis();
    Int32 count;
    ev->GetPointerCount(&count);
    Boolean accept = currentTime - mTouchHistoryLastAcceptedTime >= TOUCH_STABILIZE_TIME;
    Float total = 0;
    Int32 sampleCount = 0;
    Boolean hasLastAccepted;
    Int32 historySize, pointerSampleCount;
    Float major;
    Int64 time;

    for (Int32 i = 0; i < count; i++) {
            hasLastAccepted = !Math::IsNaN(mTouchHistoryLastAccepted);
            ev->GetHistorySize(&historySize);
        pointerSampleCount = historySize + 1;
        for (Int32 h = 0; h < pointerSampleCount; h++) {
            if (h < historySize) {
                ev->GetHistoricalSize(i, h, &major);
            }
            else {
                ev->GetTouchMajor(i, &major);
            }
            if (major < mTouchMinMajor) major = mTouchMinMajor;
            total += major;

            if (Math::IsNaN(mTouchUpper) || major > mTouchUpper) {
                mTouchUpper = major;
            }
            if (Math::IsNaN(mTouchLower) || major < mTouchLower) {
                mTouchLower = major;
            }

            if (hasLastAccepted) {
                Int32 directionSig = (Int32) Math::Signum(major - mTouchHistoryLastAccepted);
                if (directionSig != mTouchHistoryDirection ||
                        (directionSig == 0 && mTouchHistoryDirection == 0)) {
                    mTouchHistoryDirection = directionSig;
                    if (h < historySize) {
                        ev->GetHistoricalEventTime(h, &time);
                    }
                    else {
                        IInputEvent::Probe(ev)->GetEventTime(&time);
                    }
                    mTouchHistoryLastAcceptedTime = time;
                    accept = FALSE;
                }
            }
        }
        sampleCount += pointerSampleCount;
    }

    Float avg = total / sampleCount;

    if (accept) {
        Float newAccepted = (mTouchUpper + mTouchLower + avg) / 3;
        mTouchUpper = (mTouchUpper + newAccepted) / 2;
        mTouchLower = (mTouchLower + newAccepted) / 2;
        mTouchHistoryLastAccepted = newAccepted;
        mTouchHistoryDirection = 0;
        IInputEvent::Probe(ev)->GetEventTime(&mTouchHistoryLastAcceptedTime);
    }
}

void ScaleGestureDetector::ClearTouchHistory()
{
    using Elastos::Core::Math;

    mTouchUpper = Math::FLOAT_NAN;
    mTouchLower = Math::FLOAT_NAN;
    mTouchHistoryLastAccepted = Math::FLOAT_NAN;
    mTouchHistoryDirection = 0;
    mTouchHistoryLastAcceptedTime = 0;
}

ECode ScaleGestureDetector::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    using Elastos::Core::Math;

    if (mInputEventConsistencyVerifier != NULL) {
        mInputEventConsistencyVerifier->OnTouchEvent(event, 0);
    }

    Int32 action;
    event->GetActionMasked(&action);

    // Forward the event to check for double tap gesture
    if (mQuickScaleEnabled) {
        Boolean res;
        mGestureDetector->OnTouchEvent(event, &res);
    }

    Boolean streamComplete = action == IMotionEvent::ACTION_UP ||
            action == IMotionEvent::ACTION_CANCEL;
    if (action == IMotionEvent::ACTION_DOWN || streamComplete) {
        // Reset any scale in progress with the listener.
        // If it's an ACTION_DOWN we're beginning a new event stream.
        // This means the app probably didn't give us all the events. Shame on it.
        if (mInProgress) {
            mListener->OnScaleEnd(this);
            mInProgress = FALSE;
            mInitialSpan = 0;
            mDoubleTapMode = DOUBLE_TAP_MODE_NONE;
        }
        else if (mDoubleTapMode == DOUBLE_TAP_MODE_IN_PROGRESS && streamComplete) {
            mInProgress = FALSE;
            mInitialSpan = 0;
            mDoubleTapMode = DOUBLE_TAP_MODE_NONE;
        }

        if (streamComplete) {
            ClearTouchHistory();
            *result = TRUE;
            return NOERROR;
        }
    }

    Boolean configChanged = action == IMotionEvent::ACTION_DOWN ||
            action == IMotionEvent::ACTION_POINTER_UP ||
            action == IMotionEvent::ACTION_POINTER_DOWN;
    Boolean pointerUp = action == IMotionEvent::ACTION_POINTER_UP;
    Int32 skipIndex = -1;
    if (pointerUp) {
        event->GetActionIndex(&skipIndex);
    }

    // Determine focal point
    Float sumX = 0, sumY = 0;
    Float x, y;
    Int32 count;
    event->GetPointerCount(&count);
    Int32 div = pointerUp ? count - 1 : count;
    Float focusX;
    Float focusY;
    if (mDoubleTapMode == DOUBLE_TAP_MODE_IN_PROGRESS) {
        // In double tap mode, the focal pt is always where the double tap
        // gesture started
        mDoubleTapEvent->GetX(&focusX);
        mDoubleTapEvent->GetY(&focusY);
        if ((event->GetY(&y), y) < focusY) {
            mEventBeforeOrAboveStartingGestureEvent = TRUE;
        }
        else {
            mEventBeforeOrAboveStartingGestureEvent = FALSE;
        }
    }
    else {
        for (Int32 i = 0; i < count; i++) {
            if (skipIndex == i) continue;
            event->GetX(i, &x);
            event->GetY(i, &y);
            sumX += x;
            sumY += y;
        }

        focusX = sumX / div;
        focusY = sumY / div;
    }

    AddTouchHistory(event);

    // Determine average deviation from focal point
    Float devSumX = 0, devSumY = 0;
    for (Int32 i = 0; i < count; i++) {
        if (skipIndex == i) continue;

        // Convert the resulting diameter into a radius.
        Float touchSize = mTouchHistoryLastAccepted / 2;
        event->GetX(i, &x);
        event->GetY(i, &y);
        devSumX += Math::Abs(x - focusX) + touchSize;
        devSumY += Math::Abs(y - focusY) + touchSize;
    }
    Float devX = devSumX / div;
    Float devY = devSumY / div;

    // Span is the average distance between touch points through the focal point;
    // i.e. the diameter of the circle with a radius of the average deviation from
    // the focal point.
    Float spanX = devX * 2;
    Float spanY = devY * 2;
    Float span;
    if (InDoubleTapMode()) {
        span = spanY;
    }
    else {
        span = Math::Sqrt(spanX * spanX + spanY * spanY);
    }

    // Dispatch begin/end events as needed.
    // If the configuration changes, notify the app to reset its current state by beginning
    // a fresh scale event stream.
    Boolean wasInProgress = mInProgress;
    mFocusX = focusX;
    mFocusY = focusY;
    if ( !InDoubleTapMode() && mInProgress && (span < mMinSpan || configChanged)) {
        mListener->OnScaleEnd(this);
        mInProgress = FALSE;
        mInitialSpan = span;
        mDoubleTapMode = DOUBLE_TAP_MODE_NONE;
    }
    if (configChanged) {
        mPrevSpanX = mCurrSpanX = spanX;
        mPrevSpanY = mCurrSpanY = spanY;
        mInitialSpan = mPrevSpan = mCurrSpan = span;
    }

    Int32 minSpan = InDoubleTapMode() ? mSpanSlop : mMinSpan;
    if (!mInProgress && span >= minSpan &&
            (wasInProgress || Math::Abs(span - mInitialSpan) > mSpanSlop)) {
        mPrevSpanX = mCurrSpanX = spanX;
        mPrevSpanY = mCurrSpanY = spanY;
        mPrevSpan = mCurrSpan = span;
        Boolean result;
        mInProgress = mListener->OnScaleBegin(this, &result);
    }

    // Handle motion; focal point and span/scale factor are changing.
    if (action == IMotionEvent::ACTION_MOVE) {
        mCurrSpanX = spanX;
        mCurrSpanY = spanY;
        mCurrSpan = span;

        Boolean updatePrev = TRUE;
        if (mInProgress) {
            Boolean result;
            updatePrev = mListener->OnScale(this, &result);
        }

        if (updatePrev) {
            mPrevSpanX = mCurrSpanX;
            mPrevSpanY = mCurrSpanY;
            mPrevSpan = mCurrSpan;
        }
    }

    *result = TRUE;
    return NOERROR;
}

Boolean ScaleGestureDetector::InDoubleTapMode()
{
    return mDoubleTapMode == DOUBLE_TAP_MODE_IN_PROGRESS;
}

ECode ScaleGestureDetector::SetQuickScaleEnabled(
    /* [in] */ Boolean scales)
{
    mQuickScaleEnabled = scales;
    if (mQuickScaleEnabled && mGestureDetector == NULL) {
        AutoPtr<MySimpleOnGestureListener> gestureListener = new MySimpleOnGestureListener(this);
        CGestureDetector::New(mContext, gestureListener, mHandler, (IGestureDetector**)&mGestureDetector);
    }
    return NOERROR;
}

ECode ScaleGestureDetector::IsQuickScaleEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mQuickScaleEnabled;
    return NOERROR;
}

ECode ScaleGestureDetector::IsInProgress(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInProgress;
    return NOERROR;
}

ECode ScaleGestureDetector::GetFocusX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);
    *x = mFocusX;
    return NOERROR;
}

ECode ScaleGestureDetector::GetFocusY(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mFocusY;
    return NOERROR;
}

ECode ScaleGestureDetector::GetCurrentSpan(
    /* [out] */ Float* span)
{
    VALIDATE_NOT_NULL(span);
    *span = mCurrSpan;
    return NOERROR;
}

ECode ScaleGestureDetector::GetCurrentSpanX(
    /* [out] */ Float* spanX)
{
    VALIDATE_NOT_NULL(spanX);
    *spanX = mCurrSpanX;
    return NOERROR;
}

ECode ScaleGestureDetector::GetCurrentSpanY(
    /* [out] */ Float* spanY)
{
    VALIDATE_NOT_NULL(spanY);
    *spanY = mCurrSpanY;
    return NOERROR;
}

ECode ScaleGestureDetector::GetPreviousSpan(
    /* [out] */ Float* span)
{
    VALIDATE_NOT_NULL(span);
    *span = mPrevSpan;
    return NOERROR;
}

ECode ScaleGestureDetector::GetPreviousSpanX(
    /* [out] */ Float* spanX)
{
    VALIDATE_NOT_NULL(spanX);
    *spanX = mPrevSpanX;
    return NOERROR;
}

ECode ScaleGestureDetector::GetPreviousSpanY(
    /* [out] */ Float* spanY)
{
    VALIDATE_NOT_NULL(spanY);
    *spanY = mPrevSpanY;
    return NOERROR;
}

ECode ScaleGestureDetector::GetScaleFactor(
    /* [out] */ Float* factor)
{
    VALIDATE_NOT_NULL(factor);

    if (InDoubleTapMode()) {
        // Drag is moving up; the further away from the gesture
        // start, the smaller the span should be, the closer,
        // the larger the span, and therefore the larger the scale
        Boolean scaleUp =
                (mEventBeforeOrAboveStartingGestureEvent && (mCurrSpan < mPrevSpan)) ||
                (!mEventBeforeOrAboveStartingGestureEvent && (mCurrSpan > mPrevSpan));
        Float spanDiff = (Elastos::Core::Math::Abs(1 - (mCurrSpan / mPrevSpan)) * SCALE_FACTOR);
        *factor = mPrevSpan <= 0 ? 1 : scaleUp ? (1 + spanDiff) : (1 - spanDiff);
        return NOERROR;
    }

    *factor = mPrevSpan > 0 ? mCurrSpan / mPrevSpan : 1;
    return NOERROR;
}

ECode ScaleGestureDetector::GetTimeDelta(
    /* [out] */ Int64* delta)
{
    VALIDATE_NOT_NULL(delta);
    *delta = mCurrTime - mPrevTime;
    return NOERROR;
}

ECode ScaleGestureDetector::GetEventTime(
    /* [out] */ Int64* delta)
{
    VALIDATE_NOT_NULL(delta);
    *delta = mCurrTime;
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
