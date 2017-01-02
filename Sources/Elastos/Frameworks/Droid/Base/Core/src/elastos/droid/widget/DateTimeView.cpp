//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/widget/DateTimeView.h"
#include "elastos/droid/text/format/CTime.h"
#include "elastos/droid/text/format/CDateFormat.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"

#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Widget::TextView;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::IDateFormatHelper;
using Elastos::Text::CDateFormatHelper;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int64 DateTimeView::TWELVE_HOURS_IN_MINUTES = 12 * 60;
const Int64 DateTimeView::TWENTY_FOUR_HOURS_IN_MILLIS = 24 * 60 * 60 * 1000;

const Int32 DateTimeView::SHOW_TIME = 0;
const Int32 DateTimeView::SHOW_MONTH_DAY_YEAR = 1;

//=====================================================================
//              DateTimeView::DateTimeViewReceiver
//=====================================================================
DateTimeView::DateTimeViewReceiver::DateTimeViewReceiver(
    /* [in] */ DateTimeView* host) :
    mHost(host)
{
}

ECode DateTimeView::DateTimeViewReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_TIME_TICK)) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now = 0;
        system->GetCurrentTimeMillis(&now);
        if (now < mHost->mUpdateTimeMillis) {
            return NOERROR;
        }
    }
    mHost->mLastFormat = NULL;
    mHost->Update();
    return NOERROR;
}

//=====================================================================
//              DateTimeView::DateTimeViewObserver
//=====================================================================
DateTimeView::DateTimeViewObserver::DateTimeViewObserver(
    /* [in] */ DateTimeView* host) :
    mHost(host)
{
}

ECode DateTimeView::DateTimeViewObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->mLastFormat = NULL;
    mHost->Update();
    return NOERROR;
}

//========================================================================================
//              DateTimeView::
//========================================================================================
CAR_INTERFACE_IMPL(DateTimeView, TextView, IDateTimeView)

DateTimeView::DateTimeView()
    : mAttachedToWindow(FALSE)
    , mTimeMillis(0)
    , mLastDisplay(-1)
{
    mBroadcastReceiver = new DateTimeViewReceiver(this);
}

ECode DateTimeView::constructor(
    /* [in] */ IContext* context)
{
    return TextView::constructor(context);
}

ECode DateTimeView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TextView::constructor(context, attrs);
}

ECode DateTimeView::OnAttachedToWindow()
{
    TextView::OnAttachedToWindow();
    RegisterReceivers();
    mAttachedToWindow = TRUE;
    return NOERROR;
}

ECode DateTimeView::OnDetachedFromWindow()
{
    TextView::OnDetachedFromWindow();
    UnRegisterReceivers();
    mAttachedToWindow = FALSE;
    return NOERROR;
}

ECode DateTimeView::SetTime(
    /* [in] */ Int64 time)
{
    AutoPtr<ITime> inTime;
    CTime::New((ITime**)&inTime);
    inTime->Set(time);
    inTime->SetSecond(0);
    inTime->ToMillis(FALSE, &mTimeMillis);
    Int32 year, month, monthDay, hour, minute;
    inTime->GetYear(&year);
    inTime->GetMonth(&month);
    inTime->GetMonthDay(&monthDay);
    inTime->GetHour(&hour);
    inTime->GetMinute(&minute);
    mTime = NULL;
    ASSERT_SUCCEEDED(CDate::New((year - 1900), month, monthDay, hour, minute, 0, (IDate**)&mTime));

    Update();
    return NOERROR;
}

ECode DateTimeView::Update()
{
    if (!mTime) {
        return NOERROR;
    }

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 start = 0;
    system->GetCurrentTimeMillis(&start);

    Int32 display = 0;
    AutoPtr<IDate> date = mTime;
    AutoPtr<ITime> time;
    ASSERT_SUCCEEDED(CTime::New((ITime**)&time));
    time->Set(mTimeMillis);
    time->SetSecond(0);
    Int32 hour = 0;
    time->GetHour(&hour);
    time->SetHour(hour-12);
    Int64 twelveHoursBefore = 0;
    time->ToMillis(FALSE, &twelveHoursBefore);
    Int32 hour1 = 0;
    time->GetHour(&hour1);
    time->SetHour(hour1+12);
    Int64 twelveHoursAfter = 0;
    time->ToMillis(FALSE, &twelveHoursAfter);
    time->SetHour(0);
    time->SetMinute(0);
    Int64 midnightBefore = 0;
    time->ToMillis(FALSE, &midnightBefore);
    Int32 monthDay = 0;
    time->GetMonthDay(&monthDay);
    time->SetMonthDay(monthDay + 1);
    Int64 midnightAfter = 0;
    time->ToMillis(FALSE, &midnightAfter);

    Int64 nowMillis = 0;
    system->GetCurrentTimeMillis(&nowMillis);
    time->Set(nowMillis);
    time->SetSecond(0);
    time->Normalize(FALSE, &nowMillis);

    if((nowMillis >= midnightBefore && nowMillis < midnightAfter)
            || (nowMillis >= twelveHoursBefore && nowMillis < twelveHoursAfter)) {
        display = SHOW_TIME;
    }
    else {
        display = SHOW_MONTH_DAY_YEAR;
    }

    AutoPtr<IDateFormat> format;
    if(display == mLastDisplay && mLastFormat != NULL) {
        format = mLastFormat;
    }
    else {
        switch (display){
            case SHOW_TIME:
                format = GetTimeFormat();
                break;
            case SHOW_MONTH_DAY_YEAR:
                format = GetDateFormat();
                break;
            default:
                return E_RUNTIME_EXCEPTION;
                //throw new RuntimeException("unknown display value: " + display);
        }
       mLastFormat = format;
    }

    assert(format != NULL);
    String text;
    format->Format(mTime, &text);

    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(text, (ICharSequence**)&seq));
    TextView::SetText(seq);

    if (display == SHOW_TIME) {
        mUpdateTimeMillis = twelveHoursAfter > midnightAfter ? twelveHoursAfter: midnightAfter;
    }
    else {
        if (mTimeMillis < nowMillis) {
            mUpdateTimeMillis = 0;
        }
        else {
            mUpdateTimeMillis = twelveHoursBefore < midnightBefore ? twelveHoursBefore : midnightBefore;
        }
    }

    Int64 finish = 0;
    system->GetNanoTime(&finish);
    return NOERROR;
}

AutoPtr<IDateFormat> DateTimeView::GetTimeFormat()
{
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<Elastos::Droid::Text::Format::IDateFormat> df;
    Elastos::Droid::Text::Format::CDateFormat::AcquireSingleton((Elastos::Droid::Text::Format::IDateFormat**)&df);
    AutoPtr<IDateFormat> obj;
    df->GetTimeFormat(cxt, (IDateFormat**)&obj);
    return obj;
}

AutoPtr<IDateFormat> DateTimeView::GetDateFormat()
{
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetContentResolver((IContentResolver**)&resolver);
    String format;
    Settings::System::GetString(resolver, ISettingsSystem::DATE_FORMAT, &format);

    AutoPtr<IDateFormat> df;
    if (format.IsNullOrEmpty()) {
        AutoPtr<IDateFormatHelper> helper;
        CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&helper);
        helper->GetDateInstance(IDateFormat::SHORT, (IDateFormat**)&df);
    }
    else {
        //try {
        AutoPtr<ISimpleDateFormat> sf;
        CSimpleDateFormat::New(format, (ISimpleDateFormat**)&sf);
        df = IDateFormat::Probe(sf.Get());
       /* } catch (IllegalArgumentException e) {
            return DateFormat.getDateInstance(DateFormat.SHORT);
        }*/
    }
    return df;
}

ECode DateTimeView::RegisterReceivers()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IIntentFilter> filter;
    ASSERT_SUCCEEDED(CIntentFilter::New((IIntentFilter**)&filter));
    AutoPtr<IIntent> intent;
    ASSERT_SUCCEEDED(CIntent::New((IIntent**)&intent));
    filter->AddAction(intent->ACTION_TIME_TICK);
    filter->AddAction(intent->ACTION_TIME_CHANGED);
    filter->AddAction(intent->ACTION_CONFIGURATION_CHANGED);
    filter->AddAction(intent->ACTION_TIMEZONE_CHANGED);

    AutoPtr<IIntent> registerIntent;
    context->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&registerIntent);

    AutoPtr<IUri> uri;
    Settings::System::GetUriFor(ISettingsSystem::DATE_FORMAT, (IUri**)&uri);

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->RegisterContentObserver(uri, TRUE, mContentObserver);
    return NOERROR;
}

ECode DateTimeView::UnRegisterReceivers()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->UnregisterReceiver(mBroadcastReceiver);

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->UnregisterContentObserver(mContentObserver);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
