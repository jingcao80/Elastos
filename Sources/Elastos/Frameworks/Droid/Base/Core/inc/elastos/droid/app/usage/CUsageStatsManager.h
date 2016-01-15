
#ifndef __ELASTOS_DROID_APP_USAGE_CUSAGE_STATS_MANAGER_H__
#define __ELASTOS_DROID_APP_USAGE_CUSAGE_STATS_MANAGER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_App_Usage_CUsageStatsManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace App {
namespace Usage {

/**
 * Provides access to device usage history and statistics. Usage data is aggregated into
 * time intervals: days, weeks, months, and years.
 * <p />
 * When requesting usage data since a particular time, the request might look something like this:
 * <pre>
 * PAST                   REQUEST_TIME                    TODAY                   FUTURE
 * ————————————————————————————||———————————————————————————¦-----------------------|
 *                        YEAR ||                           ¦                       |
 * ————————————————————————————||———————————————————————————¦-----------------------|
 *  MONTH            |         ||                MONTH      ¦                       |
 * ——————————————————|—————————||———————————————————————————¦-----------------------|
 *   |      WEEK     |     WEEK||    |     WEEK     |     WE¦EK     |      WEEK     |
 * ————————————————————————————||———————————————————|———————¦-----------------------|
 *                             ||           |DAY|DAY|DAY|DAY¦DAY|DAY|DAY|DAY|DAY|DAY|
 * ————————————————————————————||———————————————————————————¦-----------------------|
 * </pre>
 * A request for data in the middle of a time interval will include that interval.
 * <p/>
 * <b>NOTE:</b> This API requires the permission android.permission.PACKAGE_USAGE_STATS_MANAGER, which
 * is a system-level permission and will not be granted to third-party apps. However, declaring
 * the permission implies intention to use the API and the user of the device can grant permission
 * through the Settings application.
 */
CarClass(CUsageStatsManager)
    , public Object
    , public IUsageStatsManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUsageStatsManager();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IIUsageStatsManager* service);

    /**
     * Gets application usage stats for the given time range, aggregated by the specified interval.
     * <p>The returned list will contain a {@link UsageStats} object for each package that
     * has data for an interval that is a subset of the time range given. To illustrate:</p>
     * <pre>
     * intervalType = INTERVAL_YEARLY
     * beginTime = 2013
     * endTime = 2015 (exclusive)
     *
     * Results:
     * 2013 - com.example.alpha
     * 2013 - com.example.beta
     * 2014 - com.example.alpha
     * 2014 - com.example.beta
     * 2014 - com.example.charlie
     * </pre>
     *
     * @param intervalType The time interval by which the stats are aggregated.
     * @param beginTime The inclusive beginning of the range of stats to include in the results.
     * @param endTime The exclusive end of the range of stats to include in the results.
     * @return A list of {@link UsageStats} or null if none are available.
     *
     * @see #INTERVAL_DAILY
     * @see #INTERVAL_WEEKLY
     * @see #INTERVAL_MONTHLY
     * @see #INTERVAL_YEARLY
     * @see #INTERVAL_BEST
     */
    CARAPI QueryUsageStats(
        /* [in] */ Int32 intervalType,
        /* [in] */ Int64 beginTime,
        /* [in] */ Int64 endTime,
        /* [out] */ IList** result);

    /**
     * Gets the hardware configurations the device was in for the given time range, aggregated by
     * the specified interval. The results are ordered as in
     * {@link #queryUsageStats(int, long, long)}.
     *
     * @param intervalType The time interval by which the stats are aggregated.
     * @param beginTime The inclusive beginning of the range of stats to include in the results.
     * @param endTime The exclusive end of the range of stats to include in the results.
     * @return A list of {@link ConfigurationStats} or null if none are available.
     */
    CARAPI QueryConfigurations(
        /* [in] */ Int32 intervalType,
        /* [in] */ Int64 beginTime,
        /* [in] */ Int64 endTime,
        /* [out] */ IList** result);

    /**
     * Query for events in the given time range. Events are only kept by the system for a few
     * days.
     * <p />
     * <b>NOTE:</b> The last few minutes of the event log will be truncated to prevent abuse
     * by applications.
     *
     * @param beginTime The inclusive beginning of the range of events to include in the results.
     * @param endTime The exclusive end of the range of events to include in the results.
     * @return A {@link UsageEvents}.
     */
    CARAPI QueryEvents(
        /* [in] */ Int64 beginTime,
        /* [in] */ Int64 endTime,
        /* [out] */ IUsageEvents** result);

    /**
     * A convenience method that queries for all stats in the given range (using the best interval
     * for that range), merges the resulting data, and keys it by package name.
     * See {@link #queryUsageStats(int, long, long)}.
     *
     * @param beginTime The inclusive beginning of the range of stats to include in the results.
     * @param endTime The exclusive end of the range of stats to include in the results.
     * @return A {@link java.util.Map} keyed by package name, or null if no stats are
     *         available.
     */
    CARAPI QueryAndAggregateUsageStats(
        /* [in] */ Int64 beginTime,
        /* [in] */ Int64 endTime,
        /* [out] */ IMap** result);

private:
    static AutoPtr<IUsageEvents> sEmptyResults;// = new UsageEvents();

    AutoPtr<IContext> mContext;
    AutoPtr<IIUsageStatsManager> mService;
};

} // namespace Usage
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_USAGE_CUSAGE_STATS_MANAGER_H__
