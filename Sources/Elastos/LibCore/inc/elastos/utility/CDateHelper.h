#ifndef __ELASTOS_UTILITY_CDATEHELPER_H__
#define __ELASTOS_UTILITY_CDATEHELPER_H__

#include "_Elastos_Utility_CDateHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Utility {

CarClass(CDateHelper)
    , public Singleton
    , public IDateHelper
{
public:
    CAR_SINGLETON_DECL()
    CAR_INTERFACE_DECL()

    /**
     * Returns the millisecond value of the date and time parsed from the
     * specified {@code String}. Many date/time formats are recognized, including IETF
     * standard syntax, i.e. Tue, 22 Jun 1999 12:16:00 GMT-0500
     *
     * @param string
     *            the String to parse.
     * @return the millisecond value parsed from the String.
     *
     * @deprecated use {@link DateFormat}
     */
    //@Deprecated
    CARAPI Parse(
        /* [in] */ const String& string,
        /* [out] */ Int64* value);

    /**
     * Returns the millisecond value of the specified date and time in GMT.
     *
     * @param year
     *            the year, 0 is 1900.
     * @param month
     *            the month, 0 - 11.
     * @param day
     *            the day of the month, 1 - 31.
     * @param hour
     *            the hour of day, 0 - 23.
     * @param minute
     *            the minute of the hour, 0 - 59.
     * @param second
     *            the second of the minute, 0 - 59.
     * @return the date and time in GMT in milliseconds.
     *
     * @deprecated use: <code>
     *  Calendar cal = new GregorianCalendar(TimeZone.getTimeZone("GMT"));
     *  cal.set(year + 1900, month, day, hour, minute, second);
     *  cal.getTime().getTime();</code>
     */
    //@Deprecated
    CARAPI UTC(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second,
        /* [out] */ Int64* value);
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CDATEHELPER_H__
