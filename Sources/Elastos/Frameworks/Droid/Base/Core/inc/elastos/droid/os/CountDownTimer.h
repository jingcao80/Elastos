
#ifndef __ELASTOS_DROID_OS_COUNT_DOWN_TIMER_H__
#define __ELASTOS_DROID_OS_COUNT_DOWN_TIMER_H__

#include <elastos/core/Object.h>
#include "elastos/droid/os/Handler.h"

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Schedule a countdown until a time in the future, with
 * regular notifications on intervals along the way.
 *
 * Example of showing a 30 second countdown in a text field:
 *
 * <pre class="prettyprint">
 * new CountDownTimer(30000, 1000) {
 *
 *     public void onTick(long millisUntilFinished) {
 *         mTextField.setText("seconds remaining: " + millisUntilFinished / 1000);
 *     }
 *
 *     public void onFinish() {
 *         mTextField.setText("done!");
 *     }
 *  }.start();
 * </pre>
 *
 * The calls to {@link #onTick(long)} are synchronized to this object so that
 * one call to {@link #onTick(long)} won't ever occur before the previous
 * callback is complete.  This is only relevant when the implementation of
 * {@link #onTick(long)} takes an amount of time to execute that is significant
 * compared to the countdown interval.
 */
class ECO_PUBLIC CountDownTimer
    : public Object
    , public ICountDownTimer
{
public:
    CAR_INTERFACE_DECL()

    CountDownTimer();

    virtual ~CountDownTimer();

    /**
     * @param millisInFuture The number of millis in the future from the call
     *   to {@link #start()} until the countdown is done and {@link #onFinish()}
     *   is called.
     * @param countDownInterval The interval along the way to receive
     *   {@link #onTick(long)} callbacks.
     */
    CARAPI constructor(
        /* [in] */ Int64 millisInFuture,
        /* [in] */ Int64 countDownInterval);

    /**
     * Cancel the countdown.
     */
    CARAPI Cancel();

    /**
     * Start the countdown.
     */
    CARAPI Start();

    /**
     * Callback fired on regular interval.
     * @param millisUntilFinished The amount of time until finished.
     */
    virtual CARAPI OnTick(
        /* [in] */ Int64 millisInFuture) = 0;

    /**
     * Callback fired when the time is up.
     */
    virtual CARAPI OnFinish() = 0;

private:
    class MyHandler : public Handler
    {
    public:
        TO_STRING_IMPL("CountDownTimer::MyHandler")

        MyHandler(
            /* [in] */ IWeakReference* wr);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

private:
    friend class MyHandler;

    /**
     * Millis since epoch when alarm should stop.
     */
    Int64 mMillisInFuture;

    /**
     * The interval in millis that the user receives callbacks
     */
    Int64 mCountdownInterval;

    Int64 mStopTimeInFuture;

    /**
    * boolean representing if the timer was cancelled
    */
    Boolean mCancelled;// = false;

    static const Int32 MSG;// = 1;

    AutoPtr<Handler> mHandler;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_COUNT_DOWN_TIMER_H__
