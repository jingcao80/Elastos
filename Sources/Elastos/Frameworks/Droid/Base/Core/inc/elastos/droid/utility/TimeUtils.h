#ifndef __ELASTOS_DROID_UTILITY_TIMEUTILS_H__
#define __ELASTOS_DROID_UTILITY_TIMEUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::ITimeZone;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * A class containing utility methods related to time zones.
 */
class ECO_PUBLIC TimeUtils
{
public:
    /**
     * Tries to return a time zone that would have had the specified offset
     * and DST value at the specified moment in the specified country.
     * Returns null if no suitable zone could be found.
     */
    static CARAPI_(AutoPtr<ITimeZone>) GetTimeZone(
        /* [in] */ Int32 offset,
        /* [in] */ Boolean dst,
        /* [in] */ Int64 when,
        /* [in] */ const String& country);

    /**
     * Return list of unique time zones for the country. Do not modify
     *
     * @param country to find
     * @return list of unique time zones, maybe empty but never null. Do not modify.
     * @hide
     */
    static CARAPI_(AutoPtr<ArrayOf<ITimeZone *> >) GetTimeZonesWithUniqueOffsets(
        /* [in] */ const String& country);

    /**
     * Returns the time zones for the country, which is the code
     * attribute of the timezone element in time_zones_by_country.xml. Do not modify.
     *
     * @param country is a two character country code.
     * @return TimeZone list, maybe empty but never null. Do not modify.
     * @hide
     */
    static CARAPI_(AutoPtr<ArrayOf<ITimeZone *> >) GetTimeZones(
        /* [in] */ const String& country);

    /**
     * Returns a String indicating the version of the time zone database currently
     * in use.  The format of the string is dependent on the underlying time zone
     * database implementation, but will typically contain the year in which the database
     * was updated plus a letter from a to z indicating changes made within that year.
     *
     * <p>Time zone database updates should be expected to occur periodically due to
     * political and legal changes that cannot be anticipated in advance.  Therefore,
     * when computing the UTC time for a future event, applications should be aware that
     * the results may differ following a time zone database update.  This method allows
     * applications to detect that a database change has occurred, and to recalculate any
     * cached times accordingly.
     *
     * <p>The time zone database may be assumed to change only when the device runtime
     * is restarted.  Therefore, it is not necessary to re-query the database version
     * during the lifetime of an activity.
     */
    static CARAPI_(String) GetTimeZoneDatabaseVersion();

    /** @hide Just for debugging; not internationalized. */
    static CARAPI_(void) FormatDuration(
        /* [in] */ Int64 duration,
        /* [in] */ StringBuilder& builder);

        /** @hide Just for debugging; not internationalized. */
    static CARAPI_(void) FormatDuration(
        /* [in] */ Int64 duration,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 fieldLen);

    /** @hide Just for debugging; not internationalized. */
    static CARAPI_(void) FormatDuration(
        /* [in] */ Int64 duration,
        /* [in] */ IPrintWriter* pw);

    /** @hide Just for debugging; not internationalized. */
    static CARAPI_(void) FormatDuration(
        /* [in] */ Int64 duration,
        /* [in] */ Int64 now,
        /* [in] */ IPrintWriter* pw);

    /** @hide Just for debugging; not internationalized. */
    static CARAPI_(String) FormatUptime(
        /* [in] */ Int64 time);

    /**
     * Convert a System.currentTimeMillis() value to a time of day value like
     * that printed in logs. MM-DD HH:MM:SS.MMM
     *
     * @param millis since the epoch (1/1/1970)
     * @return String representation of the time.
     * @hide
     */
    static CARAPI_(String) LogTimeOfDay(
        /* [in] */ Int64 millis);

private:
    ECO_LOCAL static CARAPI_(Int32) AccumField(
        /* [in] */ Int32 amt,
        /* [in] */ Int32 suffix,
        /* [in] */ Boolean always,
        /* [in] */ Int32 zeropad);

    ECO_LOCAL static CARAPI_(Int32) PrintField(
        /* [in] */ ArrayOf<Char32>* formatStr,
        /* [in] */ Int32 amt,
        /* [in] */ Char32 suffix,
        /* [in] */ Int32 pos,
        /* [in] */ Boolean always,
        /* [in] */ Int32 zeropad);

    ECO_LOCAL static CARAPI_(Int32) FormatDurationLocked(
        /* [in] */ Int64 duration,
        /* [in] */ Int32 fieldLen);

private:
    ECO_LOCAL TimeUtils();
    ECO_LOCAL TimeUtils(const TimeUtils& other);

public:
    /** @hide Field length that can hold 999 days of time */
    static const Int32 HUNDRED_DAY_FIELD_LEN;// = 19;

    static const Int32 SECONDS_PER_MINUTE;// = 60;
    static const Int32 SECONDS_PER_HOUR;// = 60 * 60;
    static const Int32 SECONDS_PER_DAY;// = 24 * 60 * 60;

    static Object sFormatSync;
    static AutoPtr<ArrayOf<Char32> > sFormatStr;// = new char[HUNDRED_DAY_FIELD_LEN+5];

    static const Int64 LARGEST_DURATION;// = (1000 * DateUtils.DAY_IN_MILLIS) - 1;

    /** @hide */
    static const Int64 NANOS_PER_MS;// = 1000000;

private:
    static const Boolean DBG;// = false;
    static const String TAG;// = "TimeUtils";

    /** Cached results of getTineZones */
    static Object sLastLockObj;
    static AutoPtr<ArrayOf<ITimeZone *> > sLastZones;
    static String sLastCountry;

    /** Cached results of getTimeZonesWithUniqueOffsets */
    static Object sLastUniqueLockObj;
    static AutoPtr<ArrayOf<ITimeZone *> > sLastUniqueZoneOffsets;
    static String sLastUniqueCountry;

};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_UTILITY_TIMEUTILS_H__
