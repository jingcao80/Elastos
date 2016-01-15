
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/app/CDownloadManager.h"
#include "elastos/droid/app/CDownloadManagerQuery.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::App::IDownloadManager;
using Elastos::Droid::Provider::IDownloadsImpl;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CDownloadManagerQuery, Object, IDownloadManagerQuery)

CAR_OBJECT_IMPL(CDownloadManagerQuery)

ECode CDownloadManagerQuery::constructor()
{
    return NOERROR;
}

CDownloadManagerQuery::CDownloadManagerQuery()
    : mOrderByColumn(IDownloadsImpl::COLUMN_LAST_MODIFICATION)
    , mOrderDirection(IDownloadManagerQuery::ORDER_DESCENDING)
    , mOnlyIncludeVisibleInDownloadsUi(FALSE)
{}

ECode CDownloadManagerQuery::SetFilterById(
    /* [in] */ ArrayOf<Int64>* ids)
{
    mIds = ids;
    return NOERROR;
}

ECode CDownloadManagerQuery::SetFilterByStatus(
    /* [in] */ Int32 flags)
{
    mStatusFlags = NULL;
    CInteger32::New(flags, (IInteger32**)&mStatusFlags);
    return NOERROR;
}

ECode CDownloadManagerQuery::SetOnlyIncludeVisibleInDownloadsUi(
    /* [in] */ Boolean value)
{
    mOnlyIncludeVisibleInDownloadsUi = value;
    return NOERROR;
}

ECode CDownloadManagerQuery::OrderBy(
    /* [in] */ const String& column,
    /* [in] */ Int32 direction)
{
    if (direction != ORDER_ASCENDING && direction != ORDER_DESCENDING) {
        // throw new IllegalArgumentException("Invalid direction: " + direction);
        Slogger::E("CDownloadManagerQuery", "Invalid direction: %d", direction);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (column.Equals(IDownloadManager::COLUMN_LAST_MODIFIED_TIMESTAMP)) {
        mOrderByColumn = IDownloadsImpl::COLUMN_LAST_MODIFICATION;
    } else if (column.Equals(IDownloadManager::COLUMN_TOTAL_SIZE_BYTES)) {
        mOrderByColumn = IDownloadsImpl::COLUMN_TOTAL_BYTES;
    } else {
        // throw new IllegalArgumentException("Cannot order by " + column);
        Slogger::E("CDownloadManagerQuery", "Cannot order by %s", column.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mOrderDirection = direction;
    return NOERROR;
}

/**
 * Run this query using the given ContentResolver.
 * @param projection the projection to pass to ContentResolver.query()
 * @return the Cursor returned by ContentResolver.query()
 */
AutoPtr<ICursor> CDownloadManagerQuery::RunQuery(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ IUri* baseUri)
{
    AutoPtr<IUri> uri = baseUri;
    AutoPtr< List<String> > selectionParts = new List<String>();
    AutoPtr<ArrayOf<String> > selectionArgs;

    if (mIds != NULL) {
        selectionParts->PushBack(CDownloadManager::GetWhereClauseForIds(mIds));
        selectionArgs = CDownloadManager::GetWhereArgsForIds(mIds);
    }

    if (mStatusFlags != NULL) {
        Int32 statusFlags;
        mStatusFlags->GetValue(&statusFlags);
        AutoPtr< List<String> > parts = new List<String>();
        if ((statusFlags & IDownloadManager::STATUS_PENDING) != 0) {
            parts->PushBack(StatusClause(String("="), IDownloadsImpl::STATUS_PENDING));
        }
        if ((statusFlags & IDownloadManager::STATUS_RUNNING) != 0) {
            parts->PushBack(StatusClause(String("="), IDownloadsImpl::STATUS_RUNNING));
        }
        if ((statusFlags & IDownloadManager::STATUS_PAUSED) != 0) {
            parts->PushBack(StatusClause(String("="), IDownloadsImpl::STATUS_PAUSED_BY_APP));
            parts->PushBack(StatusClause(String("="), IDownloadsImpl::STATUS_WAITING_TO_RETRY));
            parts->PushBack(StatusClause(String("="), IDownloadsImpl::STATUS_WAITING_FOR_NETWORK));
            parts->PushBack(StatusClause(String("="), IDownloadsImpl::STATUS_QUEUED_FOR_WIFI));
        }
        if ((statusFlags & IDownloadManager::STATUS_SUCCESSFUL) != 0) {
            parts->PushBack(StatusClause(String("="), IDownloadsImpl::STATUS_SUCCESS));
        }
        if ((statusFlags & IDownloadManager::STATUS_FAILED) != 0) {
            parts->PushBack(String("(") + StatusClause(String(">="), 400)
                      + " AND " + StatusClause(String("<"), 600) + ")");
        }
        selectionParts->PushBack(JoinStrings(String(" OR "), parts));
    }

    if (mOnlyIncludeVisibleInDownloadsUi) {
        selectionParts->PushBack(IDownloadsImpl::COLUMN_IS_VISIBLE_IN_DOWNLOADS_UI + " != '0'");
    }

    // only return rows which are not marked 'deleted = 1'
    selectionParts->PushBack(IDownloadsImpl::COLUMN_DELETED + " != '1'");

    String selection = JoinStrings(String(" AND "), selectionParts);
    String orderDirection = (mOrderDirection == ORDER_ASCENDING ? String("ASC") : String("DESC"));
    String orderBy = mOrderByColumn + " " + orderDirection;

    AutoPtr<ICursor> cursor;
    resolver->Query(uri, projection, selection, selectionArgs, orderBy, (ICursor**)&cursor);
    return cursor;
}

String CDownloadManagerQuery::JoinStrings(
    /* [in] */ const String& joiner,
    /* [in] */ List<String>* parts)
{
    StringBuilder builder;
    Boolean first = TRUE;
    List<String>::Iterator it;
    for (it = parts->Begin(); it != parts->End(); ++it) {
        String part = *it;
        if (!first) {
            builder += joiner;
        }
        builder += part;
        first = FALSE;
    }
    return builder.ToString();
}

String CDownloadManagerQuery::StatusClause(
    /* [in] */ const String& op,
    /* [in] */ Int32 value)
{
    return IDownloadsImpl::COLUMN_STATUS + op + "'" + StringUtils::ToString(value) + "'";
}

} // namespace App
} // namespace Droid
} // namespace Elastos
