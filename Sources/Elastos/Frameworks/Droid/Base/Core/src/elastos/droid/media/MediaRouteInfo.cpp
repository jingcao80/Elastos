
#include "elastos/droid/media/MediaRouteInfo.h"

#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Math.h>
#include "elastos/droid/R.h"

using Elastos::Utility::Logging::Logger;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Media {

//-------------------------------------------
//    MediaRouteInfo::MyRemoteVolumeObserver
//-------------------------------------------

CAR_INTERFACE_IMPL(MediaRouteInfo::MyRemoteVolumeObserver, IIRemoteVolumeObserver);

MediaRouteInfo::MyRemoteVolumeObserver::MyRemoteVolumeObserver()
{}

ECode MediaRouteInfo::MyRemoteVolumeObserver::DispatchRemoteVolumeUpdate(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 value)
{
    AutoPtr<MyRunnable> myRunnable = new MyRunnable(direction, value, (MediaRouteInfo*)this);
    Boolean tempState;
    CMediaRouter::sStatic->mHandler->Post(myRunnable, &tempState);
    return NOERROR;
}

//-------------------------------
//    MediaRouteInfo::MyRunnable
//-------------------------------

CAR_INTERFACE_IMPL(MediaRouteInfo::MyRunnable, IRunnable);

MediaRouteInfo::MyRunnable::MyRunnable(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 value,
    /* [in] */ MediaRouteInfo* owner)
    : mDirection(direction)
    , mValue(value)
    , mOwner(owner)
{}

ECode MediaRouteInfo::MyRunnable::Run()
{
    if (mOwner->mVcb != NULL) {
        if (mDirection != 0) {
            mOwner->mVcb->mVcb->OnVolumeUpdateRequest(mOwner->mVcb->mRoute, mDirection);
        }
        else {
            mOwner->mVcb->mVcb->OnVolumeSetRequest(mOwner->mVcb->mRoute, mValue);
        }
    }
    return NOERROR;
}

//-------------------------------

MediaRouteInfo::MediaRouteInfo()
    : mPlaybackType(IRouteInfo::PLAYBACK_TYPE_LOCAL)
    , mVolumeMax(IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME)
    , mVolume(IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME)
    , mVolumeHandling(IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME_HANDLING)
    , mPlaybackStream(IAudioManager::STREAM_MUSIC)
    , mEnabled(TRUE)
{
    mRemoteVolObserver = new MyRemoteVolumeObserver();
}

ECode MediaRouteInfo::Init(
    /* [in] */ IRouteCategory* category)
{
    mCategory = category;
    return NOERROR;
}

ECode MediaRouteInfo::GetName(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> temp = GetName(CMediaRouter::sStatic->mResources);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouteInfo::GetName(
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> temp = GetName(resources);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouteInfo::GetStatus(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mStatus;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouteInfo::GetStatusCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mStatusCode;
    return NOERROR;
}

ECode MediaRouteInfo::GetSupportedTypes(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSupportedTypes;
    return NOERROR;
}

ECode MediaRouteInfo::GetGroup(
    /* [out] */ IRouteGroup** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mGroup;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouteInfo::SetGroup(
    /* [in] */ IRouteGroup* group)
{
    mGroup = group;
    return NOERROR;
}

ECode MediaRouteInfo::SetCategory(
    /* [in] */ IRouteCategory* category)
{
    mCategory = category;
    return NOERROR;
}

ECode MediaRouteInfo::GetCategory(
    /* [out] */ IRouteCategory** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCategory;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouteInfo::GetIconDrawable(
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIcon;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouteInfo::SetTag(
    /* [in] */ IInterface* tag)
{
    mTag = tag;
    RouteUpdated();
    return NOERROR;
}

ECode MediaRouteInfo::GetTag(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTag;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouteInfo::GetPlaybackType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mPlaybackType;
    return NOERROR;
}

ECode MediaRouteInfo::GetPlaybackStream(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mPlaybackStream;
    return NOERROR;
}

ECode MediaRouteInfo::GetVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mPlaybackType == IRouteInfo::PLAYBACK_TYPE_LOCAL) {
        Int32 vol = 0;
        ECode ec;
//        try {
            ec = CMediaRouter::sStatic->mAudioService->GetStreamVolume(mPlaybackStream, &vol);
//        } catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(CMediaRouter::TAG, "Error getting local stream volume"/*, e*/);
            }
//        }
        *result = vol;
    } else {
        *result = mVolume;
    }
    return NOERROR;
}

ECode MediaRouteInfo::RequestSetVolume(
    /* [in] */ Int32 volume)
{
    if (mPlaybackType == IRouteInfo::PLAYBACK_TYPE_LOCAL) {
        ECode ec;
//        try {
            ec = CMediaRouter::sStatic->mAudioService->SetStreamVolume(mPlaybackStream, volume, 0);
//        } catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(CMediaRouter::TAG, "Error setting local stream volume"/*, e*/);
            }
//        }
    } else {
        Logger::E(CMediaRouter::TAG, /*getClass().getSimpleName() +*/ String(".requestSetVolume(): ") +
                String("Non-local volume playback on system route? ") +
                String("Could not request volume change."));
    }
    return NOERROR;
}

ECode MediaRouteInfo::RequestUpdateVolume(
    /* [in] */ Int32 direction)
{
    if (mPlaybackType == IRouteInfo::PLAYBACK_TYPE_LOCAL) {
        Int32 tempValue1, tempValue2;
        ECode ec;
//        try {
            Int32 volume =
                    Elastos::Core::Math::Max(0, Elastos::Core::Math::Min((GetVolume(&tempValue1), tempValue1) + direction, (GetVolumeMax(&tempValue2), tempValue2)));
            ec = CMediaRouter::sStatic->mAudioService->SetStreamVolume(mPlaybackStream, volume, 0);
//        } catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(CMediaRouter::TAG, "Error setting local stream volume"/*, e*/);
            }
//        }
    }
    else {
        Logger::E(CMediaRouter::TAG, /*getClass()->GetSimpleName() +*/ String(".requestChangeVolume(): ") +
                String("Non-local volume playback on system route? ") +
                String("Could not request volume change."));
    }
    return NOERROR;
}

ECode MediaRouteInfo::GetVolumeMax(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mPlaybackType == IRouteInfo::PLAYBACK_TYPE_LOCAL) {
        Int32 volMax = 0;
        ECode ec;
//        try {
            ec = CMediaRouter::sStatic->mAudioService->GetStreamMaxVolume(mPlaybackStream, &volMax);
//        } catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(CMediaRouter::TAG, "Error getting local stream volume"/*, e*/);
            }
//        }
        *result = volMax;
    } else {
        *result = mVolumeMax;
    }
    return NOERROR;
}

ECode MediaRouteInfo::GetVolumeHandling(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mVolumeHandling;
    return NOERROR;
}

ECode MediaRouteInfo::GetPresentationDisplay(
    /* [out] */ IDisplay** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mPresentationDisplay;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouteInfo::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mEnabled;
    return NOERROR;
}

ECode MediaRouteInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 tempValue;
    GetSupportedTypes(&tempValue);
    String supportedTypes = CMediaRouter::TypesToString(tempValue);
    String info, name, status, category;
    AutoPtr<ICharSequence> statusSeq, nameSeq;
    GetName((ICharSequence**)&nameSeq);
    GetStatus((ICharSequence**)&statusSeq);
    nameSeq->ToString(&name);
    statusSeq->ToString(&status);
    AutoPtr<IRouteCategory> catObj;
    GetCategory((IRouteCategory**)&catObj);
    catObj->ToString(&category);

    info.AppendFormat("MediaRouteInfo{name=%s, status=%s, category=%s, supportedTypes=%s, presentationDisplay=%p",
        name.string(), status.string(), category.string(), supportedTypes.string(), mPresentationDisplay.Get());
    return NOERROR;
}

ECode MediaRouteInfo::SetName(
    /* [in] */ Int32 nameResId)
{
    mNameResId = nameResId;
    return NOERROR;
}

ECode MediaRouteInfo::SetSupportedTypes(
    /* [in] */ Int32 supportedTypes)
{
    mSupportedTypes = supportedTypes;
    return NOERROR;
}

ECode MediaRouteInfo::SetPresentationDisplay(
    /* [in] */ IDisplay* presentationDisplay)
{
    mPresentationDisplay = presentationDisplay;
    return NOERROR;
}

ECode MediaRouteInfo::SetName(
    /* [in] */ ICharSequence* name)
{
    mName = name;
    return NOERROR;
}

ECode MediaRouteInfo::GetDeviceAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDeviceAddress;
    return NOERROR;
}

ECode MediaRouteInfo::SetDeviceAddress(
    /* [in] */ const String& deviceAddress)
{
    mDeviceAddress = deviceAddress;
    return NOERROR;
}

ECode MediaRouteInfo::SetVolumeHandling(
    /* [in] */ Int32 volumeHandling)
{
    mVolumeHandling = volumeHandling;
    return NOERROR;
}

ECode MediaRouteInfo::SetPlaybackType(
    /* [in] */ Int32 playbackType)
{
    mPlaybackType = playbackType;
    return NOERROR;
}

ECode MediaRouteInfo::SetEnabled(
    /* [in] */ Boolean enabled)
{
    mEnabled = enabled;
    return NOERROR;
}

ECode MediaRouteInfo::GetEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mEnabled;
    return NOERROR;
}

ECode MediaRouteInfo::SetStatusCode(
    /* [in] */ Int32 statusCode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (statusCode != mStatusCode) {
        mStatusCode = statusCode;
        Int32 resId = 0;
        switch (statusCode) {
            case IRouteInfo::STATUS_SCANNING:
            {
                resId = R::string::media_route_status_scanning;
                break;
            }
            case IRouteInfo::STATUS_CONNECTING:
            {
                resId = R::string::media_route_status_connecting;
                break;
            }
            case IRouteInfo::STATUS_AVAILABLE:
            {
                resId = R::string::media_route_status_available;
                break;
            }
            case IRouteInfo::STATUS_NOT_AVAILABLE:
            {
                resId = R::string::media_route_status_not_available;
                break;
            }
        }

        mStatus = NULL;
        if (resId != 0) {
            CMediaRouter::sStatic->mResources->GetText(resId, (ICharSequence**)&mStatus);
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

AutoPtr<ICharSequence> MediaRouteInfo::GetName(
    /* [in] */ IResources* res)
{
    if (mNameResId != 0) {
        res->GetText(mNameResId, (ICharSequence**)&mName);
        return mName;
    }
    return mName;
}

void MediaRouteInfo::SetStatusInt(
    /* [in] */ ICharSequence* status)
{
    String tempText1, tempText2;
    status->ToString(&tempText1);
    mStatus->ToString(&tempText2);
    if (!tempText1.Equals(tempText2)) {
        mStatus = status;
        if (mGroup != NULL) {
            mGroup->MemberStatusChanged(THIS_PROBE(IRouteInfo), status);
        }
        RouteUpdated();
    }
}

void MediaRouteInfo::RouteUpdated()
{
    CMediaRouter::UpdateRoute(THIS_PROBE(IRouteInfo));
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
