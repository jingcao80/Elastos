
#include "elastos/droid/services/telephony/GsmConnection.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Telecomm.h"

using Elastos::Droid::Telecomm::Telecom::IPhoneCapabilities;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
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
} // namespace Droid
} // namespace Elastos