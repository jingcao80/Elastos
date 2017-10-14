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

#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSION_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSION_H__

#include "_Elastos_Droid_Media_Session_CMediaSession.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IMediaMetadata;
using Elastos::Droid::Media::IRating;
using Elastos::Droid::Media::IVolumeProvider;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Allows interaction with media controllers, volume keys, media buttons, and
 * transport controls.
 * <p>
 * A MediaSession should be created when an app wants to publish media playback
 * information or handle media keys. In general an app only needs one session
 * for all playback, though multiple sessions can be created to provide finer
 * grain controls of media.
 * <p>
 * Once a session is created the owner of the session may pass its
 * {@link #getSessionToken() session token} to other processes to allow them to
 * create a {@link MediaController} to interact with the session.
 * <p>
 * To receive commands, media keys, and other events a {@link Callback} must be
 * set with {@link #setCallback(Callback)} and {@link #setActive(boolean)
 * setActive(true)} must be called.
 * <p>
 * When an app is finished performing playback it must call {@link #release()}
 * to clean up the session and notify any controllers.
 * <p>
 * MediaSession objects are thread safe.
 */
CarClass(CMediaSession)
    , public Object
    , public IMediaSession
{
private:
    class Command
        : public Object
    {
    public:
        Command(
            /* [in] */ const String& command,
            /* [in] */ IBundle * extras,
            /* [in] */ IResultReceiver * stub);

    public:
        String mCommand;
        AutoPtr<IBundle> mExtras;
        AutoPtr<IResultReceiver> mStub;
    };

    class CallbackMessageHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CMediaSession::CallbackMessageHandler")

        CallbackMessageHandler(
            /* [in] */ CMediaSession* host,
            /* [in] */ ILooper* looper,
            /* [in] */ IMediaSessionCallback * callback);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        using Handler::Post;

        CARAPI Post(
            /* [in] */ Int32 what,
            /* [in] */ IInterface * obj,
            /* [in] */ IBundle * bundle);

        CARAPI Post(
            /* [in] */ Int32 what,
            /* [in] */ IInterface * obj);

        CARAPI Post(
            /* [in] */ Int32 what);

        CARAPI Post(
            /* [in] */ Int32 what,
            /* [in] */ IInterface * obj,
            /* [in] */ Int32 arg1);

    public:
        const static Int32 MSG_PLAY;
        const static Int32 MSG_PLAY_MEDIA_ID;
        const static Int32 MSG_PLAY_SEARCH;
        const static Int32 MSG_SKIP_TO_ITEM;
        const static Int32 MSG_PAUSE;
        const static Int32 MSG_STOP;
        const static Int32 MSG_NEXT;
        const static Int32 MSG_PREVIOUS;
        const static Int32 MSG_FAST_FORWARD;
        const static Int32 MSG_REWIND;
        const static Int32 MSG_SEEK_TO;
        const static Int32 MSG_RATE;
        const static Int32 MSG_CUSTOM_ACTION;
        const static Int32 MSG_MEDIA_BUTTON;
        const static Int32 MSG_COMMAND;
        const static Int32 MSG_SET_BROWSED_PLAYER;
        const static Int32 MSG_SET_PLAY_ITEM;
        const static Int32 MSG_GET_NOW_PLAYING_ITEMS;

        AutoPtr<IMediaSessionCallback> mCallback;

    private:
        CMediaSession* mHost;
    };

    class VolumeProviderCallback
        : public Object
        , public IVolumeProviderCallback
    {
    public:
        CAR_INTERFACE_DECL()

        VolumeProviderCallback(
            /* [in] */ CMediaSession* host)
            : mHost(host)
        {}

        CARAPI OnVolumeChanged(
            /* [in] */ IVolumeProvider* volumeProvider);

    private:
        CMediaSession* mHost;
    };

    class PlayItemToken
        : public Object
    {
    public:
        PlayItemToken(
            /* [in] */ Int64 uid,
            /* [in] */ Int32 scope);

        CARAPI GetScope(
            /* [out] */ Int32* result);

        CARAPI GetUid(
            /* [out] */ Int64* result);

    private:
        Int64 mUid;
        Int32 mScope;
    };

public:
    CMediaSession();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaSession();

    /**
     * Creates a new session. The session will automatically be registered with
     * the system but will not be published until {@link #setActive(boolean)
     * setActive(true)} is called. You must call {@link #release()} when
     * finished with the session.
     *
     * @param context The context to use to create the session.
     * @param tag A short name for debugging purposes.
     */
    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ const String& tag);

    /**
     * Creates a new session as the specified user. To create a session as a
     * user other than your own you must hold the
     * {@link android.Manifest.permission#INTERACT_ACROSS_USERS_FULL}
     * permission.
     *
     * @param context The context to use to create the session.
     * @param tag A short name for debugging purposes.
     * @param userId The user id to create the session as.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ const String& tag,
        /* [in] */ Int32 userId);

    /**
     * Set the callback to receive updates for the MediaSession. This includes
     * media button events and transport controls. The caller's thread will be
     * used to post updates.
     * <p>
     * Set the callback to null to stop receiving updates.
     *
     * @param callback The callback object
     */
    CARAPI SetCallback(
        /* [in] */ IMediaSessionCallback * cb);

    /**
     * Set the callback to receive updates for the MediaSession. This includes
     * media button events and transport controls.
     * <p>
     * Set the callback to null to stop receiving updates.
     *
     * @param callback The callback to receive updates on.
     * @param handler The handler that events should be posted on.
     */
    CARAPI SetCallback(
        /* [in] */ IMediaSessionCallback * cb,
        /* [in] */ IHandler * handler);

    /**
     * Set an intent for launching UI for this Session. This can be used as a
     * quick link to an ongoing media screen. The intent should be for an
     * activity that may be started using {@link Activity#startActivity(Intent)}.
     *
     * @param pi The intent to launch to show UI for this Session.
     */
    CARAPI SetSessionActivity(
        /* [in] */ IPendingIntent * pi);

    /**
     * Set a pending intent for your media button receiver to allow restarting
     * playback after the session has been stopped. If your app is started in
     * this way an {@link Intent#ACTION_MEDIA_BUTTON} intent will be sent via
     * the pending intent.
     *
     * @param mbr The {@link PendingIntent} to send the media button event to.
     */
    CARAPI SetMediaButtonReceiver(
        /* [in] */ IPendingIntent * mbr);

    /**
     * Set any flags for the session.
     *
     * @param flags The flags to set for this session.
     */
    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    /**
     * Set the attributes for this session's audio. This will affect the
     * system's volume handling for this session. If
     * {@link #setPlaybackToRemote} was previously called it will stop receiving
     * volume commands and the system will begin sending volume changes to the
     * appropriate stream.
     * <p>
     * By default sessions use attributes for media.
     *
     * @param attributes The {@link AudioAttributes} for this session's audio.
     */
    CARAPI SetPlaybackToLocal(
        /* [in] */ IAudioAttributes * attributes);

    /**
     * Configure this session to use remote volume handling. This must be called
     * to receive volume button events, otherwise the system will adjust the
     * appropriate stream volume for this session. If
     * {@link #setPlaybackToLocal} was previously called the system will stop
     * handling volume changes for this session and pass them to the volume
     * provider instead.
     *
     * @param volumeProvider The provider that will handle volume changes. May
     *            not be null.
     */
    CARAPI SetPlaybackToRemote(
        /* [in] */ IVolumeProvider * volumeProvider);

    /**
     * Set if this session is currently active and ready to receive commands. If
     * set to false your session's controller may not be discoverable. You must
     * set the session to active before it can start receiving media button
     * events or transport commands.
     *
     * @param active Whether this session is active or not.
     */
    CARAPI SetActive(
        /* [in] */ Boolean active);

    /**
     * Get the current active state of this session.
     *
     * @return True if the session is active, false otherwise.
     */
    CARAPI IsActive(
        /* [out] */ Boolean * result);

    /**
     * Send a proprietary event to all MediaControllers listening to this
     * Session. It's up to the Controller/Session owner to determine the meaning
     * of any events.
     *
     * @param event The name of the event to send
     * @param extras Any extras included with the event
     */
    CARAPI SendSessionEvent(
        /* [in] */ const String& event,
        /* [in] */ IBundle * extras);

    /**
     * This must be called when an app has finished performing playback. If
     * playback is expected to start again shortly the session can be left open,
     * but it must be released if your activity or service is being destroyed.
     */
    CARAPI ReleaseResources();

    /**
     * Retrieve a token object that can be used by apps to create a
     * {@link MediaController} for interacting with this session. The owner of
     * the session is responsible for deciding how to distribute these tokens.
     *
     * @return A token that can be used to create a MediaController for this
     *         session
     */
    CARAPI GetSessionToken(
        /* [out] */ IMediaSessionToken ** result);

    /**
     * Get a controller for this session. This is a convenience method to avoid
     * having to cache your own controller in process.
     *
     * @return A controller for this session.
     */
    CARAPI GetController(
        /* [out] */ IMediaController ** result);

    /**
     * Update the current playback state.
     *
     * @param state The current state of playback
     */
    CARAPI SetPlaybackState(
        /* [in] */ IPlaybackState * state);

    /**
     * Update the current metadata. New metadata can be created using
     * {@link android.media.MediaMetadata.Builder}.
     *
     * @param metadata The new metadata
     */
    CARAPI SetMetadata(
        /* [in] */ IMediaMetadata * metadata);

    /**
     * Update the list of items in the play queue. It is an ordered list and
     * should contain the current item, and previous or upcoming items if they
     * exist. Specify null if there is no current play queue.
     * <p>
     * The queue should be of reasonable size. If the play queue is unbounded
     * within your app, it is better to send a reasonable amount in a sliding
     * window instead.
     *
     * @param queue A list of items in the play queue.
     */
    CARAPI SetQueue(
        /* [in] */ IList * queue);

    /**
     * Set the title of the play queue. The UI should display this title along
     * with the play queue itself.
     * e.g. "Play Queue", "Now Playing", or an album name.
     *
     * @param title The title of the play queue.
     */
    CARAPI SetQueueTitle(
        /* [in] */ ICharSequence * title);

    /**
     * Set some extras that can be associated with the {@link MediaSession}. No assumptions should
     * be made as to how a {@link MediaController} will handle these extras.
     * Keys should be fully qualified (e.g. com.example.MY_EXTRA) to avoid conflicts.
     *
     * @param extras The extras associated with the {@link MediaSession}.
     */
    CARAPI SetExtras(
        /* [in] */ IBundle * extras);

    CARAPI PlayItemResponse(
        /* [in] */ Boolean success);

    CARAPI UpdateNowPlayingEntries(
        /* [in] */ ArrayOf<Int64>* playList);

    CARAPI UpdateFolderInfoBrowsedPlayer(
        /* [in] */ const String& stringUri);

    CARAPI UpdateNowPlayingContentChange();

    /**
     * Notify the system that the remote volume changed.
     *
     * @param provider The provider that is handling volume changes.
     * @hide
     */
    CARAPI NotifyRemoteVolumeChanged(
        /* [in] */ IVolumeProvider * provider);

    /**
     * Return true if this is considered an active playback state.
     *
     * @hide
     */
    static CARAPI IsActiveState(
        /* [in] */ Int32 state,
        /* [out] */ Boolean * result);

    CARAPI DispatchPlay();

    CARAPI DispatchPlayFromMediaId(
        /* [in] */ const String& mediaId,
        /* [in] */ IBundle * extras);

    CARAPI DispatchPlayFromSearch(
        /* [in] */ const String& query,
        /* [in] */ IBundle * extras);

    CARAPI DispatchSkipToItem(
        /* [in] */ Int64 id);

    CARAPI DispatchPause();

    CARAPI DispatchStop();

    CARAPI DispatchNext();

    CARAPI DispatchPrevious();

    CARAPI DispatchFastForward();

    CARAPI DispatchRewind();

    CARAPI DispatchSeekTo(
        /* [in] */ Int64 pos);

    CARAPI DispatchRate(
        /* [in] */ IRating * rating);

    CARAPI DispatchCustomAction(
        /* [in] */ const String& action,
        /* [in] */ IBundle * args);

    CARAPI DispatchSetBrowsedPlayerCommand();

    CARAPI DispatchSetPlayItemCommand(
        /* [in] */ Int64 uid,
        /* [in] */ Int32 scope);

    CARAPI DispatchGetNowPlayingItemsCommand();

    CARAPI DispatchMediaButton(
        /* [in] */ IIntent * mediaButtonIntent);

    CARAPI PostToCallback(
        /* [in] */ Int32 what);

    CARAPI PostCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle * args,
        /* [in] */ IResultReceiver * resultCb);

    CARAPI PostToCallback(
        /* [in] */ Int32 what,
        /* [in] */ IInterface * obj);

    CARAPI PostToCallback(
        /* [in] */ Int32 what,
        /* [in] */ IInterface * obj,
        /* [in] */ IBundle * extras);

public:
    AutoPtr<IVolumeProvider> mVolumeProvider;
    AutoPtr<IPlaybackState> mPlaybackState;

private:
    const static String TAG;
    Object mLock;
    AutoPtr<IMediaSessionToken> mSessionToken;
    AutoPtr<IMediaController> mController;
    AutoPtr<IISession> mBinder;
    AutoPtr<IISessionCallback> mCbStub;
    AutoPtr<CallbackMessageHandler> mCallback;
    Boolean mActive;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSION_H__
