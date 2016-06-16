
#include "elastos/droid/services/telephony/GsmConference.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

GsmConference::GsmConference(
    /* [in] */ IPhoneAccountHandle* phoneAccount)
{
    Conference::constructor(phoneAccount);
    SetCapabilities(
            IPhoneCapabilities::SUPPORT_HOLD |
            IPhoneCapabilities::HOLD |
            IPhoneCapabilities::MUTE |
            IPhoneCapabilities::MANAGE_CONFERENCE);
    SetActive();
}

ECode GsmConference::OnDisconnect()
{
    AutoPtr<IList> connections;
    GetConnections((IList**)&connections);

    Int32 size;
    connections->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        connections->Get(i, (IInterface**)&obj);
        AutoPtr<IConnection> connection = IConnection::Probe(obj);

        AutoPtr<ICall> call = GetMultipartyCallForConnection(connection, String("onDisconnect"));
        if (call != NULL) {
            Logger::D("GsmConference", "Found multiparty call to hangup for conference.");
            //try {
            ECode ec = call->Hangup();
            if (ec != (ECode)CallStateException) {
                break;
            }
            //} catch (CallStateException e) {
            else {
                Logger::E("GsmConference", "%d Exception thrown trying to hangup conference", ec);
            }
            //}
        }
    }
    return NOERROR;
}

ECode GsmConference::OnSeparate(
    /* [in] */ IConnection* connection)
{
    AutoPtr<com.android.internal.telephony.Connection> radioConnection =
            GetOriginalConnection(connection, String("onSeparate"));
    //try {
    ECode ec = radioConnection->Separate();
    //} catch (CallStateException e) {
    if (ec == (ECode)CallStateException) {
        Logger::E("GsmConference", "%d Exception thrown trying to separate a conference call", ec);
    }
    //}
    return NOERROR;
}

ECode GsmConference::OnMerge(
    /* [in] */ IConnection* connection)
{
    //try {
    ECode ec = NOERROR;
    AutoPtr<IPhone> phone;
    ITelephonyConnection::Probe(connection)->GetPhone((IPhone**)&phone);
    if (phone != NULL) {
        ec = phone->Conference();
    }
    //} catch (CallStateException e) {
    if (ec == (ECode)CallStateException) {
        Logger::E("GsmConference", "%d Exception thrown trying to merge call into a conference",ec);
    }
    return NOERROR;
}

ECode GsmConference::OnHold()
{
    AutoPtr<IGsmConnection> connection = GetFirstConnection();
    if (connection != NULL) {
        connection->PerformHold();
    }
    return NOERROR;
}

ECode GsmConference::OnUnhold()
{
    AutoPtr<IGsmConnection> connection = GetFirstConnection();
    if (connection != NULL) {
        connection->PerformUnhold();
    }
    return NOERROR;
}

ECode GsmConference::OnPlayDtmfTone(
    /* [in] */ Char32 c)
{
    AutoPtr<IGsmConnection> connection = GetFirstConnection();
    if (connection != NULL) {
        connection->OnPlayDtmfTone(c);
    }
    return NOERROR;
}

ECode GsmConference::OnStopDtmfTone()
{
    AutoPtr<IGsmConnection> connection = GetFirstConnection();
    if (connection != NULL) {
        connection->OnStopDtmfTone();
    }
    return NOERROR;
}

AutoPtr<ICall> GsmConference::GetMultipartyCallForConnection(
    /* [in] */ IConnection* connection,
    /* [in] */ const String& tag)
{
    AutoPtr<com.android.internal.telephony.Connection> radioConnection =
            GetOriginalConnection(connection, tag);
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

AutoPtr<com.android.internal.telephony.Connection> GsmConference::GetOriginalConnection(
    /* [in] */ IConnection* connection,
    /* [in] */ const String& tag)
{
    if (IGsmConnection::Probe(connection) != NULL) {
        AutoPtr<com.android.internal.telephony.Connection> tmp;
        IGsmConnection::Probe(connection)->GetOriginalConnection(----tmp);
        return tmp;
    }
    else {
        Logger::E("GsmConference", "Non GSM connection found in a Gsm conference (%s)", tag.string());
        return NULL;
    }
}

AutoPtr<IGsmConnection> GsmConference::GetFirstConnection()
{
    AutoPtr<IList> connections;
    GetConnections((IList**)&connections);
    Boolean res;
    if (connections->IsEmpty(&res), res) {
        return NULL;
    }

    AutoPtr<IInterface> obj;
    connections->Get(0, (IInterface**)&obj);
    AutoPtr<IGsmConnection> tmp = IGsmConnection::Probe(obj);
    return tmp;
}

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos