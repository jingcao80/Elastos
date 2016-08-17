
#ifndef __ELASTOS_DROID_INCALLUI_CALLERINFOASYNCQUERY_H__
#define __ELASTOS_DROID_INCALLUI_CALLERINFOASYNCQUERY_H__

#include "elastos/droid/incallui/CallerInfo.h"
#include "elastos/droid/content/AsyncQueryHandler.h"

using Elastos::Droid::Content::AsyncQueryHandler;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Helper class to make it easier to run asynchronous caller-id lookup queries.
 * @see CallerInfo
 *
 */
class CallerInfoAsyncQuery
    : public Object
{
public:
    /**
     * Simple exception used to communicate problems with the query pool.
     */
    class QueryPoolException
        : public Object     // SQLException
    {
    public:
        QueryPoolException(
            /* [in] */ const String& error);
    };

private:
    /**
     * Wrap the cookie from the WorkerArgs with additional information needed by our
     * classes.
     */
    class CookieWrapper
        : public Object
    {
    public:
        CookieWrapper()
            : mEvent(0)
            , mNumber(NULL)
        {}

    public:
        AutoPtr<IOnQueryCompleteListener> mListener;
        AutoPtr<IInterface> mCookie;
        Int32 mEvent;
        String mNumber;
    };

    /**
     * Our own implementation of the AsyncQueryHandler.
     */
    class CallerInfoAsyncQueryHandler
        : public AsyncQueryHandler
    {
    protected:
        /**
         * Our own query worker thread.
         *
         * This thread handles the messages enqueued in the looper.  The normal sequence
         * of events is that a new query shows up in the looper queue, followed by 0 or
         * more add listener requests, and then an end request.  Of course, these requests
         * can be interlaced with requests from other tokens, but is irrelevant to this
         * handler since the handler has no state.
         *
         * Note that we depend on the queue to keep things in order; in other words, the
         * looper queue must be FIFO with respect to input from the synchronous startQuery
         * calls and output to this handleMessage call.
         *
         * This use of the queue is required because CallerInfo objects may be accessed
         * multiple times before the query is complete.  All accesses (listeners) must be
         * queued up and informed in order when the query is complete.
         */
        class CallerInfoWorkerHandler
            : public WorkerHandler
        {
        public:
            CallerInfoWorkerHandler(
                /* [in] */ IWeakReference* wr);

            //@Override
            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);
        };

    public:
        /**
         * Asynchronous query handler class for the contact / callerinfo object.
         */
        CallerInfoAsyncQueryHandler(
            /* [in] */ IContext* context,
            /* [in] */ CallerInfoAsyncQuery* host);

        // @Override
        CARAPI StartQuery(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ IUri* uri,
            /* [in] */ ArrayOf<String>* projection,
            /* [in] */ const String& selection,
            /* [in] */ ArrayOf<String>* selectionArgs,
            /* [in] */ const String& orderBy);

        //@Override
        CARAPI CreateHandler(
            /* [in] */ ILooper* looper,
            /* [out] */ IHandler** handler);

        /**
         * Overrides onQueryComplete from AsyncQueryHandler.
         *
         * This method takes into account the state of this class; we construct the CallerInfo
         * object only once for each set of listeners. When the query thread has done its work
         * and calls this method, we inform the remaining listeners in the queue, until we're
         * out of listeners.  Once we get the message indicating that we should expect no new
         * listeners for this CallerInfo object, we release the AsyncCursorInfo back into the
         * pool.
         */
        //@Override
        CARAPI OnQueryComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ ICursor* cursor);

    public:
        /**
         * The information relevant to each CallerInfo query.  Each query may have multiple
         * listeners, so each AsyncCursorInfo is associated with 2 or more CookieWrapper
         * objects in the queue (one with a new query event, and one with a end event, with
         * 0 or more additional listeners in between).
         */
        AutoPtr<IContext> mQueryContext;
        AutoPtr<IUri> mQueryUri;
        AutoPtr<CallerInfo> mCallerInfo;
        CallerInfoAsyncQuery* mHost;
    };

public:
    /**
     * Private constructor for factory methods.
     */
    CallerInfoAsyncQuery();

    /**
     * Factory method to start the query based on a CallerInfo object.
     *
     * Note: if the number contains an "@" character we treat it
     * as a SIP address, and look it up directly in the Data table
     * rather than using the PhoneLookup table.
     * TODO: But eventually we should expose two separate methods, one for
     * numbers and one for SIP addresses, and then have
     * PhoneUtils.startGetCallerInfo() decide which one to call based on
     * the phone type of the incoming connection.
     */
    static CARAPI_(AutoPtr<CallerInfoAsyncQuery>) StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IContext* context,
        /* [in] */ CallerInfo* info,
        /* [in] */ IOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie);

    /**
     * Releases the relevant data.
     */
    CARAPI_(UInt32) CallerInfoAsyncQueryRelease();

    /**
     * Method to create a new CallerInfoAsyncQueryHandler object, ensuring correct
     * state of context and uri.
     */
    CARAPI_(void) Allocate(
        /* [in] */ IContext* context,
        /* [in] */ IUri* contactRef);

    static CARAPI_(String) SanitizeUriToString(
        /* [in] */ IUri* uri);

public:
    static Boolean DBG;
    static String LOGTAG;

    static const Int32 EVENT_NEW_QUERY;
    static const Int32 EVENT_ADD_LISTENER;
    static const Int32 EVENT_END_OF_QUEUE;
    static const Int32 EVENT_EMERGENCY_NUMBER;
    static const Int32 EVENT_VOICEMAIL_NUMBER;

    AutoPtr<CallerInfoAsyncQueryHandler> mHandler;

    // If the CallerInfo query finds no contacts, should we use the
    // PhoneNumberOfflineGeocoder to look up a "geo description"?
    // (TODO: This could become a flag in config.xml if it ever needs to be
    // configured on a per-product basis.)
    static Boolean ENABLE_UNKNOWN_NUMBER_GEO_DESCRIPTION;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CALLERINFOASYNCQUERY_H__
