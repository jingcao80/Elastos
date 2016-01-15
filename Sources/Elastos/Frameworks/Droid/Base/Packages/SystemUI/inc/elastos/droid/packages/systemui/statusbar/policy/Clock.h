
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_CLOCK_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_CLOCK_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/TextView.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Utility::ILocale;
using Elastos::Utility::ICalendar;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class Clock;

class ClockBroadcastReceiver : public BroadcastReceiver
{
public:
    ClockBroadcastReceiver(
        /* [in] */ Clock* host);

    virtual CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info);
        *info = String("ClockBroadcastReceiver: ");
        (*info).AppendFormat("%p", this);
        return NOERROR;
    }
private:
    Clock* mHost;
};

/**
 * Digital clock for the status bar.
 */
class Clock : public TextView
{
public:
    Clock();

    Clock(
        /* [in] */ IContext* context);

    Clock(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    Clock(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

protected:
    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    CARAPI_(void) UpdateClock();

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL,
        /* [in] */ Int32 defStyle = 0);

private:
    AutoPtr<ICharSequence> GetSmallTime();

private:
    friend class ClockBroadcastReceiver;
    friend class TimeUpdateRunnable;

    Boolean mAttached;
    AutoPtr<ICalendar> mCalendar;
    String mClockFormatString;
    String mClockPreFormat;
    AutoPtr<ISimpleDateFormat> mClockFormat;
    AutoPtr<ClockBroadcastReceiver> mIntentReceiver;
    AutoPtr<ILocale> mLocale;

    static const Int32 AM_PM_STYLE_NORMAL;//  = 0;
    static const Int32 AM_PM_STYLE_SMALL;//   = 1;
    static const Int32 AM_PM_STYLE_GONE;//    = 2;

    static const Int32 AM_PM_STYLE;// = AM_PM_STYLE_GONE;

    static const Int32 CLOCK_TEXTVIEW_SCALER;// = 18;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_CLOCK_H__
