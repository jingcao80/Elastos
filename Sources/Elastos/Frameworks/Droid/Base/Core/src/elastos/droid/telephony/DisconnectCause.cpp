#include "elastos/droid/telephony/DisconnectCause.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

ECode DisconnectCause::ToString(
    /* [in] */ Int32 cause,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    switch (cause) {
        case IDisconnectCause::NOT_DISCONNECTED:
            *result = String("NOT_DISCONNECTED");
            break;
        case IDisconnectCause::INCOMING_MISSED:
            *result = String("INCOMING_MISSED");
            break;
        case IDisconnectCause::NORMAL:
            *result = String("NORMAL");
            break;
        case IDisconnectCause::LOCAL:
            *result = String("LOCAL");
            break;
        case IDisconnectCause::BUSY:
            *result = String("BUSY");
            break;
        case IDisconnectCause::CONGESTION:
            *result = String("CONGESTION");
            break;
        case IDisconnectCause::INVALID_NUMBER:
            *result = String("INVALID_NUMBER");
            break;
        case IDisconnectCause::NUMBER_UNREACHABLE:
            *result = String("NUMBER_UNREACHABLE");
            break;
        case IDisconnectCause::SERVER_UNREACHABLE:
            *result = String("SERVER_UNREACHABLE");
            break;
        case IDisconnectCause::INVALID_CREDENTIALS:
            *result = String("INVALID_CREDENTIALS");
            break;
        case IDisconnectCause::OUT_OF_NETWORK:
            *result = String("OUT_OF_NETWORK");
            break;
        case IDisconnectCause::SERVER_ERROR:
            *result = String("SERVER_ERROR");
            break;
        case IDisconnectCause::TIMED_OUT:
            *result = String("TIMED_OUT");
            break;
        case IDisconnectCause::LOST_SIGNAL:
            *result = String("LOST_SIGNAL");
            break;
        case IDisconnectCause::LIMIT_EXCEEDED:
            *result = String("LIMIT_EXCEEDED");
            break;
        case IDisconnectCause::INCOMING_REJECTED:
            *result = String("INCOMING_REJECTED");
            break;
        case IDisconnectCause::POWER_OFF:
            *result = String("POWER_OFF");
            break;
        case IDisconnectCause::OUT_OF_SERVICE:
            *result = String("OUT_OF_SERVICE");
            break;
        case IDisconnectCause::ICC_ERROR:
            *result = String("ICC_ERROR");
            break;
        case IDisconnectCause::CALL_BARRED:
            *result = String("CALL_BARRED");
            break;
        case IDisconnectCause::FDN_BLOCKED:
            *result = String("FDN_BLOCKED");
            break;
        case IDisconnectCause::CS_RESTRICTED:
            *result = String("CS_RESTRICTED");
            break;
        case IDisconnectCause::CS_RESTRICTED_NORMAL:
            *result = String("CS_RESTRICTED_NORMAL");
            break;
        case IDisconnectCause::CS_RESTRICTED_EMERGENCY:
            *result = String("CS_RESTRICTED_EMERGENCY");
            break;
        case IDisconnectCause::UNOBTAINABLE_NUMBER:
            *result = String("UNOBTAINABLE_NUMBER");
            break;
        case IDisconnectCause::CDMA_LOCKED_UNTIL_POWER_CYCLE:
            *result = String("CDMA_LOCKED_UNTIL_POWER_CYCLE");
            break;
        case IDisconnectCause::CDMA_DROP:
            *result = String("CDMA_DROP");
            break;
        case IDisconnectCause::CDMA_INTERCEPT:
            *result = String("CDMA_INTERCEPT");
            break;
        case IDisconnectCause::CDMA_REORDER:
            *result = String("CDMA_REORDER");
            break;
        case IDisconnectCause::CDMA_SO_REJECT:
            *result = String("CDMA_SO_REJECT");
            break;
        case IDisconnectCause::CDMA_RETRY_ORDER:
            *result = String("CDMA_RETRY_ORDER");
            break;
        case IDisconnectCause::CDMA_ACCESS_FAILURE:
            *result = String("CDMA_ACCESS_FAILURE");
            break;
        case IDisconnectCause::CDMA_PREEMPTED:
            *result = String("CDMA_PREEMPTED");
            break;
        case IDisconnectCause::CDMA_NOT_EMERGENCY:
            *result = String("CDMA_NOT_EMERGENCY");
            break;
        case IDisconnectCause::CDMA_ACCESS_BLOCKED:
            *result = String("CDMA_ACCESS_BLOCKED");
            break;
        case IDisconnectCause::EMERGENCY_ONLY:
            *result = String("EMERGENCY_ONLY");
            break;
        case IDisconnectCause::NO_PHONE_NUMBER_SUPPLIED:
            *result = String("NO_PHONE_NUMBER_SUPPLIED");
            break;
        case IDisconnectCause::DIALED_MMI:
            *result = String("DIALED_MMI");
            break;
        case IDisconnectCause::VOICEMAIL_NUMBER_MISSING:
            *result = String("VOICEMAIL_NUMBER_MISSING");
            break;
        case IDisconnectCause::CDMA_CALL_LOST:
            *result = String("CDMA_CALL_LOST");
            break;
        case IDisconnectCause::EXITED_ECM:
            *result = String("EXITED_ECM");
            break;
        case IDisconnectCause::ERROR_UNSPECIFIED:
            *result = String("ERROR_UNSPECIFIED");
            break;
        case IDisconnectCause::OUTGOING_FAILURE:
            *result = String("OUTGOING_FAILURE");
            break;
        case IDisconnectCause::OUTGOING_CANCELED:
            *result = String("OUTGOING_CANCELED");
            break;
        default:
            *result = String("INVALID: ") + cause;
            break;
    }
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

