
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/incallui/CallerInfoAsyncQuery.h"
#include "elastos/droid/incallui/Call.h"
#include "elastos/droid/contacts/common/util/TelephonyManagerUtils.h"
#include "elastos/droid/contacts/common/util/PhoneNumberHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Contacts::Common::Util::TelephonyManagerUtils;
using Elastos::Droid::Contacts::Common::Util::PhoneNumberHelper;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Provider::CContactsContractPhoneLookup;
using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;

using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Arrays;
using Elastos::Utility::ILocale;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

Boolean CallerInfoAsyncQuery::DBG = FALSE;
String CallerInfoAsyncQuery::LOGTAG("CallerInfoAsyncQuery");

const Int32 CallerInfoAsyncQuery::EVENT_NEW_QUERY = 1;
const Int32 CallerInfoAsyncQuery::EVENT_ADD_LISTENER = 2;
const Int32 CallerInfoAsyncQuery::EVENT_END_OF_QUEUE = 3;
const Int32 CallerInfoAsyncQuery::EVENT_EMERGENCY_NUMBER = 4;
const Int32 CallerInfoAsyncQuery::EVENT_VOICEMAIL_NUMBER = 5;

Boolean CallerInfoAsyncQuery::ENABLE_UNKNOWN_NUMBER_GEO_DESCRIPTION = TRUE;

//================================================================
// CallerInfoAsyncQuery::QueryPoolException::
//================================================================
CallerInfoAsyncQuery::QueryPoolException::QueryPoolException(
    /* [in] */ const String& error)
//    : SQLException(error)
{
}

//================================================================
// CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoWorkerHandler::
//================================================================
CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoWorkerHandler::CallerInfoWorkerHandler(
    /* [in] */ IWeakReference* wr)
    : WorkerHandler(wr)
{
}

ECode CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoWorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<WorkerArgs> args = (WorkerArgs*)IObject::Probe(obj);
    AutoPtr<CookieWrapper> cw = (CookieWrapper*)IObject::Probe(args->mCookie);

    Int32 what = 0;
    msg->GetWhat(&what);
    Int32 arg1 = 0;
    msg->GetArg1(&arg1);
    if (cw == NULL) {
        // Normally, this should never be the case for calls originating
        // from within this code.
        // However, if there is any code that this Handler calls (such as in
        // super.handleMessage) that DOES place unexpected messages on the
        // queue, then we need pass these messages on.
        Logger::D("CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoWorkerHandler",
                "Unexpected command (CookieWrapper is NULL): %d ignored by CallerInfoWorkerHandler, passing onto parent.",
                what);

        WorkerHandler::HandleMessage(msg);
    }
    else {
        Logger::D("CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoWorkerHandler",
                "Processing event: %d token (arg1): %d command: %d query URI: %s",
                cw->mEvent, arg1, what,
                SanitizeUriToString(args->mUri).string());

        switch (cw->mEvent) {
            case EVENT_NEW_QUERY:
                //start the sql command.
                WorkerHandler::HandleMessage(msg);
                break;

            // shortcuts to avoid query for recognized numbers.
            case EVENT_EMERGENCY_NUMBER:
            case EVENT_VOICEMAIL_NUMBER:

            case EVENT_ADD_LISTENER:
            case EVENT_END_OF_QUEUE: {
                // query was already completed, so just send the reply.
                // passing the original token value back to the caller
                // on top of the event values in arg1.
                AutoPtr<IMessage> reply;
                args->mHandler->ObtainMessage(what, (IMessage**)&reply);
                reply->SetObj((IObject*)args.Get());
                reply->SetArg1(arg1);

                reply->SendToTarget();

                break;
            }
            default:
                break;
        }
    }
    return NOERROR;
}

//================================================================
// CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::
//================================================================

ECode CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& orderBy)
{
    if (DBG) {
        // Show stack trace with the arguments.
        Logger::D(LOGTAG, "InCall: startQuery: url=%s projection=[%s] selection=%s args=[%s]",
                    TO_CSTR(uri), Arrays::ToString(projection).string(),
                    selection.string(), Arrays::ToString(selectionArgs).string());
                // new RuntimeException("STACKTRACE"));
    }
    AsyncQueryHandler::StartQuery(token, cookie, uri, projection, selection, selectionArgs, orderBy);
    return NOERROR;
}

CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CallerInfoAsyncQueryHandler(
    /* [in] */ IContext* context,
    /* [in] */ CallerInfoAsyncQuery* host)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AsyncQueryHandler::constructor(cr);
    mHost = host;
}

ECode CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::CreateHandler(
    /* [in] */ ILooper* looper,
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    AutoPtr<IWeakReference> wr;
    IWeakReferenceSource::Probe(mQueryContext)->GetWeakReference((IWeakReference**)&wr);
    AutoPtr<CallerInfoWorkerHandler> p = new CallerInfoWorkerHandler(wr);
    p->constructor(looper);
    *handler = IHandler::Probe(p);
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* cursor)
{
    // try {
        Logger::D("CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler",
            "##### onQueryComplete() #####   query complete for token: %d",
            token);

        //get the cookie and notify the listener.
        AutoPtr<CookieWrapper> cw = (CookieWrapper*)IObject::Probe(cookie);
        if (cw == NULL) {
            // Normally, this should never be the case for calls originating
            // from within this code.
            // However, if there is any code that calls this method, we should
            // check the parameters to make sure they're viable.
            Logger::D("CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler",
                "Cookie is NULL, ignoring onQueryComplete() request.");
            return NOERROR;
        }

        if (cw->mEvent == EVENT_END_OF_QUEUE) {
            mHost->CallerInfoAsyncQueryRelease();
            return NOERROR;
        }

        // check the token and if needed, create the callerinfo object.
        if (mCallerInfo == NULL) {
            if ((mQueryContext == NULL) || (mQueryUri == NULL)) {
                // throw new QueryPoolException
                //     ("Bad context or query uri, or CallerInfoAsyncQuery already released.");
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
                AutoPtr<CallerInfo> p = new CallerInfo();
                mCallerInfo = p->MarkAsEmergency(mQueryContext, cw->mNumber);
            }
            else if (cw->mEvent == EVENT_VOICEMAIL_NUMBER) {
                AutoPtr<CallerInfo> p = new CallerInfo();
                mCallerInfo = p->MarkAsVoiceMail(mQueryContext);
            }
            else {
                mCallerInfo = CallerInfo::GetCallerInfo(mQueryContext, mQueryUri, cursor);
                Logger::D("CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler", "==> Got mCallerInfo: %s",
                        TO_CSTR(mCallerInfo));

                AutoPtr<CallerInfo> newCallerInfo = CallerInfo::DoSecondaryLookupIfNecessary(
                        mQueryContext, cw->mNumber, mCallerInfo);
                if (newCallerInfo != mCallerInfo) {
                    mCallerInfo = newCallerInfo;
                    Logger::D("CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler",
                            "#####async contact look up with numeric username %s",
                            TO_CSTR(mCallerInfo));
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

                    if (TextUtils::IsEmpty(mCallerInfo->mName)) {
                        // Actually when no contacts match the incoming phone number,
                        // the CallerInfo object is totally blank here (i.e. no name
                        // *or* phoneNumber).  So we need to pass in cw.number as
                        // a fallback number.
                        mCallerInfo->UpdateGeoDescription(mQueryContext, cw->mNumber);
                    }
                }

                // Use the number entered by the user for display.
                if (!TextUtils::IsEmpty(cw->mNumber)) {
                    AutoPtr<ILocaleHelper> lhlp;
                    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lhlp);
                    AutoPtr<ILocale> loc;
                    lhlp->GetDefault((ILocale**)&loc);
                    String str;
                    TelephonyManagerUtils::GetCurrentCountryIso(mQueryContext,
                                    loc, &str);
                    mCallerInfo->mPhoneNumber = PhoneNumberHelper::FormatNumber(cw->mNumber,
                            mCallerInfo->mNormalizedNumber, str);
                }
            }

            Logger::D("CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler",
                "constructing CallerInfo object for token: %d",
                token);

            //notify that we can clean up the queue after this.
            AutoPtr<CookieWrapper> endMarker = new CookieWrapper();
            endMarker->mEvent = EVENT_END_OF_QUEUE;
            StartQuery(token, (IObject*)endMarker.Get(), NULL, NULL, String(NULL), NULL, String(NULL));
        }

        //notify the listener that the query is complete.
        if (cw->mListener != NULL) {
            Logger::D("CallerInfoAsyncQuery::CallerInfoAsyncQueryHandler",
                    "notifying listener:  for token: %d %s",
                    /*+ cw->mListener->GetClass()->ToString()*/ +
                    token, TO_CSTR(mCallerInfo));
            cw->mListener->OnQueryComplete(token, cw->mCookie, mCallerInfo);
        }
    // } finally {
        // The cursor may have been closed in CallerInfo.getCallerInfo()
        Boolean bClosed = FALSE;
        if (cursor != NULL && !(cursor->IsClosed(&bClosed), bClosed)) {
            ICloseable::Probe(cursor)->Close();
        }
    // }
    return NOERROR;
}

//================================================================
// CallerInfoAsyncQuery::
//================================================================
CallerInfoAsyncQuery::CallerInfoAsyncQuery()
{
}

AutoPtr<CallerInfoAsyncQuery> CallerInfoAsyncQuery::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* context,
    /* [in] */ CallerInfo* info,
    /* [in] */ IOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie)
{
    Logger::D(LOGTAG, "##### CallerInfoAsyncQuery startQuery()... #####");
    Logger::D(LOGTAG, "- number: %s", info->mPhoneNumber.string());
    Logger::D(LOGTAG, "- cookie: %s", TO_CSTR(cookie));

    // Construct the URI object and query params, and start the query.

    AutoPtr<IContactsContractPhoneLookup> lookup;
    CContactsContractPhoneLookup::AcquireSingleton((IContactsContractPhoneLookup**)&lookup);
    AutoPtr<IUri> filter_uri;
    lookup->GetENTERPRISE_CONTENT_FILTER_URI((IUri**)&filter_uri);
    AutoPtr<IUriBuilder> build;
    filter_uri->BuildUpon((IUriBuilder**)&build);
    build->AppendPath(info->mPhoneNumber);
    build->AppendQueryParameter(IContactsContractPhoneLookup::QUERY_PARAMETER_SIP_ADDRESS,
                    StringUtils::ToString(PhoneNumberHelper::IsUriNumber(info->mPhoneNumber)));
    AutoPtr<IUri> contactRef;
    build->Build((IUri**)&contactRef);

    if (DBG) {
        Logger::D(LOGTAG, "==> contactRef: %s", SanitizeUriToString(contactRef).string());
    }

    AutoPtr<CallerInfoAsyncQuery> c = new CallerInfoAsyncQuery();
    c->Allocate(context, contactRef);

    //create cookieWrapper, start query
    AutoPtr<CookieWrapper> cw = new CookieWrapper();
    cw->mListener = listener;
    cw->mCookie = cookie;
    cw->mNumber = info->mPhoneNumber;

    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    Int64 subId = 0;
    sm->GetDefaultVoiceSubId(&subId);
    AutoPtr<Call> call = (Call*)ICall::Probe(cookie);
    if (call != NULL) {
        subId = call->GetSubId();
    }
    // check to see if these are recognized numbers, and use shortcuts if we can.
    AutoPtr<IPhoneNumberUtils> pu;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pu);
    Boolean bEmergencyNum = FALSE, bVoiceNumber = FALSE;
    pu->IsLocalEmergencyNumber(context, info->mPhoneNumber, &bEmergencyNum);
    pu->IsVoiceMailNumber(subId, info->mPhoneNumber, &bVoiceNumber);
    if (bEmergencyNum) {
        cw->mEvent = EVENT_EMERGENCY_NUMBER;
    }
    else if (info->IsVoiceMailNumber()
            || bVoiceNumber) {
        cw->mEvent = EVENT_VOICEMAIL_NUMBER;
    }
    else {
        cw->mEvent = EVENT_NEW_QUERY;
    }

    c->mHandler->StartQuery(token,
                            (IObject*)cw.Get(),  // cookie
                            contactRef,  // uri
                            NULL,  // projection
                            String(NULL),  // selection
                            NULL,  // selectionArgs
                            String(NULL));  // orderBy
    return c;
}

void CallerInfoAsyncQuery::Allocate(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactRef)
{
    if ((context == NULL) || (contactRef == NULL)){
        // throw new QueryPoolException("Bad context or query uri.");
        return;
    }
    mHandler = new CallerInfoAsyncQueryHandler(context, this);
    mHandler->mQueryContext = context;
    mHandler->mQueryUri = contactRef;
}

UInt32 CallerInfoAsyncQuery::CallerInfoAsyncQueryRelease()
{
    mHandler->mQueryContext = NULL;
    mHandler->mQueryUri = NULL;
    mHandler->mCallerInfo = NULL;
    mHandler = NULL;
    return 0;
}

String CallerInfoAsyncQuery::SanitizeUriToString(
    /* [in] */ IUri* uri)
{
    if (uri != NULL) {
        String uriString;
        IObject::Probe(uri)->ToString(&uriString);
        Int32 indexOfLastSlash = uriString.LastIndexOf('/');
        if (indexOfLastSlash > 0) {
            return uriString.Substring(0, indexOfLastSlash) + String("/xxxxxxx");
        }
        else {
            return uriString;
        }
    }
    else {
        return String("");
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
