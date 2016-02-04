
#include "elastos/droid/media/CUserRouteInfo.h"
#include "elastos/droid/media/CRouteGroup.h"

#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

CUserRouteInfo::CUserRouteInfo()
{}

CUserRouteInfo::~CUserRouteInfo()
{}

CAR_OBJECT_IMPL(CUserRouteInfo)

CAR_INTERFACE_IMPL(CUserRouteInfo, Object, IMediaRouterUserRouteInfo, )

ECode CUserRouteInfo::constructor(
    /* [in] */ IRouteCategory* category)
{
    MediaRouteInfo::Init(category);
    mSupportedTypes = IMediaRouter::ROUTE_TYPE_USER;
    mPlaybackType = IRouteInfo::PLAYBACK_TYPE_REMOTE;
    mVolumeHandling = IRouteInfo::PLAYBACK_VOLUME_FIXED;
    return NOERROR;
}

PInterface CUserRouteInfo::Probe(
    /* [in] */ REIID riid)
{
    return _CUserRouteInfo::Probe(riid);
}

ECode CUserRouteInfo::SetName(
    /* [in] */ ICharSequence* name)
{
    mName = name;
    MediaRouteInfo::RouteUpdated();
    return NOERROR;
}

ECode CUserRouteInfo::SetName(
    /* [in] */ Int32 resId)
{
    mNameResId = resId;
    mName = NULL;
    MediaRouteInfo::RouteUpdated();
    return NOERROR;
}

ECode CUserRouteInfo::SetStatus(
    /* [in] */ ICharSequence* status)
{
    MediaRouteInfo::SetStatusInt(status);
    return NOERROR;
}

ECode CUserRouteInfo::SetRemoteControlClient(
    /* [in] */ IRemoteControlClient* rcc)
{
    mRcc = rcc;
    UpdatePlaybackInfoOnRcc();
    return NOERROR;
}

ECode CUserRouteInfo::GetRemoteControlClient(
    /* [out] */ IRemoteControlClient** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRcc;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CUserRouteInfo::SetIconDrawable(
    /* [in] */ IDrawable* icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode CUserRouteInfo::SetIconResource(
    /* [in] */ Int32 resId)
{
    AutoPtr<IDrawable> drawable;
    CMediaRouter::sStatic->mResources->GetDrawable(resId, (IDrawable**)&drawable);
    SetIconDrawable(drawable);
    return NOERROR;
}

ECode CUserRouteInfo::SetVolumeCallback(
    /* [in] */ IVolumeCallback* vcb)
{
    mVcb = new CMediaRouter::VolumeCallbackInfo(vcb, THIS_PROBE(IRouteInfo));
    return NOERROR;
}

ECode CUserRouteInfo::SetPlaybackType(
    /* [in] */ Int32 type)
{
    if (mPlaybackType != type) {
        mPlaybackType = type;
        SetPlaybackInfoOnRcc(IRemoteControlClient::PLAYBACKINFO_PLAYBACK_TYPE, type);
    }
    return NOERROR;
}

ECode CUserRouteInfo::SetVolumeHandling(
    /* [in] */ Int32 volumeHandling)
{
    if (mVolumeHandling != volumeHandling) {
        mVolumeHandling = volumeHandling;
        SetPlaybackInfoOnRcc(
                IRemoteControlClient::PLAYBACKINFO_VOLUME_HANDLING, volumeHandling);
    }
    return NOERROR;
}

ECode CUserRouteInfo::SetVolume(
    /* [in] */ Int32 volume)
{
    using Elastos::Core::Math;
    Int32 tempValue;
    MediaRouteInfo::GetVolumeMax(&tempValue);
    volume = Math::Max(0, Math::Min(volume, tempValue));
    if (mVolume != volume) {
        mVolume = volume;
        SetPlaybackInfoOnRcc(IRemoteControlClient::PLAYBACKINFO_VOLUME, volume);
        CMediaRouter::DispatchRouteVolumeChanged(THIS_PROBE(IRouteInfo));
        if (mGroup != NULL) {
            AutoPtr<IRouteGroup> routeGroup = mGroup;
            CRouteGroup* group = (CRouteGroup*)routeGroup.Get();
            group->MemberVolumeChanged(THIS_PROBE(IRouteInfo));
        }
    }
    return NOERROR;
}

ECode CUserRouteInfo::RequestSetVolume(
    /* [in] */ Int32 volume)
{
    if (mVolumeHandling == IRouteInfo::PLAYBACK_VOLUME_VARIABLE) {
        if (mVcb == NULL) {
            Logger::E(CMediaRouter::TAG, "Cannot requestSetVolume on user route - no volume callback set");
            return NOERROR;
        }
        mVcb->mVcb->OnVolumeSetRequest(THIS_PROBE(IRouteInfo), volume);
    }
    return NOERROR;
}

ECode CUserRouteInfo::RequestUpdateVolume(
    /* [in] */ Int32 direction)
{
    if (mVolumeHandling == IRouteInfo::PLAYBACK_VOLUME_VARIABLE) {
        if (mVcb == NULL) {
            Logger::E(CMediaRouter::TAG, "Cannot requestChangeVolume on user route - no volumec callback set");
            return NOERROR;
        }
        mVcb->mVcb->OnVolumeUpdateRequest(THIS_PROBE(IRouteInfo), direction);
    }
    return NOERROR;
}

ECode CUserRouteInfo::SetVolumeMax(
    /* [in] */ Int32 volumeMax)
{
    if (mVolumeMax != volumeMax) {
        mVolumeMax = volumeMax;
        SetPlaybackInfoOnRcc(IRemoteControlClient::PLAYBACKINFO_VOLUME_MAX, volumeMax);
    }
    return NOERROR;
}

ECode CUserRouteInfo::SetPlaybackStream(
    /* [in] */ Int32 stream)
{
    if (mPlaybackStream != stream) {
        mPlaybackStream = stream;
        SetPlaybackInfoOnRcc(IRemoteControlClient::PLAYBACKINFO_USES_STREAM, stream);
    }
    return NOERROR;
}

void CUserRouteInfo::UpdatePlaybackInfoOnRcc()
{
    Int32 tempValue;
    if ((mRcc != NULL) && ((mRcc->GetRcseId(&tempValue), tempValue) != IRemoteControlClient::RCSE_ID_UNREGISTERED))
    {
        mRcc->SetPlaybackInformation(
                IRemoteControlClient::PLAYBACKINFO_VOLUME_MAX, mVolumeMax);
        mRcc->SetPlaybackInformation(
                IRemoteControlClient::PLAYBACKINFO_VOLUME, mVolume);
        mRcc->SetPlaybackInformation(
                IRemoteControlClient::PLAYBACKINFO_VOLUME_HANDLING, mVolumeHandling);
        mRcc->SetPlaybackInformation(
                IRemoteControlClient::PLAYBACKINFO_USES_STREAM, mPlaybackStream);
        mRcc->SetPlaybackInformation(
                IRemoteControlClient::PLAYBACKINFO_PLAYBACK_TYPE, mPlaybackType);
        // let AudioService know whom to call when remote volume needs to be updated
//        try {
        mRcc->GetRcseId(&tempValue);
        ECode ec = CMediaRouter::sStatic->mAudioService->RegisterRemoteVolumeObserverForRcc(
                tempValue /* rccId */, mRemoteVolObserver /* rvo */);
//        } catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(CMediaRouter::TAG, "Error registering remote volume observer"/*, e*/);
            }
//        }
    }
}

void CUserRouteInfo::SetPlaybackInfoOnRcc(
    /* [in] */ Int32 what,
    /* [in] */ Int32 value)
{
    if (mRcc != NULL) {
        mRcc->SetPlaybackInformation(what, value);
    }
}

//-------------------------------
//    MediaRouteInfo
//-------------------------------

ECode CUserRouteInfo::GetName(
    /* [out] */ ICharSequence** result)
{
    return MediaRouteInfo::GetName(result);
}

ECode CUserRouteInfo::GetName(
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** result)
{
    return MediaRouteInfo::GetName(context, result);
}

ECode CUserRouteInfo::GetStatus(
    /* [out] */ ICharSequence** result)
{
    return MediaRouteInfo::GetStatus(result);
}

ECode CUserRouteInfo::GetStatusCode(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetStatusCode(result);
}

ECode CUserRouteInfo::GetSupportedTypes(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetSupportedTypes(result);
}

ECode CUserRouteInfo::SetGroup(
    /* [in] */ IRouteGroup* group)
{
    return MediaRouteInfo::SetGroup(group);
}

ECode CUserRouteInfo::GetGroup(
    /* [out] */ IRouteGroup** result)
{
    return MediaRouteInfo::GetGroup(result);
}

ECode CUserRouteInfo::SetCategory(
    /* [in] */ IRouteCategory* category)
{
    return MediaRouteInfo::SetCategory(category);
}

ECode CUserRouteInfo::GetCategory(
    /* [out] */ IRouteCategory** result)
{
    return MediaRouteInfo::GetCategory(result);
}

ECode CUserRouteInfo::GetIconDrawable(
    /* [out] */ IDrawable** result)
{
    return MediaRouteInfo::GetIconDrawable(result);
}

ECode CUserRouteInfo::SetTag(
    /* [in] */ IInterface* tag)
{
    return MediaRouteInfo::SetTag(tag);
}

ECode CUserRouteInfo::GetTag(
    /* [out] */ IInterface** result)
{
    return MediaRouteInfo::GetTag(result);
}

ECode CUserRouteInfo::GetPlaybackType(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetPlaybackType(result);
}

ECode CUserRouteInfo::GetPlaybackStream(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetPlaybackStream(result);
}

ECode CUserRouteInfo::GetVolume(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetVolume(result);
}

ECode CUserRouteInfo::GetVolumeMax(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetVolumeMax(result);
}

ECode CUserRouteInfo::GetVolumeHandling(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetVolumeHandling(result);
}

ECode CUserRouteInfo::GetPresentationDisplay(
    /* [out] */ IDisplay** result)
{
    return MediaRouteInfo::GetPresentationDisplay(result);
}

ECode CUserRouteInfo::IsEnabled(
    /* [out] */ Boolean* result)
{
    return MediaRouteInfo::IsEnabled(result);
}

ECode CUserRouteInfo::ToString(
    /* [out] */ String* result)
{
    return MediaRouteInfo::ToString(result);
}

ECode CUserRouteInfo::SetSupportedTypes(
    /* [in] */ Int32 supportedTypes)
{
    return MediaRouteInfo::SetSupportedTypes(supportedTypes);
}

ECode CUserRouteInfo::SetPresentationDisplay(
    /* [in] */ IDisplay* presentationDisplay)
{
    return MediaRouteInfo::SetPresentationDisplay(presentationDisplay);
}

ECode CUserRouteInfo::GetDeviceAddress(
    /* [out] */ String* result)
{
    return MediaRouteInfo::GetDeviceAddress(result);
}

ECode CUserRouteInfo::SetDeviceAddress(
    /* [in] */ const String& deviceAddress)
{
    return MediaRouteInfo::SetDeviceAddress(deviceAddress);
}

ECode CUserRouteInfo::SetEnabled(
    /* [in] */ Boolean enabled)
{
    return MediaRouteInfo::SetEnabled(enabled);
}

ECode CUserRouteInfo::GetEnabled(
    /* [out] */ Boolean* result)
{
    return MediaRouteInfo::GetEnabled(result);
}

ECode CUserRouteInfo::SetStatusCode(
    /* [in] */ Int32 statusCode,
    /* [out] */ Boolean* result)
{
    return MediaRouteInfo::SetStatusCode(statusCode, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
