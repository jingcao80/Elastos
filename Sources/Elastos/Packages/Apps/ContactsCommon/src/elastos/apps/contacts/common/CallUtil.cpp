
#include "CallUtil.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ const String& number)
{
    return GetCallIntent(number, String(NULL), NULL);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ IUri* uri)
{
    return getCallIntent(uri, String(NULL), NULL);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin)
{
    return GetCallIntent(GetCallUri(number), callOrigin, NULL);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetCallIntent(number, String(NULL), accountHandle);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ IUri* uri,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetCallIntent(uri, String(NULL), accountHandle);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetCallIntent(GetCallUri(number), callOrigin, accountHandle);

}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ IUri* uri,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetCallIntent(uri, callOrigin, accountHandle,
            IVideoProfileVideoState::AUDIO_ONLY);

}

AutoPtr<IIntent> CallUtil::GetVideoCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin)
{
    return GetCallIntent(GetCallUri(number), callOrigin, NULL,
                IVideoProfileVideoState::BIDIRECTIONAL);
}

AutoPtr<IIntent> CallUtil::GetVideoCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetCallIntent(GetCallUri(number), callOrigin, accountHandle,
                IVideoProfileVideoState::BIDIRECTIONAL);
}

AutoPtr<IIntent> CallUtil::GetVideoCallIntent(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetVideoCallIntent(number, String(NULL), accountHandle);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ IUri* uri,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle accountHandle*,
    /* [in] */ Int32 videoState)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_CALL_PRIVILEGED, uri, (IIntent**)&intent);
    intent->PutExtra(ITelecomManager::EXTRA_START_CALL_WITH_VIDEO_STATE, videoState);
    if (!callOrigin.IsNull()) {
        assert(0 && "TODO")
        // intent->PutExtra(IPhoneConstants::EXTRA_CALL_ORIGIN, callOrigin);
    }
    if (accountHandle != NULL) {
        intent->PutExtra(ITelecomManager::EXTRA_PHONE_ACCOUNT_HANDLE, accountHandle);
    }

    return intent;
}

AutoPtr<IUri> CallUtil::GetCallUri(
    /* [in] */ const String& number)
{
     if (PhoneNumberHelper::IsUriNumber(number)) {
         return Uri::FromParts(IPhoneAccount::SCHEME_SIP, number, NULL);
    }
    return Uri::fromParts(IPhoneAccount::SCHEME_TEL, number, NULL);
}

Boolean CallUtil::IsVideoEnabled(
    /* [in] */ IContext* context)
{
    AutoPtr<IInerface> service;
    context->GetSystemService(Context.TELECOM_SERVICE, (IInerface**)&service);
    ITelecomManager* telecommMgr = ITelecomManager::Probe(service);
    if (telecommMgr == NULL) {
        return FALSE;
    }

    // TODO: Check telecommManager for value instead.
    // return telecommMgr.isVideoEnabled();
    return FALSE;
}

} // Common
} // Contacts
} // Apps
} // Elastos
