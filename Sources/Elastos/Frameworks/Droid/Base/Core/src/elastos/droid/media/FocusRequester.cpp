#include "elastos/droid/media/FocusRequester.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

String FocusRequester::TAG("MediaFocusControl");
Boolean FocusRequester::DEBUG = FALSE;

CAR_INTERFACE_IMPL(FocusRequester, Object, IFocusRequester)

FocusRequester::FocusRequester()
    : mCallingUid(0)
    , mFocusGainRequest(0)
    , mFocusLossReceived(0)
    , mStreamType(0)
{
}

FocusRequester::~FocusRequester()
{
}

ECode FocusRequester::constructor(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 focusRequest,
    /* [in] */ IIAudioFocusDispatcher* afl,
    /* [in] */ IBinder* source,
    /* [in] */ const String& id,
    /* [in] */ IProxyDeathRecipient* hdlr,
    /* [in] */ const String& pn,
    /* [in] */ Int32 uid)
{
    mStreamType = streamType;
    mFocusDispatcher = afl;
    mSourceRef = source;
    mClientId = id;
    mDeathHandler = hdlr;
    mPackageName = pn;
    mCallingUid = uid;
    mFocusGainRequest = focusRequest;
    mFocusLossReceived = IAudioManager::AUDIOFOCUS_NONE;
    return NOERROR;
}

ECode FocusRequester::HasSameClient(
    /* [in] */ const String& otherClient,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    *result = mClientId.Compare(otherClient) == 0;
    return NOERROR;
    // } catch (NullPointerException e) {
    //     return false;
    // }
}

ECode FocusRequester::HasSameBinder(
    /* [in] */ IBinder* ib,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    IObject::Probe(mSourceRef)->Equals(ib, &b);
    *result = (mSourceRef != NULL) && b;
    return NOERROR;
}

ECode FocusRequester::HasSamePackage(
    /* [in] */ const String& pack,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    *result = mPackageName.Compare(pack) == 0;
    return NOERROR;
    // } catch (NullPointerException e) {
    //     return false;
    // }
}

ECode FocusRequester::HasSameUid(
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallingUid == uid;
    return NOERROR;
}

ECode FocusRequester::GetGainRequest(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFocusGainRequest;
    return NOERROR;
}

ECode FocusRequester::GetStreamType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStreamType;
    return NOERROR;
}

ECode FocusRequester::Dump(
    /* [in] */ IPrintWriter* pw)
{
    String str;
    mSourceRef->ToString(&str);
    StringBuilder sb;
    sb += "  source:";
    sb += str;
    sb += " -- pack: ";
    sb += mPackageName;
    sb += " -- client: ";
    sb += mClientId;
    sb += " -- gain: ";
    sb += FocusGainToString();
    sb += " -- loss: ";
    sb += FocusLossToString();
    sb += " -- uid: ";
    sb += mCallingUid;
    sb += " -- stream: ";
    sb += mStreamType;
    return pw->Println(sb.ToString());
}

ECode FocusRequester::ReleaseResources()
{
    // try {
    if (mSourceRef != NULL && mDeathHandler != NULL) {
        AutoPtr<IProxy> proxy = (IProxy*)mSourceRef->Probe(EIID_IProxy);
        if (proxy != NULL) {
            Boolean b;
            proxy->UnlinkToDeath(mDeathHandler, 0, &b);
        }
        mDeathHandler = NULL;
    }
    return NOERROR;
    // } catch (java.util.NoSuchElementException e) {
    //     Log.e(TAG, "FocusRequester.release() hit ", e);
    // }
}

ECode FocusRequester::HandleExternalFocusGain(
    /* [in] */ Int32 focusGain)
{
    Int32 focusLoss = FocusLossForGainRequest(focusGain);
    return HandleFocusLoss(focusLoss);
}

ECode FocusRequester::HandleFocusGain(
    /* [in] */ Int32 focusGain)
{
    // try {
    if (mFocusDispatcher != NULL) {
        if (DEBUG) {
            Logger::V(TAG, "dispatching %s to %s",
                    FocusChangeToString(focusGain).string(), mClientId.string());
        }
        mFocusDispatcher->DispatchAudioFocusChange(focusGain, mClientId);
    }
    mFocusLossReceived = IAudioManager::AUDIOFOCUS_NONE;
    return NOERROR;
    // } catch (android.os.RemoteException e) {
    //     Log.e(TAG, "Failure to signal gain of audio focus due to: ", e);
    // }
}

ECode FocusRequester::HandleFocusLoss(
    /* [in] */ Int32 focusLoss)
{
    // try {
    if (focusLoss != mFocusLossReceived) {
        if (mFocusDispatcher != NULL) {
            if (DEBUG) {
                Logger::V(TAG, "dispatching %s to %s",
                        FocusChangeToString(focusLoss).string(), mClientId.string());
            }
            mFocusDispatcher->DispatchAudioFocusChange(focusLoss, mClientId);
        }
        mFocusLossReceived = focusLoss;
    }
    return NOERROR;
    // } catch (android.os.RemoteException e) {
    //     Log.e(TAG, "Failure to signal loss of audio focus due to:", e);
    // }
}

String FocusRequester::FocusChangeToString(
    /* [in] */ Int32 focus)
{
    switch(focus) {
        case IAudioManager::AUDIOFOCUS_NONE:
            return String("none");
        case IAudioManager::AUDIOFOCUS_GAIN:
            return String("GAIN");
        case IAudioManager::AUDIOFOCUS_GAIN_TRANSIENT:
            return String("GAIN_TRANSIENT");
        case IAudioManager::AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK:
            return String("GAIN_TRANSIENT_MAY_DUCK");
        case IAudioManager::AUDIOFOCUS_GAIN_TRANSIENT_EXCLUSIVE:
            return String("GAIN_TRANSIENT_EXCLUSIVE");
        case IAudioManager::AUDIOFOCUS_LOSS:
            return String("LOSS");
        case IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT:
            return String("LOSS_TRANSIENT");
        case IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:
            return String("LOSS_TRANSIENT_CAN_DUCK");
        default:
            return String("[invalid focus change") + StringUtils::ToString(focus) + "]";
    }
}

String FocusRequester::FocusGainToString()
{
    return FocusChangeToString(mFocusGainRequest);
}

String FocusRequester::FocusLossToString()
{
    return FocusChangeToString(mFocusLossReceived);
}

Int32 FocusRequester::FocusLossForGainRequest(
    /* [in] */ Int32 gainRequest)
{
    switch(gainRequest) {
        case IAudioManager::AUDIOFOCUS_GAIN:
            switch(mFocusLossReceived) {
                case IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:
                case IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT:
                case IAudioManager::AUDIOFOCUS_LOSS:
                case IAudioManager::AUDIOFOCUS_NONE:
                    return IAudioManager::AUDIOFOCUS_LOSS;
            }
        case IAudioManager::AUDIOFOCUS_GAIN_TRANSIENT_EXCLUSIVE:
        case IAudioManager::AUDIOFOCUS_GAIN_TRANSIENT:
            switch(mFocusLossReceived) {
                case IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:
                case IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT:
                case IAudioManager::AUDIOFOCUS_NONE:
                    return IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT;
                case IAudioManager::AUDIOFOCUS_LOSS:
                    return IAudioManager::AUDIOFOCUS_LOSS;
            }
        case IAudioManager::AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK:
            switch(mFocusLossReceived) {
                case IAudioManager::AUDIOFOCUS_NONE:
                case IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK:
                    return IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK;
                case IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT:
                    return IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT;
                case IAudioManager::AUDIOFOCUS_LOSS:
                    return IAudioManager::AUDIOFOCUS_LOSS;
            }
        default:
            Logger::E(TAG, "focusLossForGainRequest() for invalid focus request %d", gainRequest);
            return IAudioManager::AUDIOFOCUS_NONE;
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
