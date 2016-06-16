
#include "elastos/droid/services/telephony/CdmaConference.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Phone {

CdmaConference::CdmaConference(
    /* [in] */ IPhoneAccountHandle* phoneAccount,
    /* [in] */ Int32 capabilities)
    : mCapabilities(PhoneCapabilities::MUTE)
{
    Conference::constructor(phoneAccount);
    SetCapabilities(mCapabilities | capabilities);
    SetActive();
}

void CdmaConference::UpdateCapabilities()
{
    SetCapabilities(mCapabilities);
}

ECode CdmaConference::OnDisconnect()
{
    AutoPtr<ICall> call = GetOriginalCall();
    if (call != NULL) {
        Logger::D("CdmaConference", "Found multiparty call to hangup for conference.");
        //try {
        ECode ec = call->Hangup();
        //} catch (CallStateException e) {
        if (ec == (ECode) CallStateException) {
            Logger::E("CdmaConference", "%d Exception thrown trying to hangup conference", ec);
        }
        //}
    }
    return NOERROR;
}

ECode CdmaConference::OnSeparate(
    /* [in] */ IConnection* connection)
{
    Logger::E("CdmaConference", "Separate not supported for CDMA conference call.");
    return Exception;
}

ECode CdmaConference::OnHold()
{
    Logger::E("CdmaConference", "Hold not supported for CDMA conference call.");
    return Exception;
}

ECode CdmaConference::OnUnhold()
{
    Logger::E("CdmaConference", "Unhold not supported for CDMA conference call.");
    return Exception;
}

ECode CdmaConference::OnMerge()
{
    Logger::I("CdmaConference", "Merging CDMA conference call.");
    // Can only merge once
    mCapabilities &= ~IPhoneCapabilities::MERGE_CONFERENCE;
    // Once merged, swap is enabled.
    mCapabilities |= IPhoneCapabilities::SWAP_CONFERENCE;
    UpdateCapabilities();
    SendFlash();
    return NOERROR;
}

ECode CdmaConference::OnPlayDtmfTone(
    /* [in] */ Char32 c)
{
    AutoPtr<ICdmaConnection> connection = GetFirstConnection();
    if (connection != NULL) {
        connection->OnPlayDtmfTone(c);
    }
    else {
        Logger::W("CdmaConference", "No CDMA connection found while trying to play dtmf tone.");
    }
    return NOERROR;
}

ECode CdmaConference::OnStopDtmfTone()
{
    AutoPtr<ICdmaConnection> connection = GetFirstConnection();
    if (connection != NULL) {
        connection->OnStopDtmfTone();
    }
    else {
        Logger::W("CdmaConference", "No CDMA connection found while trying to stop dtmf tone.");
    }
    return NOERROR;
}

ECode CdmaConference::OnSwap()
{
    Logger::I("CdmaConference", "Swapping CDMA conference call.");
    SendFlash();
    return NOERROR;
}

void CdmaConference::SendFlash()
{
    AutoPtr<ICall> call = GetOriginalCall();
    if (call != NULL) {
        //try {
            // For CDMA calls, this just sends a flash command.
        AutoPtr<IPhone> phone;
        call->GetPhone((IPhone**)&phone);
        ECode ec = phone->SwitchHoldingAndActive();
        //} catch (CallStateException e) {
        if (ec == (ECode)CallStateException) {
            Logger::E("CdmaConference", "%d Error while trying to send flash command.",ec);
        }
        //}
    }
}

AutoPtr<ICall> CdmaConference::GetMultipartyCallForConnection(
    /* [in] */ IConnection* connection)
{
    AutoPtr<com.android.internal.telephony.Connection> radioConnection =
            GetOriginalConnection(connection);
    if (radioConnection != NULL) {
        AutoPtr<ICall> call;
        radioConnection->GetCall((ICall**)&call);
        Boolean res;
        if (call != NULL && (call->IsMultiparty(&res), res)) {
            return call;
        }
    }
    return NULL;
}

AutoPtr<ICall> CdmaConference::GetOriginalCall()
{
    AutoPtr<IList> connections;
    GetConnections((IList**)&connections);

    Boolean res;
    if (connections->IsEmpty(&res), !res) {
        AutoPtr<IInterface> obj;
        connections->Get(0, (IInterface**)&obj);
        AutoPtr<com.android.internal.telephony.Connection> originalConnection =
                GetOriginalConnection(obj);
        if (originalConnection != NULL) {
            AutoPtr<ICall> tmp;
            originalConnection->GetCall((ICall**)&tmp);
            return tmp;
        }
    }
    return NULL;
}

AutoPtr<com.android.internal.telephony.Connection> CdmaConference::GetOriginalConnection(
    /* [in] */ IConnection* connection)
{
    if (ICdmaConnection::Probe(connection) != NULL) {
        AutoPtr<com.android.internal.telephony.Connection> con;
        ICdmaConnection::Probe(connection)->GetOriginalConnection((com.android.internal.telephony.Connection*)con);
        return con;
    }
    else {
        Logger::E("CdmaConference", "Non CDMA connection found in a CDMA conference");
        return NULL;
    }
}

AutoPtr<ICdmaConnection> CdmaConference::GetFirstConnection()
{
    AutoPtr<IList> connections;
    GetConnections((IList**)&connections);

    Boolean res;
    if (connections->IsEmpty(&res)) {
        return NULL;
    }
    AutoPtr<IInterface> obj;
    connections->Get(0, (IInterface**)&obj);
    AutoPtr<ICdmaConnection> tmp = ICdmaConnection::Probe(obj);
    return tmp;
}

} // namespace Phone
} // namespace Telephony
} // namespace Droid
} // namespace Elastos