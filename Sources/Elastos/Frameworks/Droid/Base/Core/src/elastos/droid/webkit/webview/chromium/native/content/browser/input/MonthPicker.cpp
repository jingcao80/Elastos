
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/MonthPicker.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"
#include <elastos/core/Math.h>
#include "elastos/utility/Arrays.h"

using Elastos::Droid::Webkit::Webview::Chromium::Content::R;
using Elastos::Text::IDateFormatSymbols;
using Elastos::Text::IDateFormatSymbolsHelper;
using Elastos::Text::CDateFormatSymbolsHelper;
using Elastos::Utility::Arrays;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

const Int32 MonthPicker::MONTHS_NUMBER;

MonthPicker::MonthPicker(
    /* [in] */ IContext* context,
    /* [in] */ Double minValue,
    /* [in] */ Double maxValue)
    : TwoFieldDatePicker(context, minValue, maxValue)
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String str;
    resources->GetString(R::string::accessibility_date_picker_month, &str);
    AutoPtr<ICharSequence> cs;
    CString::New(str, (ICharSequence**)&cs);
    IView::Probe(GetPositionInYearSpinner())->SetContentDescription(cs);

    // initialization based on locale
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);
    AutoPtr<IDateFormatSymbolsHelper> dfsHelper;
    CDateFormatSymbolsHelper::AcquireSingleton((IDateFormatSymbolsHelper**)&dfsHelper);
    AutoPtr<IDateFormatSymbols> symbols;
    dfsHelper->GetInstance(locale, (IDateFormatSymbols**)&symbols);
    symbols->GetShortMonths((ArrayOf<String>**)&mShortMonths);

    // initialize to current date
    AutoPtr<ICalendarHelper> calendarHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calendarHelper);
    AutoPtr<ICalendar> cal;
    AutoPtr<ITimeZoneHelper> timeZoneHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&timeZoneHelper);
    AutoPtr<ITimeZone> timeZone;
    timeZoneHelper->GetTimeZone(String("UTC"), (ITimeZone**)&timeZone);
    calendarHelper->GetInstance(timeZone, (ICalendar**)&cal);
    Int32 year, month;
    cal->Get(ICalendar::YEAR, &year);
    cal->Get(ICalendar::MONTH, &month);
    Init(year, month, NULL);
}

/**
 * Creates a date object from the |value| which is months since epoch.
 */
AutoPtr<ICalendar> MonthPicker::CreateDateFromValue(
    /* [in] */ Double value)
{
    Int32 year = (Int32) Elastos::Core::Math::Min((Int32)(value / 12 + 1970), Elastos::Core::Math::INT32_MIN_VALUE);
    Int32 month = (Int32) ((Int32)value % 12);
    AutoPtr<ICalendarHelper> calendarHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calendarHelper);
    AutoPtr<ICalendar> cal;
    AutoPtr<ITimeZoneHelper> timeZoneHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&timeZoneHelper);
    AutoPtr<ITimeZone> timeZone;
    timeZoneHelper->GetTimeZone(String("UTC"), (ITimeZone**)&timeZone);
    calendarHelper->GetInstance(timeZone, (ICalendar**)&cal);
    cal->Clear();
    cal->Set(year, month, 1);
    return cal;
}

//@Override
AutoPtr<ICalendar> MonthPicker::GetDateForValue(
    /* [in] */ Double value)
{
    return MonthPicker::CreateDateFromValue(value);
}

//@Override
ECode MonthPicker::SetCurrentDate(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month)
{
    AutoPtr<ICalendarHelper> calendarHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calendarHelper);
    AutoPtr<ICalendar> date;
    AutoPtr<ITimeZoneHelper> timeZoneHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&timeZoneHelper);
    AutoPtr<ITimeZone> timeZone;
    calendarHelper->GetInstance(timeZone, (ICalendar**)&date);
    date->Set(year, month, 1);
    Boolean bFlag;
    if (date->IsBefore(GetMinDate(), &bFlag), bFlag) {
        TwoFieldDatePicker::SetCurrentDate(GetMinDate());
    }
    else if (date->IsAfter(GetMaxDate(), &bFlag), bFlag) {
        TwoFieldDatePicker::SetCurrentDate(GetMaxDate());
    }
    else {
        TwoFieldDatePicker::SetCurrentDate(date);
    }

    return NOERROR;
}

//@Override
void MonthPicker::UpdateSpinners()
{
    TwoFieldDatePicker::UpdateSpinners();

    // make sure the month names are a zero based array
    // with the months in the month spinner
    AutoPtr< ArrayOf<String> > displayedValues;
    Int32 minValue, maxValue;
    GetPositionInYearSpinner()->GetMinValue(&minValue);
    GetPositionInYearSpinner()->GetMaxValue(&maxValue);
    Arrays::CopyOfRange(mShortMonths,
             minValue,
             maxValue + 1,
             (ArrayOf<String>**)&displayedValues);
    GetPositionInYearSpinner()->SetDisplayedValues(displayedValues);
}

/**
 * @return The selected month.
 */
Int32 MonthPicker::GetMonth()
{
    Int32 month;
    GetCurrentDate()->Get(ICalendar::MONTH, &month);
    return month;
}

//@Override
Int32 MonthPicker::GetPositionInYear()
{
    return GetMonth();
}

//@Override
Int32 MonthPicker::GetMaxYear()
{
    Int32 year;
    GetMaxDate()->Get(ICalendar::YEAR, &year);
    return year;
}

//@Override
Int32 MonthPicker::GetMinYear()
{
    Int32 year;
    GetMinDate()->Get(ICalendar::YEAR, &year);
    return year;
}

//@Override
Int32 MonthPicker::GetMaxPositionInYear(
    /* [in] */ Int32 year)
{
    Int32 cYear;
    GetMaxDate()->Get(ICalendar::YEAR, &cYear);
    if (year == cYear) {
        Int32 month;
        GetMaxDate()->Get(ICalendar::MONTH, &month);
        return month;
    }

    return MONTHS_NUMBER - 1;
}

//@Override
Int32 MonthPicker::GetMinPositionInYear(
    /* [in] */ Int32 year)
{
    Int32 cYear;
    GetMinDate()->Get(ICalendar::YEAR, &cYear);
    if (year == cYear) {
        Int32 month;
        GetMinDate()->Get(ICalendar::MONTH, &month);
        return month;
    }

    return 0;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
