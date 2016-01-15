
#ifndef __ELASTOS_DROID_APP_CDOWNLOADMANAGERQUERY_H__
#define __ELASTOS_DROID_APP_CDOWNLOADMANAGERQUERY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_App_CDownloadManagerQuery.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::IInteger32;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CDownloadManagerQuery)
    , public Object
    , public IDownloadManagerQuery
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CDownloadManagerQuery();

    /**
     * Include only the downloads with the given IDs.
     * @return this object
     */
    CARAPI SetFilterById(
        /* [in] */ ArrayOf<Int64>* ids);

    /**
     * Include only downloads with status matching any the given status flags.
     * @param flags any combination of the STATUS_* bit flags
     * @return this object
     */
    CARAPI SetFilterByStatus(
        /* [in] */ Int32 flags);

    /**
     * Controls whether this query includes downloads not visible in the system's Downloads UI.
     * @param value if true, this query will only include downloads that should be displayed in
     *            the system's Downloads UI; if false (the default), this query will include
     *            both visible and invisible downloads.
     * @return this object
     * @hide
     */
    CARAPI SetOnlyIncludeVisibleInDownloadsUi(
        /* [in] */ Boolean value);

    /**
     * Change the sort order of the returned Cursor.
     *
     * @param column one of the COLUMN_* constants; currently, only
     *         {@link #COLUMN_LAST_MODIFIED_TIMESTAMP} and {@link #COLUMN_TOTAL_SIZE_BYTES} are
     *         supported.
     * @param direction either {@link #ORDER_ASCENDING} or {@link #ORDER_DESCENDING}
     * @return this object
     * @hide
     */
    CARAPI OrderBy(
        /* [in] */ const String& column,
        /* [in] */ Int32 direction);

    /**
     * Run this query using the given ContentResolver.
     * @param projection the projection to pass to ContentResolver.query()
     * @return the Cursor returned by ContentResolver.query()
     */
    AutoPtr<ICursor> RunQuery(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ IUri* baseUri);

private:
    CARAPI_(String) JoinStrings(
        /* [in] */ const String& joiner,
        /* [in] */ List<String>* parts);

    CARAPI_(String) StatusClause(
        /* [in] */ const String& op,
        /* [in] */ Int32 value);

private:
    AutoPtr<ArrayOf<Int64> > mIds;
    AutoPtr<IInteger32> mStatusFlags;
    String mOrderByColumn;
    Int32 mOrderDirection;
    Boolean mOnlyIncludeVisibleInDownloadsUi;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CDOWNLOADMANAGERQUERY_H__
