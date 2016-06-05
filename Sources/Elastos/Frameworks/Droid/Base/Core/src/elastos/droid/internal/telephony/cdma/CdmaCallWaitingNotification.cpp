#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaCallWaitingNotification.h"

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

ECode CdmaCallWaitingNotification::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return super.toString() + "Call Waiting Notification  "
    //     + " number: " + number
    //     + " numberPresentation: " + numberPresentation
    //     + " name: " + name
    //     + " namePresentation: " + namePresentation
    //     + " numberType: " + numberType
    //     + " numberPlan: " + numberPlan
    //     + " isPresent: " + isPresent
    //     + " signalType: " + signalType
    //     + " alertPitch: " + alertPitch
    //     + " signal: " + signal ;
    assert(0);
    return NOERROR;
}

Int32 CdmaCallWaitingNotification::PresentationFromCLIP(
    /* [in] */ Int32 cli)
{
    // ==================before translated======================
    // switch(cli) {
    //     case 0: return PhoneConstants.PRESENTATION_ALLOWED;
    //     case 1: return PhoneConstants.PRESENTATION_RESTRICTED;
    //     case 2: return PhoneConstants.PRESENTATION_UNKNOWN;
    //     default:
    //         // This shouldn't happen, just log an error and treat as Unknown
    //         Rlog.d(LOGTAG, "Unexpected presentation " + cli);
    //         return PhoneConstants.PRESENTATION_UNKNOWN;
    // }
    assert(0);
    return 0;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
