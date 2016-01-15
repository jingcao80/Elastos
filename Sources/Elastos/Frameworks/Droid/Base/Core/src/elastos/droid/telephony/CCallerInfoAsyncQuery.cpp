
#include "CCallerInfoAsyncQuery.h"
#include "CPhoneNumberUtils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::EIID_IHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String CCallerInfoAsyncQuery::LOG_TAG("CallerInfoAsyncQuery");

CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoAsyncQueryHandler(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    AsyncQueryHandler::Init(contentResolver);
}

CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::~CallerInfoAsyncQueryHandler()
{}

CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoWorkerHandler::CallerInfoWorkerHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ CallerInfoAsyncQueryHandler* context)
    : WorkerHandler(looper, ((AsyncQueryHandler*)context))
{
    //mOwner = context;
}

CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoWorkerHandler::~CallerInfoWorkerHandler()
{}

ECode CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CreateHandler(
    /* [in] */ ILooper* looper,
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler);
    AutoPtr<CallerInfoWorkerHandler> worker = new CallerInfoWorkerHandler(looper, this);
    *handler = (IHandler*)worker.Get();
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

ECode CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoWorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    AutoPtr<WorkerArgs> args = (WorkerArgs*)obj.Get();
    AutoPtr<CookieWrapper> cw = (CookieWrapper*)(args->mCookie.Get());
    if (cw == NULL) {
        // Normally, this should never be the case for calls originating
        // from within this code.
        // However, if there is any code that this Handler calls (such as in
        // super.handleMessage) that DOES place unexpected messages on the
        // queue, then we need pass these messages on.
        if (DBG) Slogger::D(LOG_TAG, "Unexpected command (CookieWrapper is null): %d ignored by CallerInfoWorkerHandler, passing onto parent.", what);

        AsyncQueryHandler::WorkerHandler::HandleMessage(msg);
    }
    else {
        String tempStr;
        SanitizeUriToString(args->mUri, &tempStr);
        if (DBG) Slogger::D(LOG_TAG, "Processing event: %d token (arg1): %d command: %d query URI: ", cw->mEvent, arg1, what, tempStr.string());

        switch (cw->mEvent) {
            case EVENT_NEW_QUERY:
                //start the sql command.
                AsyncQueryHandler::WorkerHandler::HandleMessage(msg);
                break;

            // shortcuts to avoid query for recognized numbers.
            case EVENT_EMERGENCY_NUMBER:
            case EVENT_VOICEMAIL_NUMBER:

            case EVENT_ADD_LISTENER:
            case EVENT_END_OF_QUEUE:
                {
                    // query was already completed, so just send the reply.
                    // passing the original token value back to the caller
                    // on top of the event values in arg1.
                    AutoPtr<IMessage> reply;
                    args->mHandler->ObtainMessage(what, arg1, 0, args, (IMessage**)&reply);
                    reply->SendToTarget();
                }
                break;
            default:
                break;
        }
    }
    return NOERROR;
}

ECode CCallerInfoAsyncQuery::AddQueryListener(
    /* [in] */ Int32 token,
    /* [in] */ IOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie)
{
    String queryuriStr;
    SanitizeUriToString(mHandler->mQueryUri, &queryuriStr);
    if (DBG) Slogger::D(LOG_TAG, "adding listener to query: %s  handler:CallerInfoAsyncQueryHandler ", queryuriStr.string());

    //create cookieWrapper, add query request to end of queue.
    AutoPtr<CookieWrapper> cw = new CookieWrapper();
    cw->mListener = listener;
    cw->mCookie = cookie;
    cw->mEvent = EVENT_ADD_LISTENER;

    return mHandler->StartQuery(token, (IInterface*)(CookieWrapper*)cw, (IUri*)NULL, (ArrayOf<String>*)NULL, String(NULL), (ArrayOf<String>*)NULL, String(NULL));
}

ECode CCallerInfoAsyncQuery::constructor()
{
    return NOERROR;
}

ECode CCallerInfoAsyncQuery::SanitizeUriToString(
        /* [in] */ IUri* uri,
        /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    if (uri != NULL) {
        String uriString;
        uri->ToString(&uriString);
        Int32 indexOfLastSlash = uriString.LastIndexOf('/');
        if (indexOfLastSlash > 0) {
            *str = uriString.Substring(0, indexOfLastSlash) + "/xxxxxxx";
            return NOERROR;
        } else {
            *str = uriString;
            return NOERROR;
        }
    } else {
        *str = String("");
        return NOERROR;
    }
}

ECode CCallerInfoAsyncQuery::Allocate(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactRef)
{

    if ((context == NULL) || (contactRef == NULL)){
        //throw new QueryPoolException("Bad context or query uri.");
        Slogger::E(LOG_TAG, "Bad context or query uri.");
        return E_FAIL;
    }
    mHandler = new CallerInfoAsyncQueryHandler(context);
    mHandler->mQueryContext = context;
    mHandler->mQueryUri = contactRef;
    return NOERROR;
}

ECode CCallerInfoAsyncQuery::Release()
{
    mHandler->mQueryContext = NULL;
    mHandler->mQueryUri = NULL;
    mHandler->mCallerInfo = NULL;
    mHandler = NULL;

    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony
