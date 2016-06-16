
#include "elastos/droid/services/telephony/CdmaConnection.h"

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
    msg->GwtWhat(&what);
    switch (what) {
        case MSG_CALL_WAITING_MISSED:
            mHost->HangupCallWaiting(DisconnectCause.INCOMING_MISSED);
            break;
        case MSG_DTMF_SEND_CONFIRMATION:
            mHost->HandleBurstDtmfConfirmation();
            break;
        default:
            break;
    }
    return NOERROR;
}

const Int32 CdmaConnection::MSG_CALL_WAITING_MISSED = 1;
const Int32 CdmaConnection::MSG_DTMF_SEND_CONFIRMATION = 2;
const Int32 CdmaConnection::TIMEOUT_CALL_WAITING_MILLIS = 20 * 1000;

CdmaConnection::CdmaConnection(
    /* [in] */ IConnection* connection,
    /* [in] */ IEmergencyTonePlayer* emergencyTonePlayer,
    /* [in] */ Boolean allowMute,
    /* [in] */ Boolean isOutgoing)
    : mAllowMute(allowMute)
    , mIsOutgoing(isOutgoing)
    , mDtmfBurstConfirmationPending(FALSE)
    , mEmergencyTonePlayer(emergencyTonePlayer)
{
    TelephonyConnection::constructor(connection);
    State state;
    mIsCallWaiting = (connection != NULL) && ((connection->GetState(&state), state) == Call.State.WAITING);
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
    AutoPtr<IConnection> connection;
    GetOriginalConnection((IConnection**)&connection);
    if (connection != NULL) {
        State state;
        connection->GetState(&state);
        switch (state) {
            case INCOMING:
                // Normal ringing calls are handled the generic way.
                TelephonyConnection::OnReject();
                break;
            case WAITING:
                HangupCallWaiting(IDisconnectCause::INCOMING_REJECTED);
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
    return TelephonyConnection::OnAnswer();
}

ECode CdmaConnection::OnStateChanged(
    /* [in] */ Int32 state)
{
    AutoPtr<IConnection> originalConnection;
    GetOriginalConnection((IConnection**)&originalConnection);

    State state;
    mIsCallWaiting = (originalConnection != NULL) &&
            ((originalConnection->GetState(&state), state) == Call.State.WAITING);

    if (state == android.telecom.Connection.STATE_DIALING) {
        Boolean res;
        if (IsEmergency(&res), res) {
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
    if (IsDialing) {
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
    mHandler->SendEmptyMessageDelayed(MSG_CALL_WAITING_MISSED, TIMEOUT_CALL_WAITING_MILLIS);
}

void CdmaConnection::HangupCallWaiting(
    /* [in] */ Int32 telephonyDisconnectCause)
{
    AutoPtr<IConnection> originalConnection;
    GetOriginalConnection((IConnection**)&originalConnection);
    if (originalConnection != NULL) {
        //try {
        ECode ec = originalConnection->Hangup();
        //} catch (CallStateException e) {
        if (ec == (ECode)CallStateException) {
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
            AutoPtr<ICharacter> obj = CoreUtil::Convert(digit);
            mDtmfQueue->Add(TO_IINTERFACE(obj));
        }
        else {
            String str = StringUtils::ToString(digit);
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
                builder->Append(mDtmfQueue.poll());
            }
            builder->ToString(&dtmfDigits);

            // It would be nice to log the digit, but since DTMF digits can be passwords
            // to things, or other secure account numbers, we want to keep it away from
            // the logs.
            Logger::I("CdmaConnection", "%d dtmf character[s] removed from the queue", dtmfDigits.GetLength());
        }
        if (IdtmfDigits.IsNull()) {
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
    return phone != NULL &&
            PhoneNumberUtils::IsLocalEmergencyNumber(
                context, getAddress().getSchemeSpecificPart());
}

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos