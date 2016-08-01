#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaCallWaitingNotification.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                     CdmaCallWaitingNotification
//=====================================================================
CAR_INTERFACE_IMPL(CdmaCallWaitingNotification, Object, ICdmaCallWaitingNotification);

const String CdmaCallWaitingNotification::LOGTAG("CdmaCallWaitingNotification");

CdmaCallWaitingNotification::CdmaCallWaitingNotification()
    : numberPresentation(0)
    , namePresentation(0)
    , numberType(0)
    , numberPlan(0)
    , isPresent(0)
    , signalType(0)
    , alertPitch(0)
    , signal(0)
{
}

ECode CdmaCallWaitingNotification::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = /*super.toString() +*/ String("Call Waiting Notification  ")
        + " number: " + number
        + " numberPresentation: " + StringUtils::ToString(numberPresentation)
        + " name: " + name
        + " namePresentation: " + StringUtils::ToString(namePresentation)
        + " numberType: " + StringUtils::ToString(numberType)
        + " numberPlan: " + StringUtils::ToString(numberPlan)
        + " isPresent: " + StringUtils::ToString(isPresent)
        + " signalType: " + StringUtils::ToString(signalType)
        + " alertPitch: " + StringUtils::ToString(alertPitch)
        + " signal: " + StringUtils::ToString(signal) ;
    return NOERROR;
}

Int32 CdmaCallWaitingNotification::PresentationFromCLIP(
    /* [in] */ Int32 cli)
{
    switch(cli) {
        case 0: return IPhoneConstants::PRESENTATION_ALLOWED;
        case 1: return IPhoneConstants::PRESENTATION_RESTRICTED;
        case 2: return IPhoneConstants::PRESENTATION_UNKNOWN;
        default:
            // This shouldn't happen, just log an error and treat as Unknown
            Logger::D(LOGTAG, "Unexpected presentation %d", cli);
            return IPhoneConstants::PRESENTATION_UNKNOWN;
    }
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
