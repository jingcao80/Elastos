//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/teleservice/services/telephony/GsmConference.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
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
            if (ec != (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
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
    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> radioConnection =
            GetOriginalConnection(connection, String("onSeparate"));
    //try {
    ECode ec = radioConnection->Separate();
    //} catch (CallStateException e) {
    if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
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
    if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
        Logger::E("GsmConference", "%d Exception thrown trying to merge call into a conference",ec);
    }
    return NOERROR;
}

ECode GsmConference::OnHold()
{
    AutoPtr<IGsmConnection> connection = GetFirstConnection();
    if (connection != NULL) {
        ITelephonyConnection::Probe(connection)->PerformHold();
    }
    return NOERROR;
}

ECode GsmConference::OnUnhold()
{
    AutoPtr<IGsmConnection> connection = GetFirstConnection();
    if (connection != NULL) {
        ITelephonyConnection::Probe(connection)->PerformUnhold();
    }
    return NOERROR;
}

ECode GsmConference::OnPlayDtmfTone(
    /* [in] */ Char32 c)
{
    AutoPtr<IGsmConnection> connection = GetFirstConnection();
    if (connection != NULL) {
        IConnection::Probe(connection)->OnPlayDtmfTone(c);
    }
    return NOERROR;
}

ECode GsmConference::OnStopDtmfTone()
{
    AutoPtr<IGsmConnection> connection = GetFirstConnection();
    if (connection != NULL) {
        IConnection::Probe(connection)->OnStopDtmfTone();
    }
    return NOERROR;
}

AutoPtr<ICall> GsmConference::GetMultipartyCallForConnection(
    /* [in] */ IConnection* connection,
    /* [in] */ const String& tag)
{
    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> radioConnection =
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

AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> GsmConference::GetOriginalConnection(
    /* [in] */ IConnection* connection,
    /* [in] */ const String& tag)
{
    if (IGsmConnection::Probe(connection) != NULL) {
        AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> tmp;
        ITelephonyConnection::Probe(connection)->GetOriginalConnection((
                Elastos::Droid::Internal::Telephony::IConnection**)&tmp);
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
} // namespace TeleService
} // namespace Droid
} // namespace Elastos