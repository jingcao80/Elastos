
#include "elastos/droid/teleservice/services/telephony/PstnIncomingCallNotifier.h"
#include "elastos/droid/teleservice/services/telephony/CPstnIncomingCallNotifierBroadcastReceiver.h"
#include "elastos/droid/teleservice/services/telephony/TelecomAccountRegistry.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::CTelecomManagerHelper;
using Elastos::Droid::Telecom::ITelecomManagerHelper;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::ICallState_INCOMING;
using Elastos::Droid::Internal::Telephony::ICallState_WAITING;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaCallWaitingNotification;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

PstnIncomingCallNotifier::MyHandler::MyHandler(
    /* [in] */ PstnIncomingCallNotifier* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode PstnIncomingCallNotifier::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case EVENT_NEW_RINGING_CONNECTION:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> ar = (AsyncResult*)IObject::Probe(obj);
            mHost->HandleNewRingingConnection(ar);
            break;
        }
        case EVENT_CDMA_CALL_WAITING:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> ar = (AsyncResult*)IObject::Probe(obj);
            mHost->HandleCdmaCallWaiting(ar);
            break;
        }
        case EVENT_UNKNOWN_CONNECTION:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> ar = (AsyncResult*)IObject::Probe(obj);
            mHost->HandleNewUnknownConnection(ar);
            break;
        }
        default:
            break;
    }
    return NOERROR;
}

ECode PstnIncomingCallNotifier::MyBroadcastReceiver::constructor(
    /* [in] */ IPstnIncomingCallNotifier* host)
{
    mHost = (PstnIncomingCallNotifier*)host;
    return BroadcastReceiver::constructor();
}

ECode PstnIncomingCallNotifier::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (ITelephonyIntents::ACTION_RADIO_TECHNOLOGY_CHANGED.Equals(action)) {
        String newPhone;
        intent->GetStringExtra(IPhoneConstants::PHONE_NAME_KEY, &newPhone);
        Logger::D("PstnIncomingCallNotifier", "Radio technology switched. Now %s is active.", newPhone.string());

        mHost->RegisterForNotifications();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(PstnIncomingCallNotifier, Object, IPstnIncomingCallNotifier)

PstnIncomingCallNotifier::PstnIncomingCallNotifier(
    /* [in] */ IPhoneProxy* phoneProxy)
{
    mHandler = new MyHandler(this);

    CPstnIncomingCallNotifierBroadcastReceiver::New(this, (IBroadcastReceiver**)&mRATReceiver);

    AutoPtr<IPreconditions> helper;
    CPreconditions::AcquireSingleton((IPreconditions**)&helper);
    helper->CheckNotNull(phoneProxy);

    mPhoneProxy = phoneProxy;

    RegisterForNotifications();

    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(ITelephonyIntents::ACTION_RADIO_TECHNOLOGY_CHANGED, (IIntentFilter**)&intentFilter);

    AutoPtr<IContext> context;
    IPhone::Probe(mPhoneProxy)->GetContext((IContext**)&context);
    AutoPtr<IIntent> tmp;
    context->RegisterReceiver(mRATReceiver, intentFilter, (IIntent**)&tmp);
}

ECode PstnIncomingCallNotifier::Teardown()
{
    UnregisterForNotifications();
    AutoPtr<IContext> context;
    IPhone::Probe(mPhoneProxy)->GetContext((IContext**)&context);
    return context->UnregisterReceiver(mRATReceiver);
}

void PstnIncomingCallNotifier::RegisterForNotifications()
{
    AutoPtr<IPhone> newPhone;
    mPhoneProxy->GetActivePhone((IPhone**)&newPhone);
    if (newPhone != mPhoneBase) {
        UnregisterForNotifications();

        if (newPhone != NULL) {
            Logger::I("PstnIncomingCallNotifier", "Registering: %s", TO_CSTR(newPhone));
            mPhoneBase = newPhone;
            mPhoneBase->RegisterForNewRingingConnection(
                    mHandler, EVENT_NEW_RINGING_CONNECTION, NULL);
            mPhoneBase->RegisterForCallWaiting(
                    mHandler, EVENT_CDMA_CALL_WAITING, NULL);
            mPhoneBase->RegisterForUnknownConnection(mHandler, EVENT_UNKNOWN_CONNECTION,
                    NULL);
        }
    }
}

void PstnIncomingCallNotifier::UnregisterForNotifications()
{
    if (mPhoneBase != NULL) {
        Logger::I("PstnIncomingCallNotifier", "Unregistering: %s", TO_CSTR(mPhoneBase));
        mPhoneBase->UnregisterForNewRingingConnection(mHandler);
        mPhoneBase->UnregisterForCallWaiting(mHandler);
        mPhoneBase->UnregisterForUnknownConnection(mHandler);
    }
}

void PstnIncomingCallNotifier::HandleNewRingingConnection(
    /* [in] */ AsyncResult* asyncResult)
{
    Logger::D("PstnIncomingCallNotifier", "handleNewRingingConnection");
    AutoPtr<IConnection> connection = IConnection::Probe(asyncResult->mResult);
    if (connection != NULL) {
        AutoPtr<ICall> call;
        connection->GetCall((ICall**)&call);

        // Final verification of the ringing state before sending the intent to Telecom.
        if (call != NULL) {
            ICallState state;
            call->GetState(&state);
            Boolean res;
            //state->IsRinging(&res);
            res = state == ICallState_INCOMING || state == ICallState_WAITING;
            if (res) {
                SendIncomingCallIntent(connection);
            }
        }
    }
}

void PstnIncomingCallNotifier::HandleCdmaCallWaiting(
    /* [in] */ AsyncResult* asyncResult)
{
    Logger::D("PstnIncomingCallNotifier", "handleCdmaCallWaiting");
    AutoPtr<ICdmaCallWaitingNotification> ccwi = ICdmaCallWaitingNotification::Probe(asyncResult->mResult);
    AutoPtr<ICall> call;
    mPhoneBase->GetRingingCall((ICall**)&call);

    Int32 state;
    call->GetState(&state);
    if (state == ICallState_WAITING) {
        AutoPtr<IConnection> connection;
        call->GetLatestConnection((IConnection**)&connection);
        if (connection != NULL) {
            String number;
            connection->GetAddress(&number);
            assert(0);
            // if (number != NULL && Objects.equals(number, ccwi.number)) {
            //     SendIncomingCallIntent(connection);
            // }
        }
    }
}

void PstnIncomingCallNotifier::HandleNewUnknownConnection(
    /* [in] */ AsyncResult* asyncResult)
{
    Logger::I("PstnIncomingCallNotifier", "handleNewUnknownConnection");
    if (IConnection::Probe(asyncResult->mResult) == NULL) {
        Logger::W("PstnIncomingCallNotifier", "handleNewUnknownConnection called with non-Connection object");
        return;
    }
    AutoPtr<IConnection> connection = IConnection::Probe(asyncResult->mResult);
    if (connection != NULL) {
        AutoPtr<ICall> call;
        connection->GetCall((ICall**)&call);
        if (call != NULL) {
            ICallState state;
            call->GetState(&state);
            Boolean res;
            assert(0);
            //state->IsAlive(&res);
            if (res) {
                AddNewUnknownCall(connection);
            }
        }
    }
}

void PstnIncomingCallNotifier::AddNewUnknownCall(
    /* [in] */ IConnection* connection)
{
    AutoPtr<IBundle> extras;
    Int32 num;
    connection->GetNumberPresentation(&num);
    String address;
    if (num == ITelecomManager::PRESENTATION_ALLOWED &&
            !TextUtils::IsEmpty((connection->GetAddress(&address), address))) {
        CBundle::New((IBundle**)&extras);

        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->FromParts(IPhoneAccount::SCHEME_TEL, (connection->GetAddress(&address), address),
                String(NULL), (IUri**)&uri);

        extras->PutParcelable(ITelecomManager::EXTRA_UNKNOWN_CALL_HANDLE, IParcelable::Probe(uri));
    }

    AutoPtr<IContext> context;
    IPhone::Probe(mPhoneProxy)->GetContext((IContext**)&context);

    AutoPtr<ITelecomManagerHelper> helper;
    CTelecomManagerHelper::AcquireSingleton((ITelecomManagerHelper**)&helper);
    AutoPtr<ITelecomManager> manager;
    helper->From(context, (ITelecomManager**)&manager);

    AutoPtr<IPhoneAccountHandle> handle = TelecomAccountRegistry::MakePstnPhoneAccountHandle(
            Elastos::Droid::Internal::Telephony::IPhone::Probe(mPhoneProxy));
    manager->AddNewUnknownCall(handle, extras);
}

void PstnIncomingCallNotifier::SendIncomingCallIntent(
    /* [in] */ IConnection* connection)
{
    AutoPtr<IBundle> extras;
    Int32 num;
    connection->GetNumberPresentation(&num);
    String address;
    if (num == ITelecomManager::PRESENTATION_ALLOWED &&
            !TextUtils::IsEmpty((connection->GetAddress(&address), address))) {
        CBundle::New((IBundle**)&extras);

        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->FromParts(IPhoneAccount::SCHEME_TEL, (connection->GetAddress(&address), address),
                String(NULL), (IUri**)&uri);

        extras->PutParcelable(ITelephonyManager::EXTRA_INCOMING_NUMBER, IParcelable::Probe(uri));
    }

    AutoPtr<IContext> context;
    IPhone::Probe(mPhoneProxy)->GetContext((IContext**)&context);

    AutoPtr<ITelecomManagerHelper> helper;
    CTelecomManagerHelper::AcquireSingleton((ITelecomManagerHelper**)&helper);
    AutoPtr<ITelecomManager> manager;
    helper->From(context, (ITelecomManager**)&manager);

    AutoPtr<IPhoneAccountHandle> handle = TelecomAccountRegistry::MakePstnPhoneAccountHandle(
            Elastos::Droid::Internal::Telephony::IPhone::Probe(mPhoneProxy));
    manager->AddNewIncomingCall(handle, extras);
}

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos
