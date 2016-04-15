
#include "CCallLogQueryHandler.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::IStringBuilder;
using Elastos::Core::CStringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

const AutoPtr<ArrayOf<IInterface*> > CCallLogQueryHandler::EMPTY_STRING_ARRAY = ArrayOf<IInterface*>::Alloc(0);

const String CCallLogQueryHandler::TAG("CallLogQueryHandler");
const Int32 CCallLogQueryHandler::NUM_LOGS_TO_DISPLAY = 1000;

/** The token for the query to fetch the old entries from the call log. */
const Int32 CCallLogQueryHandler::QUERY_CALLLOG_TOKEN = 54;
/** The token for the query to mark all missed calls as old after seeing the call log. */
const Int32 CCallLogQueryHandler::UPDATE_MARK_AS_OLD_TOKEN = 55;
/** The token for the query to mark all new voicemails as old. */
const Int32 CCallLogQueryHandler::UPDATE_MARK_VOICEMAILS_AS_OLD_TOKEN = 56;
/** The token for the query to mark all missed calls as read after seeing the call log. */
const Int32 CCallLogQueryHandler::UPDATE_MARK_MISSED_CALL_AS_READ_TOKEN = 57;
/** The token for the query to fetch voicemail status messages. */
const Int32 CCallLogQueryHandler::QUERY_VOICEMAIL_STATUS_TOKEN = 58;

//=================================================================
// CCallLogQueryHandler::CatchingWorkerHandler
//=================================================================
CCallLogQueryHandler::CatchingWorkerHandler::CatchingWorkerHandler(
    /* [in] */ ILooper* looper)
{
    AsyncQueryHandler::WorkerHandler(looper);
}

ECode CCallLogQueryHandler::CatchingWorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // try {
    // Perform same query while catching any exceptions
    ECode ec = AsyncQueryHandler::WorkerHandler::HandleMessage(msg);
    if (ec == (ECode)E_SQLITE_DISK_IO_EXCEPTION) {
        Logger::W(TAG, "Exception on background worker thread %x", ec);
    }
    else if (ec == (ECode)E_SQLITE_FULL_EXCEPTION) {
        Logger::W(TAG, "Exception on background worker thread %x", ec);
    }
    else if (ec == (ECode)E_SQLITE_DATABASE_CORRUPT_EXCEPTION) {
        Logger::W(TAG, "Exception on background worker thread %x", ec);
    }
    else if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Logger::W(TAG, "ContactsProvider not present on device %x", ec);
    }
    return NOERROR;
    // } catch (SQLiteDiskIOException e) {
    //     Log.w(TAG, "Exception on background worker thread", e);
    // } catch (SQLiteFullException e) {
    //     Log.w(TAG, "Exception on background worker thread", e);
    // } catch (SQLiteDatabaseCorruptException e) {
    //     Log.w(TAG, "Exception on background worker thread", e);
    // } catch (IllegalArgumentException e) {
    //     Log.w(TAG, "ContactsProvider not present on device", e);
    // }
}

//=================================================================
// CCallLogQueryHandler
//=================================================================
CAR_INTERFACE_IMPL(CCallLogQueryHandler, NoNullCursorAsyncQueryHandler, ICallLogQueryHandler)

CAR_OBJECT_IMPL(CCallLogQueryHandler)

CCallLogQueryHandler::CallLogQueryHandler()
    : mLogLimit(0)
{}

ECode CCallLogQueryHandler::CreateHandler(
    /* [in] */ ILooper* looper,
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler);
    // Provide our special handler that catches exceptions
    AutoPtr<CatchingWorkerHandler> cwHandler = new CatchingWorkerHandler(looper);
    *handler = (IHandler*)cwHandler;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

ECode CCallLogQueryHandler::constructor(
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ ICallLogQueryHandlerListener* listener)
{
    return constructor(contentResolver, listener, -1);
}

ECode CCallLogQueryHandler::constructor(
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ ICallLogQueryHandlerListener* listener,
    /* [in] */ Int32 limit)
{
    NoNullCursorAsyncQueryHandler(contentResolver);
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(listener);
    wrs->GetWeakReference((IWeakReference**)&mListener);
    mLogLimit = limit;
    return NOERROR;
}

ECode CCallLogQueryHandler::FetchCalls(
    /* [in] */ Int32 callType,
    /* [in] */ Int64 newerThan)
{
    CancelFetch();
    FetchCalls(QUERY_CALLLOG_TOKEN, callType, FALSE /* newOnly */, newerThan);
    return NOERROR;
}

ECode CCallLogQueryHandler::FetchCalls(
    /* [in] */ Int32 callType)
{
    FetchCalls(callType, 0);
    return NOERROR;
}

ECode CCallLogQueryHandler::FetchVoicemailStatus()
{
    StartQuery(QUERY_VOICEMAIL_STATUS_TOKEN, NULL, IVoicemailContractStatus::CONTENT_URI,
                VoicemailStatusHelperImpl::PROJECTION, NULL, NULL, NULL);
    return NOERROR;
}

void CCallLogQueryHandler::FetchCalls(
    /* [in] */ Int32 token,
    /* [in] */ Int32 callType,
    /* [in] */ Boolean newOnly,
    /* [in] */ Int64 newerThan)
{
    // We need to check for NULL explicitly otherwise entries with where READ is NULL
    // may not match either the query or its negation.
    // We consider the calls that are not yet consumed (i.e. IS_READ = 0) as "new".
    AutoPtr<IStringBuilder> where;
    CStringBuilder::New((IStringBuilder**)&where);

    AutoPtr<IArrayList> selectionArgs;
    CArrayList::New((IArrayList**)&selectionArgs);

    if (newOnly) {
        where->Append(ICalls::NEW);
        where->Append(" = 1");
    }

    Int32 length = 0;
    if (callType > CALL_TYPE_ALL) {
        Int32 length = 0;
        ICharSequence::Probe(where)->GetLength(&length);
        if (length > 0) {
            where->Append(" AND ");
        }
        // Add a clause to fetch only items of type voicemail.
        where->Append("(");
        where->Append(ICalls::TYPE);
        where->Append(" = ?)");
        // Add a clause to fetch only items newer than the requested date
        selectionArgs->Add(StringUtils::ToString(callType));
    }

    if (newerThan > 0) {
        ICharSequence::Probe(where)->GetLength(&length);
        if (length > 0) {
            where->Append(" AND ");
        }
        where->Append("(");
        where->Append(ICalls::DATE);
        where->Append(" = ?)");
        selectionArgs->add(StringUtils::ToString(newerThan));
    }

    Int32 limit = (mLogLimit == -1) ? NUM_LOGS_TO_DISPLAY : mLogLimit;
    ICharSequence::Probe(where)->GetLength(&length);
    String str;
    String selection = length > 0
            ? ICharSequence::Probe(where)->ToString(&str), str : NULL;

    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> contentUri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&contentUri);
    AutoPtr<IUriBuilder> builder;
    contentUri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendQueryParameter(ICalls::LIMIT_PARAM_KEY, StringUtils::ToString(limit));
    AutoPtr<IUri> uri;
    builder->Build((IUri**)&uri);

    AutoPtr<ArrayOf<IInterface*> > array;
    selectionArgs->ToArray(EMPTY_STRING_ARRAY, (ArrayOf<IInterface*>**)&array);
    StartQuery(token, NULL, uri, ICallLogQuery::_PROJECTION,
             selection, array, ICalls::DEFAULT_SORT_ORDER);
    return NOERROR;
}

void CCallLogQueryHandler::CancelFetch()
{
    CancelOperation(QUERY_CALLLOG_TOKEN);
}

ECode CCallLogQueryHandler::MarkNewCallsAsOld()
{
    // Mark all "new" calls as not new anymore.
    AutoPtr<IStringBuilder> where;
    CStringBuilder::New((IStringBuilder**)&where);

    where->Append(ICalls::NEW);
    where->Append(" = 1");

    AutoPtr<IContentValues> values;
    CContentValues::New(1, (IContentValues)&values);
    values->Put(ICalls::NEW, String("0"));

    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> contentUri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&contentUri);
    String str;
    ICharSequence::Probe(where)->ToString(&str);
    StartUpdate(UPDATE_MARK_AS_OLD_TOKEN,
            NULL, contentUri, values, str, NULL);

    return NOERROR;
}

ECode CCallLogQueryHandler::MarkNewVoicemailsAsOld()
{
    // Mark all "new" voicemails as not new anymore.
    AutoPtr<IStringBuilder> where;
    CStringBuilder::New((IStringBuilder**)&where);
    where->Append(ICalls::NEW);
    where->Append(" = 1 AND ");
    where->Append(ICalls::TYPE);
    where->Append(" = ?");

    AutoPtr<IContentValues> values;
    CContentValues::New(1, (IContentValues)&values);
    values->Put(ICalls::NEW, String("0"));

    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> contentUri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&contentUri);
    String str;
    ICharSequence::Probe(where)->ToString(&str);

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    array->Set(0, StringUtils::ToString(ICalls::VOICEMAIL_TYPE));
    StartUpdate(UPDATE_MARK_VOICEMAILS_AS_OLD_TOKEN,
            NULL, contentUri, values, str, array);

    return NOERROR;
}

ECode CCallLogQueryHandler::MarkMissedCallsAsRead()
{
    // Mark all "new" calls as not new anymore.
    AutoPtr<IStringBuilder> where;
    CStringBuilder::New((IStringBuilder**)&where);
    where->Append(ICalls::IS_READ);
    where->Append(" = 0");
    where->Append(" AND ");
    where->Append(ICalls::TYPE);
    where->Append(" = ");
    where->Append(ICalls::MISSED_TYPE);

    AutoPtr<IContentValues> values;
    CContentValues::New(1, (IContentValues)&values);
    values->Put(ICalls::IS_READ, String("1"));

    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> contentUri;
    calls->GetCONTENT_URI((IUri**)&contentUri);
    String str;
    ICharSequence::Probe(where)->ToString(&str);
    startUpdate(UPDATE_MARK_MISSED_CALL_AS_READ_TOKEN,
            NULL, contentUri, values,str, NULL);

    return NOERROR;
}

ECode CCallLogQueryHandler::OnNotNullableQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* cursor)
{
    synchronized(this) {
        if (cursor == NULL) {
            return NOERROR;
        }
        // try {
        if (token == QUERY_CALLLOG_TOKEN) {
            if (UpdateAdapterData(cursor)) {
                cursor = NULL;
            }
        }
        else if (token == QUERY_VOICEMAIL_STATUS_TOKEN) {
            UpdateVoicemailStatus(cursor);
        }
        else {
            Log::W(TAG, "Unknown query completed: ignoring: %d", token);
        }

        if (cursor != NULL) {
            cursor->Close();
        }
        // } finally {
        //     if (cursor != null) {
        //         cursor.close();
        //     }
        // }
        return NOERROR;
    }
}

Boolean CCallLogQueryHandler::UpdateAdapterData(
    /* [in] */ ICursor* cursor)
{
    AutoPtr<ICallLogQueryHandlerListener> listener;
    mListener->Resolve(EIID_ICallLogQueryHandlerListener,
            (ICallLogQueryHandlerListener**)&listener);
    if (listener != NULL) {
        Boolean result;
        listener->OnCallsFetched(cursor, &result);
        return result;
    }
    return FALSE;
}

void CCallLogQueryHandler::UpdateVoicemailStatus(
    /* [in] */ ICursor* statusCursor)
{
    AutoPtr<ICallLogQueryHandlerListener> listener;
    mListener->Resolve(EIID_ICallLogQueryHandlerListener,
            (ICallLogQueryHandlerListener**)&listener);
    if (listener != NULL) {
        listener->OnVoicemailStatusFetched(statusCursor);
    }
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
