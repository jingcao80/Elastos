
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERY_H__

#include "_Elastos_Droid_Internal_Telephony_CCallerInfoAsyncQuery.h"
#include "elastos/droid/content/AsyncQueryHandler.h"
#include "CCallerInfoAsyncQueryHelper.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::AsyncQueryHandler;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CCallerInfoAsyncQuery)
{
public:

    friend class CallerInfoAsyncQueryHandler;
    friend class CallerInfoWorkerHandler;
    friend class CCallerInfoAsyncQueryHelper;

    CARAPI constructor();

    CARAPI AddQueryListener(
        /* [in] */ Int32 token,
        /* [in] */ IOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie);


    static CARAPI SanitizeUriToString(
        /* [in] */ IUri* uri,
        /* [out] */ String* res);


private:

    class  CookieWrapper
        : public ElRefBase
    {
    public:
        AutoPtr<IOnQueryCompleteListener> mListener;
        AutoPtr<IInterface> mCookie;
        Int32 mEvent;
        String mNumber;
    };

    class CallerInfoAsyncQueryHandler
        : public AsyncQueryHandler
    {
    public:

        /**
         * Asynchronous query handler class for the contact / callerinfo object.
         */

        CallerInfoAsyncQueryHandler(
            /* [in] */ IContext* context);

        virtual ~CallerInfoAsyncQueryHandler();

    protected:

        class CallerInfoWorkerHandler
            : public AsyncQueryHandler::WorkerHandler
        {
            public:

                CallerInfoWorkerHandler(
                        /* [in] */ ILooper* looper,
                        /* [in] */ CallerInfoAsyncQueryHandler* context);

                virtual ~CallerInfoWorkerHandler();

                CARAPI HandleMessage(
                    /* [in] */ IMessage* msg);
        };

        CARAPI CreateHandler(
            /* [in] */ ILooper* looper,
            /* [out] */ IHandler** handler);

    private:
        friend class CCallerInfoAsyncQuery;
        /**
         * The information relevant to each CallerInfo query.  Each query may have multiple
         * listeners, so each AsyncCursorInfo is associated with 2 or more CookieWrapper
         * objects in the queue (one with a new query event, and one with a end event, with
         * 0 or more additional listeners in between).
         */
        AutoPtr<IContext> mQueryContext;
        AutoPtr<IUri> mQueryUri;
        AutoPtr<ICallerInfo> mCallerInfo;
        AutoPtr<CallerInfoAsyncQueryHandler> mOwner;
    };

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
    CARAPI Release();

    static const Boolean DBG = FALSE;
    static const String LOG_TAG/* = "CallerInfoAsyncQuery"*/;

    static const Int32 EVENT_NEW_QUERY = 1;
    static const Int32 EVENT_ADD_LISTENER = 2;
    static const Int32 EVENT_END_OF_QUEUE = 3;
    static const Int32 EVENT_EMERGENCY_NUMBER = 4;
    static const Int32 EVENT_VOICEMAIL_NUMBER = 5;

    AutoPtr<CallerInfoAsyncQueryHandler> mHandler;

    // If the CallerInfo query finds no contacts, should we use the
    // PhoneNumberOfflineGeocoder to look up a "geo description"?
    // (TODO: This could become a flag in config.xml if it ever needs to be
    // configured on a per-product basis.)
    static const Boolean ENABLE_UNKNOWN_NUMBER_GEO_DESCRIPTION = TRUE;
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERY_H__
