
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/GestureDetector.h"
#include "elastos/droid/view/InputEventConsistencyVerifier.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include "elastos/droid/view/VelocityTracker.h"
#include "elastos/droid/view/MotionEvent.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Os::ILooper;

namespace Elastos {
namespace Droid {
namespace View {

Int32 GestureDetector::LONGPRESS_TIMEOUT =  ViewConfiguration::GetLongPressTimeout();
Int32 GestureDetector::TAP_TIMEOUT = ViewConfiguration::GetTapTimeout();
Int32 GestureDetector::DOUBLE_TAP_TIMEOUT = ViewConfiguration::GetDoubleTapTimeout();
Int32 GestureDetector::DOUBLE_TAP_MIN_TIME = ViewConfiguration::GetDoubleTapMinTime();

const Int32 GestureDetector::SHOW_PRESS = 1;
const Int32 GestureDetector::LONG_PRESS = 2;
const Int32 GestureDetector::TAP = 3;

/*GestureDetector::SimpleOnGestureListener*/
CAR_INTERFACE_IMPL_2(GestureDetector::SimpleOnGestureListener, Object,
        IGestureDetectorOnGestureListener, IGestureDetectorOnDoubleTapListener);

GestureDetector::SimpleOnGestureListener::SimpleOnGestureListener()
{}

GestureDetector::SimpleOnGestureListener::~SimpleOnGestureListener()
{}

ECode GestureDetector::SimpleOnGestureListener::constructor()
{
    return NOERROR;
}

ECode GestureDetector::SimpleOnGestureListener::OnSingleTapUp(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode GestureDetector::SimpleOnGestureListener::OnLongPress(
    /* [in] */ IMotionEvent* e)
{
    return NOERROR;
}

ECode GestureDetector::SimpleOnGestureListener::OnScroll(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float distanceX,
    /* [in] */ Float distanceY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode GestureDetector::SimpleOnGestureListener::OnFling(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode GestureDetector::SimpleOnGestureListener::OnShowPress(
    /* [in] */ IMotionEvent* e)
{
    return NOERROR;
}

ECode GestureDetector::SimpleOnGestureListener::OnDown(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode GestureDetector::SimpleOnGestureListener::OnDoubleTap(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode GestureDetector::SimpleOnGestureListener::OnDoubleTapEvent(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode GestureDetector::SimpleOnGestureListener::OnSingleTapConfirmed(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

/* GestureDetector::GestureHandler */
ECode GestureDetector::GestureHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case GestureDetector::SHOW_PRESS:
            mHost->mListener->OnShowPress(
                mHost->mCurrentDownEvent);
            break;

        case GestureDetector::LONG_PRESS:
            mHost->DispatchLongPress();
            break;

        case GestureDetector::TAP:
            // If the user's finger is still down, do not count it as a tap
            if (mHost->mDoubleTapListener != NULL) {
                if (!mHost->mStillDown) {
                    Boolean tmp = FALSE;
                    mHost->mDoubleTapListener->OnSingleTapConfirmed(
                        mHost->mCurrentDownEvent, &tmp);
                }
                else {
                    mHost->mDeferConfirmSingleTap = TRUE;
                }
            }
            break;

        default:
            return E_RUNTIME_EXCEPTION;
            // throw new RuntimeException("Unknown message " + msg); //never
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(GestureDetector, Object, IGestureDetector);

GestureDetector::GestureDetector()
    : mTouchSlopSquare(0)
    , mDoubleTapTouchSlopSquare(0)
    , mDoubleTapSlopSquare(0)
    , mMinimumFlingVelocity(0)
    , mMaximumFlingVelocity(0)
    , mStillDown(FALSE)
    , mDeferConfirmSingleTap(FALSE)
    , mInLongPress(FALSE)
    , mAlwaysInTapRegion(FALSE)
    , mAlwaysInBiggerTapRegion(FALSE)
    , mIsDoubleTapping(FALSE)
    , mLastFocusX(0.0)
    , mLastFocusY(0.0)
    , mDownFocusX(0.0)
    , mDownFocusY(0.0)
    , mIsLongpressEnabled(FALSE)
{
}

GestureDetector::~GestureDetector()
{}

ECode GestureDetector::constructor(
    /* [in] */ IGestureDetectorOnGestureListener* listener,
    /* [in] */ IHandler* handler)
{
    return constructor(NULL, listener, handler);
}

ECode GestureDetector::constructor(
    /* [in] */ IGestureDetectorOnGestureListener* listener)
{
    return constructor(NULL, listener, NULL);
}

ECode GestureDetector::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IGestureDetectorOnGestureListener* listener)
{
    return constructor(context, listener, NULL);
}

ECode GestureDetector::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IGestureDetectorOnGestureListener* listener,
    /* [in] */ IHandler* handler,
    /* [in] */ Boolean unused)
{
    return constructor(context, listener, handler);
}

ECode GestureDetector::constructor(
   /* [in] */ IContext* context,
   /* [in] */ IGestureDetectorOnGestureListener* listener,
   /* [in] */ IHandler* handler)
{
    mInputEventConsistencyVerifier =
            InputEventConsistencyVerifier::IsInstrumentationEnabled() ?
                    new InputEventConsistencyVerifier(TO_IINTERFACE(this), 0) : NULL;

    if (handler != NULL) {
        AutoPtr<ILooper> looper;
        handler->GetLooper((ILooper**)&looper);
        mHandler = new GestureHandler(looper, this);
        mHandler->constructor();
    }
    else {
        mHandler = new GestureHandler(this);
        mHandler->constructor();
    }
    mListener = listener;
    if (IGestureDetectorOnDoubleTapListener::Probe(listener)) {
        SetOnDoubleTapListener(IGestureDetectorOnDoubleTapListener::Probe(listener));
    }

    if (mListener == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mIsLongpressEnabled = TRUE;

    // Fallback to support pre-donuts releases
    Int32 touchSlop, doubleTapSlop, doubleTapTouchSlop;
    if (context == NULL) {
        //noinspection deprecation
        touchSlop = ViewConfiguration::GetTouchSlop();
        doubleTapTouchSlop = touchSlop; // Hack rather than adding a hiden method for this
        doubleTapSlop = ViewConfiguration::GetDoubleTapSlop();
        //noinspection deprecation
        mMinimumFlingVelocity = ViewConfiguration::GetMinimumFlingVelocity();
        mMaximumFlingVelocity = ViewConfiguration::GetMaximumFlingVelocity();
    }
    else {
        AutoPtr<IViewConfiguration> configuration = ViewConfiguration::Get(context);
        configuration->GetScaledTouchSlop(&touchSlop);
        configuration->GetScaledDoubleTapTouchSlop(&doubleTapTouchSlop);
        configuration->GetScaledDoubleTapSlop(&doubleTapSlop);
        configuration->GetScaledMinimumFlingVelocity(&mMinimumFlingVelocity);
        configuration->GetScaledMaximumFlingVelocity(&mMaximumFlingVelocity);
    }
    mTouchSlopSquare = touchSlop * touchSlop;
    mDoubleTapTouchSlopSquare = doubleTapTouchSlop * doubleTapTouchSlop;
    mDoubleTapSlopSquare = doubleTapSlop * doubleTapSlop;
    return NOERROR;
}

ECode GestureDetector::SetOnDoubleTapListener(
    /* [in] */ IGestureDetectorOnDoubleTapListener* onDoubleTapListener)
{
    mDoubleTapListener = onDoubleTapListener;
    return NOERROR;
}

ECode GestureDetector::SetIsLongpressEnabled(
    /* [in] */ Boolean isLongpressEnabled)
{
    mIsLongpressEnabled = isLongpressEnabled;
    return NOERROR;
}

ECode GestureDetector::IsLongpressEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsLongpressEnabled;
    return NOERROR;
}

ECode GestureDetector::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mInputEventConsistencyVerifier != NULL) {
        mInputEventConsistencyVerifier->OnTouchEvent(ev, 0);
    }

    Int32 action = 0;
    ev->GetAction(&action);

    if (mVelocityTracker == NULL) {
        mVelocityTracker = VelocityTracker::Obtain();
    }

    mVelocityTracker->AddMovement(ev);
    const Boolean pointerUp = (action & IMotionEvent::ACTION_MASK) == IMotionEvent::ACTION_POINTER_UP;
    Int32 skipIndex = -1;
    if (pointerUp) {
        ev->GetActionIndex(&skipIndex);
    }

    // Determine focal point
    Float sumX = 0.f, sumY = 0.f;
    Int32 count = 0;
    ev->GetPointerCount(&count);
    for (Int32 i = 0; i < count; i++) {
        if (skipIndex == i) continue;

        Float tmp = 0.f;
        sumX += (ev->GetX(i, &tmp), tmp);
        sumY += (ev->GetY(i, &tmp), tmp);
    }

    const Int32 div = pointerUp ? count - 1 : count;
    const Float focusX = sumX / div;
    const Float focusY = sumY / div;

    Boolean handled = FALSE;

    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_POINTER_DOWN: {
            mDownFocusX = mLastFocusX = focusX;
            mDownFocusY = mLastFocusY = focusY;
            // Cancel long press and taps
            CancelTaps();
            break;
        }

        case IMotionEvent::ACTION_POINTER_UP: {
            mDownFocusX = mLastFocusX = focusX;
            mDownFocusY = mLastFocusY = focusY;

            // Check the dot product of current velocities.
            // If the pointer that left was opposing another velocity vector, clear.
            mVelocityTracker->ComputeCurrentVelocity(1000, mMaximumFlingVelocity);
            Int32 upIndex = 0;
            ev->GetActionIndex(&upIndex);

            Int32 id1 = 0;
            ev->GetPointerId(upIndex, &id1);
            Float x1;
            mVelocityTracker->GetXVelocity(id1, &x1);
            Float y1;
            mVelocityTracker->GetYVelocity(id1, &y1);
            for (Int32 i = 0; i < count; i++) {
                if (i == upIndex) continue;

                Int32 id2 = 0;
                ev->GetPointerId(i, &id2);
                Float x;
                mVelocityTracker->GetXVelocity(id2, &x);
                x *= x1;
                Float y;
                mVelocityTracker->GetYVelocity(id2, &y);
                y *= y1;

                const Float dot = x + y;
                if (dot < 0) {
                    mVelocityTracker->Clear();
                    break;
                }
            }
            break;
        }

        case IMotionEvent::ACTION_DOWN: {

            if (mDoubleTapListener != NULL) {
                Boolean hadTapMessage = FALSE;
                mHandler->HasMessages(TAP, &hadTapMessage);
                if (hadTapMessage)
                    mHandler->RemoveMessages(TAP);

                if ((mCurrentDownEvent != NULL) && (mPreviousUpEvent != NULL) && hadTapMessage &&
                       IsConsideredDoubleTap(mCurrentDownEvent, mPreviousUpEvent, ev)) {
                    // This is a second tap
                    mIsDoubleTapping = TRUE;
                    // Give a callback with the first tap of the double-tap
                    Boolean dtap = FALSE;
                    mDoubleTapListener->OnDoubleTap(mCurrentDownEvent, &dtap);
                    handled |= dtap;
                    // Give a callback with down event of the double-tap
                    mDoubleTapListener->OnDoubleTapEvent(ev, &dtap);
                    handled |= dtap;
                }
                else {
                    // This is a first tap
                    AutoPtr<IParcel> params;
                    CCallbackParcel::New((IParcel**)&params);
                    params->WriteInt32(TAP);

                    Boolean result = FALSE;
                    mHandler->SendEmptyMessageDelayed(TAP, DOUBLE_TAP_TIMEOUT, &result);
                }
            }

            mDownFocusX = mLastFocusX = focusX;
            mDownFocusY = mLastFocusY = focusY;
            if (mCurrentDownEvent != NULL) {
                IInputEvent::Probe(mCurrentDownEvent)->Recycle();
                mCurrentDownEvent = NULL;
            }

            MotionEvent::Obtain(ev, (IMotionEvent**)&mCurrentDownEvent);
            mAlwaysInTapRegion = TRUE;
            mAlwaysInBiggerTapRegion = TRUE;
            mStillDown = TRUE;
            mInLongPress = FALSE;
            mDeferConfirmSingleTap = FALSE;

            Int64 dt = 0;
            mCurrentDownEvent->GetDownTime(&dt);
            Boolean result = FALSE;
            if (mIsLongpressEnabled) {
                mHandler->RemoveMessages(LONG_PRESS);
                mHandler->SendEmptyMessageAtTime(LONG_PRESS,
                    dt + TAP_TIMEOUT + LONGPRESS_TIMEOUT, &result);
            }

            mHandler->SendEmptyMessageAtTime(SHOW_PRESS, dt + TAP_TIMEOUT, &result);

            mListener->OnDown(ev, &result);
            handled |= result;
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            if (mInLongPress) {
                break;
            }
            const Float scrollX = mLastFocusX - focusX;
            const Float scrollY = mLastFocusY - focusY;
            if (mIsDoubleTapping) {
                // Give the move events of the double-tap
                Boolean dtap = FALSE;
                handled |= (mDoubleTapListener->OnDoubleTapEvent(ev, &dtap), dtap);
            }
            else if (mAlwaysInTapRegion) {
                const Int32 deltaX = (Int32) (focusX - mDownFocusX);
                const Int32 deltaY = (Int32) (focusY - mDownFocusY);
                Int32 distance = (deltaX * deltaX) + (deltaY * deltaY);
                if (distance > mTouchSlopSquare) {
                    mListener->OnScroll(mCurrentDownEvent, ev, scrollX, scrollY, &handled);
                    mLastFocusX = focusX;
                    mLastFocusY = focusY;
                    mAlwaysInTapRegion = FALSE;
                    mHandler->RemoveMessages(TAP);
                    mHandler->RemoveMessages(SHOW_PRESS);
                    mHandler->RemoveMessages(LONG_PRESS);
                }
                if (distance > mDoubleTapTouchSlopSquare) {
                    mAlwaysInBiggerTapRegion = FALSE;
                }
            }
            else if ((Elastos::Core::Math::Abs(scrollX) >= 1) || (Elastos::Core::Math::Abs(scrollY) >= 1)) {
                mListener->OnScroll(mCurrentDownEvent, ev, scrollX, scrollY, &handled);
                mLastFocusX = focusX;
                mLastFocusY = focusY;
            }
            break;
        }

        case IMotionEvent::ACTION_UP: {
            mStillDown = FALSE;
            AutoPtr<IMotionEvent> currentUpEvent;
            MotionEvent::Obtain(ev, (IMotionEvent**)&currentUpEvent);
            if (mIsDoubleTapping) {
                // Finally, give the up event of the double-tap
                Boolean dtap = FALSE;
                handled |= (mDoubleTapListener->OnDoubleTapEvent(ev, &dtap), dtap);
            }
            else if (mInLongPress) {
                mHandler->RemoveMessages(TAP);
                mInLongPress = FALSE;
            }
            else if (mAlwaysInTapRegion) {
                mListener->OnSingleTapUp(ev, &handled);
                if (mDeferConfirmSingleTap && mDoubleTapListener != NULL) {
                    Boolean tmp = FALSE;
                    mDoubleTapListener->OnSingleTapConfirmed(ev, &tmp);
                }
            }
            else {
                // A fling must travel the minimum tap distance
                AutoPtr<IVelocityTracker> velocityTracker = mVelocityTracker;
                Int32 pointerId = 0;
                ev->GetPointerId(0, &pointerId);
                velocityTracker->ComputeCurrentVelocity(1000, mMaximumFlingVelocity);
                Float velocityY;
                velocityTracker->GetYVelocity(pointerId, &velocityY);
                Float velocityX;
                velocityTracker->GetXVelocity(pointerId, &velocityX);

                if ((Elastos::Core::Math::Abs(velocityY) > mMinimumFlingVelocity)
                        || (Elastos::Core::Math::Abs(velocityX) > mMinimumFlingVelocity)){
                    mListener->OnFling(mCurrentDownEvent, ev, velocityX, velocityY, &handled);
                }
            }
            if (mPreviousUpEvent != NULL) {
                IInputEvent::Probe(mPreviousUpEvent)->Recycle();
            }
            // Hold the event we obtained above - listeners may have changed the original.
            mPreviousUpEvent = currentUpEvent;
            if (mVelocityTracker != NULL) {
                // This may have been cleared when we called out to the
                // application above.
                mVelocityTracker->Recycle();
                mVelocityTracker = NULL;
            }
            mIsDoubleTapping = FALSE;
            mDeferConfirmSingleTap = FALSE;
            mHandler->RemoveMessages(SHOW_PRESS);
            mHandler->RemoveMessages(LONG_PRESS);
            break;
        }

        case IMotionEvent::ACTION_CANCEL: {
            Cancel();
            break;
        }
    }

    if (!handled && mInputEventConsistencyVerifier != NULL) {
       mInputEventConsistencyVerifier->OnUnhandledEvent(IInputEvent::Probe(ev), 0);
    }

    *result =  handled;
    return NOERROR;
}

void GestureDetector::Cancel()
{
    mHandler->RemoveMessages(SHOW_PRESS);
    mHandler->RemoveMessages(LONG_PRESS);
    mHandler->RemoveMessages(TAP);
    mVelocityTracker->Recycle();
    mVelocityTracker = NULL;
    mIsDoubleTapping = FALSE;
    mStillDown = FALSE;
    mAlwaysInTapRegion = FALSE;
    mAlwaysInBiggerTapRegion = FALSE;
    mDeferConfirmSingleTap = FALSE;
    if (mInLongPress) {
        mInLongPress = FALSE;
    }
}

void GestureDetector::CancelTaps()
{
    mHandler->RemoveMessages(SHOW_PRESS);
    mHandler->RemoveMessages(LONG_PRESS);
    mHandler->RemoveMessages(TAP);
    mIsDoubleTapping = FALSE;
    mAlwaysInTapRegion = FALSE;
    mAlwaysInBiggerTapRegion = FALSE;
    mDeferConfirmSingleTap = FALSE;
    if (mInLongPress) {
        mInLongPress = FALSE;
    }
}

Boolean GestureDetector::IsConsideredDoubleTap(
    /* [in] */ IMotionEvent* firstDown,
    /* [in] */ IMotionEvent* firstUp,
    /* [in] */ IMotionEvent* secondDown)
{
    if (!mAlwaysInBiggerTapRegion) {
        return FALSE;
    }

    assert(secondDown != NULL && firstUp != NULL);

    Int64 et1 = 0, et2 = 0;
    IInputEvent::Probe(secondDown)->GetEventTime(&et1);
    IInputEvent::Probe(firstUp)->GetEventTime(&et2);
    Int64 deltaTime = et1 - et2;
    if (deltaTime > DOUBLE_TAP_TIMEOUT || deltaTime < DOUBLE_TAP_MIN_TIME) {
        return FALSE;
    }

    Float v1 = 0.f, v2 = 0.f;
    Int32 deltaX = (Int32) (firstDown->GetX(&v1), v1) - (Int32) (secondDown->GetX(&v2), v2);
    Int32 deltaY = (Int32) (firstDown->GetY(&v1), v1) - (Int32) (secondDown->GetY(&v2), v2);
    return (deltaX * deltaX + deltaY * deltaY < mDoubleTapSlopSquare);
}

void GestureDetector::DispatchLongPress()
{
    mHandler->RemoveMessages(TAP);
    mDeferConfirmSingleTap = FALSE;
    mInLongPress = TRUE;
    mListener->OnLongPress(mCurrentDownEvent);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
