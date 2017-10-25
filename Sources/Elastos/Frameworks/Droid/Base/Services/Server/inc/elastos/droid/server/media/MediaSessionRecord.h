//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_MEDIA_MEDIASESSIONRECORD_H__
#define __ELASTOS_DROID_SERVER_MEDIA_MEDIASESSIONRECORD_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.View.h>
#include <_Elastos.Droid.Server.h>
#include "elastos/droid/server/media/CSessionController.h"
#include "elastos/droid/server/media/CMediaSession.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Media::IMediaMetadata;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioManagerInternal;
using Elastos::Droid::Media::IRating;
using Elastos::Droid::Media::Session::IISessionControllerCallback;
using Elastos::Droid::Media::Session::IPlaybackState;
using Elastos::Droid::Media::Session::IISessionCallback;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

class MediaSessionService;

/**
 * This is the system implementation of a Session. Apps will interact with the
 * MediaSession wrapper class instead.
 */
class MediaSessionRecord
    : public Object
    , public IMediaSessionRecord
    , public IProxyDeathRecipient
{
public:
    class SessionCb : public Object
    {
    public:
        SessionCb(
            /* [in] */ IISessionCallback* cb)
            : mCb(cb)
        {}

        CARAPI SendMediaButton(
            /* [in] */ IKeyEvent* keyEvent,
            /* [in] */ Int32 sequenceId,
            /* [in] */ IResultReceiver* cb,
            /* [out] */ Boolean* result);

        CARAPI SendCommand(
            /* [in] */ const String& command,
            /* [in] */ IBundle* args,
            /* [in] */ IResultReceiver* cb);

        CARAPI SendCustomAction(
            /* [in] */ const String& action,
            /* [in] */ IBundle* args);

        CARAPI Play();

        CARAPI PlayFromMediaId(
            /* [in] */ const String& mediaId,
            /* [in] */ IBundle* extras);

        CARAPI PlayFromSearch(
            /* [in] */ const String& query,
            /* [in] */ IBundle* extras);

        CARAPI SkipToTrack(
            /* [in] */ Int64 id);

        CARAPI Pause();

        CARAPI Stop();

        CARAPI Next();

        CARAPI Previous();

        CARAPI FastForward();

        CARAPI Rewind();

        CARAPI SeekTo(
            /* [in] */ Int64 pos);

        CARAPI Rate(
            /* [in] */ IRating* rating);

        CARAPI AdjustVolume(
            /* [in] */ Int32 direction);

        CARAPI SetVolumeTo(
            /* [in] */ Int32 value);

    private:
        AutoPtr<IISessionCallback> mCb;

        friend class MediaSessionRecord;
    };

private:
    class MessageHandler : public Handler
    {
    public:
        TO_STRING_IMPL("MediaSessionRecord::MessageHandler")

        MessageHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ MediaSessionRecord* host)
            : Handler(looper)
            , mHost(host)
        {}

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        using Handler::Post;

        CARAPI_(void) Post(
            /* [in] */ Int32 what);

        CARAPI_(void) Post(
            /* [in] */ Int32 what,
            /* [in] */ IObject* obj);

        CARAPI_(void) Post(
            /* [in] */ Int32 what,
            /* [in] */ IObject* obj,
            /* [in] */ IBundle* data);

    private:
        static const Int32 MSG_UPDATE_METADATA = 1;
        static const Int32 MSG_UPDATE_PLAYBACK_STATE = 2;
        static const Int32 MSG_UPDATE_QUEUE = 3;
        static const Int32 MSG_UPDATE_QUEUE_TITLE = 4;
        static const Int32 MSG_UPDATE_EXTRAS = 5;
        static const Int32 MSG_SEND_EVENT = 6;
        static const Int32 MSG_UPDATE_SESSION_STATE = 7;
        static const Int32 MSG_UPDATE_VOLUME = 8;
        static const Int32 MSG_DESTROYED = 9;

        MediaSessionRecord* mHost;

        friend class CMediaSession;
        friend class MediaSessionRecord;
    };

    class ClearOptimisticVolumeRunnable : public Runnable
    {
    public:
        ClearOptimisticVolumeRunnable(
            /* [in] */ MediaSessionRecord* host)
            : mHost(host)
        {}

        CARAPI Run( );

    private:
        MediaSessionRecord* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    MediaSessionRecord();

    CARAPI constructor(
        /* [in] */ Int32 ownerPid,
        /* [in] */ Int32 ownerUid,
        /* [in] */ Int32 userId,
        /* [in] */ const String& ownerPackageName,
        /* [in] */ IISessionCallback* cb,
        /* [in] */ const String& tag,
        /* [in] */ MediaSessionService* service,
        /* [in] */ IHandler* handler);

    /**
     * Get the binder for the {@link MediaSession}.
     *
     * @return The session binder apps talk to.
     */
    CARAPI_(AutoPtr<IISession>) GetSessionBinder();

    /**
     * Get the binder for the {@link MediaController}.
     *
     * @return The controller binder apps talk to.
     */
    CARAPI_(AutoPtr<IISessionController>) GetControllerBinder();

    /**
     * Get the info for this session.
     *
     * @return Info that identifies this session.
     */
    CARAPI_(String) GetPackageName();

    /**
     * Get the tag for the session.
     *
     * @return The session's tag.
     */
    CARAPI_(String) GetTag();

    /**
     * Get the intent the app set for their media button receiver.
     *
     * @return The pending intent set by the app or null.
     */
    CARAPI_(AutoPtr<IPendingIntent>) GetMediaButtonReceiver();

    /**
     * Get this session's flags.
     *
     * @return The flags for this session.
     */
    CARAPI_(Int64) GetFlags();

    /**
     * Check if this session has the specified flag.
     *
     * @param flag The flag to check.
     * @return True if this session has that flag set, false otherwise.
     */
    CARAPI_(Boolean) HasFlag(
        /* [in] */ Int32 flag);

    /**
     * Get the user id this session was created for.
     *
     * @return The user id for this session.
     */
    CARAPI_(Int32) GetUserId();

    /**
     * Check if this session has system priorty and should receive media buttons
     * before any other sessions.
     *
     * @return True if this is a system priority session, false otherwise
     */
    CARAPI_(Boolean) IsSystemPriority();

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
    CARAPI_(void) AdjustVolume(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 flags,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean useSuggested);

    CARAPI_(void) SetVolumeTo(
        /* [in] */ Int32 value,
        /* [in] */ Int32 flags,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid);

    /**
     * Check if this session has been set to active by the app.
     *
     * @return True if the session is active, false otherwise.
     */
    CARAPI_(Boolean) IsActive();

    /**
     * Check if the session is currently performing playback. This will also
     * return true if the session was recently paused.
     *
     * @param includeRecentlyActive True if playback that was recently paused
     *            should count, false if it shouldn't.
     * @return True if the session is performing playback, false otherwise.
     */
    CARAPI_(Boolean) IsPlaybackActive(
        /* [in] */ Boolean includeRecentlyActive);

    /**
     * Get the type of playback, either local or remote.
     *
     * @return The current type of playback.
     */
    CARAPI_(Int32) GetPlaybackType();

    /**
     * Get the local audio stream being used. Only valid if playback type is
     * local.
     *
     * @return The audio stream the session is using.
     */
    CARAPI_(AutoPtr<IAudioAttributes>) GetAudioAttributes();

    /**
     * Get the type of volume control. Only valid if playback type is remote.
     *
     * @return The volume control type being used.
     */
    CARAPI_(Int32) GetVolumeControl();

    /**
     * Get the max volume that can be set. Only valid if playback type is
     * remote.
     *
     * @return The max volume that can be set.
     */
    CARAPI_(Int32) GetMaxVolume();

    /**
     * Get the current volume for this session. Only valid if playback type is
     * remote.
     *
     * @return The current volume of the remote playback.
     */
    CARAPI_(Int32) GetCurrentVolume();

    /**
     * Get the volume we'd like it to be set to. This is only valid for a short
     * while after a call to adjust or set volume.
     *
     * @return The current optimistic volume or -1.
     */
    CARAPI_(Int32) GetOptimisticVolume();

    CARAPI_(Boolean) IsTransportControlEnabled();

    // @Override
    CARAPI ProxyDied();

    /**
     * Finish cleaning up this session, including disconnecting if connected and
     * removing the death observer from the callback binder.
     */
    CARAPI_(void) OnDestroy();

    CARAPI_(AutoPtr<IISessionCallback>) GetCallback();

    CARAPI_(void) SendMediaButton(
        /* [in] */ IKeyEvent* ke,
        /* [in] */ Int32 sequenceId,
        /* [in] */ IResultReceiver* cb);

    // CARAPI_(void) dump(PrintWriter pw, String prefix);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(String) GetShortMetadataString();

    CARAPI_(void) PushPlaybackStateUpdate();

    CARAPI_(void) PushMetadataUpdate();

    CARAPI_(void) PushQueueUpdate();

    CARAPI_(void) PushQueueTitleUpdate();

    CARAPI_(void) PushExtrasUpdate();

    CARAPI_(void) PushVolumeUpdate();

    CARAPI_(void) PushEvent(
        /* [in] */ const String& event,
        /* [in] */ IBundle* data);

    CARAPI_(void) PushSessionDestroyed();

    CARAPI_(AutoPtr<IPlaybackState>) GetStateWithUpdatedPosition();

    CARAPI_(List<AutoPtr<IISessionControllerCallback> >::Iterator) GetControllerCbIndexForCb(
        /* [in] */ IISessionControllerCallback* cb);

private:
    static const String TAG;
    static const Boolean DEBUG = FALSE;

    /**
     * The length of time a session will still be considered active after
     * pausing in ms.
     */
    static const Int32 ACTIVE_BUFFER = 30000;

    /**
     * The amount of time we'll send an assumed volume after the last volume
     * command before reverting to the last reported volume.
     */
    static const Int32 OPTIMISTIC_VOLUME_TIMEOUT = 1000;

    AutoPtr<MessageHandler> mHandler;

    Int32 mOwnerPid;
    Int32 mOwnerUid;
    Int32 mUserId;
    String mPackageName;
    String mTag;
    AutoPtr<CSessionController> mController;
    AutoPtr<CMediaSession> mSession;
    AutoPtr<SessionCb> mSessionCb;
    MediaSessionService* mService;

    Object mLock;
    List<AutoPtr<IISessionControllerCallback> > mControllerCallbacks;

    Int64 mFlags;
    AutoPtr<IPendingIntent> mMediaButtonReceiver;
    AutoPtr<IPendingIntent> mLaunchIntent;

    // TransportPerformer fields

    AutoPtr<IBundle> mExtras;
    AutoPtr<IMediaMetadata> mMetadata;
    AutoPtr<IPlaybackState> mPlaybackState;
    AutoPtr<IParceledListSlice> mQueue;
    AutoPtr<ICharSequence> mQueueTitle;
    Int32 mRatingType;
    Int64 mLastActiveTime;
    // End TransportPerformer fields

    // Volume handling fields
    AutoPtr<IAudioAttributes> mAudioAttrs;
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IAudioManagerInternal> mAudioManagerInternal;
    Int32 mVolumeType;
    Int32 mVolumeControlType;
    Int32 mMaxVolume;
    Int32 mCurrentVolume;
    Int32 mOptimisticVolume;
    // End volume handling fields

    Boolean mIsActive;
    Boolean mDestroyed;

    AutoPtr<IRunnable> mClearOptimisticVolumeRunnable;

    friend class CSessionController;
    friend class SessionCb;
    friend class ClearOptimisticVolumeRunnable;
    friend class CMediaSession;
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_MEDIASESSIONRECORD_H__
