
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERY_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Internal_Telephony_CCallerInfoAsyncQuery.h"
#include "elastos/droid/content/AsyncQueryHandler.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::AsyncQueryHandler;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CCallerInfoAsyncQuery)
    , public Object
    , public ICallerInfoAsyncQuery
{
private:
    class CookieWrapper
        : public Object
    {
    public:
        AutoPtr<ICallerInfoAsyncQueryOnQueryCompleteListener> mListener;
        AutoPtr<IInterface> mCookie;
        Int32 mEvent;
        String mNumber;
        Int64 mSubId;
    };

    class CallerInfoAsyncQueryHandler
        : public AsyncQueryHandler
    {
        friend class CCallerInfoAsyncQuery;

    protected:
        class CallerInfoWorkerHandler
            : public AsyncQueryHandler::WorkerHandler
        {
        public:
            TO_STRING_IMPL("CCallerInfoAsyncQuery::CallerInfoWorkerHandler")

            CallerInfoWorkerHandler(
                /* [in] */ IWeakReference* host);

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);
        };

    public:
        TO_STRING_IMPL("CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler")

        /**
         * Asynchronous query handler class for the contact / callerinfo object.
         */
        CallerInfoAsyncQueryHandler(
            /* [in] */ CCallerInfoAsyncQuery* host,
            /* [in] */ IContext* context);

    protected:
        CARAPI CreateHandler(
            /* [in] */ ILooper* looper,
            /* [out] */ IHandler** handler);

        CARAPI OnQueryComplete(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ ICursor* cursor);

    private:
        /*
         * The information relevant to each CallerInfo query.  Each query may have multiple
         * listeners, so each AsyncCursorInfo is associated with 2 or more CookieWrapper
         * objects in the queue (one with a new query event, and one with a end event, with
         * 0 or more additional listeners in between).
         */

        /**
         * Context passed by the caller.
         *
         * NOTE: The actual context we use for query may *not* be this context; since we query
         * against the "current" contacts provider.  In the constructor we pass the "current"
         * context resolver (obtained via {@link #getCurrentProfileContentResolver) and pass it
         * to the super class.
         */
        AutoPtr<IContext> mContext;
        AutoPtr<IUri> mQueryUri;
        AutoPtr<ICallerInfo> mCallerInfo;

        AutoPtr<CCallerInfoAsyncQuery> mHost;
    };

public:
    friend class CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler;
    friend class CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoWorkerHandler;

    CCallerInfoAsyncQuery();

    virtual ~CCallerInfoAsyncQuery();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * @return {@link ContentResolver} for the "current" user.
     */
    static CARAPI GetCurrentProfileContentResolver(
        /* [in] */ IContext* context,
        /* [out] */ IContentResolver** result);

    /**
     * Factory method to start query with a Uri query spec
     */
    static CARAPI StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IContext* ctx,
        /* [in] */ IUri* contactRef,
        /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie,
        /* [out] */ ICallerInfoAsyncQuery** cia);

    /**
     * Factory method to start the query based on a number.
     *
     * Note: if the number contains an "@" character we treat it
     * as a SIP address, and look it up directly in the Data table
     * rather than using the PhoneLookup table.
     * TODO: But eventually we should expose two separate methods, one for
     * numbers and one for SIP addresses, and then have
     * PhoneUtils.startGetCallerInfo() decide which one to call based on
     * the phone type of the incoming connection.
     */
    static CARAPI StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IContext* ctx,
        /* [in] */ const String& number,
        /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie,
        /* [out] */ ICallerInfoAsyncQuery** cia);

    /**
     * Factory method to start the query based on a number with specific subscription.
     *
     * Note: if the number contains an "@" character we treat it
     * as a SIP address, and look it up directly in the Data table
     * rather than using the PhoneLookup table.
     * TODO: But eventually we should expose two separate methods, one for
     * numbers and one for SIP addresses, and then have
     * PhoneUtils.startGetCallerInfo() decide which one to call based on
     * the phone type of the incoming connection.
     */
    static CARAPI StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IContext* ctx,
        /* [in] */ const String& number,
        /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie,
        /* [in] */ Int64 subId,
        /* [out] */ ICallerInfoAsyncQuery** cia);

    /**
     * Method to add listeners to a currently running query
     */
    CARAPI AddQueryListener(
        /* [in] */ Int32 token,
        /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie);

private:
    /**
     * Method to create a new CallerInfoAsyncQueryHandler object, ensuring correct
     * state of context and uri.
     */
    CARAPI Allocate(
        /* [in] */ IContext* context,
        /* [in] */ IUri* contactRef);

    /**
     * Releases the relevant data.
     */
    CARAPI ReleaseResources();

    static CARAPI_(String) SanitizeUriToString(
        /* [in] */ IUri* uri);

private:
    static const Boolean DBG = FALSE;
    static const String TAG;

    static const Int32 EVENT_NEW_QUERY = 1;
    static const Int32 EVENT_ADD_LISTENER = 2;
    static const Int32 EVENT_END_OF_QUEUE = 3;
    static const Int32 EVENT_EMERGENCY_NUMBER = 4;
    static const Int32 EVENT_VOICEMAIL_NUMBER = 5;

    // If the CallerInfo query finds no contacts, should we use the
    // PhoneNumberOfflineGeocoder to look up a "geo description"?
    // (TODO: This could become a flag in config.xml if it ever needs to be
    // configured on a per-product basis.)
    static const Boolean ENABLE_UNKNOWN_NUMBER_GEO_DESCRIPTION = TRUE;

    AutoPtr<CallerInfoAsyncQueryHandler> mHandler;
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERY_H__
