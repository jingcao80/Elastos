
#include "voicemail/VoicemailStatusHelperStatusMessage.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Voicemail {

CAR_INTERFACE_IMPL(VoicemailStatusHelperStatusMessage, Object, IVoicemailStatusHelperStatusMessage);

ECode VoicemailStatusHelperStatusMessage::constructor(
    /* [in] */ const String& sourcePackage,
    /* [in] */ Int32 callLogMessageId,
    /* [in] */ Int32 callDetailsMessageId,
    /* [in] */ Int32 actionMessageId,
    /* [in] */ IUri* actionUri)
{
    mSourcePackage = sourcePackage;
    mCallLogMessageId = callLogMessageId;
    mCallDetailsMessageId = callDetailsMessageId;
    mActionMessageId = actionMessageId;
    mActionUri = actionUri;
    return NOERROR;
}

ECode VoicemailStatusHelperStatusMessage::ShowInCallLog(
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = mCallLogMessageId != -1;
    return NOERROR;
}

/** Whether this message should be shown in the call details page. */
ECode VoicemailStatusHelperStatusMessage::ShowInCallDetails(
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = mCallDetailsMessageId != -1;
    return NOERROR;
}

} // Voicemail
} // Dialer
} // Apps
} // Elastos
