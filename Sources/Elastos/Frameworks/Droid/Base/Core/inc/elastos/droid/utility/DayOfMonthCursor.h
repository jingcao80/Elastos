
#ifndef __ELASTOS_DROID_UTILITY_DAY_OF_MONTH_CURSOR_H__
#define __ELASTOS_DROID_UTILITY_DAY_OF_MONTH_CURSOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/MonthDisplayHelper.h"

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * Helps control and display a month view of a calendar that has a current
 * selected day.
 * <ul>
 *   <li>Keeps track of current month, day, year</li>
 *   <li>Keeps track of current cursor position (row, column)</li>
 *   <li>Provides methods to help display the calendar</li>
 *   <li>Provides methods to move the cursor up / down / left / right.</li>
 * </ul>
 *
 * This should be used by anyone who presents a month view to users and wishes
 * to behave consistently with other widgets and apps; if we ever change our
 * mind about when to flip the month, we can change it here only.
 *
 * @hide
 */
class DayOfMonthCursor
    : public MonthDisplayHelper
    , public IDayOfMonthCursor
{
public:
    CAR_INTERFACE_DECL()

    DayOfMonthCursor();

    /**
     * @param year The initial year.
     * @param month The initial month.
     * @param dayOfMonth The initial dayOfMonth.
     * @param weekStartDay What dayOfMonth of the week the week should start,
     *   in terms of {@link java.util.Calendar} constants such as
     *   {@link java.util.Calendar#SUNDAY}.
     */
    CARAPI constructor(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 dayOfMonth,
        /* [in] */ Int32 weekStartDay);

    CARAPI GetSelectedRow(
        /* [out] */ Int32* value);

    CARAPI GetSelectedColumn(
        /* [out] */ Int32* value);

    CARAPI SetSelectedRowColumn(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col);

    CARAPI GetSelectedDayOfMonth(
        /* [out] */ Int32* value);

    /**
     * @return 0 if the selection is in the current month, otherwise -1 or +1
     * depending on whether the selection is in the first or last row.
     */
    CARAPI GetSelectedMonthOffset(
        /* [out] */ Int32* value);

    CARAPI SetSelectedDayOfMonth(
        /* [in] */ Int32 dayOfMonth);

    CARAPI IsSelected(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col,
        /* [out] */ Boolean* result);

    /**
     * Move up one box, potentially flipping to the previous month.
     * @return Whether the month was flipped to the previous month
     *   due to the move.
     */
    CARAPI Up(
        /* [out] */ Boolean* value);

    /**
     * Move down one box, potentially flipping to the next month.
     * @return Whether the month was flipped to the next month
     *   due to the move.
     */
    CARAPI Down(
        /* [out] */ Boolean* value);

    /**
     * Move left one box, potentially flipping to the previous month.
     * @return Whether the month was flipped to the previous month
     *   due to the move.
     */
    CARAPI Left(
        /* [out] */ Boolean* value);

    /**
     * Move right one box, potentially flipping to the next month.
     * @return Whether the month was flipped to the next month
     *   due to the move.
     */
    CARAPI Right(
        /* [out] */ Boolean* value);

private:
    Int32 mRow;
    Int32 mColumn;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_DAY_OF_MONTH_CURSOR_H__
