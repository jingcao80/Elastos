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

#ifndef __ELASTOS_DROID_MEDIA_CREMOTECONTROLCLIENT_H__
#define __ELASTOS_DROID_MEDIA_CREMOTECONTROLCLIENT_H__

#include "_Elastos_Droid_Media_CRemoteControlClient.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.App.h"
#include "elastos/droid/media/CMediaMetadata.h"
#include "elastos/droid/media/MediaMetadataEditor.h"
#include "elastos/droid/media/session/MediaSessionCallback.h"
#include <elastos/core/Object.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Media::Session::IMediaSession;
using Elastos::Droid::Media::Session::IMediaSessionCallback;
using Elastos::Droid::Media::Session::IMediaSessionLegacyHelper;
using Elastos::Droid::Media::Session::IPlaybackState;
using Elastos::Droid::Media::Session::MediaSessionCallback;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IResultReceiver;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CRemoteControlClient)
    , public Object
    , public IRemoteControlClient
{
public:
    /**
     * Class used to modify metadata in a {@link RemoteControlClient} object.
     * Use {@link RemoteControlClient#editMetadata(boolean)} to create an instance of an editor,
     * on which you set the metadata for the RemoteControlClient instance. Once all the information
     * has been set, use {@link #apply()} to make it the new metadata that should be displayed
     * for the associated client. Once the metadata has been "applied", you cannot reuse this
     * instance of the MetadataEditor.
     */
    class MetadataEditor
        : public MediaMetadataEditor
        , public IRemoteControlClientMetadataEditor
    {
    friend class CRemoteControlClient;

    public:
        CAR_INTERFACE_DECL()

        // only use RemoteControlClient.editMetadata() to get a MetadataEditor instance
        MetadataEditor(
            /* [in] */ CRemoteControlClient* owner);

        /**
         * @hide
         */
        CARAPI Clone(
            /* [out] */ IInterface** result);

        using MediaMetadataEditor::PutString;

        /**
         * Adds textual information to be displayed.
         * Note that none of the information added after {@link #apply()} has been called,
         * will be displayed.
         * @param key The identifier of a the metadata field to set. Valid values are
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_ALBUM},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_ALBUMARTIST},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_TITLE},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_ARTIST},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_AUTHOR},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_COMPILATION},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_COMPOSER},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_DATE},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_GENRE},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_TITLE},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_WRITER}.
         * @param value The text for the given key, or {@code null} to signify there is no valid
         *      information for the field.
         * @return Returns a reference to the same MetadataEditor object, so you can chain put
         *      calls together.
         */
        /*synchronized*/ CARAPI PutString(
            /* [in] */ Int32 key,
            /* [in] */ const String& value,
            /* [out] */ IRemoteControlClientMetadataEditor** result);

        using MediaMetadataEditor::PutInt64;

        /**
         * Adds numerical information to be displayed.
         * Note that none of the information added after {@link #apply()} has been called,
         * will be displayed.
         * @param key the identifier of a the metadata field to set. Valid values are
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_CD_TRACK_NUMBER},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_DISC_NUMBER},
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_DURATION} (with a value
         *      expressed in milliseconds),
         *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_YEAR}.
         * @param value The long value for the given key
         * @return Returns a reference to the same MetadataEditor object, so you can chain put
         *      calls together.
         * @throws IllegalArgumentException
         */
        /*synchronized*/ CARAPI PutInt64(
            /* [in] */ Int32 key,
            /* [in] */ Int64 value,
            /* [out] */ IRemoteControlClientMetadataEditor** result);

        using MediaMetadataEditor::PutBitmap;

        /**
         * Sets the album / artwork picture to be displayed on the remote control.
         * @param key the identifier of the bitmap to set. The only valid value is
         *      {@link #BITMAP_KEY_ARTWORK}
         * @param bitmap The bitmap for the artwork, or null if there isn't any.
         * @return Returns a reference to the same MetadataEditor object, so you can chain put
         *      calls together.
         * @throws IllegalArgumentException
         * @see android.graphics.Bitmap
         */
        /*synchronized*/ CARAPI PutBitmap(
            /* [in] */ Int32 key,
            /* [in] */ IBitmap* bitmap,
            /* [out] */ IRemoteControlClientMetadataEditor** result);

        using MediaMetadataEditor::PutObject;

        /*synchronized*/ CARAPI PutObject(
            /* [in] */ Int32 key,
            /* [in] */ IInterface* value,
            /* [out] */ IRemoteControlClientMetadataEditor** result);

        /**
         * Clears all the metadata that has been set since the MetadataEditor instance was
         *     created with {@link RemoteControlClient#editMetadata(boolean)}.
         */
        /*synchronized*/ CARAPI Clear();

        /**
         * Associates all the metadata that has been set since the MetadataEditor instance was
         *     created with {@link RemoteControlClient#editMetadata(boolean)}, or since
         *     {@link #clear()} was called, with the RemoteControlClient. Once "applied",
         *     this MetadataEditor cannot be reused to edit the RemoteControlClient's metadata.
         */
        /*synchronized*/ CARAPI Apply();

    private:
        Object mThisLock;
        CRemoteControlClient* mOwner;
    };

private:
    class InnerMediaSessionCallback
        : public MediaSessionCallback
    {
    public:
        InnerMediaSessionCallback(
            /* [in] */ CRemoteControlClient* owner);

        // @Override
        CARAPI OnSeekTo(
            /* [in] */ Int64 pos);

        // @Override
        CARAPI OnSetRating(
            /* [in] */ IRating* rating);

        // @Override
        CARAPI SetPlayItem(
            /* [in] */ Int32 scope,
            /* [in] */ Int64 uid);

        // @Override
        CARAPI GetNowPlayingEntries();

        // @Override
        CARAPI SetBrowsedPlayer();

    private:
        CRemoteControlClient* mOwner;
    };

public:
    CRemoteControlClient();

    virtual ~CRemoteControlClient();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Class constructor.
     * @param mediaButtonIntent The intent that will be sent for the media button events sent
     *     by remote controls.
     *     This intent needs to have been constructed with the {@link Intent#ACTION_MEDIA_BUTTON}
     *     action, and have a component that will handle the intent (set with
     *     {@link Intent#setComponent(ComponentName)}) registered with
     *     {@link AudioManager#registerMediaButtonEventReceiver(ComponentName)}
     *     before this new RemoteControlClient can itself be registered with
     *     {@link AudioManager#registerRemoteControlClient(RemoteControlClient)}.
     * @see AudioManager#registerMediaButtonEventReceiver(ComponentName)
     * @see AudioManager#registerRemoteControlClient(RemoteControlClient)
     */
    CARAPI constructor(
        /* [in] */ IPendingIntent* mediaButtonIntent);

    /**
     * Class constructor for a remote control client whose internal event handling
     * happens on a user-provided Looper.
     * @param mediaButtonIntent The intent that will be sent for the media button events sent
     *     by remote controls.
     *     This intent needs to have been constructed with the {@link Intent#ACTION_MEDIA_BUTTON}
     *     action, and have a component that will handle the intent (set with
     *     {@link Intent#setComponent(ComponentName)}) registered with
     *     {@link AudioManager#registerMediaButtonEventReceiver(ComponentName)}
     *     before this new RemoteControlClient can itself be registered with
     *     {@link AudioManager#registerRemoteControlClient(RemoteControlClient)}.
     * @param looper The Looper running the event loop.
     * @see AudioManager#registerMediaButtonEventReceiver(ComponentName)
     * @see AudioManager#registerRemoteControlClient(RemoteControlClient)
     */
    CARAPI constructor(
        /* [in] */ IPendingIntent* mediaButtonIntent,
        /* [in] */ ILooper* looper);

    /**
     * @hide
     */
    CARAPI RegisterWithSession(
        /* [in] */ IMediaSessionLegacyHelper * helper);

    /**
     * @hide
     */
    CARAPI UnregisterWithSession(
        /* [in] */ IMediaSessionLegacyHelper * helper);

    CARAPI GetMediaSession(
        /* [out] */ IMediaSession ** result);

    /**
     * Creates a {@link MetadataEditor}.
     * @param startEmpty Set to false if you want the MetadataEditor to contain the metadata that
     *     was previously applied to the RemoteControlClient, or true if it is to be created empty.
     * @return a new MetadataEditor instance.
     */
    CARAPI EditMetadata(
        /* [in] */ Boolean startEmpty,
        /* [out] */ IRemoteControlClientMetadataEditor** result);

    /**
     * Sets the current playback state.
     * @param state The current playback state, one of the following values:
     *       {@link #PLAYSTATE_STOPPED},
     *       {@link #PLAYSTATE_PAUSED},
     *       {@link #PLAYSTATE_PLAYING},
     *       {@link #PLAYSTATE_FAST_FORWARDING},
     *       {@link #PLAYSTATE_REWINDING},
     *       {@link #PLAYSTATE_SKIPPING_FORWARDS},
     *       {@link #PLAYSTATE_SKIPPING_BACKWARDS},
     *       {@link #PLAYSTATE_BUFFERING},
     *       {@link #PLAYSTATE_ERROR}.
     */
    CARAPI SetPlaybackState(
        /* [in] */ Int32 state);

    CARAPI SetPlaybackState(
        /* [in] */ Int32 state,
        /* [in] */ Int64 timeInMs,
        /* [in] */ Float playbackSpeed);

    CARAPI PlayItemResponse(
        /* [in] */ Boolean success);

    CARAPI UpdateNowPlayingEntries(
        /* [in] */ ArrayOf<Int64>* playList);

    CARAPI UpdateFolderInfoBrowsedPlayer(
        /* [in] */ const String& stringUri);

    CARAPI UpdateNowPlayingContentChange();

    CARAPI SetPlaybackState(
        /* [in] */ Int32 state,
        /* [in] */ Int64 timeInMs,
        /* [in] */ Float playbackSpeed,
        /* [in] */ Boolean hasPosition);

    /**
     * Sets the flags for the media transport control buttons that this client supports.
     * @param transportControlFlags A combination of the following flags:
     *      {@link #FLAG_KEY_MEDIA_PREVIOUS},
     *      {@link #FLAG_KEY_MEDIA_REWIND},
     *      {@link #FLAG_KEY_MEDIA_PLAY},
     *      {@link #FLAG_KEY_MEDIA_PLAY_PAUSE},
     *      {@link #FLAG_KEY_MEDIA_PAUSE},
     *      {@link #FLAG_KEY_MEDIA_STOP},
     *      {@link #FLAG_KEY_MEDIA_FAST_FORWARD},
     *      {@link #FLAG_KEY_MEDIA_NEXT}
     */
    CARAPI SetTransportControlFlags(
        /* [in] */ Int32 transportControlFlags);

    /**
     * @hide
     * Return playback information represented as an integer value.
     * @param what a key to specify the type of information to retrieve. Valid keys are
     *        {@link #PLAYBACKINFO_PLAYBACK_TYPE},
     *        {@link #PLAYBACKINFO_USES_STREAM},
     *        {@link #PLAYBACKINFO_VOLUME},
     *        {@link #PLAYBACKINFO_VOLUME_MAX},
     *        and {@link #PLAYBACKINFO_VOLUME_HANDLING}.
     * @return the current value for the given information type, or
     *   {@link #PLAYBACKINFO_INVALID_VALUE} if an error occurred or the request is invalid, or
     *   the value is unknown.
     */
    CARAPI GetInt32PlaybackInformation(
        /* [in] */ Int32 what,
        /* [out] */ Int32* result);

    /**
     * @hide
     * Accessor to media button intent description (includes target component)
     */
    CARAPI GetRcMediaIntent(
        /* [out] */ IPendingIntent** result);

    /**
     * Sets the listener to be called whenever the metadata is updated.
     * New metadata values will be received in the same thread as the one in which
     * RemoteControlClient was created.
     * @param l the metadata update listener
     */
    CARAPI SetMetadataUpdateListener(
        /* [in] */ IRemoteControlClientOnMetadataUpdateListener * l);

    CARAPI SetNowPlayingEntriesUpdateListener(
        /* [in] */ IRemoteControlClientOnGetNowPlayingEntriesListener* l);

    CARAPI SetBrowsedPlayerUpdateListener(
        /* [in] */ IRemoteControlClientOnSetBrowsedPlayerListener* l);

    CARAPI SetPlayItemListener(
        /* [in] */ IRemoteControlClientOnSetPlayItemListener* l);

    CARAPI SetPlaybackPositionUpdateListener(
        /* [in] */ IRemoteControlClientOnPlaybackPositionUpdateListener * l);

    CARAPI SetOnGetPlaybackPositionListener(
        /* [in] */ IRemoteControlClientOnGetPlaybackPositionListener * l);

     //===========================================================
    // Internal utilities

    /**
     * Returns whether, for the given playback state, the playback position is expected to
     * be changing.
     * @param playstate the playback state to evaluate
     * @return true during any form of playback, false if it's not playing anything while in this
     *     playback state
     */
    static CARAPI PlaybackPositionShouldMove(
        /* [in] */ Int32 playstate,
        /* [out] */ Boolean* result);
private:
    class EventHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CRemoteControlClient::EventHandler")

        EventHandler(
            /* [in] */ IRemoteControlClient* rcc,
            /* [in] */ ILooper* looper,
            /* [in] */ CRemoteControlClient* owner);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CRemoteControlClient* mOwner;
    };

private:
    static CARAPI_(AutoPtr<IIAudioService>) GetService();

    //===========================================================
    // Internal utilities

    CARAPI_(void) SetPlaybackStateInt(
        /* [in] */ Int32 state,
        /* [in] */ Int64 timeInMs,
        /* [in] */ Float playbackSpeed,
        /* [in] */ Boolean hasPosition);

    CARAPI_(void) PlayItemResponseInt(
        /* [in] */ Boolean success);

    CARAPI_(void) UpdateNowPlayingEntriesInt(
        /* [in] */ ArrayOf<Int64>* playList);

    CARAPI_(void) UpdateFolderInfoBrowsedPlayerInt(
        /* [in] */ const String& stringUri);

    CARAPI_(void) UpdateNowPlayingContentChangeInt();

    CARAPI_(void) OnPositionDriftCheck();

    static Int64 GetCheckPeriodFromSpeed(
        /*[in] */ Float speed);

    CARAPI_(void) OnSeekTo(
        /* [in] */ Int32 generationId,
        /* [in] */ Int64 timeMs);

    CARAPI_(void) OnUpdateMetadata(
        /* [in] */ Int32 generationId,
        /* [in] */ Int32 key,
        /* [in] */ IInterface* value);

    CARAPI_(void) OnSetPlayItem(
        /* [in] */ Int32 scope,
        /* [in] */ Int64 uid);


    CARAPI_(void) OnSetBrowsedPlayer();

    CARAPI_(void) OnGetNowPlayingEntries();

private:
    const static String TAG;
    const static Boolean DEBUG;

    const static Int32 PLAYBACK_TYPE_MIN;
    const static Int32 PLAYBACK_TYPE_MAX;
    const static Int64 POSITION_REFRESH_PERIOD_PLAYING_MS;
    const static Int64 POSITION_REFRESH_PERIOD_MIN_MS;
    const static Int64 POSITION_DRIFT_MAX_MS;

    const static Int32 MSG_POSITION_DRIFT_CHECK;
    const static Int32 MSG_SET_BROWSED_PLAYER;
    const static Int32 MSG_SET_PLAY_ITEM;
    const static Int32 MSG_GET_NOW_PLAYING_ENTRIES;

    AutoPtr<IMediaSession> mSession;

    /**
     * Lock for all cached data
     */
    Object mCacheLock;

    /**
     * Cache for the playback state.
     * Access synchronized on mCacheLock
     */
    Int32 mPlaybackState;

    /**
     * Time of last play state change
     * Access synchronized on mCacheLock
     */
    Int64 mPlaybackStateChangeTimeMs;

    /**
     * Last playback position in ms reported by the user
     */
    Int64 mPlaybackPositionMs;

    /**
     * Last playback speed reported by the user
     */
    Float mPlaybackSpeed;

    /**
     * Cache for the artwork bitmap.
     * Access synchronized on mCacheLock
     * Artwork and metadata are not kept in one Bundle because the bitmap sometimes needs to be
     * accessed to be resized, in which case a copy will be made. This would add overhead in
     * Bundle operations.
     */
    AutoPtr<IBitmap> mOriginalArtwork;

    /**
     * Cache for the transport control mask.
     * Access synchronized on mCacheLock
     */
    Int32 mTransportControlFlags;
    /**
     * Cache for the metadata strings.
     * Access synchronized on mCacheLock
     * This is re-initialized in apply() and so cannot be final.
     */
    AutoPtr<IBundle> mMetadata;

    AutoPtr<IRemoteControlClientOnPlaybackPositionUpdateListener> mPositionUpdateListener;

    /**
     * Provider registered by user of RemoteControlClient to provide the current playback position.
     */
    AutoPtr<IRemoteControlClientOnGetPlaybackPositionListener> mPositionProvider;

    /**
     * Listener registered by user of RemoteControlClient to receive edit changes to metadata
     * it exposes.
     */
    AutoPtr<IRemoteControlClientOnMetadataUpdateListener> mMetadataUpdateListener;

    AutoPtr<IRemoteControlClientOnSetBrowsedPlayerListener> mSetBrowsedPlayerListener;

    AutoPtr<IRemoteControlClientOnSetPlayItemListener> mSetPlayItemListener;

    AutoPtr<IRemoteControlClientOnGetNowPlayingEntriesListener> mGetNowPlayingEntriesListener;

    /**
     * The current remote control client generation ID across the system
     */
    Int32 mCurrentClientGenId;

    /**
     * The remote control client generation ID, the last time it was told it was the current RC.
     * If (mCurrentClientGenId == mInternalClientGenId) is true, it means that this remote control
     * client is the "focused" one, and that whenever this client's info is updated, it needs to
     * send it to the known IRemoteControlDisplay interfaces.
     */
    Int32 mInternalClientGenId;
    /**
     * The media button intent description associated with this remote control client
     * (can / should include target component for intent handling)
     */
    AutoPtr<IPendingIntent> mRcMediaIntent;

    /**
     * Reflects whether any "plugged in" IRemoteControlDisplay has mWantsPositonSync set to true.
     */
    // TODO consider using a ref count for IRemoteControlDisplay requiring sync instead
    Boolean mNeedsPositionSync;

    /**
     * Cache for the current playback state using Session APIs.
     */
    AutoPtr<IPlaybackState> mSessionPlaybackState;

    /**
     * Cache for metadata using Session APIs. This is re-initialized in apply().
     */
    AutoPtr<IMediaMetadata> mMediaMetadata;

    AutoPtr<EventHandler> mEventHandler;

    AutoPtr<IMediaSessionCallback> mTransportListener;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CREMOTECONTROLCLIENT_H__
