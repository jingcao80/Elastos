
#include "CCallerInfoAsyncQueryHelper.h"
#include "CPhoneNumberUtils.h"
#include "elastos/droid/net/CUriHelper.h"
#include "CCallerInfoAsyncQuery.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String CCallerInfoAsyncQueryHelper::LOG_TAG("CCallerInfoAsyncQueryHelper");
const Boolean CCallerInfoAsyncQueryHelper::DBG = FALSE;

ECode CCallerInfoAsyncQueryHelper::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactRef,
    /* [in] */ IOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [out] */ ICallerInfoAsyncQuery** cia)
{
    VALIDATE_NOT_NULL(cia);
    AutoPtr<CCallerInfoAsyncQuery> c;
    CCallerInfoAsyncQuery::NewByFriend((CCallerInfoAsyncQuery**)&c);
    c->Allocate(context, contactRef);
    String strContactRef;
    contactRef->ToString(&strContactRef);
    if (DBG) Slogger::D(LOG_TAG, "starting query for URI: %s handler: CCallerInfoAsyncQuery", strContactRef.string());

    //create cookieWrapper, start query
    AutoPtr<CCallerInfoAsyncQuery::CookieWrapper> cw = new CCallerInfoAsyncQuery::CookieWrapper();
    cw->mListener = listener;
    cw->mCookie = cookie;
    cw->mEvent = CCallerInfoAsyncQuery::EVENT_NEW_QUERY;

    c->mHandler->StartQuery(token,
                            (IInterface*)(CCallerInfoAsyncQuery::CookieWrapper*)cw, contactRef,
                            (ArrayOf<String>*)NULL,  // projection
                            String(NULL),  // selection
                            (ArrayOf<String>*)NULL,  // selectionArgs
                            String(NULL));  // orderBy

    *cia = c;
    REFCOUNT_ADD(*cia);
    return NOERROR;
}

ECode CCallerInfoAsyncQueryHelper::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ IOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [out] */ ICallerInfoAsyncQuery** cia)
{
    if (DBG) {
        Slogger::D(LOG_TAG, "##### CallerInfoAsyncQuery startQuery()... #####");
        Slogger::D(LOG_TAG, "- number: /*number*/ xxxxxxx");
        Slogger::D(LOG_TAG, "- cookie: %p", cookie);
    }

    // Construct the URI object and query params, and start the query.

    AutoPtr<IUri> contactRef;
    String selection;
    AutoPtr< ArrayOf<String> > selectionArgs;
    Boolean tempRes;

    if (CPhoneNumberUtils::IsUriNumber(number, &tempRes), tempRes) {
        // "number" is really a SIP address.
        if (DBG) Slogger::D(LOG_TAG, "  - Treating number as a SIP address:  /*number*/ xxxxxxx");

        // We look up SIP addresses directly in the Data table:
        AutoPtr<IUri> baseUri, datauri;
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        String encodedStr;
        helper->Encode(number, &encodedStr);
        helper->Parse(String("content://com.android.contacts"), (IUri**)&baseUri);
        helper->WithAppendedPath(baseUri, String("data"), (IUri**)&contactRef);

        // Note Data.DATA1 and SipAddress.SIP_ADDRESS are equivalent.
        //
        // Also note we use "upper(data1)" in the WHERE clause, and
        // uppercase the incoming SIP address, in order to do a
        // case-insensitive match.
        //
        // TODO: need to confirm that the use of upper() doesn't
        // prevent us from using the index!  (Linear scan of the whole
        // contacts DB can be very slow.)
        //
        // TODO: May also need to normalize by adding "sip:" as a
        // prefix, if we start storing SIP addresses that way in the
        // database.
        StringBuilder sb;
        sb.Append("upper(");
        sb.Append("data1");
        sb.Append(")=?");
        sb.Append(" AND ");
        sb.Append("mimetype");
        sb.Append("=");
        sb.Append("vnd.android.cursor.item/sip_address");
        sb.Append("'");
        selection = sb.ToString();
        selectionArgs = ArrayOf<String>::Alloc(1);
        (*selectionArgs)[0] = number.ToUpperCase();

    } else {
        // "number" is a regular phone number.  Use the PhoneLookup table:
        AutoPtr<IUri> baseUri, datauri;
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        String encodedNumber;
        helper->Encode(number, &encodedNumber);
        helper->Parse(String("content://com.android.contacts"), (IUri**)&baseUri);
        helper->WithAppendedPath(baseUri, String("phone_lookup"), (IUri**)&datauri);
        helper->WithAppendedPath(contactRef, encodedNumber, (IUri**)&contactRef);
        selection = String(NULL);
        selectionArgs = (ArrayOf<String>*)NULL;
    }

    if (DBG) {
        String strContactRef;
        CCallerInfoAsyncQuery::SanitizeUriToString(contactRef, &strContactRef);
        Slogger::D(LOG_TAG, "==> contactRef: %s", strContactRef.string());
        Slogger::D(LOG_TAG, "==> selection: %s", selection.string());
        if (selectionArgs != NULL) {
            for (Int32 i = 0; i < selectionArgs->GetLength(); i++) {
                Slogger::D(LOG_TAG, "==> selectionArgs[%d]: ",i, (*selectionArgs)[i].string());
            }
        }
    }

    AutoPtr<CCallerInfoAsyncQuery> c;
    CCallerInfoAsyncQuery::NewByFriend((CCallerInfoAsyncQuery**)&c);
    c->Allocate(context, contactRef);

    //create cookieWrapper, start query
    AutoPtr<CCallerInfoAsyncQuery::CookieWrapper> cw = new CCallerInfoAsyncQuery::CookieWrapper();
    cw->mListener = listener;
    cw->mCookie = cookie;
    cw->mNumber = number;

    // check to see if these are recognized numbers, and use shortcuts if we can.
    if (CPhoneNumberUtils::IsLocalEmergencyNumber(number, context, &tempRes), tempRes) {
        cw->mEvent = CCallerInfoAsyncQuery::EVENT_EMERGENCY_NUMBER;
    } else if (CPhoneNumberUtils::IsVoiceMailNumber(number, &tempRes), tempRes) {
        cw->mEvent = CCallerInfoAsyncQuery::EVENT_VOICEMAIL_NUMBER;
    } else {
        cw->mEvent = CCallerInfoAsyncQuery::EVENT_NEW_QUERY;
    }

    c->mHandler->StartQuery(token,
                          (IInterface*)(CCallerInfoAsyncQuery::CookieWrapper*)cw,  // cookie
                          contactRef,  // uri
                          (ArrayOf<String>*)NULL,  // projection
                          String(NULL),  // selection
                          (ArrayOf<String>*)selectionArgs,  // selectionArgs
                          String(NULL));  // orderBy
    *cia = c;
    REFCOUNT_ADD(*cia);
    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony