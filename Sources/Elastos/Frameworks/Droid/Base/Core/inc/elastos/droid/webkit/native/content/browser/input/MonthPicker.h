
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_MONTHPICKER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_MONTHPICKER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/input/TwoFieldDatePicker.h"

using Elastos::Droid::Content::IContext;

// import org.chromium.content.R;

// import java.text.DateFormatSymbols;
// import java.util.Arrays;
// import java.util.Calendar;
// import java.util.Locale;
// import java.util.TimeZone;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ITimeZone;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

class MonthPicker : public TwoFieldDatePicker
{
public:
    MonthPicker(
        /* [in] */ IContext* context,
        /* [in] */ Double minValue,
        /* [in] */ Double maxValue);

    /**
     * Creates a date object from the |value| which is months since epoch.
     */
    static CARAPI_(AutoPtr<ICalendar>) CreateDateFromValue(
        /* [in] */ Double value);

    /**
     * @return The selected month.
     */
    CARAPI_(Int32) GetMonth();

    //@Override
    CARAPI_(Int32) GetPositionInYear();

protected:
    //@Override
    CARAPI_(AutoPtr<ICalendar>) GetDateForValue(
        /* [in] */ Double value);

    //@Override
    CARAPI SetCurrentDate(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month);

    //@Override
    CARAPI_(void) UpdateSpinners();

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
    static const Int32 MONTHS_NUMBER = 12;

    /*const*/ AutoPtr< ArrayOf<String> > mShortMonths;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_MONTHPICKER_H__
