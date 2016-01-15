#include "elastos/droid/systemui/statusbar/policy/Clock.h"
#include <elastos/core/Character.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/R.h"
#include "elastos/droid/content/CIntentFilter.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/format/CDateFormat.h"
#include "elastos/droid/text/style/CRelativeSizeSpan.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::Character;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::IDate;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::R;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::Text::Style::ICharacterStyle;
using Elastos::Droid::Text::Style::IRelativeSizeSpan;
using Elastos::Droid::Text::Style::CRelativeSizeSpan;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

ClockBroadcastReceiver::ClockBroadcastReceiver(
    /* [in] */ Clock* host)
    : mHost(host)
{
}

ECode ClockBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_TIMEZONE_CHANGED)) {
        String tz;
        intent->GetStringExtra(String("time-zone"), &tz);

        AutoPtr<ITimeZoneHelper> tzHelper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
        AutoPtr<ITimeZone> timeZone;
        tzHelper->GetTimeZone(tz, (ITimeZone**)&timeZone);

        AutoPtr<ICalendarHelper> helper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
        mHost->mCalendar = NULL;
        helper->GetInstance(timeZone, (ICalendar**)&mHost->mCalendar);

        if (mHost->mClockFormat != NULL) {
            AutoPtr<ITimeZone> ctz;
            mHost->mCalendar->GetTimeZone((ITimeZone**)&ctz);
            mHost->mClockFormat->SetTimeZone(ctz);
        }
    }
    else if (action.Equals(IIntent::ACTION_CONFIGURATION_CHANGED)) {
        AutoPtr<IResources> res = mHost->GetResources();
        AutoPtr<IConfiguration> config;
        res->GetConfiguration((IConfiguration**)&config);
        AutoPtr<ILocale> newLocale;
        config->GetLocale((ILocale**)&newLocale);

        if (newLocale.Get() == NULL && mHost->mLocale.Get() != NULL) {
            mHost->mLocale = NULL;
            mHost->mClockFormatString = String(""); // force refresh
            mHost->mClockPreFormat = String("");
        }
        else if (newLocale != NULL) {
            Boolean equals;
            newLocale->Equals(mHost->mLocale, &equals);
            if (!equals) {
                mHost->mLocale = newLocale;
                mHost->mClockFormatString = String(""); // force refresh
                mHost->mClockPreFormat = String("");
            }
        }
    }

    mHost->UpdateClock();
    return NOERROR;
}

const Int32 Clock::AM_PM_STYLE_NORMAL  = 0;
const Int32 Clock::AM_PM_STYLE_SMALL   = 1;
const Int32 Clock::AM_PM_STYLE_GONE    = 2;

const Int32 Clock::AM_PM_STYLE = AM_PM_STYLE_SMALL;

const Int32 Clock::CLOCK_TEXTVIEW_SCALER = 18;

Clock::Clock()
    : mAttached(FALSE)
{
}

Clock::Clock(
    /* [in] */ IContext* context)
    : TextView(context)
    , mAttached(FALSE)
{
    mIntentReceiver = new ClockBroadcastReceiver(this);
    SetTextSize(CLOCK_TEXTVIEW_SCALER);
}

Clock::Clock(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : TextView(context, attrs)
    , mAttached(FALSE)
{
}

Clock::Clock(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : TextView(context, attrs, defStyle)
    , mAttached(FALSE)
{
    mIntentReceiver = new ClockBroadcastReceiver(this);
    SetTextSize(CLOCK_TEXTVIEW_SCALER);
}

ECode Clock::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    TextView::Init(context, attrs, defStyle);
    mIntentReceiver = new ClockBroadcastReceiver(this);
    return SetTextSize(CLOCK_TEXTVIEW_SCALER);
}

ECode Clock::OnAttachedToWindow()
{
    TextView::OnAttachedToWindow();

    if (!mAttached) {
        mAttached = TRUE;
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);

        filter->AddAction(IIntent::ACTION_TIME_TICK);
        filter->AddAction(IIntent::ACTION_TIME_CHANGED);
        filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
        filter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);
        filter->AddAction(IIntent::ACTION_USER_SWITCHED);

        AutoPtr<IContext> context = GetContext();
        AutoPtr<IHandler> handler = GetHandler();
        AutoPtr<IIntent> stickyIntent;
        String nullStr;
        context->RegisterReceiver(
            (IBroadcastReceiver*)mIntentReceiver, filter, nullStr,
            handler, (IIntent**)&stickyIntent);
    }

    // NOTE: It's safe to do these after registering the receiver since the receiver always runs
    // in the main thread, therefore the receiver can't run before this method returns.

    // The time zone may have changed while the receiver wasn't registered, so update the Time
    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> defaultTimeZone;
    tzHelper->GetDefault((ITimeZone**)&defaultTimeZone);
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    mCalendar = NULL;
    helper->GetInstance(defaultTimeZone, (ICalendar**)&mCalendar);

    // Make sure we update to the current time
    UpdateClock();

    return NOERROR;
}

ECode Clock::OnDetachedFromWindow()
{
    TextView::OnDetachedFromWindow();
    if (mAttached) {
        AutoPtr<IContext> context = GetContext();
        context->UnregisterReceiver(mIntentReceiver);
        mAttached = FALSE;
    }

    return NOERROR;
}

void Clock::UpdateClock()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    mCalendar->SetTimeInMillis(now);
    AutoPtr<ICharSequence> seq = GetSmallTime();
    SetText(seq);
}

AutoPtr<ICharSequence> Clock::GetSmallTime()
{
    AutoPtr<IContext> context = GetContext();
    AutoPtr<IDateFormat> helper;
    CDateFormat::AcquireSingleton((IDateFormat**)&helper);
    Boolean b24;
    helper->Is24HourFormat(context, &b24);

    Int32 res;
    if (b24) {
        res = R::string::twenty_four_hour_time_format;
    } else {
        res = R::string::twelve_hour_time_format;
    }

    const Char32 MAGIC1 = 0xEF00;
    const Char32 MAGIC2 = 0xEF01;

    AutoPtr<ISimpleDateFormat> sdf;
    String format;
    context->GetString(res, &format);
    if (!format.Equals(mClockPreFormat)) {
        mClockPreFormat = format;

        /*
         * Search for an unquoted "a" in the format string, so we can
         * add dummy characters around it to let us find it again after
         * formatting and change its size.
         */
        if (AM_PM_STYLE != AM_PM_STYLE_NORMAL) {
            Int32 a = -1;
            Boolean quoted = FALSE;
            Char32 c;
            AutoPtr<ArrayOf<Char32> > charArray = format.GetChars();
            for (Int32 i = 0; i < charArray->GetLength(); i++) {
                c = (*charArray)[i];
                if (c == '\'') {
                    quoted = !quoted;
                }
                if (!quoted && c == 'a') {
                    a = i;
                    break;
                }
            }

            if (a >= 0) {
                // Move a back so any whitespace before AM/PM is also in the alternate size.
                Int32 b = a;
                while (a > 0 && Character::IsWhitespace((*charArray)[a - 1])) {
                    a--;
                }
                StringBuilder sb(format.Substring(0, a));
                sb += MAGIC1;
                sb += format.Substring(a, b);
                sb += "a";
                sb += MAGIC2;
                sb += format.Substring(b + 1);
                format = sb.ToString();

            }
        }
        CSimpleDateFormat::New(format, (ISimpleDateFormat**)&sdf);
        mClockFormat = sdf;
        mClockFormatString = format;
    }
    else {
        sdf = mClockFormat;
    }

    AutoPtr<IDate> dateTime;
    mCalendar->GetTime((IDate**)&dateTime);

    String result;
    sdf->FormatDate(dateTime, &result);
    if (AM_PM_STYLE != AM_PM_STYLE_NORMAL) {
        Int32 magic1 = result.IndexOf(MAGIC1);
        Int32 magic2 = result.IndexOf(MAGIC2);
        if (magic1 >= 0 && magic2 > magic1) {
            AutoPtr<ICharSequence> seq;
            CString::New(result, (ICharSequence**)&seq);
            AutoPtr<ISpannableStringBuilder> formatted;
            CSpannableStringBuilder::New(seq, (ISpannableStringBuilder**)&formatted);
            if (AM_PM_STYLE == AM_PM_STYLE_GONE) {
                formatted->Delete(magic1, magic2 + 1);
            }
            else {
                if (AM_PM_STYLE == AM_PM_STYLE_SMALL) {
                    AutoPtr<IRelativeSizeSpan> obj;
                    CRelativeSizeSpan::New(0.7f, (IRelativeSizeSpan**)&obj);
                    AutoPtr<ICharacterStyle> style = ICharacterStyle::Probe(obj.Get());
                    formatted->SetSpan(style, magic1, magic2,
                        ISpanned::SPAN_EXCLUSIVE_INCLUSIVE);
                }
                formatted->Delete(magic2, magic2 + 1);
                formatted->Delete(magic1, magic1 + 1);
            }

            formatted->ToString(&result);
        }
    }

    AutoPtr<ICharSequence> seq;
    CString::New(result, (ICharSequence**)&seq);
    return seq;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
