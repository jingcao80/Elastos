
#include "elastos/droid/teleservice/services/telephony/DisconnectCauseUtil.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Media::IToneGenerator;
using Elastos::Droid::TeleService::R;
using Elastos::Droid::TeleService::Phone::PhoneGlobals;
using Elastos::Droid::Telecom::CDisconnectCause;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

AutoPtr<IDisconnectCause> DisconnectCauseUtil::ToTelecomDisconnectCause(
    /* [in] */ Int32 telephonyDisconnectCause)
{
    return ToTelecomDisconnectCause(telephonyDisconnectCause, String(NULL) /* reason */);
}

AutoPtr<IDisconnectCause> DisconnectCauseUtil::ToTelecomDisconnectCause(
    /* [in] */ Int32 telephonyDisconnectCause,
    /* [in] */ const String& reason)
{
    AutoPtr<PhoneGlobals> pg;
    PhoneGlobals::GetInstance((PhoneGlobals**)&pg);
    AutoPtr<IContext> context = IContext::Probe(pg);

    AutoPtr<IDisconnectCause> tmp;
    CDisconnectCause::New(
            ToTelecomDisconnectCauseCode(telephonyDisconnectCause),
            ToTelecomDisconnectCauseLabel(context, telephonyDisconnectCause),
            ToTelecomDisconnectCauseDescription(context, telephonyDisconnectCause),
            ToTelecomDisconnectReason(telephonyDisconnectCause, reason),
            ToTelecomDisconnectCauseTone(telephonyDisconnectCause),
            (IDisconnectCause**)&tmp);
    return tmp;
}

Int32 DisconnectCauseUtil::ToTelecomDisconnectCauseCode(
    /* [in] */ Int32 telephonyDisconnectCause)
{
    switch (telephonyDisconnectCause) {
        case Elastos::Droid::Telephony::IDisconnectCause::LOCAL:
            return Elastos::Droid::Telecom::IDisconnectCause::LOCAL;

        case Elastos::Droid::Telephony::IDisconnectCause::NORMAL:
            return Elastos::Droid::Telecom::IDisconnectCause::REMOTE;

        case Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_CANCELED:
            return Elastos::Droid::Telecom::IDisconnectCause::CANCELED;

        case Elastos::Droid::Telephony::IDisconnectCause::INCOMING_MISSED:
            return Elastos::Droid::Telecom::IDisconnectCause::MISSED;

        case Elastos::Droid::Telephony::IDisconnectCause::INCOMING_REJECTED:
            return Elastos::Droid::Telecom::IDisconnectCause::REJECTED;

        case Elastos::Droid::Telephony::IDisconnectCause::BUSY:
            return Elastos::Droid::Telecom::IDisconnectCause::BUSY;

        case Elastos::Droid::Telephony::IDisconnectCause::CALL_BARRED:
        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_ACCESS_BLOCKED:
        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_NOT_EMERGENCY:
        case Elastos::Droid::Telephony::IDisconnectCause::CS_RESTRICTED:
        case Elastos::Droid::Telephony::IDisconnectCause::CS_RESTRICTED_EMERGENCY:
        case Elastos::Droid::Telephony::IDisconnectCause::CS_RESTRICTED_NORMAL:
        case Elastos::Droid::Telephony::IDisconnectCause::EMERGENCY_ONLY:
        case Elastos::Droid::Telephony::IDisconnectCause::FDN_BLOCKED:
        case Elastos::Droid::Telephony::IDisconnectCause::LIMIT_EXCEEDED:
            return Elastos::Droid::Telecom::IDisconnectCause::RESTRICTED;

        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_ACCESS_FAILURE:
        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_CALL_LOST:
        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_DROP:
        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_INTERCEPT:
        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_LOCKED_UNTIL_POWER_CYCLE:
        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_PREEMPTED:
        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_REORDER:
        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_RETRY_ORDER:
        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_SO_REJECT:
        case Elastos::Droid::Telephony::IDisconnectCause::CONGESTION:
        case Elastos::Droid::Telephony::IDisconnectCause::ICC_ERROR:
        case Elastos::Droid::Telephony::IDisconnectCause::INVALID_CREDENTIALS:
        case Elastos::Droid::Telephony::IDisconnectCause::INVALID_NUMBER:
        case Elastos::Droid::Telephony::IDisconnectCause::LOST_SIGNAL:
        case Elastos::Droid::Telephony::IDisconnectCause::NO_PHONE_NUMBER_SUPPLIED:
        case Elastos::Droid::Telephony::IDisconnectCause::NUMBER_UNREACHABLE:
        case Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_FAILURE:
        case Elastos::Droid::Telephony::IDisconnectCause::OUT_OF_NETWORK:
        case Elastos::Droid::Telephony::IDisconnectCause::OUT_OF_SERVICE:
        case Elastos::Droid::Telephony::IDisconnectCause::POWER_OFF:
        case Elastos::Droid::Telephony::IDisconnectCause::SERVER_ERROR:
        case Elastos::Droid::Telephony::IDisconnectCause::SERVER_UNREACHABLE:
        case Elastos::Droid::Telephony::IDisconnectCause::TIMED_OUT:
        case Elastos::Droid::Telephony::IDisconnectCause::UNOBTAINABLE_NUMBER:
        case Elastos::Droid::Telephony::IDisconnectCause::VOICEMAIL_NUMBER_MISSING:
        case Elastos::Droid::Telephony::IDisconnectCause::ERROR_UNSPECIFIED:
            return Elastos::Droid::Telecom::IDisconnectCause::ERROR;

        case Elastos::Droid::Telephony::IDisconnectCause::DIALED_MMI:
        case Elastos::Droid::Telephony::IDisconnectCause::EXITED_ECM:
        case Elastos::Droid::Telephony::IDisconnectCause::MMI:
            return Elastos::Droid::Telecom::IDisconnectCause::OTHER;

        case Elastos::Droid::Telephony::IDisconnectCause::NOT_VALID:
        case Elastos::Droid::Telephony::IDisconnectCause::NOT_DISCONNECTED:
            return Elastos::Droid::Telecom::IDisconnectCause::UNKNOWN;

        default:
            //throw new IllegalStateException("Unrecognized Telephony DisconnectCause");
            return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<ICharSequence> DisconnectCauseUtil::ToTelecomDisconnectCauseLabel(
    /* [in] */ IContext* context,
    /* [in] */ Int32 telephonyDisconnectCause)
{
    if (context == NULL ) {
        AutoPtr<ICharSequence> tmp = CoreUtils::Convert(String(""));
        return tmp;
    }

    AutoPtr<IInteger32> resourceId;
    switch (telephonyDisconnectCause) {
        case Elastos::Droid::Telephony::IDisconnectCause::BUSY:
            resourceId = CoreUtils::Convert(R::string::callFailed_userBusy);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::CONGESTION:
            resourceId = CoreUtils::Convert(R::string::callFailed_congestion);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::TIMED_OUT:
            resourceId = CoreUtils::Convert(R::string::callFailed_timedOut);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::SERVER_UNREACHABLE:
            resourceId = CoreUtils::Convert(R::string::callFailed_server_unreachable);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::NUMBER_UNREACHABLE:
            resourceId = CoreUtils::Convert(R::string::callFailed_number_unreachable);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::INVALID_CREDENTIALS:
            resourceId = CoreUtils::Convert(R::string::callFailed_invalid_credentials);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::SERVER_ERROR:
            resourceId = CoreUtils::Convert(R::string::callFailed_server_error);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::OUT_OF_NETWORK:
            resourceId = CoreUtils::Convert(R::string::callFailed_out_of_network);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::LOST_SIGNAL:
        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_DROP:
            resourceId = CoreUtils::Convert(R::string::callFailed_noSignal);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::LIMIT_EXCEEDED:
            resourceId = CoreUtils::Convert(R::string::callFailed_limitExceeded);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::POWER_OFF:
            resourceId = CoreUtils::Convert(R::string::callFailed_powerOff);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::ICC_ERROR:
            resourceId = CoreUtils::Convert(R::string::callFailed_simError);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::OUT_OF_SERVICE:
            resourceId = CoreUtils::Convert(R::string::callFailed_outOfService);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::INVALID_NUMBER:
        case Elastos::Droid::Telephony::IDisconnectCause::UNOBTAINABLE_NUMBER:
            resourceId = CoreUtils::Convert(R::string::callFailed_unobtainable_number);
            break;

        default:
            break;
    }

    AutoPtr<ICharSequence> tmp;
    if (resourceId == NULL) {
        tmp = CoreUtils::Convert(String(""));
    }
    else {
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        Int32 id;
        resourceId->GetValue(&id);
        String str;
        resources->GetString(id, &str);
        tmp = CoreUtils::Convert(str);
    }
    return tmp;
}

AutoPtr<ICharSequence> DisconnectCauseUtil::ToTelecomDisconnectCauseDescription(
    /* [in] */ IContext* context,
    /* [in] */ Int32 telephonyDisconnectCause)
{
    if (context == NULL) {
        AutoPtr<ICharSequence> tmp = CoreUtils::Convert(String(""));
        return tmp;
    }

    AutoPtr<IInteger32> resourceId;
    switch (telephonyDisconnectCause) {
        case Elastos::Droid::Telephony::IDisconnectCause::CALL_BARRED:
            resourceId = CoreUtils::Convert(R::string::callFailed_cb_enabled);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::FDN_BLOCKED:
            resourceId = CoreUtils::Convert(R::string::callFailed_fdn_only);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::CS_RESTRICTED:
            resourceId = CoreUtils::Convert(R::string::callFailed_dsac_restricted);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::CS_RESTRICTED_EMERGENCY:
            resourceId = CoreUtils::Convert(R::string::callFailed_dsac_restricted_emergency);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::CS_RESTRICTED_NORMAL:
            resourceId = CoreUtils::Convert(R::string::callFailed_dsac_restricted_normal);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_FAILURE:
            // We couldn't successfully place the call; there was some
            // failure in the telephony layer.
            // TODO: Need UI spec for this failure case; for now just
            // show a generic error.
            resourceId = CoreUtils::Convert(R::string::incall_error_call_failed);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::POWER_OFF:
            // Radio is explictly powered off, presumably because the
            // device is in airplane mode.
            //
            // TODO: For now this UI is ultra-simple: we simply display
            // a message telling the user to turn off airplane mode.
            // But it might be nicer for the dialog to offer the option
            // to turn the radio on right there (and automatically retry
            // the call once network registration is complete.)
            resourceId = CoreUtils::Convert(R::string::incall_error_power_off);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::EMERGENCY_ONLY:
            // Only emergency numbers are allowed, but we tried to dial
            // a non-emergency number.
            resourceId = CoreUtils::Convert(R::string::incall_error_emergency_only);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::OUT_OF_SERVICE:
            // No network connection.
            resourceId = CoreUtils::Convert(R::string::incall_error_out_of_service);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::NO_PHONE_NUMBER_SUPPLIED:
            // The supplied Intent didn't contain a valid phone number.
            // (This is rare and should only ever happen with broken
            // 3rd-party apps.) For now just show a generic error.
            resourceId = CoreUtils::Convert(R::string::incall_error_no_phone_number_supplied);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::VOICEMAIL_NUMBER_MISSING:
            // TODO: Need to bring up the "Missing Voicemail Number" dialog, which
            // will ultimately take us to the Call Settings.
            resourceId = CoreUtils::Convert(R::string::incall_error_missing_voicemail_number);
            break;

        case Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_CANCELED:
            // We don't want to show any dialog for the canceled case since the call was
            // either canceled by the user explicitly (end-call button pushed immediately)
            // or some other app canceled the call and immediately issued a new CALL to
            // replace it.
        default:
            break;
    }

    AutoPtr<ICharSequence> tmp;
    if (resourceId == NULL) {
        tmp = CoreUtils::Convert(String(""));
    }
    else {
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        Int32 id;
        resourceId->GetValue(&id);
        String str;
        resources->GetString(id, &str);
        tmp = CoreUtils::Convert(str);
    }
    return tmp;
}

String DisconnectCauseUtil::ToTelecomDisconnectReason(
    /* [in] */ Int32 telephonyDisconnectCause,
    /* [in] */ const String& reason)
{
    AutoPtr<Elastos::Droid::Telephony::IDisconnectCause> helper;
    Elastos::Droid::Telephony::CDisconnectCause::AcquireSingleton(
                (Elastos::Droid::Telephony::IDisconnectCause**)&helper);
    String causeAsString;
    helper->ToString(telephonyDisconnectCause, &causeAsString);
    if (reason.IsNull()) {
        return causeAsString;
    }
    else {
        StringBuilder sb;
        sb += reason;
        sb += ", ";
        sb += causeAsString;
        return sb.ToString();
    }
}

Int32 DisconnectCauseUtil::ToTelecomDisconnectCauseTone(
    /* [in] */ Int32 telephonyDisconnectCause)
{
    switch (telephonyDisconnectCause) {
        case Elastos::Droid::Telephony::IDisconnectCause::BUSY:
            return IToneGenerator::TONE_SUP_BUSY;

        case Elastos::Droid::Telephony::IDisconnectCause::CONGESTION:
            return IToneGenerator::TONE_SUP_CONGESTION;

        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_REORDER:
            return IToneGenerator::TONE_CDMA_REORDER;

        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_INTERCEPT:
            return IToneGenerator::TONE_CDMA_ABBR_INTERCEPT;

        case Elastos::Droid::Telephony::IDisconnectCause::CDMA_DROP:
        case Elastos::Droid::Telephony::IDisconnectCause::OUT_OF_SERVICE:
            return IToneGenerator::TONE_CDMA_CALLDROP_LITE;

        case Elastos::Droid::Telephony::IDisconnectCause::UNOBTAINABLE_NUMBER:
            return IToneGenerator::TONE_SUP_ERROR;

        case Elastos::Droid::Telephony::IDisconnectCause::ERROR_UNSPECIFIED:
        case Elastos::Droid::Telephony::IDisconnectCause::LOCAL:
        case Elastos::Droid::Telephony::IDisconnectCause::NORMAL:
            return IToneGenerator::TONE_PROP_PROMPT;

        default:
            return IToneGenerator::TONE_UNKNOWN;
    }
}

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos
