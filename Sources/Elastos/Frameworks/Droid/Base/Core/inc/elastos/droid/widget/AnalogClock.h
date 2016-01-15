
#ifndef __ELASTOS_DROID_WIDGET_ANALOGCLOCK_H__
#define __ELASTOS_DROID_WIDGET_ANALOGCLOCK_H__

#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/format/CTime.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IPendingResult;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::R;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * This widget display an analogic clock with two hands for hours and
 * minutes.
 */
class AnalogClock
    : public Elastos::Droid::View::View
    , public IAnalogClock
{
private:
    class IntentBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        IntentBroadcastReceiver(
            /* [in] */ AnalogClock* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info);

        CARAPI GoAsync(
            /* [out] */ IPendingResult** pendingResult);

        CARAPI PeekService(
            /* [in] */ IContext* myContext,
            /* [in] */ IIntent* service,
            /* [out] */ IBinder** binder);

        CARAPI SetResultCode(
            /* [in] */ Int32 code);

        CARAPI GetResultCode(
            /* [out] */ Int32* code);

        CARAPI SetResultData(
            /* [in] */ const String& data);

        CARAPI GetResultData(
            /* [out] */ String* data);

        CARAPI SetResultExtras(
            /* [in] */ IBundle* extras);

        CARAPI GetResultExtras(
            /* [in] */ Boolean makeMap,
            /* [out] */ IBundle** extras);

        CARAPI SetResult(
            /* [in] */ Int32 code,
            /* [in] */ const String& data,
            /* [in] */ IBundle* extras);

        CARAPI GetAbortBroadcast(
            /* [out] */ Boolean* isAborted);

        CARAPI AbortBroadcast();

        CARAPI ClearAbortBroadcast();

        CARAPI IsOrderedBroadcast(
            /* [out] */ Boolean* isOrdered);

        CARAPI IsInitialStickyBroadcast(
            /* [out] */ Boolean* isInitial);

        CARAPI SetOrderedHint(
            /* [in] */ Boolean isOrdered);

        CARAPI SetPendingResult(
            /* [in] */ IPendingResult* result);

        CARAPI GetPendingResult(
            /* [out] */ IPendingResult** pendingResult);

        CARAPI GetSendingUserId(
            /* [out] */ Int32* userId);

        CARAPI SetDebugUnregister(
            /* [in] */ Boolean debug);

        CARAPI GetDebugUnregister(
            /* [out] */ Boolean* debugUnregister);

    private:
        AnalogClock* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    AnalogClock();

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

protected:
    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) OnTimeChanged();

    CARAPI_(void) UpdateContentDescription(
        /* [in] */ ITime* time);

private:
    AutoPtr<IntentBroadcastReceiver> mIntentReceiver;
    AutoPtr<ITime> mCalendar;
    AutoPtr<IDrawable> mHourHand;
    AutoPtr<IDrawable> mMinuteHand;
    AutoPtr<IDrawable> mDial;
    Int32 mDialWidth;
    Int32 mDialHeight;
    Boolean mAttached;
    AutoPtr<IHandler> mHandler;
    Float mMinutes;
    Float mHour;
    Boolean mChanged;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_ANALOGCLOCK_H__

