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

#ifndef __ELASTOS_DROID_UTILITY_MONTH_DISPLAY_HELPER_H__
#define __ELASTOS_DROID_UTILITY_MONTH_DISPLAY_HELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * Helps answer common questions that come up when displaying a month in a
 * 6 row calendar grid format.
 *
 * Not thread safe.
 */
class MonthDisplayHelper
    : public Object
    , public IMonthDisplayHelper
{
public:
    CAR_INTERFACE_DECL()

    MonthDisplayHelper();

    /**
     * @param year The year.
     * @param month The month.
     * @param weekStartDay What day of the week the week should start.
     */
    CARAPI constructor(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 weekStartDay);

    CARAPI constructor(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month);

    CARAPI GetYear(
        /* [out] */ Int32* value);

    CARAPI GetMonth(
        /* [out] */ Int32* value);

    CARAPI GetWeekStartDay(
        /* [out] */ Int32* value);

    /**
     * @return The first day of the month using a constants such as
     *   {@link java.util.Calendar#SUNDAY}.
     */
    CARAPI GetFirstDayOfMonth(
        /* [out] */ Int32* value);

    /**
     * @return The number of days in the month.
     */
    CARAPI GetNumberOfDaysInMonth(
        /* [out] */ Int32* value);

    /**
     * @return The offset from displaying everything starting on the very first
     *   box.  For example, if the calendar is set to display the first day of
     *   the week as Sunday, and the month starts on a Wednesday, the offset is 3.
     */
    CARAPI GetOffset(
        /* [out] */ Int32* value);

    /**
     * @param row Which row (0-5).
     * @return the digits of the month to display in one
     * of the 6 rows of a calendar month display.
     */
    CARAPI GetDigitsForRow(
        /* [in] */ Int32 row,
        /* [out, callee] */ ArrayOf<Int32>** outarr);

    /**
     * @param row The row, 0-5, starting from the top.
     * @param column The column, 0-6, starting from the left.
     * @return The day at a particular row, column
     */
    CARAPI GetDayAt(
        /* [in] */ Int32 row,
        /* [in] */ Int32 column,
        /* [out] */ Int32* value);

    /**
     * @return Which row day is in.
     */
    CARAPI GetRowOf(
        /* [in] */ Int32 day,
        /* [out] */ Int32* value);

    /**
     * @return Which column day is in.
     */
    CARAPI GetColumnOf(
        /* [in] */ Int32 day,
        /* [out] */ Int32* value);

    /**
     * Decrement the month.
     */
    CARAPI PreviousMonth();

    /**
     * Increment the month.
     */
    CARAPI NextMonth();

    /**
     * @return Whether the row and column fall within the month.
     */
    CARAPI IsWithinCurrentMonth(
        /* [in] */ Int32 row,
        /* [in] */ Int32 column,
        /* [out] */ Boolean* value);

private:
    // helper method that recalculates cached values based on current month / year
    CARAPI Recalculate();

private:
    // display pref
    Int32 mWeekStartDay;

    // holds current month, year, helps compute display
    AutoPtr<ICalendar> mCalendar;

    // cached computed stuff that helps with display
    Int32 mNumDaysInMonth;
    Int32 mNumDaysInPrevMonth;
    Int32 mOffset;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_MONTH_DISPLAY_HELPER_H__
