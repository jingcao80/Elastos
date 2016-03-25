
#include "elastos/droid/widget/internal/DigitalClock.h"
#include "elastos/droid/R.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/graphics/CTypefaceHelper.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/os/CHandler.h"

using Elastos::Droid::Os::CHandler;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CStringWrapper;
using Elastos::Core::EIID_IRunnable;
//using Elastos::Text::IDateFormatSymbols;
//using Elastos::Text::IDateFormatSymbolsHelper;
using Elastos::Droid::R;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Graphics::CTypefaceHelper;
using Elastos::Droid::Graphics::ITypefaceHelper;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

static AutoPtr<ITypeface> CreateStaticTypeface(
        /* [in] */ const String& fileName)
{
    AutoPtr<ITypefaceHelper> helper;
    AutoPtr<ITypeface> result;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&helper);
    helper->CreateFromFile(fileName, (ITypeface**)&result);
    return result;
}

const String DigitalClock::SYSTEM = String("/system/fonts/");
const String DigitalClock::SYSTEM_FONT_TIME_BACKGROUND = String("/system/fonts/AndroidClock.ttf");
const String DigitalClock::SYSTEM_FONT_TIME_FOREGROUND = String("/system/fonts/AndroidClock_Highlight.ttf");
const String DigitalClock::M12 = String("h:mm");
const String DigitalClock::M24 = String("kk:mm");
const AutoPtr<ITypeface> DigitalClock::sBackgroundFont = CreateStaticTypeface(String("/system/fonts/AndroidClock.ttf"));
const AutoPtr<ITypeface> DigitalClock::sForegroundFont = CreateStaticTypeface(String("/system/fonts/AndroidClock_Highlight.ttf"));

CAR_INTERFACE_IMPL(DigitalClock::OnReceiveRunnable, IRunnable)

DigitalClock::OnReceiveRunnable::OnReceiveRunnable(
        /* [in] */ Boolean timezoneChanged,
        /* [in] */ DigitalClock* host)
                 : mTimezoneChanged(timezoneChanged)
                 , mHost(host)
{}

ECode DigitalClock::OnReceiveRunnable::Run()
{
    if(mTimezoneChanged)
    {
        AutoPtr<ICalendarHelper> helper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
        mHost->mCalendar = NULL;
        helper->GetInstance((ICalendar**)&(mHost->mCalendar));
    }
    mHost->UpdateTime();
    return NOERROR;
}

DigitalClock::TimeChangedReceiver::TimeChangedReceiver(
    /* [in] */ DigitalClock* clock)
{
    mClock = clock;
    mContext = clock->GetContext();
}

ECode DigitalClock::TimeChangedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // Post a runnable to avoid blocking the broadcast.
    String action;
    intent->GetAction(&action);
    Boolean timezoneChanged = action.Equals(IIntent::ACTION_TIMEZONE_CHANGED);
    if (mClock != NULL) {
        AutoPtr<OnReceiveRunnable> runnable = new OnReceiveRunnable(timezoneChanged, mClock);
        Boolean result;
        mClock->mHandler->Post(runnable.Get(), &result);

    } else {
        //try {
            mContext->UnregisterReceiver(this);
        //} catch (RuntimeException e) {
            // Shouldn't happen
        //}
    }
    return NOERROR;
}

DigitalClock::AmPm::AmPm(
    /* [in] */ IView* parent,
    /* [in] */ ITypeface* tf)
{
    // No longer used, uncomment if we decide to use AM/PM indicator again
    // mAmPmTextView = (TextView) parent.findViewById(R.id.am_pm);
    if (mAmPmTextView != NULL && tf != NULL) {
        mAmPmTextView->SetTypeface(tf);
    }

    assert(0 && "TODO");
//    AutoPtr<IDateFormatSymbols> symbols;
//    CDateFormatSymbols::New((IDateFormatSymbols**)&symbols);
    AutoPtr<ArrayOf<String> > ampm;
//    symbols->GetAmPmStrings((ArrayOf<String>**)&ampm);
    mAmString = (*ampm)[0];
    mPmString = (*ampm)[1];
}

void DigitalClock::AmPm::SetShowAmPm(
    /* [in] */ Boolean show)
{
    if (mAmPmTextView != NULL) {
        mAmPmTextView->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    }
}

void DigitalClock::AmPm::SetIsMorning(
    /* [in] */ Boolean isMorning)
{
    if (mAmPmTextView != NULL) {
        AutoPtr<ICharSequence> cAm, cPm;
        CStringWrapper::New(mAmString, (ICharSequence**)&cAm);
        CStringWrapper::New(mPmString, (ICharSequence**)&cPm);
        mAmPmTextView->SetText(isMorning ? cAm : cPm);
    }
}

DigitalClock::FormatChangeObserver::FormatChangeObserver(
    /* [in] */ DigitalClock* clock) : ContentObserver(ForConstructor())
{
    mClock = clock;
    mContext = clock->GetContext();
}

AutoPtr<IHandler> DigitalClock::FormatChangeObserver::ForConstructor()
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    return handler;
}

ECode DigitalClock::FormatChangeObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    if (mClock != NULL) {
        mClock->SetDateFormat();
        mClock->UpdateTime();
    } else {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        cr->UnregisterContentObserver(this);
    }

    return NOERROR;
}

DigitalClock::DigitalClock()
            : mAttached(0)
{}

DigitalClock::DigitalClock(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
            : RelativeLayout(context, attrs)
            , mAttached(0)
{}

ECode DigitalClock::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RelativeLayout::Init(context, attrs);
}

ECode DigitalClock::OnFinishInflate()
{
    RelativeLayout::OnFinishInflate();

    /* The time display consists of two tones. That's why we have two overlapping text views. */
    mTimeDisplayBackground = ITextView::Probe((FindViewById(R::id::timeDisplayBackground)));
    mTimeDisplayBackground->SetTypeface(sBackgroundFont);
    mTimeDisplayBackground->SetVisibility(IView::INVISIBLE);

    mTimeDisplayForeground = ITextView::Probe((FindViewById(R::id::timeDisplayForeground)));
    mTimeDisplayForeground->SetTypeface(sForegroundFont);
    mAmPm = new AmPm(this, NULL);
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    mCalendar = NULL;
    helper->GetInstance((ICalendar**)&mCalendar);

    SetDateFormat();
    return NOERROR;
}

ECode DigitalClock::OnAttachedToWindow()
{
    RelativeLayout::OnAttachedToWindow();

    mAttached++;

    /* monitor time ticks, time changed, timezone */
    if (mIntentReceiver == NULL) {
        mIntentReceiver = new TimeChangedReceiver(this);
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);
        filter->AddAction(IIntent::ACTION_TIME_TICK);
        filter->AddAction(IIntent::ACTION_TIME_CHANGED);
        filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
        AutoPtr<IIntent> intent;
        mContext->RegisterReceiver(mIntentReceiver, filter, (IIntent**)&intent);
    }

    /* monitor 12/24-hour display preference */
    if (mFormatChangeObserver == NULL) {
        mFormatChangeObserver = new FormatChangeObserver(this);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        cr->RegisterContentObserver(
                Settings::System::CONTENT_URI, TRUE, mFormatChangeObserver);
    }

    UpdateTime();
    return NOERROR;
}

ECode DigitalClock::OnDetachedFromWindow()
{
    RelativeLayout::OnDetachedFromWindow();

    mAttached--;

    if (mIntentReceiver != NULL) {
        mContext->UnregisterReceiver(mIntentReceiver);
    }
    if (mFormatChangeObserver != NULL) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        cr->UnregisterContentObserver(
                mFormatChangeObserver);
    }

    mFormatChangeObserver = NULL;
    mIntentReceiver = NULL;
    return NOERROR;
}

void DigitalClock::UpdateTime(
    /* [in] */ ICalendar* c)
{
    mCalendar = c;
    UpdateTime();
}

ECode DigitalClock::UpdateTime()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    mCalendar->SetTimeInMillis(now);
    AutoPtr<ICharSequence> cmFormat;
    CStringWrapper::New(mFormat, (ICharSequence**)&cmFormat);
    AutoPtr<ICharSequence> newTime = DateFormat::Format(cmFormat, mCalendar);
    mTimeDisplayBackground->SetText(newTime);
    mTimeDisplayForeground->SetText(newTime);
    Int32 value;
    mCalendar->Get(ICalendar::AM_PM, &value);
    mAmPm->SetIsMorning(value == 0);
    return NOERROR;
}

void DigitalClock::SetDateFormat()
{
    mFormat = DateFormat::Is24HourFormat(GetContext())
        ? M24 : M12;
    mAmPm->SetShowAmPm(mFormat.Equals(M12));
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
