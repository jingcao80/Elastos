
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/utility/MonthDisplayHelper.h"

using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(MonthDisplayHelper, Object, IMonthDisplayHelper)

MonthDisplayHelper::MonthDisplayHelper()
    : mWeekStartDay(0)
    , mNumDaysInMonth(0)
    , mNumDaysInPrevMonth(0)
    , mOffset(0)
{}

ECode MonthDisplayHelper::constructor(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 weekStartDay)
{
    if (weekStartDay < ICalendar::SUNDAY || weekStartDay > ICalendar::SATURDAY) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mWeekStartDay = weekStartDay;

    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    helper->GetInstance((ICalendar**)&mCalendar);
    mCalendar->Set(ICalendar::YEAR, year);
    mCalendar->Set(ICalendar::MONTH, month);
    mCalendar->Set(ICalendar::DAY_OF_MONTH, 1);
    mCalendar->Set(ICalendar::HOUR_OF_DAY, 0);
    mCalendar->Set(ICalendar::MINUTE, 0);
    mCalendar->Set(ICalendar::SECOND, 0);
    Int64 tm;
    mCalendar->GetTimeInMillis(&tm);

    return Recalculate();
}

ECode MonthDisplayHelper::constructor(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month)
{
    return constructor(year, month, ICalendar::SUNDAY);
}

ECode MonthDisplayHelper::GetYear(
    /* [out] */ Int32* value)
{
    return mCalendar->Get(ICalendar::YEAR, value);
}

ECode MonthDisplayHelper::GetMonth(
    /* [out] */ Int32* value)
{
    return mCalendar->Get(ICalendar::MONTH, value);
}

ECode MonthDisplayHelper::GetWeekStartDay(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mWeekStartDay;
    return NOERROR;
}

ECode MonthDisplayHelper::GetFirstDayOfMonth(
    /* [out] */ Int32* value)
{
    return mCalendar->Get(ICalendar::DAY_OF_WEEK, value);
}

ECode MonthDisplayHelper::GetNumberOfDaysInMonth(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mNumDaysInMonth;
    return NOERROR;
}

ECode MonthDisplayHelper::GetOffset(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mOffset;
    return NOERROR;
}

ECode MonthDisplayHelper::GetDigitsForRow(
    /* [in] */ Int32 row,
    /* [out, callee] */ ArrayOf<Int32>** outarr)
{
    VALIDATE_NOT_NULL(outarr)
    *outarr = NULL;

    if (row < 0 || row > 5) {
        // throw new IllegalArgumentException("row " + row
        //         + " out of range (0-5)");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(7);
    for (Int32 column = 0; column < 7; column++) {
        GetDayAt(row, column, &(*result)[column]);
    }

    *outarr = result;
    REFCOUNT_ADD(*outarr)
    return NOERROR;
}

ECode MonthDisplayHelper::GetDayAt(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    if (row == 0 && column < mOffset) {
        *value = mNumDaysInPrevMonth + column - mOffset + 1;
        return NOERROR;
    }

    Int32 day = 7 * row + column - mOffset + 1;
    *value = (day > mNumDaysInMonth) ? day - mNumDaysInMonth : day;
    return NOERROR;
}

ECode MonthDisplayHelper::GetRowOf(
    /* [in] */ Int32 day,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = (day + mOffset - 1) / 7;
    return NOERROR;
}

ECode MonthDisplayHelper::GetColumnOf(
    /* [in] */ Int32 day,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = (day + mOffset - 1) % 7;
    return NOERROR;
}

ECode MonthDisplayHelper::PreviousMonth()
{
    mCalendar->Add(ICalendar::MONTH, -1);
    return Recalculate();
}

ECode MonthDisplayHelper::NextMonth()
{
    mCalendar->Add(ICalendar::MONTH, 1);
    return Recalculate();
}

ECode MonthDisplayHelper::IsWithinCurrentMonth(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    if (row < 0 || column < 0 || row > 5 || column > 6) {
        return NOERROR;
    }

    if (row == 0 && column < mOffset) {
        return NOERROR;
    }

    Int32 day = 7 * row + column - mOffset + 1;
    if (day > mNumDaysInMonth) {
        return NOERROR;
    }
    *value = TRUE;
    return NOERROR;
}

ECode MonthDisplayHelper::Recalculate()
{
    mCalendar->GetActualMaximum(ICalendar::DAY_OF_MONTH, &mNumDaysInMonth);

    mCalendar->Add(ICalendar::MONTH, -1);
    mCalendar->GetActualMaximum(ICalendar::DAY_OF_MONTH, &mNumDaysInPrevMonth);
    mCalendar->Add(ICalendar::MONTH, 1);

    Int32 firstDayOfMonth;
    GetFirstDayOfMonth(&firstDayOfMonth);
    Int32 offset = firstDayOfMonth - mWeekStartDay;
    if (offset < 0) {
        offset += 7;
    }
    mOffset = offset;
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
