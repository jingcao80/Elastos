
#include "elastos/droid/systemui/statusbar/policy/CClock.h"
#include "elastos/droid/systemui/statusbar/policy/CClockIntentReceiver.h"
#include "../R.h"
#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/text/format/DateFormat.h>
#include <elastos/core/Character.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::Text::Style::CRelativeSizeSpan;
using Elastos::Droid::Text::Style::ICharacterStyle;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IDateFormat;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::IDate;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Core::Character;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Libcore::ICU::CLocaleDataHelper;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::ILocaleData;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_OBJECT_IMPL(CClockIntentReceiver)

ECode CClockIntentReceiver::constructor(
    /* [in] */ IClock* host)
{
    mHost = (CClock*)host;
    return NOERROR;
}

ECode CClockIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_TIMEZONE_CHANGED)) {
        String tz;
        intent->GetStringExtra(String("time-zone"), &tz);
        AutoPtr<ICalendarHelper> helper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);

        AutoPtr<ITimeZoneHelper> tzHelper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
        AutoPtr<ITimeZone> tzone;
        tzHelper->GetTimeZone(tz, (ITimeZone**)&tzone);
        helper->GetInstance(tzone, (ICalendar**)&mHost->mCalendar);
        if (mHost->mClockFormat != NULL) {
            AutoPtr<ITimeZone> t;
            mHost->mCalendar->GetTimeZone((ITimeZone**)&t);
            IDateFormat::Probe(mHost->mClockFormat)->SetTimeZone(t);
        }
    }
    else if (action.Equals(IIntent::ACTION_CONFIGURATION_CHANGED)) {
        AutoPtr<IResources> res;
        mHost->GetResources((IResources**)&res);
        AutoPtr<IConfiguration> cf;
        res->GetConfiguration((IConfiguration**)&cf);
        AutoPtr<ILocale> newLocale;
        cf->GetLocale((ILocale**)&newLocale);
        Boolean tmp = FALSE;
        if (newLocale->Equals(mHost->mLocale, &tmp), !tmp) {
            mHost->mLocale = newLocale;
            mHost->mClockFormatString = ""; // force refresh
        }
    }
    mHost->UpdateClock();
    return NOERROR;
}

const Int32 CClock::AM_PM_STYLE_NORMAL  = 0;
const Int32 CClock::AM_PM_STYLE_SMALL   = 1;
const Int32 CClock::AM_PM_STYLE_GONE    = 2;

CAR_INTERFACE_IMPL_2(CClock, TextView, IClock, IDemoMode);

CAR_OBJECT_IMPL(CClock)

CClock::CClock()
    : mAttached(FALSE)
    , mAmPmStyle(0)
    , mDemoMode(FALSE)
{
    CClockIntentReceiver::New(this, (IBroadcastReceiver**)&mIntentReceiver);
}

ECode CClock::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CClock::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet * attrs)
{
    return constructor(context, attrs, 0);
}

ECode CClock::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    TextView::constructor(context, attrs, defStyle);
    AutoPtr<IResourcesTheme> rt;
    context->GetTheme((IResourcesTheme**)&rt);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::Clock);
    AutoPtr<ITypedArray> a;
    rt->ObtainStyledAttributes(attrs, attrIds,
            0, 0, (ITypedArray**)&a);
    // try {
    a->GetInt32(R::styleable::Clock_amPmStyle, AM_PM_STYLE_GONE, &mAmPmStyle);
    // } finally {
    a->Recycle();
    // }
    return NOERROR;
}

ECode CClock::OnAttachedToWindow()
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

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IHandler> h;
        GetHandler((IHandler**)&h);
        AutoPtr<IIntent> i;
        context->RegisterReceiver(mIntentReceiver, filter, String(NULL), h, (IIntent**)&i);
    }

    // NOTE: It's safe to do these after registering the receiver since the receiver always runs
    // in the main thread, therefore the receiver can't run before this method returns.

    // The time zone may have changed while the receiver wasn't registered, so update the Time
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);

    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> tzone;
    tzHelper->GetDefault((ITimeZone**)&tzone);
    helper->GetInstance(tzone, (ICalendar**)&mCalendar);

    // Make sure we update to the current time
    UpdateClock();
    return NOERROR;
}

ECode CClock::OnDetachedFromWindow()
{
    TextView::OnDetachedFromWindow();
    if (mAttached) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->UnregisterReceiver(mIntentReceiver);
        mAttached = FALSE;
    }
    return NOERROR;
}

void CClock::UpdateClock()
{
    if (mDemoMode) return;
    AutoPtr<ISystem> s;
    CSystem::AcquireSingleton((ISystem**)&s);
    Int64 t;
    s->GetCurrentTimeMillis(&t);
    mCalendar->SetTimeInMillis(t);
    SetText(GetSmallTime());
}

AutoPtr<ICharSequence> CClock::GetSmallTime()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Boolean is24 = DateFormat::Is24HourFormat(context);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> cf;
    res->GetConfiguration((IConfiguration**)&cf);
    AutoPtr<ILocale> locale;
    cf->GetLocale((ILocale**)&locale);
    AutoPtr<ILocaleDataHelper> helper;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&helper);
    AutoPtr<ILocaleData> d;
    helper->Get(locale, (ILocaleData**)&d);

    Char32 MAGIC1 = L'\uEF00';
    Char32 MAGIC2 = L'\uEF01';

    AutoPtr<ISimpleDateFormat> sdf;
    String format;
    if (is24) {
        d->GetTimeFormat24(&format);
    }
    else {
        d->GetTimeFormat12(&format);
    }
    if (!format.Equals(mClockFormatString)) {
        /*
         * Search for an unquoted "a" in the format string, so we can
         * add dummy characters around it to let us find it again after
         * formatting and change its size.
         */
        if (mAmPmStyle != AM_PM_STYLE_NORMAL) {
            Int32 a = -1;
            Boolean quoted = FALSE;
            for (Int32 i = 0; i < format.GetLength(); i++) {
                Char32 c = format.GetChar(i);

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
                const Int32 b = a;
                while (a > 0 && Character::IsWhitespace(format.GetChar(a - 1))) {
                    a--;
                }
                StringBuilder sb(format.Substring(0, a)); sb += MAGIC1; sb += format.Substring(a, b);
                sb += "a"; sb += MAGIC2; sb += format.Substring(b + 1);
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
    AutoPtr<IDate> date;
    mCalendar->GetTime((IDate**)&date);
    String result;
    IDateFormat::Probe(sdf)->Format(date, &result);
    AutoPtr<ICharSequence> value;
    CString::New(result, (ICharSequence**)&value);

    if (mAmPmStyle != AM_PM_STYLE_NORMAL) {
        Int32 magic1 = result.IndexOf(MAGIC1);
        Int32 magic2 = result.IndexOf(MAGIC2);
        if (magic1 >= 0 && magic2 > magic1) {
            AutoPtr<ISpannableStringBuilder> formatted;
            CSpannableStringBuilder::New(value, (ISpannableStringBuilder**)&formatted);
            if (mAmPmStyle == AM_PM_STYLE_GONE) {
                IEditable::Probe(formatted)->Delete(magic1, magic2 + 1);
            }
            else {
                if (mAmPmStyle == AM_PM_STYLE_SMALL) {
                    AutoPtr<ICharacterStyle> style;
                    CRelativeSizeSpan::New(0.7f, (ICharacterStyle**)&style);
                    ISpannable::Probe(formatted)->SetSpan(style, magic1, magic2,
                                      ISpanned::SPAN_EXCLUSIVE_INCLUSIVE);
                }
                IEditable::Probe(formatted)->Delete(magic2, magic2 + 1);
                IEditable::Probe(formatted)->Delete(magic1, magic1 + 1);
            }
            return ICharSequence::Probe(formatted);
        }
    }

    return value;
}

ECode CClock::DispatchDemoCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* args)
{
    if (!mDemoMode && command.Equals(COMMAND_ENTER)) {
        mDemoMode = TRUE;
    }
    else if (mDemoMode && command.Equals(COMMAND_EXIT)) {
        mDemoMode = FALSE;
        UpdateClock();
    }
    else if (mDemoMode && command.Equals(COMMAND_CLOCK)) {
        String millis;
        args->GetString(String("millis"), &millis);
        String hhmm;
        args->GetString(String("hhmm"), &hhmm);
        if (millis != NULL) {
            mCalendar->SetTimeInMillis(StringUtils::ParseInt64(millis));
        }
        else if (hhmm != NULL && hhmm.GetLength() == 4) {
            Int32 hh = StringUtils::ParseInt32(hhmm.Substring(0, 2));
            Int32 mm = StringUtils::ParseInt32(hhmm.Substring(2));
            mCalendar->Set(ICalendar::HOUR, hh);
            mCalendar->Set(ICalendar::MINUTE, mm);
        }
        SetText(GetSmallTime());
    }
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
