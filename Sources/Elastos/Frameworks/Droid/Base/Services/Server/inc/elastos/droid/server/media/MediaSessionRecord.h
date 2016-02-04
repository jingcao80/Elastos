/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.server.media;

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioManagerInternal;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Media::IMediaDescription;
using Elastos::Droid::Media::IMediaMetadata;
using Elastos::Droid::Media::IRating;
using Elastos::Droid::Media::IVolumeProvider;
using Elastos::Droid::Media::Isession.ISession;
using Elastos::Droid::Media::Isession.ISessionCallback;
using Elastos::Droid::Media::Isession.ISessionController;
using Elastos::Droid::Media::Isession.ISessionControllerCallback;
using Elastos::Droid::Media::Isession.MediaController;
using Elastos::Droid::Media::Isession.MediaController.PlaybackInfo;
using Elastos::Droid::Media::Isession.MediaSession;
using Elastos::Droid::Media::Isession.ParcelableVolumeInfo;
using Elastos::Droid::Media::Isession.PlaybackState;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IDeadObjectException;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::View::IKeyEvent;

using Elastos::Droid::Server::ILocalServices;

using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IUUID;

/**
 * This is the system implementation of a Session. Apps will interact with the
 * MediaSession wrapper class instead.
 */
public class MediaSessionRecord implements IBinder.DeathRecipient {
    private static const String TAG = "MediaSessionRecord";
    private static const Boolean DEBUG = FALSE;

    /**
     * The length of time a session will still be considered active after
     * pausing in ms.
     */
    private static const Int32 ACTIVE_BUFFER = 30000;

    /**
     * The amount of time we'll send an assumed volume after the last volume
     * command before reverting to the last reported volume.
     */
    private static const Int32 OPTIMISTIC_VOLUME_TIMEOUT = 1000;

    private final MessageHandler mHandler;

    private final Int32 mOwnerPid;
    private final Int32 mOwnerUid;
    private final Int32 mUserId;
    private final String mPackageName;
    private final String mTag;
    private final ControllerStub mController;
    private final SessionStub mSession;
    private final SessionCb mSessionCb;
    private final MediaSessionService mService;

    private final Object mLock = new Object();
    private final ArrayList<ISessionControllerCallback> mControllerCallbacks =
            new ArrayList<ISessionControllerCallback>();

    private Int64 mFlags;
    private PendingIntent mMediaButtonReceiver;
    private PendingIntent mLaunchIntent;

    // TransportPerformer fields

    private Bundle mExtras;
    private MediaMetadata mMetadata;
    private PlaybackState mPlaybackState;
    private ParceledListSlice mQueue;
    private CharSequence mQueueTitle;
    private Int32 mRatingType;
    private Int64 mLastActiveTime;
    // End TransportPerformer fields

    // Volume handling fields
    private AudioAttributes mAudioAttrs;
    private AudioManager mAudioManager;
    private AudioManagerInternal mAudioManagerInternal;
    private Int32 mVolumeType = PlaybackInfo.PLAYBACK_TYPE_LOCAL;
    private Int32 mVolumeControlType = VolumeProvider.VOLUME_CONTROL_ABSOLUTE;
    private Int32 mMaxVolume = 0;
    private Int32 mCurrentVolume = 0;
    private Int32 mOptimisticVolume = -1;
    // End volume handling fields

    private Boolean mIsActive = FALSE;
    private Boolean mDestroyed = FALSE;

    public MediaSessionRecord(Int32 ownerPid, Int32 ownerUid, Int32 userId, String ownerPackageName,
            ISessionCallback cb, String tag, MediaSessionService service, Handler handler) {
        mOwnerPid = ownerPid;
        mOwnerUid = ownerUid;
        mUserId = userId;
        mPackageName = ownerPackageName;
        mTag = tag;
        mController = new ControllerStub();
        mSession = new SessionStub();
        mSessionCb = new SessionCb(cb);
        mService = service;
        mHandler = new MessageHandler(handler->GetLooper());
        mAudioManager = (AudioManager) service->GetContext()->GetSystemService(Context.AUDIO_SERVICE);
        mAudioManagerInternal = LocalServices->GetService(AudioManagerInternal.class);
        mAudioAttrs = new AudioAttributes->Builder()->SetUsage(AudioAttributes.USAGE_MEDIA).Build();
    }

    /**
     * Get the binder for the {@link MediaSession}.
     *
     * @return The session binder apps talk to.
     */
    public ISession GetSessionBinder() {
        return mSession;
    }

    /**
     * Get the binder for the {@link MediaController}.
     *
     * @return The controller binder apps talk to.
     */
    public ISessionController GetControllerBinder() {
        return mController;
    }

    /**
     * Get the info for this session.
     *
     * @return Info that identifies this session.
     */
    public String GetPackageName() {
        return mPackageName;
    }

    /**
     * Get the tag for the session.
     *
     * @return The session's tag.
     */
    public String GetTag() {
        return mTag;
    }

    /**
     * Get the intent the app set for their media button receiver.
     *
     * @return The pending intent set by the app or NULL.
     */
    public PendingIntent GetMediaButtonReceiver() {
        return mMediaButtonReceiver;
    }

    /**
     * Get this session's flags.
     *
     * @return The flags for this session.
     */
    public Int64 GetFlags() {
        return mFlags;
    }

    /**
     * Check if this session has the specified flag.
     *
     * @param flag The flag to check.
     * @return True if this session has that flag set, FALSE otherwise.
     */
    public Boolean HasFlag(Int32 flag) {
        return (mFlags & flag) != 0;
    }

    /**
     * Get the user id this session was created for.
     *
     * @return The user id for this session.
     */
    public Int32 GetUserId() {
        return mUserId;
    }

    /**
     * Check if this session has system priorty and should receive media buttons
     * before any other sessions.
     *
     * @return True if this is a system priority session, FALSE otherwise
     */
    public Boolean IsSystemPriority() {
        return (mFlags & MediaSession.FLAG_EXCLUSIVE_GLOBAL_PRIORITY) != 0;
    }

    /**
     * Send a volume adjustment to the session owner. Direction must be one of
     * {@link AudioManager#ADJUST_LOWER}, {@link AudioManager#ADJUST_RAISE},
     * {@link AudioManager#ADJUST_SAME}.
     *
     * @param direction The direction to adjust volume in.
     * @param flags Any of the flags from {@link AudioManager}.
     * @param packageName The package that made the original volume request.
     * @param uid The uid that made the original volume request.
     * @param useSuggested True to use adjustSuggestedStreamVolume instead of
     *            adjustStreamVolume.
     */
    CARAPI AdjustVolume(Int32 direction, Int32 flags, String packageName, Int32 uid,
            Boolean useSuggested) {
        Int32 previousFlagPlaySound = flags & AudioManager.FLAG_PLAY_SOUND;
        if (IsPlaybackActive(FALSE) || HasFlag(MediaSession.FLAG_EXCLUSIVE_GLOBAL_PRIORITY)) {
            flags &= ~AudioManager.FLAG_PLAY_SOUND;
        }
        if (direction > 1) {
            direction = 1;
        } else if (direction < -1) {
            direction = -1;
        }
        if (mVolumeType == PlaybackInfo.PLAYBACK_TYPE_LOCAL) {
            Int32 stream = AudioAttributes->ToLegacyStreamType(mAudioAttrs);
            if (useSuggested) {
                if (AudioSystem->IsStreamActive(stream, 0)) {
                    mAudioManagerInternal->AdjustSuggestedStreamVolumeForUid(stream, direction,
                            flags, packageName, uid);
                } else {
                    flags |= previousFlagPlaySound;
                    mAudioManagerInternal->AdjustSuggestedStreamVolumeForUid(
                            AudioManager.USE_DEFAULT_STREAM_TYPE, direction, flags, packageName,
                            uid);
                }
            } else {
                mAudioManagerInternal->AdjustStreamVolumeForUid(stream, direction, flags,
                        packageName, uid);
            }
        } else {
            if (mVolumeControlType == VolumeProvider.VOLUME_CONTROL_FIXED) {
                // Nothing to do, the volume cannot be changed
                return;
            }
            mSessionCb->AdjustVolume(direction);

            Int32 volumeBefore = (mOptimisticVolume < 0 ? mCurrentVolume : mOptimisticVolume);
            mOptimisticVolume = volumeBefore + direction;
            mOptimisticVolume = Math->Max(0, Math->Min(mOptimisticVolume, mMaxVolume));
            mHandler->RemoveCallbacks(mClearOptimisticVolumeRunnable);
            mHandler->PostDelayed(mClearOptimisticVolumeRunnable, OPTIMISTIC_VOLUME_TIMEOUT);
            if (volumeBefore != mOptimisticVolume) {
                PushVolumeUpdate();
            }

            if (DEBUG) {
                Logger::D(TAG, "Adjusted optimistic volume to " + mOptimisticVolume + " max is "
                        + mMaxVolume);
            }
        }
    }

    CARAPI SetVolumeTo(Int32 value, Int32 flags, String packageName, Int32 uid) {
        if (mVolumeType == PlaybackInfo.PLAYBACK_TYPE_LOCAL) {
            Int32 stream = AudioAttributes->ToLegacyStreamType(mAudioAttrs);
            mAudioManagerInternal->SetStreamVolumeForUid(stream, value, flags, packageName, uid);
        } else {
            if (mVolumeControlType != VolumeProvider.VOLUME_CONTROL_ABSOLUTE) {
                // Nothing to do. The volume can't be set directly.
                return;
            }
            value = Math->Max(0, Math->Min(value, mMaxVolume));
            mSessionCb->SetVolumeTo(value);

            Int32 volumeBefore = (mOptimisticVolume < 0 ? mCurrentVolume : mOptimisticVolume);
            mOptimisticVolume = Math->Max(0, Math->Min(value, mMaxVolume));
            mHandler->RemoveCallbacks(mClearOptimisticVolumeRunnable);
            mHandler->PostDelayed(mClearOptimisticVolumeRunnable, OPTIMISTIC_VOLUME_TIMEOUT);
            if (volumeBefore != mOptimisticVolume) {
                PushVolumeUpdate();
            }

            if (DEBUG) {
                Logger::D(TAG, "Set optimistic volume to " + mOptimisticVolume + " max is "
                        + mMaxVolume);
            }
        }
    }

    /**
     * Check if this session has been set to active by the app.
     *
     * @return True if the session is active, FALSE otherwise.
     */
    public Boolean IsActive() {
        return mIsActive && !mDestroyed;
    }

    /**
     * Check if the session is currently performing playback. This will also
     * return TRUE if the session was recently paused.
     *
     * @param includeRecentlyActive True if playback that was recently paused
     *            should count, FALSE if it shouldn't.
     * @return True if the session is performing playback, FALSE otherwise.
     */
    public Boolean IsPlaybackActive(Boolean includeRecentlyActive) {
        Int32 state = mPlaybackState == NULL ? 0 : mPlaybackState->GetState();
        if (MediaSession->IsActiveState(state)) {
            return TRUE;
        }
        if (includeRecentlyActive && state == mPlaybackState.STATE_PAUSED) {
            Int64 inactiveTime = SystemClock->UptimeMillis() - mLastActiveTime;
            if (inactiveTime < ACTIVE_BUFFER) {
                return TRUE;
            }
        }
        return FALSE;
    }

    /**
     * Get the type of playback, either local or remote.
     *
     * @return The current type of playback.
     */
    public Int32 GetPlaybackType() {
        return mVolumeType;
    }

    /**
     * Get the local audio stream being used. Only valid if playback type is
     * local.
     *
     * @return The audio stream the session is using.
     */
    public AudioAttributes GetAudioAttributes() {
        return mAudioAttrs;
    }

    /**
     * Get the type of volume control. Only valid if playback type is remote.
     *
     * @return The volume control type being used.
     */
    public Int32 GetVolumeControl() {
        return mVolumeControlType;
    }

    /**
     * Get the max volume that can be set. Only valid if playback type is
     * remote.
     *
     * @return The max volume that can be set.
     */
    public Int32 GetMaxVolume() {
        return mMaxVolume;
    }

    /**
     * Get the current volume for this session. Only valid if playback type is
     * remote.
     *
     * @return The current volume of the remote playback.
     */
    public Int32 GetCurrentVolume() {
        return mCurrentVolume;
    }

    /**
     * Get the volume we'd like it to be set to. This is only valid for a short
     * while after a call to adjust or set volume.
     *
     * @return The current optimistic volume or -1.
     */
    public Int32 GetOptimisticVolume() {
        return mOptimisticVolume;
    }

    public Boolean IsTransportControlEnabled() {
        return HasFlag(MediaSession.FLAG_HANDLES_TRANSPORT_CONTROLS);
    }

    //@Override
    CARAPI BinderDied() {
        mService->SessionDied(this);
    }

    /**
     * Finish cleaning up this session, including disconnecting if connected and
     * removing the death observer from the callback binder.
     */
    CARAPI OnDestroy() {
        synchronized(mLock) {
            if (mDestroyed) {
                return;
            }
            mDestroyed = TRUE;
            mHandler->Post(MessageHandler.MSG_DESTROYED);
        }
    }

    public ISessionCallback GetCallback() {
        return mSessionCb.mCb;
    }

    CARAPI SendMediaButton(KeyEvent ke, Int32 sequenceId, ResultReceiver cb) {
        mSessionCb->SendMediaButton(ke, sequenceId, cb);
    }

    CARAPI Dump(PrintWriter pw, String prefix) {
        pw->Println(prefix + mTag + " " + this);

        final String indent = prefix + "  ";
        pw->Println(indent + "ownerPid=" + mOwnerPid + ", ownerUid=" + mOwnerUid
                + ", userId=" + mUserId);
        pw->Println(indent + "package=" + mPackageName);
        pw->Println(indent + "launchIntent=" + mLaunchIntent);
        pw->Println(indent + "mediaButtonReceiver=" + mMediaButtonReceiver);
        pw->Println(indent + "active=" + mIsActive);
        pw->Println(indent + "flags=" + mFlags);
        pw->Println(indent + "rating type=" + mRatingType);
        pw->Println(indent + "controllers: " + mControllerCallbacks->Size());
        pw->Println(indent + "state=" + (mPlaybackState == NULL ? NULL : mPlaybackState->ToString()));
        pw->Println(indent + "audioAttrs=" + mAudioAttrs);
        pw->Println(indent + "volumeType=" + mVolumeType + ", controlType=" + mVolumeControlType
                + ", max=" + mMaxVolume + ", current=" + mCurrentVolume);
        pw->Println(indent + "metadata:" + GetShortMetadataString());
        pw->Println(indent + "queueTitle=" + mQueueTitle + ", size="
                + (mQueue == NULL ? 0 : mQueue->GetList()->Size()));
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return mPackageName + "/" + mTag;
    }

    private String GetShortMetadataString() {
        Int32 fields = mMetadata == NULL ? 0 : mMetadata->Size();
        MediaDescription description = mMetadata == NULL ? NULL : mMetadata
                .GetDescription();
        return "size=" + fields + ", description=" + description;
    }

    private void PushPlaybackStateUpdate() {
        synchronized(mLock) {
            if (mDestroyed) {
                return;
            }
            for (Int32 i = mControllerCallbacks->Size() - 1; i >= 0; i--) {
                ISessionControllerCallback cb = mControllerCallbacks->Get(i);
                try {
                    cb->OnPlaybackStateChanged(mPlaybackState);
                } catch (DeadObjectException e) {
                    mControllerCallbacks->Remove(i);
                    Logger::W(TAG, "Removed dead callback in pushPlaybackStateUpdate.", e);
                } catch (RemoteException e) {
                    Logger::W(TAG, "unexpected exception in pushPlaybackStateUpdate.", e);
                }
            }
        }
    }

    private void PushMetadataUpdate() {
        synchronized(mLock) {
            if (mDestroyed) {
                return;
            }
            for (Int32 i = mControllerCallbacks->Size() - 1; i >= 0; i--) {
                ISessionControllerCallback cb = mControllerCallbacks->Get(i);
                try {
                    cb->OnMetadataChanged(mMetadata);
                } catch (DeadObjectException e) {
                    Logger::W(TAG, "Removing dead callback in pushMetadataUpdate. ", e);
                    mControllerCallbacks->Remove(i);
                } catch (RemoteException e) {
                    Logger::W(TAG, "unexpected exception in pushMetadataUpdate. ", e);
                }
            }
        }
    }

    private void PushQueueUpdate() {
        synchronized(mLock) {
            if (mDestroyed) {
                return;
            }
            for (Int32 i = mControllerCallbacks->Size() - 1; i >= 0; i--) {
                ISessionControllerCallback cb = mControllerCallbacks->Get(i);
                try {
                    cb->OnQueueChanged(mQueue);
                } catch (DeadObjectException e) {
                    mControllerCallbacks->Remove(i);
                    Logger::W(TAG, "Removed dead callback in pushQueueUpdate.", e);
                } catch (RemoteException e) {
                    Logger::W(TAG, "unexpected exception in pushQueueUpdate.", e);
                }
            }
        }
    }

    private void PushQueueTitleUpdate() {
        synchronized(mLock) {
            if (mDestroyed) {
                return;
            }
            for (Int32 i = mControllerCallbacks->Size() - 1; i >= 0; i--) {
                ISessionControllerCallback cb = mControllerCallbacks->Get(i);
                try {
                    cb->OnQueueTitleChanged(mQueueTitle);
                } catch (DeadObjectException e) {
                    mControllerCallbacks->Remove(i);
                    Logger::W(TAG, "Removed dead callback in pushQueueTitleUpdate.", e);
                } catch (RemoteException e) {
                    Logger::W(TAG, "unexpected exception in pushQueueTitleUpdate.", e);
                }
            }
        }
    }

    private void PushExtrasUpdate() {
        synchronized(mLock) {
            if (mDestroyed) {
                return;
            }
            for (Int32 i = mControllerCallbacks->Size() - 1; i >= 0; i--) {
                ISessionControllerCallback cb = mControllerCallbacks->Get(i);
                try {
                    cb->OnExtrasChanged(mExtras);
                } catch (DeadObjectException e) {
                    mControllerCallbacks->Remove(i);
                    Logger::W(TAG, "Removed dead callback in pushExtrasUpdate.", e);
                } catch (RemoteException e) {
                    Logger::W(TAG, "unexpected exception in pushExtrasUpdate.", e);
                }
            }
        }
    }

    private void PushVolumeUpdate() {
        synchronized(mLock) {
            if (mDestroyed) {
                return;
            }
            ParcelableVolumeInfo info = mController->GetVolumeAttributes();
            for (Int32 i = mControllerCallbacks->Size() - 1; i >= 0; i--) {
                ISessionControllerCallback cb = mControllerCallbacks->Get(i);
                try {
                    cb->OnVolumeInfoChanged(info);
                } catch (DeadObjectException e) {
                    Logger::W(TAG, "Removing dead callback in pushVolumeUpdate. ", e);
                } catch (RemoteException e) {
                    Logger::W(TAG, "Unexpected exception in pushVolumeUpdate. ", e);
                }
            }
        }
    }

    private void PushEvent(String event, Bundle data) {
        synchronized(mLock) {
            if (mDestroyed) {
                return;
            }
            for (Int32 i = mControllerCallbacks->Size() - 1; i >= 0; i--) {
                ISessionControllerCallback cb = mControllerCallbacks->Get(i);
                try {
                    cb->OnEvent(event, data);
                } catch (DeadObjectException e) {
                    Logger::W(TAG, "Removing dead callback in pushEvent.", e);
                    mControllerCallbacks->Remove(i);
                } catch (RemoteException e) {
                    Logger::W(TAG, "unexpected exception in pushEvent.", e);
                }
            }
        }
    }

    private void PushSessionDestroyed() {
        synchronized(mLock) {
            // This is the only method that may be (and can only be) called
            // after the session is destroyed.
            if (!mDestroyed) {
                return;
            }
            for (Int32 i = mControllerCallbacks->Size() - 1; i >= 0; i--) {
                ISessionControllerCallback cb = mControllerCallbacks->Get(i);
                try {
                    cb->OnSessionDestroyed();
                } catch (DeadObjectException e) {
                    Logger::W(TAG, "Removing dead callback in pushEvent.", e);
                    mControllerCallbacks->Remove(i);
                } catch (RemoteException e) {
                    Logger::W(TAG, "unexpected exception in pushEvent.", e);
                }
            }
            // After notifying clear all listeners
            mControllerCallbacks->Clear();
        }
    }

    private PlaybackState GetStateWithUpdatedPosition() {
        PlaybackState state;
        Int64 duration = -1;
        synchronized(mLock) {
            state = mPlaybackState;
            if (mMetadata != NULL && mMetadata->ContainsKey(MediaMetadata.METADATA_KEY_DURATION)) {
                duration = mMetadata->GetLong(MediaMetadata.METADATA_KEY_DURATION);
            }
        }
        PlaybackState result = NULL;
        if (state != NULL) {
            if (state->GetState() == PlaybackState.STATE_PLAYING
                    || state->GetState() == PlaybackState.STATE_FAST_FORWARDING
                    || state->GetState() == PlaybackState.STATE_REWINDING) {
                Int64 updateTime = state->GetLastPositionUpdateTime();
                Int64 currentTime = SystemClock->ElapsedRealtime();
                if (updateTime > 0) {
                    Int64 position = (Int64) (state->GetPlaybackSpeed()
                            * (currentTime - updateTime)) + state->GetPosition();
                    if (duration >= 0 && position > duration) {
                        position = duration;
                    } else if (position < 0) {
                        position = 0;
                    }
                    PlaybackState.Builder builder = new PlaybackState->Builder(state);
                    builder->SetState(state->GetState(), position, state->GetPlaybackSpeed(),
                            currentTime);
                    result = builder->Build();
                }
            }
        }
        return result == NULL ? state : result;
    }

    private Int32 GetControllerCbIndexForCb(ISessionControllerCallback cb) {
        IBinder binder = cb->AsBinder();
        for (Int32 i = mControllerCallbacks->Size() - 1; i >= 0; i--) {
            if (binder->Equals(mControllerCallbacks->Get(i).AsBinder())) {
                return i;
            }
        }
        return -1;
    }

    private final Runnable mClearOptimisticVolumeRunnable = new Runnable() {
        //@Override
        CARAPI Run() {
            Boolean needUpdate = (mOptimisticVolume != mCurrentVolume);
            mOptimisticVolume = -1;
            if (needUpdate) {
                PushVolumeUpdate();
            }
        }
    };

    private final class SessionStub extends ISession.Stub {
        //@Override
        CARAPI Destroy() {
            mService->DestroySession(MediaSessionRecord.this);
        }

        //@Override
        CARAPI SendEvent(String event, Bundle data) {
            mHandler->Post(MessageHandler.MSG_SEND_EVENT, event,
                    data == NULL ? NULL : new Bundle(data));
        }

        //@Override
        public ISessionController GetController() {
            return mController;
        }

        //@Override
        CARAPI SetActive(Boolean active) {
            mIsActive = active;
            mService->UpdateSession(MediaSessionRecord.this);
            mHandler->Post(MessageHandler.MSG_UPDATE_SESSION_STATE);
        }

        //@Override
        CARAPI SetFlags(Int32 flags) {
            if ((flags & MediaSession.FLAG_EXCLUSIVE_GLOBAL_PRIORITY) != 0) {
                Int32 pid = GetCallingPid();
                Int32 uid = GetCallingUid();
                mService->EnforcePhoneStatePermission(pid, uid);
            }
            mFlags = flags;
            mHandler->Post(MessageHandler.MSG_UPDATE_SESSION_STATE);
        }

        //@Override
        CARAPI SetMediaButtonReceiver(PendingIntent pi) {
            mMediaButtonReceiver = pi;
        }

        //@Override
        CARAPI SetLaunchPendingIntent(PendingIntent pi) {
            mLaunchIntent = pi;
        }

        //@Override
        CARAPI SetMetadata(MediaMetadata metadata) {
            synchronized(mLock) {
                MediaMetadata temp = metadata == NULL ? NULL : new MediaMetadata->Builder(metadata)
                        .Build();
                // This is to guarantee that the underlying bundle is unparceled
                // before we set it to prevent concurrent reads from throwing an
                // exception
                if (temp != NULL) {
                    temp->Size();
                }
                mMetadata = temp;
            }
            mHandler->Post(MessageHandler.MSG_UPDATE_METADATA);
        }

        //@Override
        CARAPI SetPlaybackState(PlaybackState state) {
            Int32 oldState = mPlaybackState == NULL ? 0 : mPlaybackState->GetState();
            Int32 newState = state == NULL ? 0 : state->GetState();
            if (MediaSession->IsActiveState(oldState) && newState == PlaybackState.STATE_PAUSED) {
                mLastActiveTime = SystemClock->ElapsedRealtime();
            }
            synchronized(mLock) {
                mPlaybackState = state;
            }
            mService->OnSessionPlaystateChange(MediaSessionRecord.this, oldState, newState);
            mHandler->Post(MessageHandler.MSG_UPDATE_PLAYBACK_STATE);
        }

        //@Override
        CARAPI SetQueue(ParceledListSlice queue) {
            synchronized(mLock) {
                mQueue = queue;
            }
            mHandler->Post(MessageHandler.MSG_UPDATE_QUEUE);
        }

        //@Override
        CARAPI SetQueueTitle(CharSequence title) {
            mQueueTitle = title;
            mHandler->Post(MessageHandler.MSG_UPDATE_QUEUE_TITLE);
        }

        //@Override
        CARAPI SetExtras(Bundle extras) {
            synchronized(mLock) {
                mExtras = extras == NULL ? NULL : new Bundle(extras);
            }
            mHandler->Post(MessageHandler.MSG_UPDATE_EXTRAS);
        }

        //@Override
        CARAPI SetRatingType(Int32 type) {
            mRatingType = type;
        }

        //@Override
        CARAPI SetCurrentVolume(Int32 volume) {
            mCurrentVolume = volume;
            mHandler->Post(MessageHandler.MSG_UPDATE_VOLUME);
        }

        //@Override
        CARAPI SetPlaybackToLocal(AudioAttributes attributes) {
            Boolean typeChanged;
            synchronized(mLock) {
                typeChanged = mVolumeType == PlaybackInfo.PLAYBACK_TYPE_REMOTE;
                mVolumeType = PlaybackInfo.PLAYBACK_TYPE_LOCAL;
                if (attributes != NULL) {
                    mAudioAttrs = attributes;
                } else {
                    Logger::E(TAG, "Received NULL audio attributes, using existing attributes");
                }
            }
            if (typeChanged) {
                mService->OnSessionPlaybackTypeChanged(MediaSessionRecord.this);
            }
        }

        //@Override
        CARAPI SetPlaybackToRemote(Int32 control, Int32 max) {
            Boolean typeChanged;
            synchronized(mLock) {
                typeChanged = mVolumeType == PlaybackInfo.PLAYBACK_TYPE_LOCAL;
                mVolumeType = PlaybackInfo.PLAYBACK_TYPE_REMOTE;
                mVolumeControlType = control;
                mMaxVolume = max;
            }
            if (typeChanged) {
                mService->OnSessionPlaybackTypeChanged(MediaSessionRecord.this);
            }
        }
    }

    class SessionCb {
        private final ISessionCallback mCb;

        public SessionCb(ISessionCallback cb) {
            mCb = cb;
        }

        public Boolean SendMediaButton(KeyEvent keyEvent, Int32 sequenceId, ResultReceiver cb) {
            Intent mediaButtonIntent = new Intent(IIntent::ACTION_MEDIA_BUTTON);
            mediaButtonIntent->PutExtra(Intent.EXTRA_KEY_EVENT, keyEvent);
            try {
                mCb->OnMediaButton(mediaButtonIntent, sequenceId, cb);
                return TRUE;
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in sendMediaRequest.", e);
            }
            return FALSE;
        }

        CARAPI SendCommand(String command, Bundle args, ResultReceiver cb) {
            try {
                mCb->OnCommand(command, args, cb);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in sendCommand.", e);
            }
        }

        CARAPI SendCustomAction(String action, Bundle args) {
            try {
                mCb->OnCustomAction(action, args);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in sendCustomAction.", e);
            }
        }

        CARAPI Play() {
            try {
                mCb->OnPlay();
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in play.", e);
            }
        }

        CARAPI PlayFromMediaId(String mediaId, Bundle extras) {
            try {
                mCb->OnPlayFromMediaId(mediaId, extras);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in playUri.", e);
            }
        }

        CARAPI PlayFromSearch(String query, Bundle extras) {
            try {
                mCb->OnPlayFromSearch(query, extras);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in playFromSearch.", e);
            }
        }

        CARAPI SkipToTrack(Int64 id) {
            try {
                mCb->OnSkipToTrack(id);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in skipToTrack", e);
            }
        }

        CARAPI Pause() {
            try {
                mCb->OnPause();
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in pause.", e);
            }
        }

        CARAPI Stop() {
            try {
                mCb->OnStop();
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in stop.", e);
            }
        }

        CARAPI Next() {
            try {
                mCb->OnNext();
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in next.", e);
            }
        }

        CARAPI Previous() {
            try {
                mCb->OnPrevious();
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in previous.", e);
            }
        }

        CARAPI FastForward() {
            try {
                mCb->OnFastForward();
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in fastForward.", e);
            }
        }

        CARAPI Rewind() {
            try {
                mCb->OnRewind();
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in rewind.", e);
            }
        }

        CARAPI SeekTo(Int64 pos) {
            try {
                mCb->OnSeekTo(pos);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in seekTo.", e);
            }
        }

        CARAPI Rate(Rating rating) {
            try {
                mCb->OnRate(rating);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in rate.", e);
            }
        }

        CARAPI AdjustVolume(Int32 direction) {
            try {
                mCb->OnAdjustVolume(direction);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in adjustVolume.", e);
            }
        }

        CARAPI SetVolumeTo(Int32 value) {
            try {
                mCb->OnSetVolumeTo(value);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Remote failure in setVolumeTo.", e);
            }
        }
    }

    class ControllerStub extends ISessionController.Stub {
        //@Override
        CARAPI SendCommand(String command, Bundle args, ResultReceiver cb)
                throws RemoteException {
            mSessionCb->SendCommand(command, args, cb);
        }

        //@Override
        public Boolean SendMediaButton(KeyEvent mediaButtonIntent) {
            return mSessionCb->SendMediaButton(mediaButtonIntent, 0, NULL);
        }

        //@Override
        CARAPI RegisterCallbackListener(ISessionControllerCallback cb) {
            synchronized(mLock) {
                // If this session is already destroyed tell the caller and
                // don't add them.
                if (mDestroyed) {
                    try {
                        cb->OnSessionDestroyed();
                    } catch (Exception e) {
                        // ignored
                    }
                    return;
                }
                if (GetControllerCbIndexForCb(cb) < 0) {
                    mControllerCallbacks->Add(cb);
                    if (DEBUG) {
                        Logger::D(TAG, "registering controller callback " + cb);
                    }
                }
            }
        }

        //@Override
        CARAPI UnregisterCallbackListener(ISessionControllerCallback cb)
                throws RemoteException {
            synchronized(mLock) {
                Int32 index = GetControllerCbIndexForCb(cb);
                if (index != -1) {
                    mControllerCallbacks->Remove(index);
                }
                if (DEBUG) {
                    Logger::D(TAG, "unregistering callback " + cb + ". index=" + index);
                }
            }
        }

        //@Override
        public String GetPackageName() {
            return mPackageName;
        }

        //@Override
        public String GetTag() {
            return mTag;
        }

        //@Override
        public PendingIntent GetLaunchPendingIntent() {
            return mLaunchIntent;
        }

        //@Override
        public Int64 GetFlags() {
            return mFlags;
        }

        //@Override
        public ParcelableVolumeInfo GetVolumeAttributes() {
            synchronized(mLock) {
                Int32 type;
                Int32 max;
                Int32 current;
                if (mVolumeType == PlaybackInfo.PLAYBACK_TYPE_REMOTE) {
                    type = mVolumeControlType;
                    max = mMaxVolume;
                    current = mOptimisticVolume != -1 ? mOptimisticVolume
                            : mCurrentVolume;
                } else {
                    Int32 stream = AudioAttributes->ToLegacyStreamType(mAudioAttrs);
                    type = VolumeProvider.VOLUME_CONTROL_ABSOLUTE;
                    max = mAudioManager->GetStreamMaxVolume(stream);
                    current = mAudioManager->GetStreamVolume(stream);
                }
                return new ParcelableVolumeInfo(mVolumeType, mAudioAttrs, type, max, current);
            }
        }

        //@Override
        CARAPI AdjustVolume(Int32 direction, Int32 flags, String packageName) {
            Int32 uid = Binder->GetCallingUid();
            final Int64 token = Binder->ClearCallingIdentity();
            try {
                MediaSessionRecord.this->AdjustVolume(direction, flags, packageName, uid, FALSE);
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }

        //@Override
        CARAPI SetVolumeTo(Int32 value, Int32 flags, String packageName) {
            Int32 uid = Binder->GetCallingUid();
            final Int64 token = Binder->ClearCallingIdentity();
            try {
                MediaSessionRecord.this->SetVolumeTo(value, flags, packageName, uid);
            } finally {
                Binder->RestoreCallingIdentity(token);
            }
        }

        //@Override
        CARAPI Play() throws RemoteException {
            mSessionCb->Play();
        }

        //@Override
        CARAPI PlayFromMediaId(String mediaId, Bundle extras) throws RemoteException {
            mSessionCb->PlayFromMediaId(mediaId, extras);
        }

        //@Override
        CARAPI PlayFromSearch(String query, Bundle extras) throws RemoteException {
            mSessionCb->PlayFromSearch(query, extras);
        }

        //@Override
        CARAPI SkipToQueueItem(Int64 id) {
            mSessionCb->SkipToTrack(id);
        }


        //@Override
        CARAPI Pause() throws RemoteException {
            mSessionCb->Pause();
        }

        //@Override
        CARAPI Stop() throws RemoteException {
            mSessionCb->Stop();
        }

        //@Override
        CARAPI Next() throws RemoteException {
            mSessionCb->Next();
        }

        //@Override
        CARAPI Previous() throws RemoteException {
            mSessionCb->Previous();
        }

        //@Override
        CARAPI FastForward() throws RemoteException {
            mSessionCb->FastForward();
        }

        //@Override
        CARAPI Rewind() throws RemoteException {
            mSessionCb->Rewind();
        }

        //@Override
        CARAPI SeekTo(Int64 pos) throws RemoteException {
            mSessionCb->SeekTo(pos);
        }

        //@Override
        CARAPI Rate(Rating rating) throws RemoteException {
            mSessionCb->Rate(rating);
        }

        //@Override
        CARAPI SendCustomAction(String action, Bundle args)
                throws RemoteException {
            mSessionCb->SendCustomAction(action, args);
        }


        //@Override
        public MediaMetadata GetMetadata() {
            synchronized(mLock) {
                return mMetadata;
            }
        }

        //@Override
        public PlaybackState GetPlaybackState() {
            return GetStateWithUpdatedPosition();
        }

        //@Override
        public ParceledListSlice GetQueue() {
            synchronized(mLock) {
                return mQueue;
            }
        }

        //@Override
        public CharSequence GetQueueTitle() {
            return mQueueTitle;
        }

        //@Override
        public Bundle GetExtras() {
            synchronized(mLock) {
                return mExtras;
            }
        }

        //@Override
        public Int32 GetRatingType() {
            return mRatingType;
        }

        //@Override
        public Boolean IsTransportControlEnabled() {
            return MediaSessionRecord.this->IsTransportControlEnabled();
        }
    }

    private class MessageHandler extends Handler {
        private static const Int32 MSG_UPDATE_METADATA = 1;
        private static const Int32 MSG_UPDATE_PLAYBACK_STATE = 2;
        private static const Int32 MSG_UPDATE_QUEUE = 3;
        private static const Int32 MSG_UPDATE_QUEUE_TITLE = 4;
        private static const Int32 MSG_UPDATE_EXTRAS = 5;
        private static const Int32 MSG_SEND_EVENT = 6;
        private static const Int32 MSG_UPDATE_SESSION_STATE = 7;
        private static const Int32 MSG_UPDATE_VOLUME = 8;
        private static const Int32 MSG_DESTROYED = 9;

        public MessageHandler(Looper looper) {
            Super(looper);
        }
        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case MSG_UPDATE_METADATA:
                    PushMetadataUpdate();
                    break;
                case MSG_UPDATE_PLAYBACK_STATE:
                    PushPlaybackStateUpdate();
                    break;
                case MSG_UPDATE_QUEUE:
                    PushQueueUpdate();
                    break;
                case MSG_UPDATE_QUEUE_TITLE:
                    PushQueueTitleUpdate();
                    break;
                case MSG_UPDATE_EXTRAS:
                    PushExtrasUpdate();
                    break;
                case MSG_SEND_EVENT:
                    PushEvent((String) msg.obj, msg->GetData());
                    break;
                case MSG_UPDATE_SESSION_STATE:
                    // TODO add session state
                    break;
                case MSG_UPDATE_VOLUME:
                    PushVolumeUpdate();
                    break;
                case MSG_DESTROYED:
                    PushSessionDestroyed();
            }
        }

        CARAPI Post(Int32 what) {
            Post(what, NULL);
        }

        CARAPI Post(Int32 what, Object obj) {
            ObtainMessage(what, obj).SendToTarget();
        }

        CARAPI Post(Int32 what, Object obj, Bundle data) {
            Message msg = ObtainMessage(what, obj);
            msg->SetData(data);
            msg->SendToTarget();
        }
    }

}
