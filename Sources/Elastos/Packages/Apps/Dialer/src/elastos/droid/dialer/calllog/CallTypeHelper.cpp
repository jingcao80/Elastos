
#include "elastos/droid/dialer/calllog/CallTypeHelper.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Content.h"
#include "elastos/core/CoreUtils.h"
#include "R.h"

using Elastos::Droid::Provider::ICalls;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CallTypeHelper::CallTypeHelper(
    /* [in] */ IResources* resources)
    : mNewMissedColor(0)
    , mNewVoicemailColor(0)
{
    // Cache these values so that we do not need to look them up each time.
    resources->GetString(Elastos::Droid::Dialer::R::string::type_incoming, &mIncomingName);
    resources->GetString(Elastos::Droid::Dialer::R::string::type_outgoing, &mOutgoingName);
    resources->GetString(Elastos::Droid::Dialer::R::string::type_missed, &mMissedName);
    resources->GetString(Elastos::Droid::Dialer::R::string::type_incoming_video, &mIncomingVideoName);
    resources->GetString(Elastos::Droid::Dialer::R::string::type_outgoing_video, &mOutgoingVideoName);
    resources->GetString(Elastos::Droid::Dialer::R::string::type_missed_video, &mMissedVideoName);
    resources->GetString(Elastos::Droid::Dialer::R::string::type_voicemail, &mVoicemailName);
    resources->GetColor(Elastos::Droid::Dialer::R::color::call_log_missed_call_highlight_color, &mNewMissedColor);
    resources->GetColor(Elastos::Droid::Dialer::R::color::call_log_voicemail_highlight_color, &mNewVoicemailColor);
}

AutoPtr<ICharSequence> CallTypeHelper::GetCallTypeText(
    /* [in] */ Int32 callType,
    /* [in] */ Boolean isVideoCall)
{
    String result;
    switch (callType) {
        case ICalls::INCOMING_TYPE:
            if (isVideoCall) {
                result = mIncomingVideoName;
            }
            else {
                result = mIncomingName;
            }
            break;

        case ICalls::OUTGOING_TYPE:
            if (isVideoCall) {
                result = mOutgoingVideoName;
            }
            else {
                result = mOutgoingName;
            }
            break;

        case ICalls::MISSED_TYPE:
            if (isVideoCall) {
                result = mMissedVideoName;
            }
            else {
                result = mMissedName;
            }
            break;

        case ICalls::VOICEMAIL_TYPE:
            result = mVoicemailName;
            break;

        default:
            result = mMissedName;
            break;
    }

    return CoreUtils::Convert(result);
}

AutoPtr<IInteger32> CallTypeHelper::GetHighlightedColor(
    /* [in] */ Int32 callType)
{
    AutoPtr<IInteger32> obj;
    switch (callType) {
        case ICalls::INCOMING_TYPE:
            // New incoming calls are not highlighted.
            return NULL;

        case ICalls::OUTGOING_TYPE:
            // New outgoing calls are not highlighted.
            return NULL;

        case ICalls::MISSED_TYPE:
            return CoreUtils::Convert(mNewMissedColor);

        case ICalls::VOICEMAIL_TYPE:
            return CoreUtils::Convert(mNewVoicemailColor);

        default:
            // Don't highlight calls of unknown types. They are treated as missed calls by
            // the rest of the UI, but since they will never be marked as read by
            // {@link CallLogQueryHandler}, just don't ever highlight them anyway.
            return NULL;
    }
}

Boolean CallTypeHelper::IsMissedCallType(
    /* [in] */ Int32 callType)
{
    return (callType != ICalls::INCOMING_TYPE && callType != ICalls::OUTGOING_TYPE
             && callType != ICalls::VOICEMAIL_TYPE);
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
