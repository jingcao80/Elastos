#ifndef __ELASTOS_DROID_TEXT_FORMAT_DATEFORMAT_H__
#define __ELASTOS_DROID_TEXT_FORMAT_DATEFORMAT_H__

#include <Elastos.CoreLibrary.Text.h>
#include "Elastos.Droid.Core.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IDate;
using Elastos::Utility::ILocale;
using Libcore::ICU::ILocaleData;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

class ECO_PUBLIC DateFormat
{
public:
    /**
     * Returns true if user preference is set to 24-hour format.
     * @param context the context to use for the content resolver
     * @return true if 24 hour time format is selected, false otherwise.
     */
    static CARAPI_(Boolean) Is24HourFormat(
        /* [in] */ IContext* context);

    static CARAPI_(String) GetBestDateTimePattern(
        /* [in] */ ILocale* locale,
        /* [in] */ const String& skeleton);

    /**
     * Returns a {@link java.text.DateFormat} object that can format the time according
     * to the current locale and the user's 12-/24-hour clock preference.
     * @param context the application context
     * @return the {@link java.text.DateFormat} object that properly formats the time.
     */
    static CARAPI_(AutoPtr<Elastos::Text::IDateFormat>) GetTimeFormat(
        /* [in] */  IContext* context);

    /**
     * Returns a String pattern that can be used to format the time according
     * to the current locale and the user's 12-/24-hour clock preference.
     * @param context the application context
     * @hide
     */
    static CARAPI_(String) GetTimeFormatString(
        /* [in] */ IContext* context);

    /**
     * Returns a {@link java.text.DateFormat} object that can format the date
     * in short form (such as 12/31/1999) according
     * to the current locale and the user's date-order preference.
     * @param context the application context
     * @return the {@link java.text.DateFormat} object that properly formats the date.
     */
    static CARAPI_(AutoPtr<Elastos::Text::IDateFormat>) GetDateFormat(
        /* [in] */ IContext* context);

    /**
     * Returns a {@link java.text.DateFormat} object to format the date
     * as if the date format setting were set to <code>value</code>,
     * including null to use the locale's default format.
     * @param context the application context
     * @param value the date format setting string to interpret for
     *              the current locale
     * @hide
     */
    static CARAPI_(AutoPtr<Elastos::Text::IDateFormat>) GetDateFormatForSetting(
        /* [in] */ IContext* context,
        /* [in] */ const String& value);

    /**
     * Returns a {@link java.text.DateFormat} object that can format the date
     * in long form (such as {@code Monday, January 3, 2000}) for the current locale.
     * @param context the application context
     * @return the {@link java.text.DateFormat} object that formats the date in long form.
     */
    static CARAPI_(AutoPtr<Elastos::Text::IDateFormat>) GetLongDateFormat(
        /* [in] */ IContext* context);

    /**
     * Returns a {@link java.text.DateFormat} object that can format the date
     * in medium form (such as {@code Jan 3, 2000}) for the current locale.
     * @param context the application context
     * @return the {@link java.text.DateFormat} object that formats the date in long form.
     */
    static CARAPI_(AutoPtr<Elastos::Text::IDateFormat>) GetMediumDateFormat(
        /* [in] */ IContext* context);

    /**
     * Gets the current date format stored as a char array. The array will contain
     * 3 elements ({@link #DATE}, {@link #MONTH}, and {@link #YEAR}) in the order
     * specified by the user's format preference.  Note that this order is
     * only appropriate for all-numeric dates; spelled-out (MEDIUM and LONG)
     * dates will generally contain other punctuation, spaces, or words,
     * not just the day, month, and year, and not necessarily in the same
     * order returned here.
     */
    static CARAPI GetDateFormatOrder(
        /* [in] */ IContext* context,
        /* [out] */ ArrayOf<Char32>** ret);

    /**
     * Given a format string and a time in milliseconds since Jan 1, 1970 GMT, returns a
     * CharSequence containing the requested date.
     * @param inFormat the format string, as described in {@link android.text.format.DateFormat}
     * @param inTimeInMillis in milliseconds since Jan 1, 1970 GMT
     * @return a {@link CharSequence} containing the requested text
     */
    static CARAPI_(AutoPtr<ICharSequence>) Format(
        /* [in] */ ICharSequence* inFormat,
        /* [in] */ Int64 inTimeInMillis);

    /**
     * Given a format string and a {@link java.util.Date} object, returns a CharSequence containing
     * the requested date.
     * @param inFormat the format string, as described in {@link android.text.format.DateFormat}
     * @param inDate the date to format
     * @return a {@link CharSequence} containing the requested text
     */
    static CARAPI_(AutoPtr<ICharSequence>) Format(
        /* [in] */ ICharSequence* inFormat,
        /* [in] */ IDate* inDate);

    static CARAPI_(AutoPtr<ICharSequence>) Format(
        /* [in] */ const String& format,
        /* [in] */ IDate* inDate);

    /**
     * Indicates whether the specified format string contains seconds.
     *
     * Always returns false if the input format is null.
     *
     * @param inFormat the format string, as described in {@link android.text.format.DateFormat}
     *
     * @return true if the format string contains {@link #SECONDS}, false otherwise
     *
     * @hide
     */
    static CARAPI_(Boolean) HasSeconds(
        /* [in] */ ICharSequence* inFormat);

    /**
     * Indicates whether the specified format string contains seconds.
     *
     * Always returns false if the input format is null.
     *
     * @param inFormat the format string, as described in {@link android.text.format.DateFormat}
     *
     * @return true if the format string contains {@link #SECONDS}, false otherwise
     *
     * @hide
     */
    static CARAPI_(Boolean) HasDesignator(
        /* [in] */ ICharSequence* inFormat,
        /* [in] */ Char32 designator);

    /**
     * Given a format string and a {@link java.util.Calendar} object, returns a CharSequence
     * containing the requested date.
     * @param inFormat the format string, as described in {@link android.text.format.DateFormat}
     * @param inDate the date to format
     * @return a {@link CharSequence} containing the requested text
     */
    static CARAPI_(AutoPtr<ICharSequence>) Format(
        /* [in] */ ICharSequence* inFormat,
        /* [in] */ ICalendar* inDate);

private:
    ECO_LOCAL static CARAPI_(String) GetDateFormatStringForSetting(
        /* [in] */ IContext* context,
        /* [in] */ const String& value);

    ECO_LOCAL static CARAPI_(String) GetDateFormatString(
        /* [in] */ IContext* context);

    ECO_LOCAL static CARAPI_(Int32) SkipQuotedText(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 i,
        /* [in] */ Int32 len);

    ECO_LOCAL static CARAPI_(String) GetDayOfWeekString(
        /* [in] */ ILocaleData* ld,
        /* [in] */ Int32 day,
        /* [in] */ Int32 count,
        /* [in] */ Int32 kind);

    ECO_LOCAL static CARAPI_(String) GetMonthString(
        /* [in] */ ILocaleData* ld,
        /* [in] */ Int32 month,
        /* [in] */ Int32 count,
        /* [in] */ Int32 kind);

    ECO_LOCAL static CARAPI_(String) GetTimeZoneString(
        /* [in] */ ICalendar* inDate,
        /* [in] */ Int32 count);

    ECO_LOCAL static CARAPI_(String) FormatZoneOffset(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    ECO_LOCAL static CARAPI_(String) GetYearString(
        /* [in] */ Int32 year,
        /* [in] */ Int32 count);

    ECO_LOCAL static CARAPI_(Int32) AppendQuotedText(
        /* [in] */ ISpannableStringBuilder* s,
        /* [in] */ Int32 i,
        /* [in] */ Int32 len);

    ECO_LOCAL static CARAPI_(String) ZeroPad(
        /* [in] */ Int32 inValue,
        /* [in] */ Int32 inMinDigits);

private:
    DateFormat();

private:
    ECO_LOCAL static Object sLocaleLock;
    ECO_LOCAL static AutoPtr<ILocale> sIs24HourLocale;
    ECO_LOCAL static Boolean sIs24Hour;
};

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_FORMAT_DATEFORMAT_H__
