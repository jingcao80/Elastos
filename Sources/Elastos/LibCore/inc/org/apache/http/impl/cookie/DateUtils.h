
#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_DATEUTILS_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_DATEUTILS_H__

#include "elastos/core/Object.h"

using Elastos::Utility::ITimeZone;
using Elastos::Utility::IDate;
using Elastos::Text::ISimpleDateFormat;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

/**
 * A utility class for parsing and formatting HTTP dates as used in cookies and
 * other headers.  This class handles dates as defined by RFC 2616 section
 * 3.3.1 as well as some other common non-standard formats.
 *
 * @author Christopher Brown
 * @author Michael Becke
 */
class DateUtils
    : public Object
{
private:
    /**
     * A factory for {@link SimpleDateFormat}s. The instances are stored in a
     * threadlocal way because SimpleDateFormat is not threadsafe as noted in
     * {@link SimpleDateFormat its javadoc}.
     *
     * @author Daniel Mueller
     */
    class DateFormatHolder
    {
    public:
        /**
         * creates a {@link SimpleDateFormat} for the requested format string.
         *
         * @param pattern
         *            a non-<code>null</code> format String according to
         *            {@link SimpleDateFormat}. The format is not checked against
         *            <code>null</code> since all paths go through
         *            {@link DateUtils}.
         * @return the requested format. This simple dateformat should not be used
         *         to {@link SimpleDateFormat#applyPattern(String) apply} to a
         *         different pattern.
         */
        static CARAPI_(AutoPtr<ISimpleDateFormat>) FormatFor(
            /* [in] */ const String& pattern);

    private:
        // static final ThreadLocal<SoftReference<Map<String, SimpleDateFormat>>>
        //     THREADLOCAL_FORMATS = new ThreadLocal<SoftReference<Map<String, SimpleDateFormat>>>();
    };

public:
    /**
     * Parses a date value.  The formats used for parsing the date value are retrieved from
     * the default http params.
     *
     * @param dateValue the date value to parse
     *
     * @return the parsed date
     *
     * @throws DateParseException if the value could not be parsed using any of the
     * supported date formats
     */
    static CARAPI ParseDate(
        /* [in] */ const String& dateValue,
        /* [out] */ IDate** date);

    /**
     * Parses the date value using the given date formats.
     *
     * @param dateValue the date value to parse
     * @param dateFormats the date formats to use
     *
     * @return the parsed date
     *
     * @throws DateParseException if none of the dataFormats could parse the dateValue
     */
    static CARAPI ParseDate(
        /* [in] */ const String& dateValue,
        /* [in] */ ArrayOf<String>* dateFormats,
        /* [out] */ IDate** date);

    /**
     * Parses the date value using the given date formats.
     *
     * @param dateValue the date value to parse
     * @param dateFormats the date formats to use
     * @param startDate During parsing, two digit years will be placed in the range
     * <code>startDate</code> to <code>startDate + 100 years</code>. This value may
     * be <code>null</code>. When <code>null</code> is given as a parameter, year
     * <code>2000</code> will be used.
     *
     * @return the parsed date
     *
     * @throws DateParseException if none of the dataFormats could parse the dateValue
     */
    static CARAPI ParseDate(
        /* [in] */ const String& dateValue,
        /* [in] */ ArrayOf<String>* dateFormats,
        /* [in] */ IDate* startDate,
        /* [out] */ IDate** date);

    /**
     * Formats the given date according to the RFC 1123 pattern.
     *
     * @param date The date to format.
     * @return An RFC 1123 formatted date string.
     *
     * @see #PATTERN_RFC1123
     */
    static CARAPI FormatDate(
        /* [in] */ IDate* date,
        /* [out] */ String* formatDate);

    /**
     * Formats the given date according to the specified pattern.  The pattern
     * must conform to that used by the {@link SimpleDateFormat simple date
     * format} class.
     *
     * @param date The date to format.
     * @param pattern The pattern to use for formatting the date.
     * @return A formatted date string.
     *
     * @throws IllegalArgumentException If the given date pattern is invalid.
     *
     * @see SimpleDateFormat
     */
    static CARAPI FormatDate(
        /* [in] */ IDate* date,
        /* [in] */ const String& pattern,
        /* [out] */ String* formatDate);

private:
    /** This class should not be instantiated. */
    DateUtils() {}

public:
    /**
     * Date format pattern used to parse HTTP date headers in RFC 1123 format.
     */
    static const String PATTERN_RFC1123;

    /**
     * Date format pattern used to parse HTTP date headers in RFC 1036 format.
     */
    static const String PATTERN_RFC1036;

    /**
     * Date format pattern used to parse HTTP date headers in ANSI C
     * <code>asctime()</code> format.
     */
    static const String PATTERN_ASCTIME;

    static const AutoPtr<ITimeZone> GMT;

private:
    static const AutoPtr< ArrayOf<String> > DEFAULT_PATTERNS;

    static const AutoPtr<IDate> DEFAULT_TWO_DIGIT_YEAR_START;
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_DATEUTILS_H__
