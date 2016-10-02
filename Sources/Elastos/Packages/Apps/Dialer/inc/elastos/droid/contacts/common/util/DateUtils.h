
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_DATEUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_DATEUTILS_H__

#include <Elastos.CoreLibrary.Text.h>
#include "_Elastos.Droid.Dialer.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Utility methods for processing dates.
 */
class DateUtils
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:

    /**
     * Parses the supplied string to see if it looks like a date.
     *
     * @param string The string representation of the provided date
     * @param mustContainYear If true, the string is parsed as a date containing a year. If false,
     * the string is parsed into a valid date even if the year field is missing.
     * @return A Calendar object corresponding to the date if the string is successfully parsed.
     * If not, null is returned.
     */
    static CARAPI_(AutoPtr<ICalendar>) ParseDate(
        /* [in] */ const String& string,
        /* [in] */ Boolean mustContainYear);

    static CARAPI_(Boolean) IsYearSet(
        /* [in] */ ICalendar* cal);

    /**
     * Same as {@link #formatDate(Context context, String string, boolean longForm)}, with
     * longForm set to {@code true} by default.
     *
     * @param context Valid context
     * @param string String representation of a date to parse
     * @return Returns the same date in a cleaned up format. If the supplied string does not look
     * like a date, return it unchanged.
     */

    static CARAPI_(String) FormatDate(
        /* [in] */ IContext* context,
        /* [in] */ const String& string);

    /**
     * Parses the supplied string to see if it looks like a date.
     *
     * @param context Valid context
     * @param string String representation of a date to parse
     * @param longForm If true, return the date formatted into its long string representation.
     * If false, return the date formatted using its short form representation (i.e. 12/11/2012)
     * @return Returns the same date in a cleaned up format. If the supplied string does not look
     * like a date, return it unchanged.
     */
    static CARAPI_(String) FormatDate(
        /* [in] */ IContext* context,
        /* [in] */ const String& string,
        /* [in] */ Boolean longForm);

    static CARAPI_(Boolean) IsMonthBeforeDay(
        /* [in] */ IContext* context);

    /**
     * Returns a SimpleDateFormat object without the year fields by using a regular expression
     * to eliminate the year in the string pattern. In the rare occurence that the resulting
     * pattern cannot be reconverted into a SimpleDateFormat, it uses the provided context to
     * determine whether the month field should be displayed before the day field, and returns
     * either "MMMM dd" or "dd MMMM" converted into a SimpleDateFormat.
     */
    static CARAPI_(AutoPtr<Elastos::Text::IDateFormat>) GetLocalizedDateFormatWithoutYear(
        /* [in] */ IContext* context);

    /**
     * Given a calendar (possibly containing only a day of the year), returns the earliest possible
     * anniversary of the date that is equal to or after the current point in time if the date
     * does not contain a year, or the date converted to the local time zone (if the date contains
     * a year.
     *
     * @param target The date we wish to convert(in the UTC time zone).
     * @return If date does not contain a year (year < 1900), returns the next earliest anniversary
     * that is after the current point in time (in the local time zone). Otherwise, returns the
     * adjusted Date in the local time zone.
     */
    static CARAPI_(AutoPtr<IDate>) GetNextAnnualDate(
        /* [in] */ ICalendar* target);

    /**
     * Determine the difference, in days between two dates.  Uses similar logic as the
     * {@link android.text.format.DateUtils.getRelativeTimeSpanString} method.
     *
     * @param time Instance of time object to use for calculations.
     * @param date1 First date to check.
     * @param date2 Second date to check.
     * @return The absolute difference in days between the two dates.
     */
    static CARAPI_(Int32) GetDayDifference(
        /* [in] */ ITime* time,
        /* [in] */ Int64 date1,
        /* [in] */ Int64 date2);

private:
    static CARAPI_(AutoPtr<ICalendar>) GetUtcDate(
        /* [in] */ IDate* date,
        /* [in] */ Boolean noYear);

    static CARAPI_(AutoPtr<ICalendar>) GetUtcDate(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 dayOfMonth);

public:
    static const AutoPtr<ITimeZone> UTC_TIMEZONE;

    /**
     * When parsing a date without a year, the system assumes 1970, which wasn't a leap-year.
     * Let's add a one-off hack for that day of the year
     */
    static const String NO_YEAR_DATE_FEB29TH;

private:
    // Variations of ISO 8601 date format.  Do not change the order - it does affect the
    // result in ambiguous cases.
    static const AutoPtr<ArrayOf<ISimpleDateFormat*> > DATE_FORMATS;

    static StaticInitializer sInitializer;
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_DATEUTILS_H__
