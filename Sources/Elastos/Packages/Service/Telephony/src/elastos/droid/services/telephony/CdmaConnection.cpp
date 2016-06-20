
#include "elastos/droid/services/telephony/CdmaConnection.h"
#include "elastos/droid/services/telephony/DisconnectCauseUtil.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Telecomm.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Phone::IConstants;
using Elastos::Droid::Telecomm::Telecom::IPhoneCapabilities;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::ICallState_WAITING;
using Elastos::Droid::Internal::Telephony::ICallState_INCOMING;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Core::AutoLock;
using Elastos::Core::IChar32;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

CdmaConnection::MyHandler::MyHandler(
    /* [in] */ CdmaConnection* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CdmaConnection::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_CALL_WAITING_MISSED:
            mHost->HangupCallWaiting(Elastos::Droid::Telephony::IDisconnectCause::INCOMING_MISSED);
            break;
        case MSG_DTMF_SEND_CONFIRMATION:
            mHost->HandleBurstDtmfConfirmation();
            break;
        default:
            break;
    }
    return NOERROR;
}

const Int32 CdmaConnection::TIMEOUT_CALL_WAITING_MILLIS = 20 * 1000;

CAR_INTERFACE_IMPL(CdmaConnection, TelephonyConnection, ICdmaConnection);

CdmaConnection::CdmaConnection(
    /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* connection,
    /* [in] */ EmergencyTonePlayer* emergencyTonePlayer,
    /* [in] */ Boolean allowMute,
    /* [in] */ Boolean isOutgoing)
    : mAllowMute(allowMute)
    , mIsOutgoing(isOutgoing)
    , mEmergencyTonePlayer(emergencyTonePlayer)
    , mDtmfBurstConfirmationPending(FALSE)
{
    TelephonyConnection::constructor(connection);
    ICallState state;
    mIsCallWaiting = (connection != NULL) && ((connection->GetState(&state), state) == ICallState_WAITING);
    if (mIsCallWaiting) {
        StartCallWaitingTimer();
    }

    CLinkedList::New((IQueue**)&mDtmfQueue);

    mHandler = new MyHandler(this);
}

ECode CdmaConnection::OnPlayDtmfTone(
    /* [in] */ Char32 digit)
{
    if (UseBurstDtmf()) {
        Logger::I("CdmaConnection", "sending dtmf digit as burst");
        SendShortDtmfToNetwork(digit);
    }
    else {
        Logger::I("CdmaConnection", "sending dtmf digit directly");
        AutoPtr<IPhone> phone;
        GetPhone((IPhone**)&phone);
        phone->StartDtmf(digit);
    }
    return NOERROR;
}

ECode CdmaConnection::OnStopDtmfTone()
{
    if (!UseBurstDtmf()) {
        AutoPtr<IPhone> phone;
        GetPhone((IPhone**)&phone);
        phone->StopDtmf();
    }
    return NOERROR;
}

ECode CdmaConnection::OnReject()
{
    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> connection;
    GetOriginalConnection((Elastos::Droid::Internal::Telephony::IConnection**)&connection);
    if (connection != NULL) {
        ICallState state;
        connection->GetState(&state);
        switch (state) {
            case ICallState_INCOMING:
                // Normal ringing calls are handled the generic way.
                TelephonyConnection::OnReject();
                break;
            case ICallState_WAITING:
                HangupCallWaiting(Elastos::Droid::Telephony::IDisconnectCause::INCOMING_REJECTED);
                break;
            default:
                Logger::E("CdmaConnection",  "Rejecting a non-ringing call");
                // might as well hang this up, too.
                TelephonyConnection::OnReject();
                break;
        }
    }
    return NOERROR;
}

ECode CdmaConnection::OnAnswer()
{
    mHandler->RemoveMessages(MSG_CALL_WAITING_MISSED);
    assert(0);
    //return TelephonyConnection::OnAnswer();
    return NOERROR;
}

ECode CdmaConnection::OnStateChanged(
    /* [in] */ Int32 state)
{
    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> originalConnection;
    GetOriginalConnection((Elastos::Droid::Internal::Telephony::IConnection**)&originalConnection);

    ICallState _state;
    mIsCallWaiting = (originalConnection != NULL) &&
            ((originalConnection->GetState(&_state), _state) == ICallState_WAITING);

    if (state == Elastos::Droid::Telecomm::Telecom::IConnection::STATE_DIALING) {
        if (IsEmergency()) {
            mEmergencyTonePlayer->Start();
        }
    }
    else {
        // No need to check if it is an emergency call, since it is a no-op if it isn't started.
        mEmergencyTonePlayer->Stop();
    }

    return TelephonyConnection::OnStateChanged(state);
}

ECode CdmaConnection::BuildCallCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 capabilities = 0;
    if (mAllowMute) {
        capabilities = IPhoneCapabilities::MUTE;
    }
    *result = capabilities;
    return NOERROR;
}

ECode CdmaConnection::ForceAsDialing(
    /* [in] */ Boolean isDialing)
{
    if (isDialing) {
        SetDialing();
    }
    else {
        UpdateState();
    }
    return NOERROR;
}

ECode CdmaConnection::IsOutgoing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIsOutgoing;
    return NOERROR;
}

ECode CdmaConnection::IsCallWaiting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIsCallWaiting;
    return NOERROR;
}

void CdmaConnection::StartCallWaitingTimer()
{
    Boolean res;
    mHandler->SendEmptyMessageDelayed(MSG_CALL_WAITING_MISSED, TIMEOUT_CALL_WAITING_MILLIS, &res);
}

void CdmaConnection::HangupCallWaiting(
    /* [in] */ Int32 telephonyDisconnectCause)
{
    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> originalConnection;
    GetOriginalConnection((Elastos::Droid::Internal::Telephony::IConnection**)&originalConnection);
    if (originalConnection != NULL) {
        //try {
        ECode ec = originalConnection->Hangup();
        //} catch (CallStateException e) {
        if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
            Logger::E("CdmaConnection", "%d Failed to hangup call waiting call", ec);
        }
        //}
        SetDisconnected(DisconnectCauseUtil::ToTelecomDisconnectCause(telephonyDisconnectCause));
    }
}

Boolean CdmaConnection::UseBurstDtmf()
{
    AutoPtr<IPhone> phone;
    GetPhone((IPhone**)&phone);
    AutoPtr<IContext> context;
    phone->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);

    AutoPtr<ISettingsSystem> helper;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
    Int32 dtmfTypeSetting;
    helper->GetInt32(contentResolver,
            ISettingsSystem::DTMF_TONE_TYPE_WHEN_DIALING,
            IConstants::DTMF_TONE_TYPE_NORMAL, &dtmfTypeSetting);
    return dtmfTypeSetting == IConstants::DTMF_TONE_TYPE_NORMAL;
}

void CdmaConnection::SendShortDtmfToNetwork(
    /* [in] */ Char32 digit)
{
    {
        AutoLock syncLock(mDtmfQueue);
        if (mDtmfBurstConfirmationPending) {
            AutoPtr<IChar32> obj = CoreUtils::ConvertChar32(digit);
            mDtmfQueue->Add(TO_IINTERFACE(obj));
        }
        else {
            String str = StringUtils::ToString((Int32)digit);
            SendBurstDtmfStringLocked(str);
        }
    }
}

void CdmaConnection::SendBurstDtmfStringLocked(
    /* [in] */ const String& dtmfString)
{
    AutoPtr<IPhone> phone;
    GetPhone((IPhone**)&phone);

    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MSG_DTMF_SEND_CONFIRMATION, (IMessage**)&m);
    phone->SendBurstDtmf(dtmfString, 0, 0, m);
    mDtmfBurstConfirmationPending = TRUE;
}

void CdmaConnection::HandleBurstDtmfConfirmation()
{
    String dtmfDigits;

    {
        AutoLock syncLock(mDtmfQueue);
        mDtmfBurstConfirmationPending = FALSE;
        Boolean res;
        if (mDtmfQueue->IsEmpty(&res), !res) {
            StringBuilder builder;
            Boolean tmp;
            while (mDtmfQueue->IsEmpty(&tmp), !tmp) {
                AutoPtr<IInterface> e;
                mDtmfQueue->Poll((IInterface**)&e);
                builder += TO_CSTR(e);
            }
            builder.ToString(&dtmfDigits);

            // It would be nice to log the digit, but since DTMF digits can be passwords
            // to things, or other secure account numbers, we want to keep it away from
            // the logs.
            Logger::I("CdmaConnection", "%d dtmf character[s] removed from the queue", dtmfDigits.GetLength());
        }
        if (!dtmfDigits.IsNull()) {
            SendBurstDtmfStringLocked(dtmfDigits);
        }
    }
}

Boolean CdmaConnection::IsEmergency()
{
    AutoPtr<IPhone> phone;
    GetPhone((IPhone**)&phone);
    AutoPtr<IContext> context;
    phone->GetContext((IContext**)&context);

    if (phone != NULL) {
        AutoPtr<IPhoneNumberUtils> helper;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
        AutoPtr<IUri> uri;
        GetAddress((IUri**)&uri);
        String part;
        uri->GetSchemeSpecificPart(&part);
        Boolean res;
        helper->IsLocalEmergencyNumber(context, part, &res);
        return res;
    }
    return FALSE;
}

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos