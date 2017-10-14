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

#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIACONTROLLER_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIACONTROLLER_H__

#include "_Elastos_Droid_Media_Session_CMediaController.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Allows an app to interact with an ongoing media session. Media buttons and
 * other commands can be sent to the session. A callback may be registered to
 * receive updates from the session, such as metadata and play state changes.
 * <p>
 * A MediaController can be created through {@link MediaSessionManager} if you
 * hold the "android.permission.MEDIA_CONTENT_CONTROL" permission or are an
 * enabled notification listener or by getting a {@link MediaSession.Token}
 * directly from the session owner.
 * <p>
 * MediaController objects are thread-safe.
 */
CarClass(CMediaController)
    , public Object
    , public IMediaController
{
public:
    class PlaybackInfo
        : public Object
        , public IMediaControllerPlaybackInfo
    {
    public:
        CAR_INTERFACE_DECL()

        PlaybackInfo(
            /* [in] */ Int32 type,
            /* [in] */ IAudioAttributes* attrs,
            /* [in] */ Int32 control,
            /* [in] */ Int32 max,
            /* [in] */ Int32 current);

        CARAPI  GetPlaybackType(
            /* [out] */ Int32* result);

        CARAPI GetAudioAttributes(
            /* [out] */ IAudioAttributes** result);

        CARAPI GetVolumeControl(
            /* [out] */ Int32* result);

        CARAPI GetMaxVolume(
            /* [out] */ Int32* result);

        CARAPI GetCurrentVolume(
            /* [out] */ Int32* result);

    private:
        const Int32 mVolumeType;
        const Int32 mVolumeControl;
        const Int32 mMaxVolume;
        const Int32 mCurrentVolume;
        const AutoPtr<IAudioAttributes> mAudioAttrs;
    };

private:
    class CallbackStub
        : public Object
        , public IISessionControllerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        CallbackStub(
            /* [in] */ CMediaController * controller);

        // @Override
        CARAPI OnSessionDestroyed();

        // @Override
        CARAPI OnEvent(
            /* [in] */ const String& event,
            /* [in] */ IBundle * extras);

        // @Override
        CARAPI OnPlaybackStateChanged(
            /* [in] */ IPlaybackState * state);

        // @Override
        CARAPI OnMetadataChanged(
            /* [in] */ IMediaMetadata * metadata);

        // @Override
        CARAPI OnQueueChanged(
            /* [in] */ IParceledListSlice * parceledQueue);

        // @Override
        CARAPI OnQueueTitleChanged(
            /* [in] */ ICharSequence * title);

        // @Override
        CARAPI OnExtrasChanged(
            /* [in] */ IBundle * extras);

        // @Override
        CARAPI OnVolumeInfoChanged(
            /* [in] */ IParcelableVolumeInfo * pvi);

        // @Override
        CARAPI OnUpdateFolderInfoBrowsedPlayer(
            /* [in] */ const String& stringUri);

        // @Override
        CARAPI OnUpdateNowPlayingEntries(
            /* [in] */ ArrayOf<Int64>* playList);

        // @Override
        CARAPI OnUpdateNowPlayingContentChange();

        // @Override
        CARAPI OnPlayItemResponse(
            /* [in] */ Boolean success);

    private:
        CMediaController * mController;
    };

    class MessageHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CMediaController::MessageHandler")

        MessageHandler(
            /* [in] */ ILooper * looper,
            /* [in] */ IMediaControllerCallback * cb,
            /* [in] */ CMediaController * host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        using Handler::Post;

        CARAPI Post(
            /* [in] */ Int32 what,
            /* [in] */ IInterface * obj,
            /* [in] */ IBundle * data);

    public:
        AutoPtr<IMediaControllerCallback> mCallback;
        CMediaController * mHost;
    };

public:
    CMediaController();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaController();

    /**
     * Call for creating a MediaController directly from a binder. Should only
     * be used by framework code.
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ IISessionController * sessionBinder);

    /**
     * Create a new MediaController from a session's token.
     *
     * @param context The caller's context.
     * @param token The token for the session.
     */
     CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ IMediaSessionToken * token);

    /**
     * Get a {@link TransportControls} instance to send transport actions to
     * the associated session.
     *
     * @return A transport controls instance.
     */
    CARAPI GetTransportControls(
        /* [out] */ IMediaControllerTransportControls ** result);

    /**
     * Send the specified media button event to the session. Only media keys can
     * be sent by this method, other keys will be ignored.
     *
     * @param keyEvent The media button event to dispatch.
     * @return true if the event was sent to the session, false otherwise.
     */
    CARAPI DispatchMediaButtonEvent(
        /* [in] */ IKeyEvent * keyEvent,
        /* [out] */ Boolean * result);

    /**
     * Get the current playback state for this session.
     *
     * @return The current PlaybackState or null
     */
    CARAPI GetPlaybackState(
        /* [out] */ IPlaybackState ** result);

    /**
     * Get the current metadata for this session.
     *
     * @return The current MediaMetadata or null.
     */
    CARAPI GetMetadata(
        /* [out] */ IMediaMetadata ** result);

    /**
     * Get the current play queue for this session if one is set. If you only
     * care about the current item {@link #getMetadata()} should be used.
     *
     * @return The current play queue or null.
     */
    CARAPI GetQueue(
        /* [out] */ IList ** result);

    /**
     * Get the queue title for this session.
     */
    CARAPI GetQueueTitle(
        /* [out] */ ICharSequence ** result);

    /**
     * Get the extras for this session.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle ** result);

    /**
     * Get the rating type supported by the session. One of:
     * <ul>
     * <li>{@link Rating#RATING_NONE}</li>
     * <li>{@link Rating#RATING_HEART}</li>
     * <li>{@link Rating#RATING_THUMB_UP_DOWN}</li>
     * <li>{@link Rating#RATING_3_STARS}</li>
     * <li>{@link Rating#RATING_4_STARS}</li>
     * <li>{@link Rating#RATING_5_STARS}</li>
     * <li>{@link Rating#RATING_PERCENTAGE}</li>
     * </ul>
     *
     * @return The supported rating type
     */
    CARAPI GetRatingType(
        /* [out] */ Int32 * result);

    /**
     * Get the flags for this session. Flags are defined in {@link MediaSession}.
     *
     * @return The current set of flags for the session.
     */
    CARAPI GetFlags(
        /* [out] */ Int64 * result);

    /**
     * Get the current playback info for this session.
     *
     * @return The current playback info or null.
     */
    CARAPI GetPlaybackInfo(
        /* [out] */ IMediaControllerPlaybackInfo ** result);

    /**
     * Get an intent for launching UI associated with this session if one
     * exists.
     *
     * @return A {@link PendingIntent} to launch UI or null.
     */
    CARAPI GetSessionActivity(
        /* [out] */ IPendingIntent ** result);

    /**
     * Get the token for the session this is connected to.
     *
     * @return The token for the connected session.
     */
    CARAPI GetSessionToken(
        /* [out] */ IMediaSessionToken ** result);

    /**
     * Set the volume of the output this session is playing on. The command will
     * be ignored if it does not support
     * {@link VolumeProvider#VOLUME_CONTROL_ABSOLUTE}. The flags in
     * {@link AudioManager} may be used to affect the handling.
     *
     * @see #getPlaybackInfo()
     * @param value The value to set it to, between 0 and the reported max.
     * @param flags Flags from {@link AudioManager} to include with the volume
     *            request.
     */
    CARAPI SetVolumeTo(
        /* [in] */ Int32 value,
        /* [in] */ Int32 flags);

    /**
     * Adjust the volume of the output this session is playing on. The direction
     * must be one of {@link AudioManager#ADJUST_LOWER},
     * {@link AudioManager#ADJUST_RAISE}, or {@link AudioManager#ADJUST_SAME}.
     * The command will be ignored if the session does not support
     * {@link VolumeProvider#VOLUME_CONTROL_RELATIVE} or
     * {@link VolumeProvider#VOLUME_CONTROL_ABSOLUTE}. The flags in
     * {@link AudioManager} may be used to affect the handling.
     *
     * @see #getPlaybackInfo()
     * @param direction The direction to adjust the volume in.
     * @param flags Any flags to pass with the command.
     */
    CARAPI AdjustVolume(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 flags);

    /**
     * Registers a callback to receive updates from the Session. Updates will be
     * posted on the caller's thread.
     *
     * @param callback The callback object, must not be null.
     */
    CARAPI RegisterCallback(
        /* [in] */ IMediaControllerCallback * cb);

    /**
     * Registers a callback to receive updates from the session. Updates will be
     * posted on the specified handler's thread.
     *
     * @param callback The callback object, must not be null.
     * @param handler The handler to post updates on. If null the callers thread
     *            will be used.
     */
    CARAPI RegisterCallback(
        /* [in] */ IMediaControllerCallback * cb,
        /* [in] */ IHandler * handler);

    /**
     * Unregisters the specified callback. If an update has already been posted
     * you may still receive it after calling this method.
     *
     * @param callback The callback to remove.
     */
    CARAPI UnregisterCallback(
        /* [in] */ IMediaControllerCallback * cb);

    /**
     * Sends a generic command to the session. It is up to the session creator
     * to decide what commands and parameters they will support. As such,
     * commands should only be sent to sessions that the controller owns.
     *
     * @param command The command to send
     * @param args Any parameters to include with the command
     * @param cb The callback to receive the result on
     */
    CARAPI SendCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle * args,
        /* [in] */ IResultReceiver * cb);

    /**
     * Get the session owner's package name.
     *
     * @return The package name of of the session owner.
     */
    CARAPI GetPackageName(
        /* [out] */ String * result);

    /**
     * Get the session's tag for debugging purposes.
     *
     * @return The session's tag.
     * @hide
     */
    CARAPI GetTag(
        /* [out] */ String * result);

    /**
     * @hide
     */
    CARAPI ControlsSameSession(
        /* [in] */ IMediaController * other,
        /* [out] */ Boolean * result);

    /*
     * @hide
     */
    CARAPI GetSessionBinder(
        /* [out] */ IISessionController ** result);

private:
    CARAPI_(void) AddCallbackLocked(
        /* [in] */ IMediaControllerCallback * cb,
        /* [in] */ IHandler * handler);

    CARAPI_(Boolean) RemoveCallbackLocked(
        /* [in] */ IMediaControllerCallback * cb);

    CARAPI GetHandlerForCallbackLocked(
        /* [in] */ IMediaControllerCallback * cb,
        /* [out] */ MessageHandler ** result);

    CARAPI_(void) PostMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface * obj,
        /* [in] */ IBundle * data);

public:
    const static Int32 MSG_EVENT;
    const static Int32 MSG_UPDATE_PLAYBACK_STATE;
    const static Int32 MSG_UPDATE_METADATA;
    const static Int32 MSG_UPDATE_VOLUME;
    const static Int32 MSG_UPDATE_QUEUE;
    const static Int32 MSG_UPDATE_QUEUE_TITLE;
    const static Int32 MSG_UPDATE_EXTRAS;
    const static Int32 MSG_FOLDER_INFO_BROWSED_PLAYER;
    const static Int32 MSG_UPDATE_NOWPLAYING_ENTRIES;
    const static Int32 MSG_UPDATE_NOWPLAYING_CONTENT_CHANGE;
    const static Int32 MSG_PLAY_ITEM_RESPONSE;
    const static Int32 MSG_DESTROYED;

    AutoPtr<IISessionController> mSessionBinder;

private:
    const static String TAG;

    AutoPtr<IMediaSessionToken> mToken;
    AutoPtr<IContext> mContext;
    AutoPtr<CallbackStub> mCbStub;
    AutoPtr<IArrayList> mCallbacks;
    Object mLock;
    Boolean mCbRegistered;
    String mPackageName;
    String mTag;

    AutoPtr<IMediaControllerTransportControls> mTransportControls;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIACONTROLLER_H__
