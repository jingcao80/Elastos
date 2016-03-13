
#include "elastos/droid/server/media/CSessionController.h"
#include "elastos/droid/server/media/MediaSessionRecord.h"
#include "elastos/droid/os/Binder.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Media::IAudioAttributesHelper;
using Elastos::Droid::Media::CAudioAttributesHelper;
using Elastos::Droid::Media::IVolumeProvider;
using Elastos::Droid::Media::Session::EIID_IISessionController;
using Elastos::Droid::Media::Session::IMediaControllerPlaybackInfo;
using Elastos::Droid::Media::Session::CParcelableVolumeInfo;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Binder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

CAR_INTERFACE_IMPL_2(CSessionController, Object, IISessionController, IBinder)

CAR_OBJECT_IMPL(CSessionController)

ECode CSessionController::constructor(
    /* [in] */ Handle64 host)
{
    mHost = (MediaSessionRecord*)host;
    return NOERROR;
}

ECode CSessionController::SendCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* args,
    /* [in] */ IResultReceiver* cb)
{
    return mHost->mSessionCb->SendCommand(command, args, cb);
}

ECode CSessionController::SendMediaButton(
    /* [in] */ IKeyEvent* mediaButton,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mHost->mSessionCb->SendMediaButton(mediaButton, 0, NULL, result);
}

ECode CSessionController::RegisterCallbackListener(
    /* [in] */ IISessionControllerCallback* cb)
{
    AutoLock lock(mHost->mLock);
    // If this session is already destroyed tell the caller and
    // don't add them.
    if (mHost->mDestroyed) {
        // try {
        cb->OnSessionDestroyed();
        // } catch (Exception e) {
        //     // ignored
        // }
        return NOERROR;
    }
    if (mHost->GetControllerCbIndexForCb(cb) == mHost->mControllerCallbacks.End()) {
        mHost->mControllerCallbacks.PushBack(cb);
        if (MediaSessionRecord::DEBUG) {
            Slogger::D(MediaSessionRecord::TAG, "registering controller callback %p", cb);
        }
    }
    return NOERROR;
}

ECode CSessionController::UnregisterCallbackListener(
    /* [in] */ IISessionControllerCallback* cb)
{
    AutoLock lock(mHost->mLock);
    List<AutoPtr<IISessionControllerCallback> >::Iterator it = mHost->GetControllerCbIndexForCb(cb);
    if (it != mHost->mControllerCallbacks.End()) {
        mHost->mControllerCallbacks.Erase(it);
    }
    if (MediaSessionRecord::DEBUG) {
        Slogger::D(MediaSessionRecord::TAG, "unregistering callback %p. index=", cb);
    }
    return NOERROR;
}

ECode CSessionController::GetPackageName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mPackageName;
    return NOERROR;
}

ECode CSessionController::GetTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mTag;
    return NOERROR;
}

ECode CSessionController::GetLaunchPendingIntent(
    /* [out] */ IPendingIntent** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mLaunchIntent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSessionController::GetFlags(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mFlags;
    return NOERROR;
}

ECode CSessionController::GetVolumeAttributes(
    /* [out] */ IParcelableVolumeInfo** result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mHost->mLock);
    Int32 type;
    Int32 max;
    Int32 current;
    if (mHost->mVolumeType == IMediaControllerPlaybackInfo::PLAYBACK_TYPE_REMOTE) {
        type = mHost->mVolumeControlType;
        max = mHost->mMaxVolume;
        current = mHost->mOptimisticVolume != -1 ? mHost->mOptimisticVolume : mHost->mCurrentVolume;
    }
    else {
        AutoPtr<IAudioAttributesHelper> helper;
        CAudioAttributesHelper::AcquireSingleton((IAudioAttributesHelper**)&helper);
        Int32 stream;
        helper->ToLegacyStreamType(mHost->mAudioAttrs, &stream);
        type = IVolumeProvider::VOLUME_CONTROL_ABSOLUTE;
        mHost->mAudioManager->GetStreamMaxVolume(stream, &max);
        mHost->mAudioManager->GetStreamVolume(stream, &current);
    }
    return CParcelableVolumeInfo::New(mHost->mVolumeType, mHost->mAudioAttrs, type, max, current, result);
}

ECode CSessionController::AdjustVolume(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags,
    /* [in] */ const String& packageName)
{
    Int32 uid = Binder::GetCallingUid();
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    mHost->AdjustVolume(direction, flags, packageName, uid, false);
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CSessionController::SetVolumeTo(
    /* [in] */ Int32 value,
    /* [in] */ Int32 flags,
    /* [in] */ const String& packageName)
{
    Int32 uid = Binder::GetCallingUid();
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    mHost->SetVolumeTo(value, flags, packageName, uid);
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CSessionController::Play()
{
    return mHost->mSessionCb->Play();
}

ECode CSessionController::PlayFromMediaId(
    /* [in] */ const String& mediaId,
    /* [in] */ IBundle* extras)
{
    return mHost->mSessionCb->PlayFromMediaId(mediaId, extras);
}

ECode CSessionController::PlayFromSearch(
    /* [in] */ const String& query,
    /* [in] */ IBundle* extras)
{
    return mHost->mSessionCb->PlayFromSearch(query, extras);
}

ECode CSessionController::SkipToQueueItem(
    /* [in] */ Int64 id)
{
    return mHost->mSessionCb->SkipToTrack(id);
}

ECode CSessionController::Pause()
{
    return mHost->mSessionCb->Pause();
}

ECode CSessionController::Stop()
{
    return mHost->mSessionCb->Stop();
}

ECode CSessionController::Next()
{
    return mHost->mSessionCb->Next();
}

ECode CSessionController::Previous()
{
    return mHost->mSessionCb->Previous();
}

ECode CSessionController::FastForward()
{
    return mHost->mSessionCb->FastForward();
}

ECode CSessionController::Rewind()
{
    return mHost->mSessionCb->Rewind();
}

ECode CSessionController::SeekTo(
    /* [in] */ Int64 pos)
{
    return mHost->mSessionCb->SeekTo(pos);
}

ECode CSessionController::SetRemoteControlClientBrowsedPlayer()
{
    assert(0);
    return NOERROR;
}

ECode CSessionController::SetRemoteControlClientPlayItem(
    /* [in] */ Int64 uid,
    /* [in] */ Int32 scope)
{
    assert(0);
    return NOERROR;
}

ECode CSessionController::GetRemoteControlClientNowPlayingEntries()
{
    assert(0);
    return NOERROR;
}

ECode CSessionController::Rate(
    /* [in] */ IRating* rating)
{
    return mHost->mSessionCb->Rate(rating);
}

ECode CSessionController::SendCustomAction(
    /* [in] */ const String& action,
    /* [in] */ IBundle* args)
{
    return mHost->mSessionCb->SendCustomAction(action, args);
}

ECode CSessionController::GetMetadata(
    /* [out] */ IMediaMetadata** result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mHost->mLock);
    *result = mHost->mMetadata;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSessionController::GetPlaybackState(
    /* [out] */ IPlaybackState** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->GetStateWithUpdatedPosition();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSessionController::GetQueue(
    /* [out] */ IParceledListSlice** result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mHost->mLock);
    *result = mHost->mQueue;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSessionController::GetQueueTitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mQueueTitle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSessionController::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mHost->mLock);
    *result = mHost->mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSessionController::GetRatingType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mRatingType;
    return NOERROR;
}

ECode CSessionController::IsTransportControlEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->IsTransportControlEnabled();
    return NOERROR;
}

ECode CSessionController::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
