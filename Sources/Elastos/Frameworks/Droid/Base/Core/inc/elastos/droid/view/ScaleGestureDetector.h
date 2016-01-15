#ifndef __ELASTOS_DROID_VIEW_SCALEGESTUREDETECTOR_H__
#define __ELASTOS_DROID_VIEW_SCALEGESTUREDETECTOR_H__

#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/view/GestureDetector.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::GestureDetector;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * Detects scaling transformation gestures using the supplied {@link MotionEvent}s.
 * The {@link OnScaleGestureListener} callback will notify users when a particular
 * gesture event has occurred.
 *
 * This class should only be used with {@link MotionEvent}s reported via touch.
 *
 * To use this class:
 * <ul>
 *  <li>Create an instance of the {@code ScaleGestureDetector} for your
 *      {@link View}
 *  <li>In the {@link View#onTouchEvent(MotionEvent)} method ensure you call
 *          {@link #onTouchEvent(MotionEvent)}. The methods defined in your
 *          callback will be executed when the events occur.
 * </ul>
 */
class ECO_PUBLIC ScaleGestureDetector
    : public Object
    , public IScaleGestureDetector
{
public:
    class SimpleOnScaleGestureListener
        : public Object
        , public IScaleGestureDetectorOnScaleGestureListener
    {
    public:
        CAR_INTERFACE_DECL();

        SimpleOnScaleGestureListener();

        ~SimpleOnScaleGestureListener();

        CARAPI constructor();

        CARAPI OnScale(
                /* [in ]*/ IScaleGestureDetector* detector,
                /* [out] */ Boolean* res);

        CARAPI OnScaleBegin(
            /* [in ]*/ IScaleGestureDetector* detector,
            /* [out] */ Boolean* res);

        CARAPI OnScaleEnd(
            /* [in ]*/ IScaleGestureDetector* detector);
    };

    class MySimpleOnGestureListener
        : public GestureDetector::SimpleOnGestureListener
    {
    public:
        MySimpleOnGestureListener(
            /* [in] */ ScaleGestureDetector* host);

        ~MySimpleOnGestureListener();

        CARAPI constructor();

        CARAPI OnDoubleTap(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);
    private:
        ScaleGestureDetector* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    ScaleGestureDetector();

    ~ScaleGestureDetector();

    /**
     * Creates a ScaleGestureDetector with the supplied listener.
     * You may only use this constructor from a {@link android.os.Looper Looper} thread.
     *
     * @param context the application's context
     * @param listener the listener invoked for all the callbacks, this must
     * not be null.
     *
     * @throws NullPointerException if {@code listener} is null.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IScaleGestureDetectorOnScaleGestureListener* listener);

    /**
     * Creates a ScaleGestureDetector with the supplied listener.
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
        /* [in] */ IScaleGestureDetectorOnScaleGestureListener* listener,
        /* [in] */ IHandler* handler);

    /**
     * Accepts MotionEvents and dispatches events to a {@link OnScaleGestureListener}
     * when appropriate.
     *
     * <p>Applications should pass a complete and consistent event stream to this method.
     * A complete and consistent event stream involves all MotionEvents from the initial
     * ACTION_DOWN to the final ACTION_UP or ACTION_CANCEL.</p>
     *
     * @param event The event to process
     * @return true if the event was processed and the detector wants to receive the
     *         rest of the MotionEvents in this event stream.
     */
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Set whether the associated {@link OnScaleGestureListener} should receive onScale callbacks
     * when the user performs a doubleTap followed by a swipe. Note that this is enabled by default
     * if the app targets API 19 and newer.
     * @param scales true to enable quick scaling, false to disable
     */
    CARAPI SetQuickScaleEnabled(
        /* [in] */ Boolean scales);

    /**
     * Return whether the quick scale gesture, in which the user performs a double tap followed by a
     * swipe, should perform scaling. {@see #setQuickScaleEnabled(boolean)}.
     */
    CARAPI IsQuickScaleEnabled(
        /* [out] */ Boolean* result);

    /**
     * Returns {@code true} if a scale gesture is in progress.
     */
    CARAPI IsInProgress(
        /* [out] */ Boolean* result);

    /**
     * Get the X coordinate of the current gesture's focal point.
     * If a gesture is in progress, the focal point is between
     * each of the pointers forming the gesture.
     *
     * If {@link #isInProgress()} would return false, the result of this
     * function is undefined.
     *
     * @return X coordinate of the focal point in pixels.
     */
    CARAPI GetFocusX(
        /* [out] */ Float* x);

    /**
     * Get the Y coordinate of the current gesture's focal point.
     * If a gesture is in progress, the focal point is between
     * each of the pointers forming the gesture.
     *
     * If {@link #isInProgress()} would return false, the result of this
     * function is undefined.
     *
     * @return Y coordinate of the focal point in pixels.
     */
    CARAPI GetFocusY(
        /* [out] */ Float* y);

    /**
     * Return the average distance between each of the pointers forming the
     * gesture in progress through the focal point.
     *
     * @return Distance between pointers in pixels.
     */
    CARAPI GetCurrentSpan(
        /* [out] */ Float* span);

    /**
     * Return the average X distance between each of the pointers forming the
     * gesture in progress through the focal point.
     *
     * @return Distance between pointers in pixels.
     */
    CARAPI GetCurrentSpanX(
        /* [out] */ Float* spanX);

    /**
     * Return the average Y distance between each of the pointers forming the
     * gesture in progress through the focal point.
     *
     * @return Distance between pointers in pixels.
     */
    CARAPI GetCurrentSpanY(
        /* [out] */ Float* spanY);

    /**
     * Return the previous average distance between each of the pointers forming the
     * gesture in progress through the focal point.
     *
     * @return Previous distance between pointers in pixels.
     */
    CARAPI GetPreviousSpan(
        /* [out] */ Float* span);

    /**
     * Return the previous average X distance between each of the pointers forming the
     * gesture in progress through the focal point.
     *
     * @return Previous distance between pointers in pixels.
     */
    CARAPI GetPreviousSpanX(
        /* [out] */ Float* spanX);

    /**
     * Return the previous average Y distance between each of the pointers forming the
     * gesture in progress through the focal point.
     *
     * @return Previous distance between pointers in pixels.
     */
    CARAPI GetPreviousSpanY(
        /* [out] */ Float* spanY);

    /**
     * Return the scaling factor from the previous scale event to the current
     * event. This value is defined as
     * ({@link #getCurrentSpan()} / {@link #getPreviousSpan()}).
     *
     * @return The current scaling factor.
     */
    CARAPI GetScaleFactor(
        /* [out] */ Float* factor);

    /**
     * Return the time difference in milliseconds between the previous
     * accepted scaling event and the current scaling event.
     *
     * @return Time difference since the last scaling event in milliseconds.
     */
    CARAPI GetTimeDelta(
        /* [out] */ Int64* delta);

    /**
     * Return the event time of the current event being processed.
     *
     * @return Current event time in milliseconds.
     */
    CARAPI GetEventTime(
        /* [out] */ Int64* delta);

private:
    /**
     * The touchMajor/touchMinor elements of a MotionEvent can flutter/jitter on
     * some hardware/driver combos. Smooth it out to get kinder, gentler behavior.
     * @param ev MotionEvent to add to the ongoing history
     */
    ECO_LOCAL CARAPI_(void) AddTouchHistory(
        /* [in] */ IMotionEvent* ev);

    /**
     * Clear all touch history tracking. Useful in ACTION_CANCEL or ACTION_UP.
     * @see #addTouchHistory(MotionEvent)
     */
    ECO_LOCAL CARAPI_(void) ClearTouchHistory();

    ECO_LOCAL CARAPI_(Boolean) InDoubleTapMode();

private:
    ECO_LOCAL static const String TAG;

    ECO_LOCAL static const Int64 TOUCH_STABILIZE_TIME; // ms
    ECO_LOCAL static const Int32 DOUBLE_TAP_MODE_NONE;
    ECO_LOCAL static const Int32 DOUBLE_TAP_MODE_IN_PROGRESS;
    ECO_LOCAL static const Float SCALE_FACTOR;

    AutoPtr<IContext> mContext;
    AutoPtr<IScaleGestureDetectorOnScaleGestureListener> mListener;

    Float mFocusX;
    Float mFocusY;

    Boolean mQuickScaleEnabled;

    Float mCurrSpan;
    Float mPrevSpan;
    Float mInitialSpan;
    Float mCurrSpanX;
    Float mCurrSpanY;
    Float mPrevSpanX;
    Float mPrevSpanY;
    Int64 mCurrTime;
    Int64 mPrevTime;
    Boolean mInProgress;
    Int32 mSpanSlop;
    Int32 mMinSpan;

    // Bounds for recently seen values
    Float mTouchUpper;
    Float mTouchLower;
    Float mTouchHistoryLastAccepted;
    Int32 mTouchHistoryDirection;
    Int64 mTouchHistoryLastAcceptedTime;
    Int32 mTouchMinMajor;
    AutoPtr<IMotionEvent> mDoubleTapEvent;
    Int32 mDoubleTapMode;
    AutoPtr<IHandler> mHandler;

    /**
     * Consistency verifier for debugging purposes.
     */
    AutoPtr<IInputEventConsistencyVerifier> mInputEventConsistencyVerifier;
    AutoPtr<IGestureDetector> mGestureDetector;
    Boolean mEventBeforeOrAboveStartingGestureEvent;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_SCALEGESTUREDETECTOR_H__
