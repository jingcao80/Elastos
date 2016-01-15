#ifndef __ELASTOS_DROID_WIDGET_DIGITALCLOCK_H__
#define __ELASTOS_DROID_WIDGET_DIGITALCLOCK_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/widget/TextView.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Widget {

class DigitalClock
    : public TextView
    , public IDigitalClock
{
private:
    class DigitalClockRunnable
        : public Runnable
    {
    public:
        DigitalClockRunnable(
            /* [in] */ DigitalClock* host);

        CARAPI Run();

    private:
        DigitalClock* mHost;
    };

    class FormatChangeObserver
        : public ContentObserver
    {
    public:
        FormatChangeObserver(
            /* [in] */ DigitalClock* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        DigitalClock* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DigitalClock();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

private:
    CARAPI_(void) InitClock();

    CARAPI_(void) SetFormat();

public:
    AutoPtr<ICalendar> mCalendar;
    String mFormat;

private:
    AutoPtr<FormatChangeObserver> mFormatChangeObserver;
    AutoPtr<DigitalClockRunnable> mTicker;
    AutoPtr<IHandler> mHandler;

    Boolean mTickerStopped;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_DIGITALCLOCK_H__
