
#include "CallTypeHelper.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CallTypeHelper, Object, ICallTypeHelper)

ECode CallTypeHelper::constructor(
    /* [in] */ IResources* resources)
{
    // Cache these values so that we do not need to look them up each time.
    resources->GetString(R::string::type_incoming, &mIncomingName);
    resources->GetString(R::string::type_outgoing, &mOutgoingName);
    resources->GetString(R::string::type_missed, &mMissedName);
    resources->GetString(R::string::type_incoming_video, &mIncomingVideoName);
    resources->GetString(R::string::type_outgoing_video, &mOutgoingVideoName);
    resources->GetString(R::string::type_missed_video, &mMissedVideoName);
    resources->GetString(R::string::type_voicemail, &mVoicemailName);
    resources->GetColor(R::color::call_log_missed_call_highlight_color, &mNewMissedColor);
    resources->GetColor(R::color::call_log_voicemail_highlight_color, &mNewVoicemailColor);

    return NOERROR;
}

ECode CallTypeHelper::GetCallTypeText(
    /* [in] */ Int32 callType,
    /* [in] */ Boolean isVideoCall,
    /* [out] */ ICharSequence** text)
{
    VALUE_NOT_NULL(text);

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

    *text = CoreUtils::Convert(result);
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CallTypeHelper::GetHighlightedColor(
    /* [in] */ Int32 callType,
    /* [out] */ IInteger32** result)
{
    VALUE_NOT_NULL(result);

    switch (callType) {
        case ICalls::INCOMING_TYPE:
            // New incoming calls are not highlighted.
            *result =  NULL;
            break;

        case ICalls::OUTGOING_TYPE:
            // New outgoing calls are not highlighted.
            result =  NULL;
            break;

        case ICalls::MISSED_TYPE:
            *result = CoreUtils::Convert(mNewMissedColor);
            break;

        case ICalls::VOICEMAIL_TYPE:
            *result = CoreUtils::Convert(mNewVoicemailColor);
            break;

        default:
            // Don't highlight calls of unknown types. They are treated as missed calls by
            // the rest of the UI, but since they will never be marked as read by
            // {@link CallLogQueryHandler}, just don't ever highlight them anyway.
            *result =  NULL;
            break;
    }

    if (*result != NULL) {
        REFCOUNT_ADD(*result);
    }

    return NOERROR;
}

Boolean CallTypeHelper::IsMissedCallType(
    /* [in] */ Int32 callType)
{
    return (callType != ICalls::INCOMING_TYPE && callType != ICalls::OUTGOING_TYPE
             && callType != ICalls::VOICEMAIL_TYPE);
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
