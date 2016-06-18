
#include "elastos/droid/services/telephony/CdmaConference.h"
#include "Elastos.Droid.Telecomm.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Telecomm::Telecom::IPhoneCapabilities;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

CdmaConference::CdmaConference(
    /* [in] */ IPhoneAccountHandle* phoneAccount,
    /* [in] */ Int32 capabilities)
    : mCapabilities(IPhoneCapabilities::MUTE)
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
        if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
            Logger::E("CdmaConference", "%d Exception thrown trying to hangup conference", ec);
        }
        //}
    }
    return NOERROR;
}

ECode CdmaConference::OnSeparate(
    /* [in] */ Elastos::Droid::Telecomm::Telecom::IConnection* connection)
{
    Logger::E("CdmaConference", "Separate not supported for CDMA conference call.");
    assert(0 && "return exception");
    //return Exception;
    return NOERROR;
}

ECode CdmaConference::OnHold()
{
    Logger::E("CdmaConference", "Hold not supported for CDMA conference call.");
    assert(0 && "return exception");
    //return Exception;
    return NOERROR;
}

ECode CdmaConference::OnUnhold()
{
    Logger::E("CdmaConference", "Unhold not supported for CDMA conference call.");
    assert(0 && "return exception");
    //return Exception;
    return NOERROR;
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
    AutoPtr<CdmaConnection> connection = GetFirstConnection();
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
    AutoPtr<CdmaConnection> connection = GetFirstConnection();
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
        if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
            Logger::E("CdmaConference", "%d Error while trying to send flash command.",ec);
        }
        //}
    }
}

AutoPtr<ICall> CdmaConference::GetMultipartyCallForConnection(
    /* [in] */ Elastos::Droid::Telecomm::Telecom::IConnection* connection)
{
    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> radioConnection =
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
        AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> originalConnection =
                GetOriginalConnection(Elastos::Droid::Telecomm::Telecom::IConnection::Probe(obj));
        if (originalConnection != NULL) {
            AutoPtr<ICall> tmp;
            originalConnection->GetCall((ICall**)&tmp);
            return tmp;
        }
    }
    return NULL;
}

AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> CdmaConference::GetOriginalConnection(
    /* [in] */ Elastos::Droid::Telecomm::Telecom::IConnection* connection)
{
    if (ICdmaConnection::Probe(connection) != NULL) {
        AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> con;
        ITelephonyConnection::Probe(connection)->GetOriginalConnection((Elastos::Droid::Internal::Telephony::IConnection**)&con);
        return con;
    }
    else {
        Logger::E("CdmaConference", "Non CDMA connection found in a CDMA conference");
        return NULL;
    }
}

AutoPtr<CdmaConnection> CdmaConference::GetFirstConnection()
{
    AutoPtr<IList> connections;
    GetConnections((IList**)&connections);

    Boolean res;
    if (connections->IsEmpty(&res)) {
        return NULL;
    }
    AutoPtr<IInterface> obj;
    connections->Get(0, (IInterface**)&obj);
    AutoPtr<CdmaConnection> tmp = (CdmaConnection*)ICdmaConnection::Probe(obj);
    return tmp;
}

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos