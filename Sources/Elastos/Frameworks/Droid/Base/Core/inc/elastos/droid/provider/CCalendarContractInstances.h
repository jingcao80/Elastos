#ifndef __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTINSTANCES_H__
#define __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTINSTANCES_H__

#include "_Elastos_Droid_Provider_CCalendarContractInstances.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

/**
 * Fields and helpers for interacting with Instances. An instance is a
 * single occurrence of an event including time zone specific start and end
 * days and minutes. The instances table is not writable and only provides a
 * way to query event occurrences.
 */
CarClass(CCalendarContractInstances)
    , public Singleton
    , public ICalendarContractInstances
    , public IBaseColumns
    , public ICalendarContractEventsColumns
    , public ICalendarContractCalendarColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URL for querying an instance range. The begin
     * and end of the range to query should be added as path segments if
     * this is used directly.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URL for querying an instance range by Julian
     * Day. The start and end day should be added as path segments if this
     * is used directly.
     */
    CARAPI GetCONTENT_BY_DAY_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URL for querying an instance range with a search
     * term. The begin, end, and search string should be appended as path
     * segments if this is used directly.
     */
    CARAPI GetCONTENT_SEARCH_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URL for querying an instance range with a search
     * term. The start day, end day, and search string should be appended as
     * path segments if this is used directly.
     */
    CARAPI GetCONTENT_SEARCH_BY_DAY_URI(
        /* [out] */ IUri** uri);

    /**
     * Performs a query to return all visible instances in the given range.
     * This is a blocking function and should not be done on the UI thread.
     * This will cause an expansion of recurring events to fill this time
     * range if they are not already expanded and will slow down for larger
     * time ranges with many recurring events.
     *
     * @param cr The ContentResolver to use for the query
     * @param projection The columns to return
     * @param begin The start of the time range to query in UTC millis since
     *            epoch
     * @param end The end of the time range to query in UTC millis since
     *            epoch
     * @return A Cursor containing all instances in the given range
     */
    CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ Int64 begin,
        /* [in] */ Int64 end,
        /* [out] */ ICursor** cursor);

    /**
     * Performs a query to return all visible instances in the given range
     * that match the given query. This is a blocking function and should
     * not be done on the UI thread. This will cause an expansion of
     * recurring events to fill this time range if they are not already
     * expanded and will slow down for larger time ranges with many
     * recurring events.
     *
     * @param cr The ContentResolver to use for the query
     * @param projection The columns to return
     * @param begin The start of the time range to query in UTC millis since
     *            epoch
     * @param end The end of the time range to query in UTC millis since
     *            epoch
     * @param searchQuery A string of space separated search terms. Segments
     *            enclosed by double quotes will be treated as a single
     *            term.
     * @return A Cursor of instances matching the search terms in the given
     *         time range
     */
    CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ Int64 begin,
        /* [in] */ Int64 end,
        /* [in] */ const String& searchQuery,
        /* [out] */ ICursor** cursor);

private:
    static const String WHERE_CALENDARS_SELECTED;
    static const AutoPtr<ArrayOf<String> > WHERE_CALENDARS_ARGS;

    /**
     * The default sort order for this table.
     */
    static const String DEFAULT_SORT_ORDER ;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTINSTANCES_H__
