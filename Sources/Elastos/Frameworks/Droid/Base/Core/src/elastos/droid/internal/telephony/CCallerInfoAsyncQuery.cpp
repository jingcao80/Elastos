#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/internal/telephony/CCallerInfo.h"
#include "elastos/droid/internal/telephony/CCallerInfoAsyncQuery.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CUserManager.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CCallerInfoAsyncQuery, Object, ICallerInfoAsyncQuery)

CAR_OBJECT_IMPL(CCallerInfoAsyncQuery)

const Boolean CCallerInfoAsyncQuery::DBG;
const String CCallerInfoAsyncQuery::TAG("CallerInfoAsyncQuery");
const Int32 CCallerInfoAsyncQuery::EVENT_NEW_QUERY;
const Int32 CCallerInfoAsyncQuery::EVENT_ADD_LISTENER;
const Int32 CCallerInfoAsyncQuery::EVENT_END_OF_QUEUE;
const Int32 CCallerInfoAsyncQuery::EVENT_EMERGENCY_NUMBER;
const Int32 CCallerInfoAsyncQuery::EVENT_VOICEMAIL_NUMBER;
const Boolean CCallerInfoAsyncQuery::ENABLE_UNKNOWN_NUMBER_GEO_DESCRIPTION;

//==============================================================================
//  CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoWorkerHandler
//==============================================================================

CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoWorkerHandler::CallerInfoWorkerHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ IWeakReference* weakHost)
    : WorkerHandler(looper, weakHost)
{
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

    AutoPtr<WorkerArgs> args = (WorkerArgs*)IObject::Probe(obj);
    AutoPtr<CookieWrapper> cw = (CookieWrapper*)(IObject*)(args->mCookie.Get());
    if (cw == NULL) {
        // Normally, this should never be the case for calls originating
        // from within this code.
        // However, if there is any code that this Handler calls (such as in
        // super.handleMessage) that DOES place unexpected messages on the
        // queue, then we need pass these messages on.
        if (CCallerInfoAsyncQuery::DBG) Logger::D(CCallerInfoAsyncQuery::TAG, "Unexpected command (CookieWrapper is null): %d ignored by CallerInfoWorkerHandler, passing onto parent.", what);

        AsyncQueryHandler::WorkerHandler::HandleMessage(msg);
    }
    else {
        String tempStr = SanitizeUriToString(args->mUri);
        if (CCallerInfoAsyncQuery::DBG) Logger::D(CCallerInfoAsyncQuery::TAG, "Processing event: %d token (arg1): %d command: %d query URI: ", cw->mEvent, arg1, what, tempStr.string());

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
                    args->mHandler->ObtainMessage(what, arg1, 0, (IInterface*)(IObject*)args, (IMessage**)&reply);
                    reply->SendToTarget();
                }
                break;
            default:
                break;
        }
    }
    return NOERROR;
}

//==============================================================================
//  CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler
//==============================================================================

CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoAsyncQueryHandler(
    /* [in] */ CCallerInfoAsyncQuery* host,
    /* [in] */ IContext* context)
    : mHost(host)
{
    AutoPtr<IContentResolver> contentResolver;
    GetCurrentProfileContentResolver(context, (IContentResolver**)&contentResolver);
    AsyncQueryHandler::constructor(contentResolver);
}

ECode CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CreateHandler(
    /* [in] */ ILooper* looper,
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler);

    AutoPtr<IWeakReference> weakHost;
    GetWeakReference((IWeakReference**)&weakHost);
    AutoPtr<CallerInfoWorkerHandler> worker = new CallerInfoWorkerHandler(looper, weakHost);
    *handler = IHandler::Probe(worker);
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

ECode CCallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* cursor)
{
    if (CCallerInfoAsyncQuery::DBG) Logger::D(CCallerInfoAsyncQuery::TAG, "##### onQueryComplete() #####   query complete for token: %d", token);

    //get the cookie and notify the listener.
    AutoPtr<CCallerInfoAsyncQuery::CookieWrapper> cw = (CookieWrapper*) (IObject*) cookie;
    if (cw == NULL) {
        // Normally, this should never be the case for calls originating
        // from within this code.
        // However, if there is any code that calls this method, we should
        // check the parameters to make sure they're viable.
        if (CCallerInfoAsyncQuery::DBG) Logger::D(CCallerInfoAsyncQuery::TAG, "Cookie is null, ignoring onQueryComplete() request.");
        if (cursor != NULL) {
            ICloseable::Probe(cursor)->Close();
        }
        return NOERROR;
    }

    if (cw->mEvent == EVENT_END_OF_QUEUE) {
        mHost->ReleaseResources();
        if (cursor != NULL) {
            ICloseable::Probe(cursor)->Close();
        }
        return NOERROR;
    }

    // check the token and if needed, create the callerinfo object.
    if (mCallerInfo == NULL) {
        if ((mContext == NULL) || (mQueryUri == NULL)) {
            // throw new QueryPoolException
            //         ("Bad context or query uri, or CallerInfoAsyncQuery already released.");
            return E_QUERY_POOL_EXCEPTION;
        }

        // adjust the callerInfo data as needed, and only if it was set from the
        // initial query request.
        // Change the callerInfo number ONLY if it is an emergency number or the
        // voicemail number, and adjust other data (including photoResource)
        // accordingly.
        if (cw->mEvent == EVENT_EMERGENCY_NUMBER) {
            // Note we're setting the phone number here (refer to javadoc
            // comments at the top of CallerInfo class).
            CCallerInfo::New((ICallerInfo**)&mCallerInfo);
            mCallerInfo->MarkAsEmergency(mContext, (ICallerInfo**)&mCallerInfo);
        }
        else if (cw->mEvent == EVENT_VOICEMAIL_NUMBER) {
            CCallerInfo::New((ICallerInfo**)&mCallerInfo);
            mCallerInfo->MarkAsVoiceMail(cw->mSubId, (ICallerInfo**)&mCallerInfo);
        }
        else {
            CCallerInfo::GetCallerInfo(mContext, mQueryUri, cursor, (ICallerInfo**)&mCallerInfo);
            // if (CCallerInfoAsyncQuery::DBG) Logger::D(CCallerInfoAsyncQuery::TAG, "==> Got mCallerInfo: " + mCallerInfo);

            AutoPtr<ICallerInfo> newCallerInfo;
            CCallerInfo::DoSecondaryLookupIfNecessary(
                    mContext, cw->mNumber, mCallerInfo, (ICallerInfo**)&newCallerInfo);
            if (newCallerInfo != mCallerInfo) {
                mCallerInfo = newCallerInfo;
                // if (CCallerInfoAsyncQuery::DBG) Logger::D(CCallerInfoAsyncQuery::TAG, "#####async contact look up with numeric username" + mCallerInfo);
            }

            // Final step: look up the geocoded description.
            if (ENABLE_UNKNOWN_NUMBER_GEO_DESCRIPTION) {
                // Note we do this only if we *don't* have a valid name (i.e. if
                // no contacts matched the phone number of the incoming call),
                // since that's the only case where the incoming-call UI cares
                // about this field.
                //
                // (TODO: But if we ever want the UI to show the geoDescription
                // even when we *do* match a contact, we'll need to either call
                // updateGeoDescription() unconditionally here, or possibly add a
                // new parameter to CallerInfoAsyncQuery.startQuery() to force
                // the geoDescription field to be populated.)

                String nameStr;
                mCallerInfo->GetName(&nameStr);
                if (nameStr.IsEmpty()) {
                    // Actually when no contacts match the incoming phone number,
                    // the CallerInfo object is totally blank here (i.e. no name
                    // *or* phoneNumber).  So we need to pass in cw.number as
                    // a fallback number.
                    mCallerInfo->UpdateGeoDescription(mContext, cw->mNumber);
                }
            }

            // Use the number entered by the user for display.
            if (!cw->mNumber.IsEmpty()) {
                String normalizedNumber;
                mCallerInfo->GetNormalizedNumber(&normalizedNumber);
                String phoneNumber;
                PhoneNumberUtils::FormatNumber(cw->mNumber,
                        normalizedNumber,
                        CCallerInfo::GetCurrentCountryIso(mContext), &phoneNumber);
                mCallerInfo->SetPhoneNumber(phoneNumber);
            }
        }

        if (CCallerInfoAsyncQuery::DBG) Logger::D(CCallerInfoAsyncQuery::TAG, "constructing CallerInfo object for token: %d", token);

        //notify that we can clean up the queue after this.
        AutoPtr<CCallerInfoAsyncQuery::CookieWrapper> endMarker = new CCallerInfoAsyncQuery::CookieWrapper();
        endMarker->mEvent = EVENT_END_OF_QUEUE;
        StartQuery(token, (IInterface*)(IObject*)endMarker, NULL, NULL, String(NULL), NULL, String(NULL));
    }

    //notify the listener that the query is complete.
    if (cw->mListener != NULL) {
        // if (CCallerInfoAsyncQuery::DBG) Logger::D(CCallerInfoAsyncQuery::TAG, "notifying listener: " + cw->mListener.getClass().toString() +
        //              " for token: " + token + mCallerInfo);
        cw->mListener->OnQueryComplete(token, cw->mCookie, mCallerInfo);
    }

    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }

    return NOERROR;
}

//==============================================================================
//  CCallerInfoAsyncQuery
//==============================================================================

CCallerInfoAsyncQuery::CCallerInfoAsyncQuery()
{
}

CCallerInfoAsyncQuery::~CCallerInfoAsyncQuery()
{
}

ECode CCallerInfoAsyncQuery::constructor()
{
    return NOERROR;
}

ECode CCallerInfoAsyncQuery::GetCurrentProfileContentResolver(
    /* [in] */ IContext* context,
    /* [out] */ IContentResolver** result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) Logger::D(TAG, "Trying to get current content resolver...");

    Int32 currentUser = CActivityManager::GetCurrentUser();
    Int32 myUser;
    CUserManager::Get(context)->GetUserHandle(&myUser);

    if (DBG) Logger::D(TAG, String("myUser=") + StringUtils::ToString(myUser) +
        "currentUser=" + StringUtils::ToString(currentUser));

    if (myUser != currentUser) {
        // try {
        String name;
        context->GetPackageName(&name);
        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(currentUser, (IUserHandle**)&userHandle);

        AutoPtr<IContext> otherContext;
        context->CreatePackageContextAsUser(name,
                /* flags =*/ 0, userHandle, (IContext**)&otherContext);
        return otherContext->GetContentResolver(result);
        // } catch (NameNotFoundException e) {
        //     Logger::E(TAG, "Can't find self package", e);
        //     // Fall back to the primary user.
        // }
    }
    return context->GetContentResolver(result);
}

ECode CCallerInfoAsyncQuery::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactRef,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [out] */ ICallerInfoAsyncQuery** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<CCallerInfoAsyncQuery> c;
    CCallerInfoAsyncQuery::NewByFriend((CCallerInfoAsyncQuery**)&c);
    c->Allocate(context, contactRef);

    String strContactRef;
    IObject::Probe(contactRef)->ToString(&strContactRef);
    if (DBG) Logger::D(TAG, "starting query for URI: %s handler: CCallerInfoAsyncQuery", strContactRef.string());

    //create cookieWrapper, start query
    AutoPtr<CCallerInfoAsyncQuery::CookieWrapper> cw = new CCallerInfoAsyncQuery::CookieWrapper();
    cw->mListener = listener;
    cw->mCookie = cookie;
    cw->mEvent = CCallerInfoAsyncQuery::EVENT_NEW_QUERY;

    c->mHandler->StartQuery(token,
                            (IInterface*)(IObject*)(CCallerInfoAsyncQuery::CookieWrapper*)cw, contactRef,
                            (ArrayOf<String>*)NULL,  // projection
                            String(NULL),  // selection
                            (ArrayOf<String>*)NULL,  // selectionArgs
                            String(NULL));  // orderBy

    *result = c;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCallerInfoAsyncQuery::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [out] */ ICallerInfoAsyncQuery** result)
{
    VALIDATE_NOT_NULL(result)

    Int64 subId;
    SubscriptionManager::GetDefaultSubId(&subId);
    return StartQuery(token, context, number, listener, cookie, subId, result);
}

ECode CCallerInfoAsyncQuery::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int64 subId,
    /* [out] */ ICallerInfoAsyncQuery** result)
{
    VALIDATE_NOT_NULL(result)

    // Construct the URI object and query params, and start the query.

    AutoPtr<IUri> baseUri;
    Uri::Parse(String("content://phone_lookup_enterprise"), (IUri**)&baseUri);
    AutoPtr<IUriBuilder> builder;
    baseUri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendPath(number);
    Boolean b;
    builder->AppendQueryParameter(IContactsContractPhoneLookup::QUERY_PARAMETER_SIP_ADDRESS,
                    StringUtils::BooleanToString((PhoneNumberUtils::IsUriNumber(number, &b),b)));
    AutoPtr<IUri> contactRef;
    builder->Build((IUri**)&contactRef);

    if (DBG) {
        Logger::D(TAG, String("==> contactRef: ") + SanitizeUriToString(contactRef));
    }

    AutoPtr<CCallerInfoAsyncQuery> c;
    CCallerInfoAsyncQuery::NewByFriend((CCallerInfoAsyncQuery**)&c);
    c->Allocate(context, contactRef);

    //create cookieWrapper, start query
    AutoPtr<CCallerInfoAsyncQuery::CookieWrapper> cw = new CCallerInfoAsyncQuery::CookieWrapper();
    cw->mListener = listener;
    cw->mCookie = cookie;
    cw->mNumber = number;
    cw->mSubId = subId;

    // check to see if these are recognized numbers, and use shortcuts if we can.
    if (PhoneNumberUtils::IsLocalEmergencyNumber(context, number, &b), b) {
        cw->mEvent = EVENT_EMERGENCY_NUMBER;
    }
    else if (PhoneNumberUtils::IsVoiceMailNumber(subId, number, &b), b) {
        cw->mEvent = EVENT_VOICEMAIL_NUMBER;
    }
    else {
        cw->mEvent = EVENT_NEW_QUERY;
    }

    c->mHandler->StartQuery(token,
                          (IInterface*)(IObject*)cw,  // cookie
                          contactRef,  // uri
                          NULL,  // projection
                          String(NULL),  // selection
                          NULL,  // selectionArgs
                          String(NULL));  // orderBy
    *result = c;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCallerInfoAsyncQuery::AddQueryListener(
    /* [in] */ Int32 token,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie)
{
    String queryuriStr = SanitizeUriToString(mHandler->mQueryUri);
    if (DBG) Logger::D(TAG, "adding listener to query: %s  handler:CallerInfoAsyncQueryHandler ", queryuriStr.string());

    //create cookieWrapper, add query request to end of queue.
    AutoPtr<CookieWrapper> cw = new CookieWrapper();
    cw->mListener = listener;
    cw->mCookie = cookie;
    cw->mEvent = EVENT_ADD_LISTENER;

    return mHandler->StartQuery(token, (IInterface*)(IObject*)(CookieWrapper*)cw, (IUri*)NULL, (ArrayOf<String>*)NULL, String(NULL), (ArrayOf<String>*)NULL, String(NULL));
}

ECode CCallerInfoAsyncQuery::Allocate(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactRef)
{
    if ((context == NULL) || (contactRef == NULL)){
        //throw new QueryPoolException("Bad context or query uri.");
        Logger::E(TAG, "Bad context or query uri.");
        return E_FAIL;
    }
    mHandler = new CallerInfoAsyncQueryHandler(this, context);
    mHandler->mQueryUri = contactRef;
    return NOERROR;
}

ECode CCallerInfoAsyncQuery::ReleaseResources()
{
    mHandler->mContext = NULL;
    mHandler->mQueryUri = NULL;
    mHandler->mCallerInfo = NULL;
    mHandler = NULL;

    return NOERROR;
}

String CCallerInfoAsyncQuery::SanitizeUriToString(
    /* [in] */ IUri* uri)
{
    if (uri != NULL) {
        String uriString;
        IObject::Probe(uri)->ToString(&uriString);
        Int32 indexOfLastSlash = uriString.LastIndexOf('/');
        if (indexOfLastSlash > 0) {
            return uriString.Substring(0, indexOfLastSlash) + "/xxxxxxx";
        } else {
            return uriString;
        }
    } else {
        return String("");
    }
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony
