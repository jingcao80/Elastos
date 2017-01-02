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
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/utility/DayOfMonthCursor.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(DayOfMonthCursor, MonthDisplayHelper, IDayOfMonthCursor)

DayOfMonthCursor::DayOfMonthCursor()
    : mRow(0)
    , mColumn(0)
{
}

ECode DayOfMonthCursor::constructor(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 dayOfMonth,
    /* [in] */ Int32 weekStartDay)
{
    FAIL_RETURN(MonthDisplayHelper::constructor(year, month, weekStartDay))
    GetRowOf(dayOfMonth, &mRow);
    GetColumnOf(dayOfMonth, &mColumn);
    return NOERROR;
}

ECode DayOfMonthCursor::GetSelectedRow(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mRow;
    return NOERROR;
}

ECode DayOfMonthCursor::GetSelectedColumn(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mColumn;
    return NOERROR;
}

ECode DayOfMonthCursor::SetSelectedRowColumn(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col)
{
    mRow = row;
    mColumn = col;
    return NOERROR;
}

ECode DayOfMonthCursor::GetSelectedDayOfMonth(
    /* [out] */ Int32* value)
{
    return GetDayAt(mRow, mColumn, value);
}

ECode DayOfMonthCursor::GetSelectedMonthOffset(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Boolean bval;
    IsWithinCurrentMonth(mRow, mColumn, &bval);
    if (bval) {
        *value = 0;
        return NOERROR;
    }
    if (mRow == 0) {
        *value = -1;
        return NOERROR;
    }

    *value = 1;
    return NOERROR;
}

ECode DayOfMonthCursor::SetSelectedDayOfMonth(
    /* [in] */ Int32 dayOfMonth)
{
    GetRowOf(dayOfMonth, &mRow);
    GetColumnOf(dayOfMonth, &mColumn);
    return NOERROR;
}

ECode DayOfMonthCursor::IsSelected(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mRow == row) && (mColumn == col);
    return NOERROR;
}

ECode DayOfMonthCursor::Up(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    Boolean bval;
    IsWithinCurrentMonth(mRow - 1, mColumn, &bval);
    if (bval) {
        // within current month, just move up
        mRow--;
        return NOERROR;
    }
    // flip back to previous month, same column, first position within month
    PreviousMonth();
    mRow = 5;
    while(IsWithinCurrentMonth(mRow, mColumn, &bval), !bval) {
        mRow--;
    }
    *value = true;
    return NOERROR;
}

ECode DayOfMonthCursor::Down(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    Boolean bval;
    IsWithinCurrentMonth(mRow + 1, mColumn, &bval);
    if (bval) {
        // within current month, just move down
        mRow++;
        return NOERROR;
    }
    // flip to next month, same column, first position within month
    NextMonth();
    mRow = 0;
    while (IsWithinCurrentMonth(mRow, mColumn, &bval), !bval) {
        mRow++;
    }
    *value = TRUE;
    return NOERROR;
}

ECode DayOfMonthCursor::Left(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    if (mColumn == 0) {
        mRow--;
        mColumn = 6;
    } else {
        mColumn--;
    }

    Boolean bval;
    if (IsWithinCurrentMonth(mRow, mColumn, &bval), bval) {
        return NOERROR;
    }

    // need to flip to last day of previous month
    PreviousMonth();
    Int32 lastDay;
    GetNumberOfDaysInMonth(&lastDay);
    GetRowOf(lastDay, &mRow);
    GetColumnOf(lastDay, &mColumn);
    *value = TRUE;
    return NOERROR;
}

ECode DayOfMonthCursor::Right(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    if (mColumn == 6) {
        mRow++;
        mColumn = 0;
    }
    else {
        mColumn++;
    }

    Boolean bval;
    if (IsWithinCurrentMonth(mRow, mColumn, &bval), bval) {
        return NOERROR;
    }

    // need to flip to first day of next month
    NextMonth();
    mRow = 0;
    mColumn = 0;
    while (IsWithinCurrentMonth(mRow, mColumn, &bval), !bval) {
        mColumn++;
    }
    *value = TRUE;
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
