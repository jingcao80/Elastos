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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateDialogNormalizer.h"

using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ITimeZoneHelper;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

void DateDialogNormalizer::SetLimits(
    /* [in] */ IDatePicker* picker,
    /* [in] */ Int64 minMillis,
    /* [in] */ Int64 maxMillis)
{
    // DatePicker intervals are non inclusive, the DatePicker will throw an
    // exception when setting the min/max attribute to the current date
    // so make sure this never happens
    if (maxMillis <= minMillis) {
        return;
    }

    AutoPtr<ICalendar> minCal = TrimToDate(minMillis);
    AutoPtr<ICalendar> maxCal = TrimToDate(maxMillis);
    Int32 currentYear;
    picker->GetYear(&currentYear);
    Int32 currentMonth;
    picker->GetMonth(&currentMonth);
    Int32 currentDayOfMonth;
    picker->GetDayOfMonth(&currentDayOfMonth);
    Int32 year, month, dayOfMonth;
    maxCal->Get(ICalendar::YEAR, &year);
    maxCal->Get(ICalendar::MONTH, &month);
    maxCal->Get(ICalendar::DAY_OF_MONTH, &dayOfMonth);
    picker->UpdateDate(year,
            month,
            dayOfMonth);
    Int64 timeInMillis;
    minCal->GetTimeInMillis(&timeInMillis);
    picker->SetMinDate(timeInMillis);
    Int32 _year, _month, _dayOfMonth;
    minCal->Get(ICalendar::YEAR, &_year);
    minCal->Get(ICalendar::MONTH, &_month);
    minCal->Get(ICalendar::DAY_OF_MONTH, &_dayOfMonth);
    picker->UpdateDate(_year,
            _month,
            _dayOfMonth);
    Int64 _timeInMillis;
    maxCal->GetTimeInMillis(&_timeInMillis);
    picker->SetMaxDate(_timeInMillis);

    // Restore the current date, this will keep the min/max settings
    // previously set into account.
    picker->UpdateDate(currentYear, currentMonth, currentDayOfMonth);
}

AutoPtr<ICalendar> DateDialogNormalizer::TrimToDate(
    /* [in] */ Int64 time)
{
    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> gmtTZ;
    tzHelper->GetTimeZone(String("GMT"), (ITimeZone**)&gmtTZ);

    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> cal;
    helper->GetInstance(gmtTZ, (ICalendar**)&cal);
    cal->Clear();
    cal->SetTimeInMillis(time);
    AutoPtr<ICalendar> result;
    helper->GetInstance(gmtTZ, (ICalendar**)&result);
    result->Clear();
    Int32 year, month, dayOfMonth;
    cal->Get(ICalendar::YEAR, &year);
    cal->Get(ICalendar::MONTH, &month);
    cal->Get(ICalendar::DAY_OF_MONTH, &dayOfMonth);
    result->Set(year, month, dayOfMonth,
            0, 0, 0);
    return result;
}

/**
 * Normalizes an existing DateDialogPicker changing the default date if
 * needed to comply with the {@code min} and {@code max} attributes.
 */
void DateDialogNormalizer::Normalize(
    /* [in] */ IDatePicker* picker,
    /* [in] */ IDatePickerOnDateChangedListener* listener,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [in] */ Int64 minMillis,
    /* [in] */ Int64 maxMillis)
{
    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> gmtTZ;
    tzHelper->GetTimeZone(String("GMT"), (ITimeZone**)&gmtTZ);

    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> calendar;
    helper->GetInstance(gmtTZ, (ICalendar**)&calendar);
    calendar->Clear();
    calendar->Set(year, month, day, hour, minute, 0);
    Int64 timeInMillis;
    calendar->GetTimeInMillis(&timeInMillis);
    if (timeInMillis < minMillis) {
        calendar->Clear();
        calendar->SetTimeInMillis(minMillis);
    }
    else if (timeInMillis > maxMillis) {
        calendar->Clear();
        calendar->SetTimeInMillis(maxMillis);
    }

    Int32 _year, _month, _dayOfMonth;
    calendar->Get(ICalendar::YEAR, &_year);
    calendar->Get(ICalendar::MONTH, &_month);
    calendar->Get(ICalendar::DAY_OF_MONTH, &_dayOfMonth);
    picker->Init(
            _year, _month,
            _dayOfMonth, listener);

    SetLimits(picker, minMillis, maxMillis);
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
