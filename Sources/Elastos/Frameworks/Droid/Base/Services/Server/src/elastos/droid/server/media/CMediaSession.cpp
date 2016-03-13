
#include "elastos/droid/server/media/CMediaSession.h"
#include "elastos/droid/server/media/MediaSessionRecord.h"
#include "elastos/droid/server/media/MediaSessionService.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Media::CMediaMetadataBuilder;
using Elastos::Droid::Media::IMediaMetadataBuilder;
using Elastos::Droid::Media::Session::EIID_IISession;
using Elastos::Droid::Media::Session::IMediaSessionHelper;
using Elastos::Droid::Media::Session::CMediaSessionHelper;
using Elastos::Droid::Media::Session::IPlaybackState;
using Elastos::Droid::Media::Session::IMediaControllerPlaybackInfo;
using Elastos::Droid::Media::Session::IMediaSession;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

CAR_INTERFACE_IMPL_2(CMediaSession, Object, IISession, IBinder)

CAR_OBJECT_IMPL(CMediaSession)

ECode CMediaSession::constructor(
    /* [in] */ Handle64 host)
{
    mHost = (MediaSessionRecord*)host;
    return NOERROR;
}

ECode CMediaSession::Destroy()
{
    mHost->mService->DestroySession(mHost);
    return NOERROR;
}

ECode CMediaSession::SendEvent(
    /* [in] */ const String& event,
    /* [in] */ IBundle * data)
{
    AutoPtr<IBundle> b;
    if (data != NULL) {
        CBundle::New(data, (IBundle**)&b);
    }
    AutoPtr<ICharSequence> cs;
    CString::New(event, (ICharSequence**)&cs);
    mHost->mHandler->Post(MediaSessionRecord::MessageHandler::MSG_SEND_EVENT, IObject::Probe(cs), b);
    return NOERROR;
}

ECode CMediaSession::GetController(
    /* [out] */ IISessionController** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mController;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaSession::SetActive(
    /* [in] */ Boolean active)
{
    mHost->mIsActive = active;
    mHost->mService->UpdateSession(mHost);
    mHost->mHandler->Post(MediaSessionRecord::MessageHandler::MSG_UPDATE_SESSION_STATE);
    return NOERROR;
}

ECode CMediaSession::SetFlags(
    /* [in] */ Int32 flags)
{
    if ((flags & IMediaSession::FLAG_EXCLUSIVE_GLOBAL_PRIORITY) != 0) {
        Int32 pid = Binder::GetCallingPid();
        Int32 uid = Binder::GetCallingUid();
        FAIL_RETURN(mHost->mService->EnforcePhoneStatePermission(pid, uid))
    }
    mHost->mFlags = flags;
    mHost->mHandler->Post(MediaSessionRecord::MessageHandler::MSG_UPDATE_SESSION_STATE);
    return NOERROR;
}

ECode CMediaSession::SetMediaButtonReceiver(
    /* [in] */ IPendingIntent* pi)
{
    mHost->mMediaButtonReceiver = pi;
    return NOERROR;
}

ECode CMediaSession::SetLaunchPendingIntent(
    /* [in] */ IPendingIntent* pi)
{
    mHost->mLaunchIntent = pi;
    return NOERROR;
}

ECode CMediaSession::SetMetadata(
    /* [in] */ IMediaMetadata* metadata)
{
    {
        AutoLock lock(mHost->mLock);
        AutoPtr<IMediaMetadata> temp = metadata;
        if (temp == NULL) {
            AutoPtr<IMediaMetadataBuilder> builder;
            CMediaMetadataBuilder::New(metadata, (IMediaMetadataBuilder**)&builder);
            builder->Build((IMediaMetadata**)&temp);
        }
        // This is to guarantee that the underlying bundle is unparceled
        // before we set it to prevent concurrent reads from throwing an
        // exception
        if (temp != NULL) {
            Int32 size;
            temp->Size(&size);
        }
        mHost->mMetadata = temp;
    }
    mHost->mHandler->Post(MediaSessionRecord::MessageHandler::MSG_UPDATE_METADATA);
    return NOERROR;
}

ECode CMediaSession::SetPlaybackState(
    /* [in] */ IPlaybackState* state)
{
    Int32 oldState = 0;
    if (mHost->mPlaybackState != NULL) {
        mHost->mPlaybackState->GetState(&oldState);
    }
    Int32 newState = 0;
    if (state != NULL) {
        state->GetState(&newState);
    }
    AutoPtr<IMediaSessionHelper> helper;
    CMediaSessionHelper::AcquireSingleton((IMediaSessionHelper**)&helper);
    Boolean isActiveState;
    if ((helper->IsActiveState(oldState, &isActiveState), isActiveState)
            && newState == IPlaybackState::STATE_PAUSED) {
        mHost->mLastActiveTime = SystemClock::GetElapsedRealtime();
    }
    {
        AutoLock lock(mHost->mLock);
        mHost->mPlaybackState = state;
    }
    mHost->mService->OnSessionPlaystateChange(mHost, oldState, newState);
    mHost->mHandler->Post(MediaSessionRecord::MessageHandler::MSG_UPDATE_PLAYBACK_STATE);
    return NOERROR;
}

ECode CMediaSession::SetQueue(
    /* [in] */ IParceledListSlice* queue)
{
    {
        AutoLock lock(mHost->mLock);
        mHost->mQueue = queue;
    }
    mHost->mHandler->Post(MediaSessionRecord::MessageHandler::MSG_UPDATE_QUEUE);
    return NOERROR;
}

ECode CMediaSession::SetQueueTitle(
    /* [in] */ ICharSequence* title)
{
    mHost->mQueueTitle = title;
    mHost->mHandler->Post(MediaSessionRecord::MessageHandler::MSG_UPDATE_QUEUE_TITLE);
    return NOERROR;
}

ECode CMediaSession::SetExtras(
    /* [in] */ IBundle* extras)
{
    {
        AutoLock lock(mHost->mLock);
        mHost->mExtras = NULL;
        if (extras != NULL) {
            CBundle::New(extras, (IBundle**)&mHost->mExtras);
        }
    }
    mHost->mHandler->Post(MediaSessionRecord::MessageHandler::MSG_UPDATE_EXTRAS);
    return NOERROR;
}

ECode CMediaSession::SetRatingType(
    /* [in] */ Int32 type)
{
    mHost->mRatingType = type;
    return NOERROR;
}

ECode CMediaSession::PlayItemResponse(
    /* [in] */ Boolean success)
{
    assert(0);
    return NOERROR;
}

ECode CMediaSession::UpdateNowPlayingEntries(
    /* [in] */ ArrayOf<Int64>* playList)
{
    assert(0);
    return NOERROR;
}

ECode CMediaSession::UpdateFolderInfoBrowsedPlayer(
    /* [in] */ const String& stringUri)
{
    assert(0);
    return NOERROR;
}

ECode CMediaSession::UpdateNowPlayingContentChange()
{
    assert(0);
    return NOERROR;
}

ECode CMediaSession::SetCurrentVolume(
    /* [in] */ Int32 currentVolume)
{
    mHost->mCurrentVolume = currentVolume;
    mHost->mHandler->Post(MediaSessionRecord::MessageHandler::MSG_UPDATE_VOLUME);
    return NOERROR;
}

ECode CMediaSession::SetPlaybackToLocal(
    /* [in] */ IAudioAttributes* attributes)
{
    Boolean typeChanged;
    {
        AutoLock lock(mHost->mLock);
        typeChanged = mHost->mVolumeType == IMediaControllerPlaybackInfo::PLAYBACK_TYPE_REMOTE;
        mHost->mVolumeType = IMediaControllerPlaybackInfo::PLAYBACK_TYPE_LOCAL;
        if (attributes != NULL) {
            mHost->mAudioAttrs = attributes;
        }
        else {
            Slogger::E(MediaSessionRecord::TAG, "Received null audio attributes, using existing attributes");
        }
    }
    if (typeChanged) {
        mHost->mService->OnSessionPlaybackTypeChanged(mHost);
    }
    return NOERROR;
}

ECode CMediaSession::SetPlaybackToRemote(
    /* [in] */ Int32 control,
    /* [in] */ Int32 max)
{
    Boolean typeChanged;
    {
        AutoLock lock(mHost->mLock);
        typeChanged = mHost->mVolumeType == IMediaControllerPlaybackInfo::PLAYBACK_TYPE_LOCAL;
        mHost->mVolumeType = IMediaControllerPlaybackInfo::PLAYBACK_TYPE_REMOTE;
        mHost->mVolumeControlType = control;
        mHost->mMaxVolume = max;
    }
    if (typeChanged) {
        mHost->mService->OnSessionPlaybackTypeChanged(mHost);
    }
    return NOERROR;
}

ECode CMediaSession::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
