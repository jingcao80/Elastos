
#include "elastos/droid/server/telecom/RespondViaSmsManager.h"
#include "elastos/droid/server/telecom/QuickResponseUtils.h"
#include "R.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/internal/os/SomeArgs.h>
#include <elastos/droid/net/Uri.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// RespondViaSmsManager::SubHandler
//=============================================================================
RespondViaSmsManager::SubHandler::SubHandler(
    /* [in] */ RespondViaSmsManager* host)
    : mHost(host)
{}

ECode RespondViaSmsManager::SubHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    if (msgWhat == MSG_CANNED_TEXT_MESSAGES_READY) {
        AutoPtr<SomeArgs> args = (SomeArgs*) IObject::Probe(msgObj);
        // try {
        ECode ec;
        AutoPtr<IResponse> response =
                IResponse::Probe(args->mArg1);
        AutoPtr<IList> textMessages =
                IList::Probe(args->mArg2);
        if (textMessages != NULL) {
            Int32 size;
            textMessages->GetSize(&size);
            AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(size);
            for (Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> obj;
                textMessages->Get(i, (IInterface**)&obj);
                array->Set(i, obj);
            }
            ec = response->OnResult(NULL, array);
        } else {
            ec = response->OnError(NULL, 0, String(NULL));
        }
        // } finally {
        args->Recycle();
        if (FAILED(ec)) return ec;
        // }
    }
    else if (msgWhat == MSG_SHOW_SENT_TOAST) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        ECode ec;
        String toastMessage;
        IObject::Probe(args->mArg1)->ToString(&toastMessage);
        AutoPtr<IContext> context = IContext::Probe(args->mArg2);
        ec = mHost->ShowMessageSentToast(toastMessage, context);
        // } finally {
        args->Recycle();
        if (FAILED(ec)) return ec;
        // }
    }
    return NOERROR;
}

//=============================================================================
// RespondViaSmsManager::SubThread
//=============================================================================
RespondViaSmsManager::SubThread::SubThread(
    /* [in] */ RespondViaSmsManager* host,
    /* [in] */ IResponse* response,
    /* [in] */ IContext* context)
    : mHost(host)
    , mResponse(response)
    , mContext(context)
{}

ECode RespondViaSmsManager::SubThread::Run()
{
    Log::D("RespondViaSmsManager", "loadCannedResponses() starting");
    // This function guarantees that QuickResponses will be in our
    // SharedPreferences with the proper values considering there may be
    // old QuickResponses in Telephony pre L.
    QuickResponseUtils::MaybeMigrateLegacyQuickResponses(mContext);
    AutoPtr<ISharedPreferences> prefs;
    mContext->GetSharedPreferences(
            QuickResponseUtils::SHARED_PREFERENCES_NAME, IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IArrayList> textMessages;
    CArrayList::New(
            QuickResponseUtils::NUM_CANNED_RESPONSES, (IArrayList**)&textMessages);
    // Note the default values here must agree with the corresponding
    // android:defaultValue attributes in respond_via_sms_settings.xml.
    String resStr;
    res->GetString(R::string::respond_via_sms_canned_response_1, &resStr);
    String prefsStr;
    prefs->GetString(QuickResponseUtils::KEY_CANNED_RESPONSE_PREF_1, resStr, &prefsStr);
    textMessages->Add(0, StringUtils::ParseCharSequence(prefsStr));
    res->GetString(R::string::respond_via_sms_canned_response_2, &resStr);
    prefs->GetString(QuickResponseUtils::KEY_CANNED_RESPONSE_PREF_2, resStr, &prefsStr);
    textMessages->Add(1, StringUtils::ParseCharSequence(prefsStr));
    res->GetString(R::string::respond_via_sms_canned_response_3, &resStr);
    prefs->GetString(QuickResponseUtils::KEY_CANNED_RESPONSE_PREF_3, resStr, &prefsStr);
    textMessages->Add(2, StringUtils::ParseCharSequence(prefsStr));
    res->GetString(R::string::respond_via_sms_canned_response_4, &resStr);
    prefs->GetString(QuickResponseUtils::KEY_CANNED_RESPONSE_PREF_4, resStr, &prefsStr);
    textMessages->Add(3, StringUtils::ParseCharSequence(prefsStr));
    Log::D("RespondViaSmsManager",
            "loadCannedResponses() completed, found responses: %s",
            TO_CSTR(textMessages));
    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = mResponse;
    args->mArg2 = textMessages;
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_CANNED_TEXT_MESSAGES_READY, TO_IINTERFACE(args), (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

//=============================================================================
// RespondViaSmsManager
//=============================================================================
AutoPtr<RespondViaSmsManager> RespondViaSmsManager::sInstance = new RespondViaSmsManager();
const Int32 RespondViaSmsManager::MSG_CANNED_TEXT_MESSAGES_READY = 1;
const Int32 RespondViaSmsManager::MSG_SHOW_SENT_TOAST = 2;

RespondViaSmsManager::RespondViaSmsManager()
{
    mHandler = new SubHandler(this);
}

ECode RespondViaSmsManager::GetInstance(
    /* [out] */ RespondViaSmsManager** result)
{
    VALIDATE_NOT_NULL(result)

    *result = sInstance;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RespondViaSmsManager::LoadCannedTextMessages(
    /* [in] */ IResponse* response,
    /* [in] */ IContext* context)
{
    AutoPtr<SubThread> thread = new SubThread(this, response, context);
    return thread->Start();
}

ECode RespondViaSmsManager::OnIncomingCallRejected(
    /* [in] */ ICall* call,
    /* [in] */ Boolean rejectWithMessage,
    /* [in] */ const String& textMessage)
{
    if (call != NULL && rejectWithMessage) {
        AutoPtr<ISubscriptionManager> subscriptionManagerHelper;
        CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&subscriptionManagerHelper);
        Int64 subId;
        subscriptionManagerHelper->GetDefaultSubId(&subId);
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
        if (phoneAccountHandle != NULL) {
            // try {
            ECode ec;
            do {
                String phoneAccountHandleId;
                ec = phoneAccountHandle->GetId(&phoneAccountHandleId);
                if (FAILED(ec)) break;
                subId = StringUtils::ParseInt64(phoneAccountHandleId);
            } while(FALSE);
            // } catch (NumberFormatException e) {
            if ((ECode)E_NUMBER_FORMAT_EXCEPTION == ec) {
                Log::E("RespondViaSmsManager", "Exception e ");
            }
            // }
        }
        AutoPtr<IUri> callHandle;
        ((Call*) call)->GetHandle((IUri**)&callHandle);
        String schemeSpecificPart;
        callHandle->GetSchemeSpecificPart(&schemeSpecificPart);
        AutoPtr<IContext> context;
        ((Call*) call)->GetContext((IContext**)&context);
        RejectCallWithMessage(context, schemeSpecificPart,
                textMessage, subId);
    }
    return NOERROR;
}

ECode RespondViaSmsManager::ShowMessageSentToast(
    /* [in] */ const String& phoneNumber,
    /* [in] */ IContext* context)
{
    // ...and show a brief confirmation to the user (since
    // otherwise it's hard to be sure that anything actually
    // happened.)
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    String formatString;
    res->GetString(
            R::string::respond_via_sms_confirmation_format, &formatString);
    String confirmationMsg;
    confirmationMsg.AppendFormat(formatString, phoneNumber.string());
    assert(0 && "TODO: CToastHelper");
    // Toast::MakeText(context, confirmationMsg,
    //         IToast::LENGTH_LONG)->Show();
    // TODO: If the device is locked, this toast won't actually ever
    // be visible!  (That's because we're about to dismiss the call
    // screen, which means that the device will return to the
    // keyguard.  But toasts aren't visible on top of the keyguard.)
    // Possible fixes:
    // (1) Is it possible to allow a specific Toast to be visible
    //     on top of the keyguard?
    // (2) Artificially delay the dismissCallScreen() call by 3
    //     seconds to allow the toast to be seen?
    // (3) Don't use a toast at all; instead use a transient state
    //     of the InCallScreen (perhaps via the InCallUiState
    //     progressIndication feature), and have that state be
    //     visible for 3 seconds before calling dismissCallScreen().
    return NOERROR;
}

ECode RespondViaSmsManager::RejectCallWithMessage(
    /* [in] */ IContext* context,
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& textMessage,
    /* [in] */ Int64 subId)
{
    if (textMessage != NULL) {
        assert(0 && "TODO: SmsApplication");
        // const ComponentName component =
        //         SmsApplication::GetDefaultRespondViaMessageApplication(context,
        //                 TRUE /*updateIfNeeded*/);
        // if (component != NULL) {
        //     // Build and send the intent
        //     AutoPtr<IUri> uri = Uri::FromParts(IConstants::SCHEME_SMSTO, phoneNumber, NULL);
        //     AutoPtr<IIntent> intent;
        //     CIntent::New(ITelephonyManager::ACTION_RESPOND_VIA_MESSAGE, uri, (IIntent**)&intent);
        //     intent->PutExtra(IIntent::EXTRA_TEXT, textMessage);
        //     intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
        //     AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        //     args->mArg1 = phoneNumber;
        //     args->mArg2 = context;
        //     AutoPtr<IMessage> msg;
        //     mHandler->ObtainMessage(MSG_SHOW_SENT_TOAST, args, (IMessage**)&msg);
        //     msg->SendToTarget();
        //     intent->SetComponent(component);
        //     context->StartService(intent);
        // }
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
