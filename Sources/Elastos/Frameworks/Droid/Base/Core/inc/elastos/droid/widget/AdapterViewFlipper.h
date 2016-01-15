
#ifndef __ELASTOS_DROID_WIDGET_ADAPTERVIEWFLIPPER_H__
#define __ELASTOS_DROID_WIDGET_ADAPTERVIEWFLIPPER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/AdapterViewAnimator.h"

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
  * Simple {@link ViewAnimator} that will animate between two or more views
  * that have been added to it.  Only one child is shown at a time.  If
  * requested, can automatically flip between each child at a regular interval.
  *
  * @attr ref android.R.styleable#AdapterViewFlipper_flipInterval
  * @attr ref android.R.styleable#AdapterViewFlipper_autoStart
  */
// @RemoteView
class AdapterViewFlipper
    : public AdapterViewAnimator
{
private:
    class InnerBroadcastReceiver
        : public Object
        , public IBroadcastReceiver
    {
    public:
        CAR_INTERFACE_DECL()

        InnerBroadcastReceiver(
            /* [in] */ AdapterViewFlipper* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        AdapterViewFlipper* mOwner;
    };

    class InnerHandler
        : public Object
        , public IHandler
    {
    public:
        CAR_INTERFACE_DECL()

        InnerHandler(
            /* [in] */ AdapterViewFlipper* owner);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AdapterViewFlipper* mOwner;
    };

public:
    AdapterViewFlipper();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    // @Override
    CARAPI SetAdapter(
        /* [in] */ IAdapter* adapter);

    /**
      * Returns the flip interval, in milliseconds.
      *
      * @return the flip interval in milliseconds
      *
      * @see #setFlipInterval(int)
      *
      * @attr ref android.R.styleable#AdapterViewFlipper_flipInterval
      */
    virtual CARAPI GetFlipInterval(
        /* [out] */ Int32* result);

    /**
      * How long to wait before flipping to the next view.
      *
      * @param flipInterval flip interval in milliseconds
      *
      * @see #getFlipInterval()
      *
      * @attr ref android.R.styleable#AdapterViewFlipper_flipInterval
      */
    virtual CARAPI SetFlipInterval(
        /* [in] */ Int32 flipInterval);

    /**
      * Start a timer to cycle through child views
      */
    virtual CARAPI StartFlipping();

    /**
      * No more flips
      */
    virtual CARAPI StopFlipping();

    /**
     * {@inheritDoc}
     */
    // @Override
    // @RemotableViewMethod
    virtual CARAPI ShowNext();

    /**
     * {@inheritDoc}
     */
    // @Override
    // @RemotableViewMethod
    virtual CARAPI ShowPrevious();

    /**
      * Returns true if the child views are flipping.
      */
    virtual CARAPI IsFlipping(
        /* [out] */ Boolean* result);

    /**
      * Set if this view automatically calls {@link #startFlipping()} when it
      * becomes attached to a window.
      */
    virtual CARAPI SetAutoStart(
        /* [in] */ Boolean autoStart);

    /**
      * Returns true if this view automatically calls {@link #startFlipping()}
      * when it becomes attached to a window.
      */
    virtual CARAPI IsAutoStart(
        /* [out] */ Boolean* result);

    /**
      * Called by an {@link android.appwidget.AppWidgetHost} to indicate that it will be
      * automatically advancing the views of this {@link AdapterViewFlipper} by calling
      * {@link AdapterViewFlipper#advance()} at some point in the future. This allows
      * {@link AdapterViewFlipper} to prepare by no longer Advancing its children.
      */
    // @Override
    CARAPI FyiWillBeAdvancedByHostKThx();

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    // @Override
    virtual CARAPI_(void) OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

private:
    /**
      * Internal method to start or stop dispatching flip {@link Message} based
      * on {@link #mRunning} and {@link #mVisible} state.
      */
    CARAPI UpdateRunning();

    /**
      * Internal method to start or stop dispatching flip {@link Message} based
      * on {@link #mRunning} and {@link #mVisible} state.
      *
      * @param flipNow Determines whether or not to execute the animation now, in
      *            addition to queuing future flips. If omitted, defaults to
      *            true.
      */
    CARAPI UpdateRunning(
        /* [in] */ Boolean flipNow);

private:
    static const String TAG;
    static const Boolean LOGD;
    static const Int32 DEFAULT_INTERVAL = 10000;
    Int32 mFlipInterval;
    Boolean mAutoStart;
    Boolean mRunning;
    Boolean mStarted;
    Boolean mVisible;
    Boolean mUserPresent;
    Boolean mAdvancedByHost;
    /*const*/ AutoPtr<IBroadcastReceiver> mReceiver;
    const Int32 FLIP_MSG;
    /*const*/ AutoPtr<IHandler> mHandler;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_ADAPTERVIEWFLIPPER_H__

