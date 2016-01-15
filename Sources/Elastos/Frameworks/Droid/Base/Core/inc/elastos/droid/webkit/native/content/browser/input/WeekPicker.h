
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_WEEKPICKER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_WEEKPICKER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/input/TwoFieldDatePicker.h"

using Elastos::Droid::Content::IContext;

// import org.chromium.content.R;

using Elastos::Utility::ICalendar;
using Elastos::Utility::ITimeZone;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

// This class is heavily based on android.widget.DatePicker.
class WeekPicker : public TwoFieldDatePicker
{
public:
    WeekPicker(
        /* [in] */ IContext* context,
        /* [in] */ Double minValue,
        /* [in] */ Double maxValue);

    /**
     * Creates a date object from the |year| and |week|.
     */
    static CARAPI_(AutoPtr<ICalendar>) CreateDateFromWeek(
        /* [in] */ Int32 year,
        /* [in] */ Int32 week);

    /**
     * Creates a date object from the |value| which is milliseconds since epoch.
     */
    static CARAPI_(AutoPtr<ICalendar>) CreateDateFromValue(
        /* [in] */ Double value);

    static CARAPI_(Int32) GetISOWeekYearForDate(
        /* [in] */ ICalendar* date);

    static CARAPI_(Int32) GetWeekForDate(
        /* [in] */ ICalendar* date);

    /**
     * @return The selected year.
     */
    //@Override
    CARAPI_(Int32) GetYear();

    /**
     * @return The selected week.
     */
    CARAPI_(Int32) GetWeek();

    //@Override
    CARAPI_(Int32) GetPositionInYear();

protected:
    //@Override
    CARAPI_(AutoPtr<ICalendar>) GetDateForValue(
        /* [in] */ Double value);

    //@Override
    CARAPI SetCurrentDate(
        /* [in] */ Int32 year,
        /* [in] */ Int32 week);

    //@Override
    CARAPI_(Int32) GetMaxYear();

    //@Override
    CARAPI_(Int32) GetMinYear();

    //@Override
    CARAPI_(Int32) GetMaxPositionInYear(
        /* [in] */ Int32 year);

    //@Override
    CARAPI_(Int32) GetMinPositionInYear(
        /* [in] */ Int32 year);

private:
    CARAPI_(Int32) GetNumberOfWeeks(
        /* [in] */ Int32 year);
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_WEEKPICKER_H__
