
#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_INTERNALDIGITALCLOCK_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_INTERNALDIGITALCLOCK_H__

#include "elastos/droid/widget/RelativeLayout.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/text/format/DateFormat.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Droid::View::IView;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Graphics::ITypefaceHelper;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

/**
 * Displays the time
 */
class DigitalClock : public RelativeLayout
{

    /*static {
        sBackgroundFont = Typeface.createFromFile(SYSTEM_FONT_TIME_BACKGROUND);
        sForegroundFont = Typeface.createFromFile(SYSTEM_FONT_TIME_FOREGROUND);
    }*/

    class TimeChangedReceiver : public BroadcastReceiver
    {
    public:

        TimeChangedReceiver(
            /* [in] */ DigitalClock* clock);

        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("DigitalClock::TimeChangedReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        DigitalClock* mClock;
        AutoPtr<IContext> mContext;
    };

    class OnReceiveRunnable
                : public IRunnable
                , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        OnReceiveRunnable(
            /* [in] */ Boolean timezoneChanged,
            /* [in] */ DigitalClock* host);

        CARAPI Run();

    private:
        Boolean mTimezoneChanged;
        DigitalClock* mHost;
    };

    class AmPm : public ElRefBase
    {
    public:
        AmPm(
            /* [in] */ IView* parent,
            /* [in] */ ITypeface* tf);

        virtual CARAPI_(void) SetShowAmPm(
            /* [in] */ Boolean show);

        virtual CARAPI_(void) SetIsMorning(
            /* [in] */ Boolean isMorning);

    private:
        AutoPtr<ITextView> mAmPmTextView;
        String mAmString;
        String mPmString;
    };


    class FormatChangeObserver
        : public ContentObserver
    {
    public:
        FormatChangeObserver(
            /* [in] */ DigitalClock* clock);

        virtual CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:

        CARAPI_(AutoPtr<IHandler>) ForConstructor();
    private:
        DigitalClock* mClock;
        AutoPtr<IContext> mContext;
    };

public:
    DigitalClock();

    DigitalClock(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL);

    virtual CARAPI UpdateTime();

protected:
    CARAPI OnFinishInflate();

    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    CARAPI_(void) UpdateTime(
        /* [in] */ ICalendar* c);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL);
private:
    CARAPI_(void) SetDateFormat();

private:
    static const String SYSTEM;
    static const String SYSTEM_FONT_TIME_BACKGROUND;
    static const String SYSTEM_FONT_TIME_FOREGROUND;
    static const AutoPtr<ITypeface> sBackgroundFont;
    static const AutoPtr<ITypeface> sForegroundFont;
    static const String M12;
    static const String M24;

    AutoPtr<ICalendar> mCalendar;
    String mFormat;
    AutoPtr<ITextView> mTimeDisplayBackground;
    AutoPtr<ITextView> mTimeDisplayForeground;
    AutoPtr<AmPm> mAmPm;
    AutoPtr<ContentObserver> mFormatChangeObserver;
    Int32 mAttached; // for debugging - tells us whether attach/detach is unbalanced

    /* called by system on minute ticks */
    AutoPtr<IHandler> mHandler;
    AutoPtr<IBroadcastReceiver> mIntentReceiver;


    friend AutoPtr<ITypeface> CreateStaticTypeface(String& fileName);
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_INTERNAL_INTERNALDIGITALCLOCK_H__
