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

#include "elastos/droid/teleservice/services/telephony/GsmConnection.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Telecom.h"

using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

CAR_INTERFACE_IMPL(GsmConnection, TelephonyConnection, IGsmConnection)

GsmConnection::GsmConnection(
    /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* connection)
{
    TelephonyConnection::constructor(connection);
}

ECode GsmConnection::OnPlayDtmfTone(
    /* [in] */ Char32 digit)
{
    AutoPtr<IPhone> phone;
    GetPhone((IPhone**)&phone);
    if (phone != NULL) {
        phone->StartDtmf(digit);
    }
    return NOERROR;
}

ECode GsmConnection::OnStopDtmfTone()
{
    AutoPtr<IPhone> phone;
    GetPhone((IPhone**)&phone);
    if (phone != NULL) {
        phone->StopDtmf();
    }
    return NOERROR;
}

ECode GsmConnection::PerformConference(
    /* [in] */ ITelephonyConnection* otherConnection)
{
    Logger::D("GsmConnection", "performConference - %s", TO_CSTR(this));

    AutoPtr<IPhone> phone;
    GetPhone((IPhone**)&phone);
    if (phone != NULL) {
        //try {
        // We dont use the "other" connection because there is no concept of that in the
        // implementation of calls inside telephony. Basically, you can "conference" and it
        // will conference with the background call.  We know that otherConnection is the
        // background call because it would never have called setConferenceableConnections()
        // otherwise.
        ECode ec = phone->Conference();
        //} catch (CallStateException e) {
        if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
            Logger::E("GsmConnection", "%d Failed to conference call.", ec);
        }
        //}
    }
    return NOERROR;
}

ECode GsmConnection::BuildCallCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 capabilities = IPhoneCapabilities::MUTE | IPhoneCapabilities::SUPPORT_HOLD;

    Int32 state;
    GetState(&state);
    if (state == STATE_ACTIVE || state == STATE_HOLDING) {
        capabilities |= IPhoneCapabilities::HOLD;
    }
    *result = capabilities;
    return NOERROR;
}

ECode GsmConnection::OnRemovedFromCallService()
{
    return TelephonyConnection::OnRemovedFromCallService();
}

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos