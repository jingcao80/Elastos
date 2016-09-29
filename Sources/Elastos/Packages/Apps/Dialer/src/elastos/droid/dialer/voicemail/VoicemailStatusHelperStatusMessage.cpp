
#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperStatusMessage.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Voicemail {

VoicemailStatusHelperStatusMessage::VoicemailStatusHelperStatusMessage(
    /* [in] */ const String& sourcePackage,
    /* [in] */ Int32 callLogMessageId,
    /* [in] */ Int32 callDetailsMessageId,
    /* [in] */ Int32 actionMessageId,
    /* [in] */ IUri* actionUri)
    : mSourcePackage(sourcePackage)
    , mCallLogMessageId(callLogMessageId)
    , mCallDetailsMessageId(callDetailsMessageId)
    , mActionMessageId(actionMessageId)
    , mActionUri(actionUri)
{}

Boolean VoicemailStatusHelperStatusMessage::ShowInCallLog()
{
    return mCallLogMessageId != -1;
}

Boolean VoicemailStatusHelperStatusMessage::ShowInCallDetails()
{
    return mCallDetailsMessageId != -1;
}

} // Voicemail
} // Dialer
} // Droid
} // Elastos
