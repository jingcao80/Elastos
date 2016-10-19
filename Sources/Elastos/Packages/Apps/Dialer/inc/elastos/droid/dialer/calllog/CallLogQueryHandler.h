
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGQUERYHANDLER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGQUERYHANDLER_H__

#include "elastos/droid/contacts/common/database/NoNullCursorAsyncQueryHandler.h"
#include "elastos/droid/content/AsyncQueryHandler.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Contacts::Common::Database::NoNullCursorAsyncQueryHandler;
using Elastos::Droid::Content::AsyncQueryHandler;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;

namespace Elastos{
namespace Droid{
namespace Dialer {
namespace CallLog {

class CallLogQueryHandler
    : public NoNullCursorAsyncQueryHandler
{
protected:
    /**
     * Simple handler that wraps background calls to catch
     * {@link SQLiteException}, such as when the disk is full.
     */
    class CatchingWorkerHandler : public AsyncQueryHandler::WorkerHandler
    {
    public:
        CatchingWorkerHandler(
            /* [in] */ ILooper* looper);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    };

public:
    CallLogQueryHandler();

    CARAPI constructor(
        /* [in] */ IContentResolver* contentResolver,
        /* [in] */ ICallLogQueryHandlerListener* listener);

    CARAPI constructor(
        /* [in] */ IContentResolver* contentResolver,
        /* [in] */ ICallLogQueryHandlerListener* listener,
        /* [in] */ Int32 limit);

    /**
     * Fetches the list of calls from the call log for a given type.
     * This call ignores the new or old state.
     * <p>
     * It will asynchronously update the content of the list view when the fetch completes.
     */
    CARAPI_(void) FetchCalls(
        /* [in] */ Int32 callType,
        /* [in] */ Int64 newerThan);

    CARAPI_(void) FetchCalls(
        /* [in] */ Int32 callType);

    CARAPI_(void) FetchVoicemailStatus();

    /** Updates all new calls to mark them as old. */
    CARAPI_(void) MarkNewCallsAsOld();

    /** Updates all new voicemails to mark them as old. */
    CARAPI_(void) MarkNewVoicemailsAsOld();

    /** Updates all missed calls to mark them as read. */
    CARAPI_(void) MarkMissedCallsAsRead();

protected:
    // @Override
    CARAPI CreateHandler(
        /* [in] */ ILooper* looper,
        /* [out] */ IHandler** handler);

    // @Override
    CARAPI OnNotNullableQueryComplete(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ ICursor* cursor);

private:
    /** Fetches the list of calls in the call log. */
    CARAPI_(void) FetchCalls(
        /* [in] */ Int32 token,
        /* [in] */ Int32 callType,
        /* [in] */ Boolean newOnly,
        /* [in] */ Int64 newerThan);

    /** Cancel any pending fetch request. */
    CARAPI_(void) CancelFetch();

    /**
     * Updates the adapter in the call log fragment to show the new cursor data.
     * Returns true if the listener took ownership of the cursor.
     */
    CARAPI_(Boolean) UpdateAdapterData(
        /* [in] */ ICursor* cursor);

    CARAPI_(void) UpdateVoicemailStatus(
        /* [in] */ ICursor* statusCursor);

public:
    /**
     * Call type similar to Calls.INCOMING_TYPE used to specify all types instead of one particular
     * type.
     */
    static const Int32 CALL_TYPE_ALL = -1;

private:
    static const AutoPtr<ArrayOf<String> > EMPTY_STRING_ARRAY;

    static const String TAG;
    static const Int32 NUM_LOGS_TO_DISPLAY = 1000;

    /** The token for the query to fetch the old entries from the call log. */
    static const Int32 QUERY_CALLLOG_TOKEN = 54;
    /** The token for the query to mark all missed calls as old after seeing the call log. */
    static const Int32 UPDATE_MARK_AS_OLD_TOKEN = 55;
    /** The token for the query to mark all new voicemails as old. */
    static const Int32 UPDATE_MARK_VOICEMAILS_AS_OLD_TOKEN = 56;
    /** The token for the query to mark all missed calls as read after seeing the call log. */
    static const Int32 UPDATE_MARK_MISSED_CALL_AS_READ_TOKEN = 57;
    /** The token for the query to fetch voicemail status messages. */
    static const Int32 QUERY_VOICEMAIL_STATUS_TOKEN = 58;

    Int32 mLogLimit;

    AutoPtr<IWeakReference> mListener;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CALLLOGQUERYHANDLER_H__
