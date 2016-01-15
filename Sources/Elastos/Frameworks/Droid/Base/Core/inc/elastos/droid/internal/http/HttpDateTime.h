
#ifndef __ELASTOS_DROID_INTERNAL_HTTP_HTTPDATETIME_H__
#define __ELASTOS_DROID_INTERNAL_HTTP_HTTPDATETIME_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {

/**
 * Helper for parsing an HTTP date.
 */
class HttpDateTime
{
private:
    class TimeOfDay : public Object
    {
    public:
        TimeOfDay(
            /* [in] */ Int32 h,
            /* [in] */ Int32 m,
            /* [in] */ Int32 s);

    public:
        Int32 mHour;
        Int32 mMinute;
        Int32 mSecond;
    };

public:
    static CARAPI Parse(
        /* [in] */ const String& timeString,
        /* [out] */ Int64* time);

private:
    static CARAPI_(Int32) GetDate(
        /* [in] */ const String& dateString);

    /*
     * jan = 9 + 0 + 13 = 22
     * feb = 5 + 4 + 1 = 10
     * mar = 12 + 0 + 17 = 29
     * apr = 0 + 15 + 17 = 32
     * may = 12 + 0 + 24 = 36
     * jun = 9 + 20 + 13 = 42
     * jul = 9 + 20 + 11 = 40
     * aug = 0 + 20 + 6 = 26
     * sep = 18 + 4 + 15 = 37
     * oct = 14 + 2 + 19 = 35
     * nov = 13 + 14 + 21 = 48
     * dec = 3 + 4 + 2 = 9
     */
    static CARAPI GetMonth(
        /* [in] */ const String& monthString,
        /* [out] */ Int32* month);

    static CARAPI_(Int32) GetYear(
        /* [in] */ const String& yearString);

    static CARAPI_(AutoPtr<TimeOfDay>) GetTime(
        /* [in] */ const String& timeString);

private:
    HttpDateTime();

private:
    /*
     * Regular expression for parsing HTTP-date.
     *
     * Wdy, DD Mon YYYY HH:MM:SS GMT
     * RFC 822, updated by RFC 1123
     *
     * Weekday, DD-Mon-YY HH:MM:SS GMT
     * RFC 850, obsoleted by RFC 1036
     *
     * Wdy Mon DD HH:MM:SS YYYY
     * ANSI C's asctime() format
     *
     * with following variations
     *
     * Wdy, DD-Mon-YYYY HH:MM:SS GMT
     * Wdy, (SP)D Mon YYYY HH:MM:SS GMT
     * Wdy,DD Mon YYYY HH:MM:SS GMT
     * Wdy, DD-Mon-YY HH:MM:SS GMT
     * Wdy, DD Mon YYYY HH:MM:SS -HHMM
     * Wdy, DD Mon YYYY HH:MM:SS
     * Wdy Mon (SP)D HH:MM:SS YYYY
     * Wdy Mon DD HH:MM:SS YYYY GMT
     *
     * HH can be H if the first digit is zero.
     *
     * Mon can be the full name of the month.
     */
    static const String HTTP_DATE_RFC_REGEXP;

    static const String HTTP_DATE_ANSIC_REGEXP;

    /**
     * The compiled version of the HTTP-date regular expressions.
     */
    static const AutoPtr<IPattern> HTTP_DATE_RFC_PATTERN;
    static const AutoPtr<IPattern> HTTP_DATE_ANSIC_PATTERN;
};

} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_HTTP_HTTPDATETIME_H__
