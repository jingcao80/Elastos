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
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/policy/impl/SystemGesturesPointerEventListener.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::EIID_IPointerEventListener;
using Elastos::Droid::View::IInputEvent;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//                  SystemGesturesPointerEventListener
//=====================================================================
CAR_INTERFACE_IMPL_2(SystemGesturesPointerEventListener, Object, ISystemGesturesPointerEventListener, IPointerEventListener)

const String SystemGesturesPointerEventListener::TAG("SystemGestures");
const Boolean SystemGesturesPointerEventListener::DEBUG = FALSE;
const Int64 SystemGesturesPointerEventListener::SWIPE_TIMEOUT_MS;
const Int32 SystemGesturesPointerEventListener::MAX_TRACKED_POINTERS;
const Int32 SystemGesturesPointerEventListener::UNTRACKED_POINTER;
const Int32 SystemGesturesPointerEventListener::SWIPE_NONE;
const Int32 SystemGesturesPointerEventListener::SWIPE_FROM_TOP;
const Int32 SystemGesturesPointerEventListener::SWIPE_FROM_BOTTOM;
const Int32 SystemGesturesPointerEventListener::SWIPE_FROM_RIGHT;

SystemGesturesPointerEventListener::SystemGesturesPointerEventListener()
{
    mDownPointerId = ArrayOf<Int32>::Alloc(MAX_TRACKED_POINTERS);
    mDownX = ArrayOf<Float>::Alloc(MAX_TRACKED_POINTERS);
    mDownY = ArrayOf<Float>::Alloc(MAX_TRACKED_POINTERS);
    mDownTime = ArrayOf<Int64>::Alloc(MAX_TRACKED_POINTERS);
}

ECode SystemGesturesPointerEventListener::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ISystemGesturesPointerEventListenerCallbacks* callbacks)
{
    if (callbacks == NULL)
    {
        Slogger::E(TAG, "callbacks must not be null!!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL)
    {
        Slogger::E(TAG, "context must not be null!!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCallbacks = callbacks;
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetDimensionPixelSize(R::dimen::status_bar_height, &mSwipeStartThreshold);
    mSwipeDistanceThreshold = mSwipeStartThreshold;
    if (DEBUG)
    {
        Slogger::D(TAG,  "mSwipeStartThreshold=%d, mSwipeDistanceThreshold=%d", mSwipeStartThreshold, mSwipeDistanceThreshold);
    }
    return NOERROR;
}

ECode SystemGesturesPointerEventListener::OnPointerEvent(
    /* [in] */ IMotionEvent* event)
{
    VALIDATE_NOT_NULL(event);
    Int32 actionMasked;
    event->GetActionMasked(&actionMasked);
    switch (actionMasked)
    {
        case IMotionEvent::ACTION_DOWN:
            mSwipeFireable = TRUE;
            mDebugFireable = TRUE;
            mDownPointers = 0;
            CaptureDown(event, 0);
            break;
        case IMotionEvent::ACTION_POINTER_DOWN:
        {
            Int32 actionIndex;
            event->GetActionIndex(&actionIndex);
            CaptureDown(event, actionIndex);
            if (mDebugFireable) {
                Int32 pointerCount;
                event->GetPointerCount(&pointerCount);
                mDebugFireable = (pointerCount < 5);
                if (!mDebugFireable) {
                    if (DEBUG) Slogger::D(TAG, "Firing debug");
                    mCallbacks->OnDebug();
                }
            }
            break;
        }
        case IMotionEvent::ACTION_MOVE:
            if (mSwipeFireable)
            {
                Int32 swipe = DetectSwipe(event);
                mSwipeFireable = (swipe == SWIPE_NONE);
                if (swipe == SWIPE_FROM_TOP)
                {
                    if (DEBUG) Slogger::D(TAG, "Firing onSwipeFromTop");
                    mCallbacks->OnSwipeFromTop();
                }
                else if (swipe == SWIPE_FROM_BOTTOM)
                {
                    if (DEBUG) Slogger::D(TAG, "Firing onSwipeFromBottom");
                    mCallbacks->OnSwipeFromBottom();
                }
                else if (swipe == SWIPE_FROM_RIGHT)
                {
                    if (DEBUG) Slogger::D(TAG, "Firing onSwipeFromRight");
                    mCallbacks->OnSwipeFromRight();
                }
            }
            break;
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mSwipeFireable = FALSE;
            mDebugFireable = FALSE;
            break;
        default:
            if (DEBUG) Slogger::D(TAG, "Ignoring event: ");// + event);
    }
    return NOERROR;
}

void SystemGesturesPointerEventListener::CaptureDown(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex)
{
    Int32 pointerId;
    event->GetPointerId(pointerIndex, &pointerId);
    Int32 i = FindIndex(pointerId);
    if (DEBUG)
        Slogger::D(TAG, "pointer %d down pointerIndex=%d trackingIndex=%d", pointerId, pointerIndex, i);
    if (i != UNTRACKED_POINTER)
    {
        Float x;
        event->GetX(pointerIndex, &x);
        (*mDownX)[i] = x;
        Float y;
        event->GetY(pointerIndex, &y);
        (*mDownY)[i] = y;
        Int64 eventTime;
        IInputEvent* inputEvent = IInputEvent::Probe(event);
        inputEvent->GetEventTime(&eventTime);
        (*mDownTime)[i] = eventTime;
        if (DEBUG)
            Slogger::D(TAG, "pointer %d down x=%f, y=%f", pointerId, (*mDownX)[i], (*mDownY)[i]);
    }
}

Int32 SystemGesturesPointerEventListener::FindIndex(
    /* [in] */ Int32 pointerId)
{
    for (Int32 i = 0; i < mDownPointers; ++i)
    {
        if ((*mDownPointerId)[i] == pointerId)
        {
            return i;
        }
    }
    if (mDownPointers == MAX_TRACKED_POINTERS || pointerId == IMotionEvent::INVALID_POINTER_ID)
    {
        return UNTRACKED_POINTER;
    }
    (*mDownPointerId)[mDownPointers++] = pointerId;
    return mDownPointers - 1;
}

Int32 SystemGesturesPointerEventListener::DetectSwipe(
    /* [in] */ IMotionEvent* move)
{
    Int32 historySize;
    move->GetHistorySize(&historySize);
    Int32 pointerCount;
    move->GetPointerCount(&pointerCount);
    for (Int32 p = 0; p < pointerCount; ++p)
    {
        Int32 pointerId;
        move->GetPointerId(p, &pointerId);
        Int32 i = FindIndex(pointerId);
        if (i != UNTRACKED_POINTER)
        {
            for (Int32 h = 0; h < historySize; ++h)
            {
                Int64 time;
                move->GetHistoricalEventTime(h, &time);
                Float x;
                move->GetHistoricalX(p, h, &x);
                Float y;
                move->GetHistoricalY(p, h, &y);
                Int32 swipe = DetectSwipe(i, time, x, y);
                if (swipe != SWIPE_NONE) {
                    return swipe;
                }
            }
            Int64 eventTime;
            IInputEvent* inputEvent = IInputEvent::Probe(move);
            inputEvent->GetEventTime(&eventTime);
            Float x;
            Float y;
            move->GetX(p, &x);
            move->GetY(p, &y);
            Int32 swipe = DetectSwipe(i, eventTime, x, y);
            if (swipe != SWIPE_NONE)
            {
                return swipe;
            }
        }
    }
    return SWIPE_NONE;
}

Int32 SystemGesturesPointerEventListener::DetectSwipe(
    /* [in] */ Int32 i,
    /* [in] */ Int64 time,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Float fromX = (*mDownX)[i];
    Float fromY = (*mDownY)[i];
    Int64 elapsed = time - (*mDownTime)[i];
    if (DEBUG)
        Slogger::D(TAG, "pointer %d moved ( %f -> %f, %f->%f) in %lld", (*mDownPointerId)[i], fromX, x, fromY, y, elapsed);

    if (fromY <= mSwipeStartThreshold
            && y > fromY + mSwipeDistanceThreshold
            && elapsed < SWIPE_TIMEOUT_MS)
    {
        return SWIPE_FROM_TOP;
    }
    if (fromY >= screenHeight - mSwipeStartThreshold
            && y < fromY - mSwipeDistanceThreshold
            && elapsed < SWIPE_TIMEOUT_MS)
    {
        return SWIPE_FROM_BOTTOM;
    }
    if (fromX >= screenWidth - mSwipeStartThreshold
            && x < fromX - mSwipeDistanceThreshold
            && elapsed < SWIPE_TIMEOUT_MS)
    {
        return SWIPE_FROM_RIGHT;
    }
    return SWIPE_NONE;
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

