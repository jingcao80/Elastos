
#include "elastos/droid/server/media/MediaSessionRecord.h"
#include "elastos/droid/server/media/MediaSessionService.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Media::IAudioAttributesHelper;
using Elastos::Droid::Media::CAudioAttributesHelper;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Media::CAudioSystem;
using Elastos::Droid::Media::IVolumeProvider;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IMediaDescription;
using Elastos::Droid::Media::EIID_IAudioManagerInternal;
using Elastos::Droid::Media::Session::IMediaControllerPlaybackInfo;
using Elastos::Droid::Media::Session::IMediaSession;
using Elastos::Droid::Media::Session::IPlaybackStateBuilder;
using Elastos::Droid::Media::Session::CPlaybackStateBuilder;
using Elastos::Droid::Media::Session::IMediaSessionHelper;
using Elastos::Droid::Media::Session::CMediaSessionHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Server::LocalServices;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

//==============================================================================
//                  MediaSessionRecord::SessionCb
//==============================================================================

ECode MediaSessionRecord::SessionCb::SendMediaButton(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Int32 sequenceId,
    /* [in] */ IResultReceiver* cb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIntent> mediaButtonIntent;
    CIntent::New(IIntent::ACTION_MEDIA_BUTTON, (IIntent**)&mediaButtonIntent);
    mediaButtonIntent->PutExtra(IIntent::EXTRA_KEY_EVENT, IParcelable::Probe(keyEvent));
    // try {
    ECode ec = mCb->OnMediaButton(mediaButtonIntent, sequenceId, cb);
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in sendMediaRequest.0x%08x", ec);
        *result = FALSE;
        return ec;
    }
    *result = TRUE;
    return NOERROR;
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in sendMediaRequest.", e);
    // }
    // return false;
}

ECode MediaSessionRecord::SessionCb::SendCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* args,
    /* [in] */ IResultReceiver* cb)
{
    // try {
    ECode ec = mCb->OnCommand(command, args, cb);
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in sendCommand.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in sendCommand.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::SendCustomAction(
    /* [in] */ const String& action,
    /* [in] */ IBundle* args)
{
    // try {
    ECode ec = mCb->OnCustomAction(action, args);
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in sendCustomAction.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in sendCustomAction.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::Play()
{
    // try {
    ECode ec = mCb->OnPlay();
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in play.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in play.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::PlayFromMediaId(
    /* [in] */ const String& mediaId,
    /* [in] */ IBundle* extras)
{
    // try {
    ECode ec = mCb->OnPlayFromMediaId(mediaId, extras);
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in playUri.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in playUri.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::PlayFromSearch(
    /* [in] */ const String& query,
    /* [in] */ IBundle* extras)
{
    // try {
    ECode ec = mCb->OnPlayFromSearch(query, extras);
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in playUri.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in playFromSearch.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::SkipToTrack(
    /* [in] */ Int64 id)
{
    // try {
    ECode ec = mCb->OnSkipToTrack(id);
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in skipToTrack.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in skipToTrack", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::Pause()
{
    // try {
    ECode ec = mCb->OnPause();
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in pause.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in pause.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::Stop()
{
    // try {
    ECode ec = mCb->OnStop();
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in stop.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in stop.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::Next()
{
    // try {
    ECode ec = mCb->OnNext();
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in next.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in next.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::Previous()
{
    // try {
    ECode ec = mCb->OnPrevious();
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in previous.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in previous.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::FastForward()
{
    // try {
    ECode ec = mCb->OnFastForward();
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in fastForward.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in fastForward.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::Rewind()
{
    // try {
    ECode ec = mCb->OnRewind();
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in rewind.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in rewind.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::SeekTo(
    /* [in] */ Int64 pos)
{
    // try {
    ECode ec = mCb->OnSeekTo(pos);
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in seekTo.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in seekTo.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::Rate(
    /* [in] */ IRating* rating)
{
    // try {
    ECode ec = mCb->OnRate(rating);
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in rate.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in rate.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::AdjustVolume(
    /* [in] */ Int32 direction)
{
    // try {
    ECode ec = mCb->OnAdjustVolume(direction);
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in adjustVolume.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in adjustVolume.", e);
    // }
    return ec;
}

ECode MediaSessionRecord::SessionCb::SetVolumeTo(
    /* [in] */ Int32 value)
{
    // try {
    ECode ec = mCb->OnSetVolumeTo(value);
    if (FAILED(ec)) {
        Slogger::E(MediaSessionRecord::TAG, "Remote failure in adjustVolume.0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote failure in setVolumeTo.", e);
    // }
    return ec;
}


//==============================================================================
//                  MediaSessionRecord::MessageHandler
//==============================================================================

const Int32 MediaSessionRecord::MessageHandler::MSG_UPDATE_METADATA;
const Int32 MediaSessionRecord::MessageHandler::MSG_UPDATE_PLAYBACK_STATE;
const Int32 MediaSessionRecord::MessageHandler::MSG_UPDATE_QUEUE;
const Int32 MediaSessionRecord::MessageHandler::MSG_UPDATE_QUEUE_TITLE;
const Int32 MediaSessionRecord::MessageHandler::MSG_UPDATE_EXTRAS;
const Int32 MediaSessionRecord::MessageHandler::MSG_SEND_EVENT;
const Int32 MediaSessionRecord::MessageHandler::MSG_UPDATE_SESSION_STATE;
const Int32 MediaSessionRecord::MessageHandler::MSG_UPDATE_VOLUME;
const Int32 MediaSessionRecord::MessageHandler::MSG_DESTROYED;

ECode MediaSessionRecord::MessageHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_UPDATE_METADATA:
            mHost->PushMetadataUpdate();
            break;
        case MSG_UPDATE_PLAYBACK_STATE:
            mHost->PushPlaybackStateUpdate();
            break;
        case MSG_UPDATE_QUEUE:
            mHost->PushQueueUpdate();
            break;
        case MSG_UPDATE_QUEUE_TITLE:
            mHost->PushQueueTitleUpdate();
            break;
        case MSG_UPDATE_EXTRAS:
            mHost->PushExtrasUpdate();
            break;
        case MSG_SEND_EVENT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            AutoPtr<IBundle> data;
            msg->GetData((IBundle**)&data);
            mHost->PushEvent(str, data);
            break;
        }
        case MSG_UPDATE_SESSION_STATE:
            // TODO add session state
            break;
        case MSG_UPDATE_VOLUME:
            mHost->PushVolumeUpdate();
            break;
        case MSG_DESTROYED:
            mHost->PushSessionDestroyed();
    }
    return NOERROR;
}

void MediaSessionRecord::MessageHandler::Post(
    /* [in] */ Int32 what)
{
    Post(what, NULL);
}

void MediaSessionRecord::MessageHandler::Post(
    /* [in] */ Int32 what,
    /* [in] */ IObject* obj)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(what, obj, (IMessage**)&msg);
    msg->SendToTarget();
}

void MediaSessionRecord::MessageHandler::Post(
    /* [in] */ Int32 what,
    /* [in] */ IObject* obj,
    /* [in] */ IBundle* data)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(what, obj, (IMessage**)&msg);
    msg->SetData(data);
    msg->SendToTarget();
}


//==============================================================================
//                  MediaSessionRecord::ClearOptimisticVolumeRunnable
//==============================================================================

ECode MediaSessionRecord::ClearOptimisticVolumeRunnable::Run()
{
    Boolean needUpdate = (mHost->mOptimisticVolume != mHost->mCurrentVolume);
    mHost->mOptimisticVolume = -1;
    if (needUpdate) {
        mHost->PushVolumeUpdate();
    }
    return NOERROR;
}


//==============================================================================
//                  MediaSessionRecord
//==============================================================================

const String MediaSessionRecord::TAG("MediaSessionRecord");
const Boolean MediaSessionRecord::DEBUG;
const Int32 MediaSessionRecord::ACTIVE_BUFFER;
const Int32 MediaSessionRecord::OPTIMISTIC_VOLUME_TIMEOUT;

CAR_INTERFACE_IMPL(MediaSessionRecord, Object, IProxyDeathRecipient)

MediaSessionRecord::MediaSessionRecord(
    /* [in] */ Int32 ownerPid,
    /* [in] */ Int32 ownerUid,
    /* [in] */ Int32 userId,
    /* [in] */ const String& ownerPackageName,
    /* [in] */ IISessionCallback* cb,
    /* [in] */ const String& tag,
    /* [in] */ MediaSessionService* service,
    /* [in] */ IHandler* handler)
    : mOwnerPid(ownerPid)
    , mOwnerUid(ownerUid)
    , mUserId(userId)
    , mPackageName(ownerPackageName)
    , mTag(tag)
    , mFlags(0)
    , mRatingType(0)
    , mLastActiveTime(0)
    , mVolumeType(IMediaControllerPlaybackInfo::PLAYBACK_TYPE_LOCAL)
    , mVolumeControlType(IVolumeProvider::VOLUME_CONTROL_ABSOLUTE)
    , mMaxVolume(0)
    , mCurrentVolume(0)
    , mOptimisticVolume(-1)
    , mIsActive(FALSE)
    , mDestroyed(FALSE)
{
    mClearOptimisticVolumeRunnable = (IRunnable*)new ClearOptimisticVolumeRunnable(this);
    CSessionController::NewByFriend((Handle64)this, (CSessionController**)&mController);
    CMediaSession::NewByFriend((Handle64)this, (CMediaSession**)&mSession);
    mSessionCb = new SessionCb(cb);
    mService = service;
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    mHandler = new MessageHandler(looper, this);
    AutoPtr<IContext> ctx;
    service->GetContext((IContext**)&ctx);
    AutoPtr<IInterface> sysService;
    ctx->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&sysService);
    mAudioManager = IAudioManager::Probe(sysService);
    mAudioManagerInternal = IAudioManagerInternal::Probe(LocalServices::GetService(EIID_IAudioManagerInternal).Get());
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
    builder->SetUsage(IAudioAttributes::USAGE_MEDIA);
    builder->Build((IAudioAttributes**)&mAudioAttrs);
}

AutoPtr<IISession> MediaSessionRecord::GetSessionBinder()
{
    return mSession;
}

AutoPtr<IISessionController> MediaSessionRecord::GetControllerBinder()
{
    return mController;
}

String MediaSessionRecord::GetPackageName()
{
    return mPackageName;
}

String MediaSessionRecord::GetTag()
{
    return mTag;
}

AutoPtr<IPendingIntent> MediaSessionRecord::GetMediaButtonReceiver()
{
    return mMediaButtonReceiver;
}

Int64 MediaSessionRecord::GetFlags()
{
    return mFlags;
}

Boolean MediaSessionRecord::HasFlag(
    /* [in] */ Int32 flag)
{
    return (mFlags & flag) != 0;
}

Int32 MediaSessionRecord::GetUserId()
{
    return mUserId;
}

Boolean MediaSessionRecord::IsSystemPriority()
{
    return (mFlags & IMediaSession::FLAG_EXCLUSIVE_GLOBAL_PRIORITY) != 0;
}

void MediaSessionRecord::AdjustVolume(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean useSuggested)
{
    Int32 previousFlagPlaySound = flags & IAudioManager::FLAG_PLAY_SOUND;
    if (IsPlaybackActive(FALSE) || HasFlag(IMediaSession::FLAG_EXCLUSIVE_GLOBAL_PRIORITY)) {
        flags &= ~IAudioManager::FLAG_PLAY_SOUND;
    }
    if (direction > 1) {
        direction = 1;
    }
    else if (direction < -1) {
        direction = -1;
    }
    if (mVolumeType == IMediaControllerPlaybackInfo::PLAYBACK_TYPE_LOCAL) {
        AutoPtr<IAudioAttributesHelper> helper;
        CAudioAttributesHelper::AcquireSingleton((IAudioAttributesHelper**)&helper);
        Int32 stream;
        helper->ToLegacyStreamType(mAudioAttrs, &stream);
        if (useSuggested) {
            AutoPtr<IAudioSystem> audioSys;
            CAudioSystem::AcquireSingleton((IAudioSystem**)&audioSys);
            Boolean isStreamActive;
            if (audioSys->IsStreamActive(stream, 0, &isStreamActive), isStreamActive) {
                mAudioManagerInternal->AdjustSuggestedStreamVolumeForUid(stream, direction,
                        flags, packageName, uid);
            }
            else {
                flags |= previousFlagPlaySound;
                mAudioManagerInternal->AdjustSuggestedStreamVolumeForUid(
                        IAudioManager::USE_DEFAULT_STREAM_TYPE, direction, flags, packageName,
                        uid);
            }
        }
        else {
            mAudioManagerInternal->AdjustStreamVolumeForUid(stream, direction, flags,
                    packageName, uid);
        }
    }
    else {
        if (mVolumeControlType == IVolumeProvider::VOLUME_CONTROL_FIXED) {
            // Nothing to do, the volume cannot be changed
            return;
        }
        mSessionCb->AdjustVolume(direction);

        Int32 volumeBefore = (mOptimisticVolume < 0 ? mCurrentVolume : mOptimisticVolume);
        mOptimisticVolume = volumeBefore + direction;
        mOptimisticVolume = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(mOptimisticVolume, mMaxVolume));
        mHandler->RemoveCallbacks(mClearOptimisticVolumeRunnable);
        Boolean result;
        mHandler->PostDelayed(mClearOptimisticVolumeRunnable, OPTIMISTIC_VOLUME_TIMEOUT, &result);
        if (volumeBefore != mOptimisticVolume) {
            PushVolumeUpdate();
        }

        if (DEBUG) {
            Slogger::D(TAG, "Adjusted optimistic volume to %d max is %d", mOptimisticVolume, mMaxVolume);
        }
    }
}

void MediaSessionRecord::SetVolumeTo(
    /* [in] */ Int32 value,
    /* [in] */ Int32 flags,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    if (mVolumeType == IMediaControllerPlaybackInfo::PLAYBACK_TYPE_LOCAL) {
        AutoPtr<IAudioAttributesHelper> helper;
        CAudioAttributesHelper::AcquireSingleton((IAudioAttributesHelper**)&helper);
        Int32 stream;
        helper->ToLegacyStreamType(mAudioAttrs, &stream);
        mAudioManagerInternal->SetStreamVolumeForUid(stream, value, flags, packageName, uid);
    }
    else {
        if (mVolumeControlType != IVolumeProvider::VOLUME_CONTROL_ABSOLUTE) {
            // Nothing to do. The volume can't be set directly.
            return;
        }
        value = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(value, mMaxVolume));
        mSessionCb->SetVolumeTo(value);

        Int32 volumeBefore = (mOptimisticVolume < 0 ? mCurrentVolume : mOptimisticVolume);
        mOptimisticVolume = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(value, mMaxVolume));
        mHandler->RemoveCallbacks(mClearOptimisticVolumeRunnable);
        Boolean result;
        mHandler->PostDelayed(mClearOptimisticVolumeRunnable, OPTIMISTIC_VOLUME_TIMEOUT, &result);
        if (volumeBefore != mOptimisticVolume) {
            PushVolumeUpdate();
        }

        if (DEBUG) {
            Slogger::D(TAG, "Set optimistic volume to %d max is %d", mOptimisticVolume, mMaxVolume);
        }
    }
}

Boolean MediaSessionRecord::IsActive()
{
    return mIsActive && !mDestroyed;
}

Boolean MediaSessionRecord::IsPlaybackActive(
    /* [in] */ Boolean includeRecentlyActive)
{
    Int32 state = 0;
    if (mPlaybackState != NULL) {
        mPlaybackState->GetState(&state);
    }
    AutoPtr<IMediaSessionHelper> helper;
    CMediaSessionHelper::AcquireSingleton((IMediaSessionHelper**)&helper);
    Boolean isActiveState;
    if (helper->IsActiveState(state, &isActiveState), isActiveState) {
        return TRUE;
    }
    if (includeRecentlyActive && state == IPlaybackState::STATE_PAUSED) {
        Int64 inactiveTime = SystemClock::GetUptimeMillis() - mLastActiveTime;
        if (inactiveTime < (Int64)ACTIVE_BUFFER) {
            return TRUE;
        }
    }
    return FALSE;
}

Int32 MediaSessionRecord::GetPlaybackType()
{
    return mVolumeType;
}

AutoPtr<IAudioAttributes> MediaSessionRecord::GetAudioAttributes()
{
    return mAudioAttrs;
}

Int32 MediaSessionRecord::GetVolumeControl()
{
    return mVolumeControlType;
}

Int32 MediaSessionRecord::GetMaxVolume()
{
    return mMaxVolume;
}

Int32 MediaSessionRecord::GetCurrentVolume()
{
    return mCurrentVolume;
}

Int32 MediaSessionRecord::GetOptimisticVolume()
{
    return mOptimisticVolume;
}

Boolean MediaSessionRecord::IsTransportControlEnabled()
{
    return HasFlag(IMediaSession::FLAG_HANDLES_TRANSPORT_CONTROLS);
}

ECode MediaSessionRecord::ProxyDied()
{
    mService->SessionDied(this);
    return NOERROR;
}

void MediaSessionRecord::OnDestroy()
{
    {    AutoLock syncLock(mLock);
        if (mDestroyed) {
            return;
        }
        mDestroyed = TRUE;
        mHandler->Post(MessageHandler::MSG_DESTROYED);
    }
}

AutoPtr<IISessionCallback> MediaSessionRecord::GetCallback()
{
    return mSessionCb->mCb;
}

void MediaSessionRecord::SendMediaButton(
    /* [in] */ IKeyEvent* ke,
    /* [in] */ Int32 sequenceId,
    /* [in] */ IResultReceiver* cb)
{
    Boolean result;
    mSessionCb->SendMediaButton(ke, sequenceId, cb, &result);
}

ECode MediaSessionRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mPackageName + "/" + mTag;
    return NOERROR;
}

String MediaSessionRecord::GetShortMetadataString()
{
    Int32 fields = 0;
    if (mMetadata != NULL) {
        mMetadata->Size(&fields);
    }
    AutoPtr<IMediaDescription> description;
    if (mMetadata != NULL) {
        mMetadata->GetDescription((IMediaDescription**)&description);
    }
    String str;
    IObject::Probe(description)->ToString(&str);
    return String("size=") + StringUtils::ToString(fields) + ", description=" + str;
}

void MediaSessionRecord::PushPlaybackStateUpdate()
{
    {    AutoLock syncLock(mLock);
        if (mDestroyed) {
            return;
        }
        List<AutoPtr<IISessionControllerCallback> >::ReverseIterator rit = mControllerCallbacks.RBegin();
        while (rit != mControllerCallbacks.REnd()) {
            AutoPtr<IISessionControllerCallback> cb = *rit;
            // try {
            ECode ec = cb->OnPlaybackStateChanged(mPlaybackState);
            if (ec == (ECode)E_DEAD_OBJECT_EXCEPTION) {
                mControllerCallbacks.Erase((++rit).GetBase());
                Slogger::W(TAG, "Removed dead callback in pushPlaybackStateUpdate.0x%08x", ec);
                continue;
            }
            else if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "unexpected exception in pushPlaybackStateUpdate.0x%08x", ec);
            }
            ++rit;
            // } catch (DeadObjectException e) {
            //     mControllerCallbacks.remove(i);
            //     Log.w(TAG, "Removed dead callback in pushPlaybackStateUpdate.", e);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "unexpected exception in pushPlaybackStateUpdate.", e);
            // }
        }
    }
}

void MediaSessionRecord::PushMetadataUpdate()
{
    {    AutoLock syncLock(mLock);
        if (mDestroyed) {
            return;
        }
        List<AutoPtr<IISessionControllerCallback> >::ReverseIterator rit = mControllerCallbacks.RBegin();
        while (rit != mControllerCallbacks.REnd()) {
            AutoPtr<IISessionControllerCallback> cb = *rit;
            // try {
            ECode ec = cb->OnMetadataChanged(mMetadata);
            if (ec == (ECode)E_DEAD_OBJECT_EXCEPTION) {
                mControllerCallbacks.Erase((++rit).GetBase());
                Slogger::W(TAG, "Removed dead callback in pushMetadataUpdate.0x%08x", ec);
                continue;
            }
            else if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "unexpected exception in pushMetadataUpdate.0x%08x", ec);
            }
            ++rit;
            // } catch (DeadObjectException e) {
            //     Log.w(TAG, "Removing dead callback in pushMetadataUpdate. ", e);
            //     mControllerCallbacks.remove(i);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "unexpected exception in pushMetadataUpdate. ", e);
            // }
        }
    }
}

void MediaSessionRecord::PushQueueUpdate()
{
    {    AutoLock syncLock(mLock);
        if (mDestroyed) {
            return;
        }
        List<AutoPtr<IISessionControllerCallback> >::ReverseIterator rit = mControllerCallbacks.RBegin();
        while (rit != mControllerCallbacks.REnd()) {
            AutoPtr<IISessionControllerCallback> cb = *rit;
            // try {
            ECode ec = cb->OnQueueChanged(mQueue);
            if (ec == (ECode)E_DEAD_OBJECT_EXCEPTION) {
                mControllerCallbacks.Erase((++rit).GetBase());
                Slogger::W(TAG, "Removed dead callback in pushQueueUpdate.0x%08x", ec);
                continue;
            }
            else if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "unexpected exception in pushQueueUpdate.0x%08x", ec);
            }
            ++rit;
            // } catch (DeadObjectException e) {
            //     mControllerCallbacks.remove(i);
            //     Log.w(TAG, "Removed dead callback in pushQueueUpdate.", e);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "unexpected exception in pushQueueUpdate.", e);
            // }
        }
    }
}

void MediaSessionRecord::PushQueueTitleUpdate()
{
    {    AutoLock syncLock(mLock);
        if (mDestroyed) {
            return;
        }
        List<AutoPtr<IISessionControllerCallback> >::ReverseIterator rit = mControllerCallbacks.RBegin();
        while (rit != mControllerCallbacks.REnd()) {
            AutoPtr<IISessionControllerCallback> cb = *rit;
            // try {
            ECode ec = cb->OnQueueTitleChanged(mQueueTitle);
            if (ec == (ECode)E_DEAD_OBJECT_EXCEPTION) {
                mControllerCallbacks.Erase((++rit).GetBase());
                Slogger::W(TAG, "Removed dead callback in pushQueueTitleUpdate.0x%08x", ec);
                continue;
            }
            else if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "unexpected exception in pushQueueTitleUpdate.0x%08x", ec);
            }
            ++rit;
            // } catch (DeadObjectException e) {
            //     mControllerCallbacks.remove(i);
            //     Log.w(TAG, "Removed dead callback in pushQueueTitleUpdate.", e);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "unexpected exception in pushQueueTitleUpdate.", e);
            // }
        }
    }
}

void MediaSessionRecord::PushExtrasUpdate()
{
    {    AutoLock syncLock(mLock);
        if (mDestroyed) {
            return;
        }
        List<AutoPtr<IISessionControllerCallback> >::ReverseIterator rit = mControllerCallbacks.RBegin();
        while (rit != mControllerCallbacks.REnd()) {
            AutoPtr<IISessionControllerCallback> cb = *rit;
            // try {
            ECode ec = cb->OnExtrasChanged(mExtras);
            if (ec == (ECode)E_DEAD_OBJECT_EXCEPTION) {
                mControllerCallbacks.Erase((++rit).GetBase());
                Slogger::W(TAG, "Removed dead callback in pushExtrasUpdate.0x%08x", ec);
                continue;
            }
            else if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "unexpected exception in pushExtrasUpdate.0x%08x", ec);
            }
            ++rit;
            // } catch (DeadObjectException e) {
            //     mControllerCallbacks.remove(i);
            //     Log.w(TAG, "Removed dead callback in pushExtrasUpdate.", e);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "unexpected exception in pushExtrasUpdate.", e);
            // }
        }
    }
}

void MediaSessionRecord::PushVolumeUpdate()
{
    {    AutoLock syncLock(mLock);
        if (mDestroyed) {
            return;
        }
        AutoPtr<IParcelableVolumeInfo> info;
        mController->GetVolumeAttributes((IParcelableVolumeInfo**)&info);
        List<AutoPtr<IISessionControllerCallback> >::ReverseIterator rit = mControllerCallbacks.RBegin();
        while (rit != mControllerCallbacks.REnd()) {
            AutoPtr<IISessionControllerCallback> cb = *rit;
            // try {
            ECode ec = cb->OnVolumeInfoChanged(info);
            if (ec == (ECode)E_DEAD_OBJECT_EXCEPTION) {
                Slogger::W(TAG, "Removed dead callback in pushVolumeUpdate.0x%08x", ec);
                continue;
            }
            else if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "unexpected exception in pushVolumeUpdate.0x%08x", ec);
            }
            ++rit;
            // } catch (DeadObjectException e) {
            //     Log.w(TAG, "Removing dead callback in pushVolumeUpdate. ", e);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "Unexpected exception in pushVolumeUpdate. ", e);
            // }
        }
    }
}

void MediaSessionRecord::PushEvent(
    /* [in] */ const String& event,
    /* [in] */ IBundle* data)
{
    {    AutoLock syncLock(mLock);
        if (mDestroyed) {
            return;
        }
        List<AutoPtr<IISessionControllerCallback> >::ReverseIterator rit = mControllerCallbacks.RBegin();
        while (rit != mControllerCallbacks.REnd()) {
            AutoPtr<IISessionControllerCallback> cb = *rit;
            // try {
            ECode ec = cb->OnEvent(event, data);
            if (ec == (ECode)E_DEAD_OBJECT_EXCEPTION) {
                mControllerCallbacks.Erase((++rit).GetBase());
                Slogger::W(TAG, "Removed dead callback in pushEvent.0x%08x", ec);
                continue;
            }
            else if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "unexpected exception in pushEvent.0x%08x", ec);
            }
            ++rit;
            // } catch (DeadObjectException e) {
            //     Log.w(TAG, "Removing dead callback in pushEvent.", e);
            //     mControllerCallbacks.remove(i);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "unexpected exception in pushEvent.", e);
            // }
        }
    }
}

void MediaSessionRecord::PushSessionDestroyed()
{
    {    AutoLock syncLock(mLock);
        // This is the only method that may be (and can only be) called
        // after the session is destroyed.
        if (!mDestroyed) {
            return;
        }
        List<AutoPtr<IISessionControllerCallback> >::ReverseIterator rit = mControllerCallbacks.RBegin();
        while (rit != mControllerCallbacks.REnd()) {
            AutoPtr<IISessionControllerCallback> cb = *rit;
            // try {
            ECode ec = cb->OnSessionDestroyed();
            if (ec == (ECode)E_DEAD_OBJECT_EXCEPTION) {
                Slogger::W(TAG, "Removed dead callback in pushSessionDestroyed.0x%08x", ec);
                continue;
            }
            else if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "unexpected exception in pushSessionDestroyed.0x%08x", ec);
            }
            ++rit;
            // } catch (DeadObjectException e) {
            //     Log.w(TAG, "Removing dead callback in pushEvent.", e);
            //     mControllerCallbacks.remove(i);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "unexpected exception in pushEvent.", e);
            // }
        }
        // After notifying clear all listeners
        mControllerCallbacks.Clear();
    }
    mHandler = NULL;
}

AutoPtr<IPlaybackState> MediaSessionRecord::GetStateWithUpdatedPosition()
{
    AutoPtr<IPlaybackState> state;
    Int64 duration = -1;
    {    AutoLock syncLock(mLock);
        state = mPlaybackState;
        Boolean containsKey;
        if (mMetadata != NULL &&
                (mMetadata->ContainsKey(IMediaMetadata::METADATA_KEY_DURATION, &containsKey), containsKey)) {
            mMetadata->GetInt64(IMediaMetadata::METADATA_KEY_DURATION, &duration);
        }
    }
    AutoPtr<IPlaybackState> result;
    if (state != NULL) {
        Int32 ss;
        if ((state->GetState(&ss), ss == IPlaybackState::STATE_PLAYING)
                || ss == IPlaybackState::STATE_FAST_FORWARDING
                || ss == IPlaybackState::STATE_REWINDING) {
            Int64 updateTime;
            state->GetLastPositionUpdateTime(&updateTime);
            Int64 currentTime = SystemClock::GetElapsedRealtime();
            if (updateTime > 0) {
                Float speed;
                state->GetPlaybackSpeed(&speed);
                Int64 pos;
                state->GetPosition(&pos);
                Int64 position = (Int64)(speed * (currentTime - updateTime)) + pos;
                if (duration >= 0 && position > duration) {
                    position = duration;
                }
                else if (position < 0) {
                    position = 0;
                }
                AutoPtr<IPlaybackStateBuilder> builder;
                CPlaybackStateBuilder::New(state, (IPlaybackStateBuilder**)&builder);
                builder->SetState(ss, position, speed, currentTime);
                builder->Build((IPlaybackState**)&result);
            }
        }
    }
    return result == NULL ? state : result;
}

List<AutoPtr<IISessionControllerCallback> >::Iterator MediaSessionRecord::GetControllerCbIndexForCb(
    /* [in] */ IISessionControllerCallback* cb)
{
    AutoPtr<IBinder> binder = IBinder::Probe(cb);
    List<AutoPtr<IISessionControllerCallback> >::ReverseIterator rit = mControllerCallbacks.RBegin();
    for (; rit != mControllerCallbacks.REnd(); ++rit) {
        AutoPtr<IBinder> b = IBinder::Probe(*rit);
        Boolean equals;
        if (IObject::Probe(binder)->Equals(b, &equals), equals) {
            return (++rit).GetBase();
        }
    }
    return mControllerCallbacks.End();
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
