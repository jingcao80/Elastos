
#ifndef __ELASTOS_DROID_VIEW_GESTUREDETECTOR_H__
#define __ELASTOS_DROID_VIEW_GESTUREDETECTOR_H__

#include "Elastos.Droid.View.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace View {

class ECO_PUBLIC GestureDetector
    : public Object
    , public IGestureDetector
{
public:
    class SimpleOnGestureListener
        : public Object
        , public IGestureDetectorOnGestureListener
        , public IGestureDetectorOnDoubleTapListener
    {
    public:
        CAR_INTERFACE_DECL()

        SimpleOnGestureListener();

        ~SimpleOnGestureListener();

        CARAPI constructor();

        CARAPI OnSingleTapUp(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        CARAPI OnLongPress(
            /* [in] */ IMotionEvent* e);

        CARAPI OnScroll(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float distanceX,
            /* [in] */ Float distanceY,
            /* [out] */ Boolean* res);

        CARAPI OnFling(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float velocityX,
            /* [in] */ Float velocityY,
            /* [out] */ Boolean* res);

        CARAPI OnShowPress(
            /* [in] */ IMotionEvent* e);

        CARAPI OnDown(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        CARAPI OnDoubleTap(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        CARAPI OnDoubleTapEvent(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        CARAPI OnSingleTapConfirmed(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);
    };

private:
    class ECO_LOCAL GestureHandler : public Handler
    {
    public:
        TO_STRING_IMPL("GestureDetector::GestureHandler")

        GestureHandler(
            /* [in] */ GestureDetector* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        GestureDetector* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    GestureDetector();

    ~GestureDetector();

    /**
     * Creates a GestureDetector with the supplied listener.
     * This variant of the constructor should be used from a non-UI thread
     * (as it allows specifying the Handler).
     *
     * @param listener the listener invoked for all the callbacks, this must
     * not be null.
     * @param handler the handler to use
     *
     * @throws NullPointerException if either {@code listener} or
     * {@code handler} is null.
     *
     * @deprecated Use {@link #GestureDetector(android.content.Context,
     *      android.view.GestureDetector.OnGestureListener, android.os.Handler)} instead.
     */
    //@Deprecated
    CARAPI constructor(
        /* [in] */ IGestureDetectorOnGestureListener* listener,
        /* [in] */ IHandler* handler);

    /**
     * Creates a GestureDetector with the supplied listener.
     * You may only use this constructor from a UI thread (this is the usual situation).
     * @see android.os.Handler#Handler()
     *
     * @param listener the listener invoked for all the callbacks, this must
     * not be null.
     *
     * @throws NullPointerException if {@code listener} is null.
     *
     * @deprecated Use {@link #GestureDetector(android.content.Context,
     *      android.view.GestureDetector.OnGestureListener)} instead.
     */
    //@Deprecated
    CARAPI constructor(
        /* [in] */ IGestureDetectorOnGestureListener* listener);

    /**
     * Creates a GestureDetector with the supplied listener.
     * You may only use this constructor from a {@link android.os.Looper} thread.
     * @see android.os.Handler#Handler()
     *
     * @param context the application's context
     * @param listener the listener invoked for all the callbacks, this must
     * not be null.
     *
     * @throws NullPointerException if {@code listener} is null.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IGestureDetectorOnGestureListener* listener);

    /**
     * Creates a GestureDetector with the supplied listener that runs deferred events on the
     * thread associated with the supplied {@link android.os.Handler}.
     * @see android.os.Handler#Handler()
     *
     * @param context the application's context
     * @param listener the listener invoked for all the callbacks, this must
     * not be null.
     * @param handler the handler to use for running deferred listener events.
     *
     * @throws NullPointerException if {@code listener} is null.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IGestureDetectorOnGestureListener* listener,
        /* [in] */ IHandler* handler);

    /**
     * Creates a GestureDetector with the supplied listener that runs deferred events on the
     * thread associated with the supplied {@link android.os.Handler}.
     * @see android.os.Handler#Handler()
     *
     * @param context the application's context
     * @param listener the listener invoked for all the callbacks, this must
     * not be null.
     * @param handler the handler to use for running deferred listener events.
     * @param unused currently not used.
     *
     * @throws NullPointerException if {@code listener} is null.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IGestureDetectorOnGestureListener* listener,
        /* [in] */ IHandler* handler,
        /* [in] */ Boolean unused);

    /**
     * Sets the listener which will be called for double-tap and related
     * gestures.
     *
     * @param onDoubleTapListener the listener invoked for all the callbacks, or
     *        null to stop listening for double-tap gestures.
     */
    CARAPI SetOnDoubleTapListener(
        /* [in] */ IGestureDetectorOnDoubleTapListener* onDoubleTapListener);

    /**
     * Set whether longpress is enabled, if this is enabled when a user
     * presses and holds down you get a longpress event and nothing further.
     * If it's disabled the user can press and hold down and then later
     * moved their finger and you will get scroll events. By default
     * longpress is enabled.
     *
     * @param isLongpressEnabled whether longpress should be enabled.
     */
    CARAPI SetIsLongpressEnabled(
        /* [in] */ Boolean isLongpressEnabled);

    /**
     * @return true if longpress is enabled, else false.
     */
    CARAPI IsLongpressEnabled(
        /* [out] */ Boolean* result);

    /**
     * Analyzes the given motion event and if applicable triggers the
     * appropriate callbacks on the {@link OnGestureListener} supplied.
     *
     * @param ev The current motion event.
     * @return true if the {@link OnGestureListener} consumed the event,
     *              else false.
     */
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

private:
    ECO_LOCAL CARAPI_(void) Cancel();

    ECO_LOCAL CARAPI_(void) CancelTaps();

    ECO_LOCAL CARAPI_(Boolean) IsConsideredDoubleTap(
        /* [in] */ IMotionEvent* firstDown,
        /* [in] */ IMotionEvent* firstUp,
        /* [in] */ IMotionEvent* secondDown);

    ECO_LOCAL CARAPI_(void) DispatchLongPress();

private:
    Int32 mTouchSlopSquare;
    Int32 mDoubleTapTouchSlopSquare;
    Int32 mDoubleTapSlopSquare;
    Int32 mMinimumFlingVelocity;
    Int32 mMaximumFlingVelocity;

    ECO_LOCAL static Int32 LONGPRESS_TIMEOUT;
    ECO_LOCAL static Int32 TAP_TIMEOUT;
    ECO_LOCAL static Int32 DOUBLE_TAP_TIMEOUT;
    ECO_LOCAL static Int32 DOUBLE_TAP_MIN_TIME;

    // constants for Message.what used by GestureHandler below
    static const Int32 SHOW_PRESS;
    static const Int32 LONG_PRESS;
    static const Int32 TAP;

    AutoPtr<GestureHandler> mHandler;
    IGestureDetectorOnGestureListener* mListener;
    AutoPtr<IGestureDetectorOnDoubleTapListener> mDoubleTapListener;

    Boolean mStillDown;
    Boolean mDeferConfirmSingleTap;
    Boolean mInLongPress;
    Boolean mAlwaysInTapRegion;
    Boolean mAlwaysInBiggerTapRegion;

    AutoPtr<IMotionEvent> mCurrentDownEvent;
    AutoPtr<IMotionEvent> mPreviousUpEvent;

    /**
     * True when the user is still touching for the second tap (down, move, and
     * up events). Can only be true if there is a double tap listener attached.
     */
    Boolean mIsDoubleTapping;

    Float mLastFocusX;
    Float mLastFocusY;
    Float mDownFocusX;
    Float mDownFocusY;

    Boolean mIsLongpressEnabled;

    /**
     * Determines speed during touch scrolling
     */
    AutoPtr<IVelocityTracker> mVelocityTracker;

    /**
     * Consistency verifier for debugging purposes.
     */
    AutoPtr<IInputEventConsistencyVerifier> mInputEventConsistencyVerifier;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_GESTUREDETECTOR_H__
