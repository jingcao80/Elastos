
#ifndef __ELASTOS_DROID_WIDGET_CHRONOMETER_H__
#define __ELASTOS_DROID_WIDGET_CHRONOMETER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/widget/TextView.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IFormatter;

namespace Elastos {
namespace Droid {
namespace Widget {

class Chronometer
    : public TextView
    , public IChronometer
{
private:
    class InnerHandler
        : public Handler
    {
    public:
        InnerHandler(
            /* [in] */ Chronometer* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        Chronometer* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    Chronometer();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle,
        /* [in] */ Int32 defStyleRes);

    /**
     * Set the time that the count-up timer is in reference to.
     *
     * @param base Use the {@link SystemClock#elapsedRealtime} time base.
     */
    virtual CARAPI SetBase(
        /* [in] */ Int64 base);

    /**
     * Return the base time as set through {@link #setBase}.
     */
    virtual CARAPI GetBase(
        /* [out] */ Int64* result);

    /**
     * Sets the format string used for display.  The Chronometer will display
     * this string, with the first "%s" replaced by the current timer value in
     * "MM:SS" or "H:MM:SS" form.
     *
     * If the format string is null, or if you never call setFormat(), the
     * Chronometer will simply display the timer value in "MM:SS" or "H:MM:SS"
     * form.
     *
     * @param format the format string.
     */
    virtual CARAPI SetFormat(
        /* [in] */ const String& format);

    /**
     * Returns the current format string as set through {@link #setFormat}.
     */
    virtual CARAPI GetFormat(
        /* [out] */ String* result);

    /**
     * Sets the listener to be called when the chronometer changes.
     *
     * @param listener The listener.
     */
    virtual CARAPI SetOnChronometerTickListener(
        /* [in] */ IOnChronometerTickListener* listener);

    /**
     * @return The listener (may be null) that is listening for chronometer change
     *         events.
     */
    virtual CARAPI GetOnChronometerTickListener(
        /* [out] */ IOnChronometerTickListener** result);

    /**
     * Start counting up.  This does not affect the base as set from {@link #setBase}, just
     * the view display.
     *
     * Chronometer works by regularly scheduling messages to the handler, even when the
     * Widget is not visible.  To make sure resource leaks do not occur, the user should
     * make sure that each start() call has a reciprocal call to {@link #stop}.
     */
    virtual CARAPI Start();

    /**
     * Stop counting up.  This does not affect the base as set from {@link #setBase}, just
     * the view display.
     *
     * This stops the messages to the handler, effectively releasing resources that would
     * be held as the chronometer is running, via {@link #start}.
     */
    virtual CARAPI Stop();

    /**
     * The same as calling {@link #start} or {@link #stop}.
     * @hide pending API council approval
     */
    virtual CARAPI SetStarted(
        /* [in] */ Boolean started);

    virtual CARAPI DispatchChronometerTick();

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info) ;

protected:
    virtual CARAPI OnDetachedFromWindow();


    virtual CARAPI_(void) OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

private:
    CARAPI_(void) Init();

    CARAPI_(void) UpdateText(Int64 now);

    CARAPI_(void) UpdateRunning();

private:
    static const String TAG;
    Int64 mBase;
    Boolean mVisible;
    Boolean mStarted;
    Boolean mRunning;
    Boolean mLogged;
    String mFormat;
    AutoPtr<IFormatter> mFormatter;
    AutoPtr<ILocale> mFormatterLocale;
    AutoPtr< ArrayOf<IInterface*> > mFormatterArgs;
    AutoPtr<StringBuilder> mFormatBuilder;
    AutoPtr<IOnChronometerTickListener> mOnChronometerTickListener;
    StringBuilder mRecycle;
    static const Int32 TICK_WHAT = 2;
    AutoPtr<InnerHandler> mHandler;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CHRONOMETER_H__

