
#ifndef __ELASTOS_DROID_MEDIA_CREMOTECONTROLCLIENT_H__
#define __ELASTOS_DROID_MEDIA_CREMOTECONTROLCLIENT_H__

#include "_Elastos_Droid_Media_CRemoteControlClient.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::App::IPendingIntent;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CRemoteControlClient)
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
        : public ElRefBase
        , public IMetadataEditor
    {
    friend class CRemoteControlClient;

    public:
        CAR_INTERFACE_DECL();

        // only use RemoteControlClient.editMetadata() to get a MetadataEditor instance
        MetadataEditor(
            /* [in] */ CRemoteControlClient* owner);

        /**
         * @hide
         */
        CARAPI Clone(
            /* [out] */ IMetadataEditor** result);

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
            /* [out] */ IMetadataEditor** result);

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
            /* [out] */ IMetadataEditor** result);

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
            /* [out] */ IMetadataEditor** result);

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

    protected:
        /**
         * @hide
         */
        Boolean mMetadataChanged;
        Boolean mArtworkChanged;
        AutoPtr<IBitmap> mEditorArtwork;
        AutoPtr<IBundle> mEditorMetadata;

    private:
        Boolean mApplied; // = FALSE;
        Object mThisLock;
        CRemoteControlClient* mOwner;
    };

    CRemoteControlClient();

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
     * Creates a {@link MetadataEditor}.
     * @param startEmpty Set to false if you want the MetadataEditor to contain the metadata that
     *     was previously applied to the RemoteControlClient, or true if it is to be created empty.
     * @return a new MetadataEditor instance.
     */
    CARAPI EditMetadata(
        /* [in] */ Boolean startEmpty,
        /* [out] */ IMetadataEditor** result);

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
     * Set information describing information related to the playback of media so the system
     * can implement additional behavior to handle non-local playback usecases.
     * @param what a key to specify the type of information to set. Valid keys are
     *        {@link #PLAYBACKINFO_PLAYBACK_TYPE},
     *        {@link #PLAYBACKINFO_USES_STREAM},
     *        {@link #PLAYBACKINFO_VOLUME},
     *        {@link #PLAYBACKINFO_VOLUME_MAX},
     *        and {@link #PLAYBACKINFO_VOLUME_HANDLING}.
     * @param value the value for the supplied information to set.
     */
    CARAPI SetPlaybackInformation(
        /* [in] */ Int32 what,
        /* [in] */ Int32 value);
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
     * @hide
     * Accessor to IRemoteControlClient
     */
    CARAPI GetIRemoteControlClient(
        /* [out] */ IIRemoteControlClient** result);

    /**
     * @hide
     * To be only used by AudioManager after it has received the unique id from
     * IAudioService.registerRemoteControlClient()
     * @param id the unique identifier of the RemoteControlStackEntry in AudioService with which
     *              this RemoteControlClient is associated.
     */
    CARAPI SetRcseId(
        /* [in] */ Int32 id);

    /**
     * @hide
     */
    CARAPI GetRcseId(
        /* [out] */ Int32* result);


private:
    /**
     * The IRemoteControlClient implementation
     */
    class MyRemoteControlClient
        : public ElRefBase
        , public IIRemoteControlClient
    {
    public:
        CAR_INTERFACE_DECL();

        MyRemoteControlClient(
            /* [in] */ CRemoteControlClient* owner);

        CARAPI OnInformationRequested(
            /* [in] */ Int32 clientGeneration,
            /* [in] */ Int32 infoFlags,
            /* [in] */ Int32 artWidth,
            /* [in] */ Int32 artHeight);

        CARAPI SetCurrentClientGenerationId(
            /* [in] */ Int32 clientGeneration);

        CARAPI PlugRemoteControlDisplay(
            /* [in] */ IIRemoteControlDisplay* rcd);

        CARAPI UnplugRemoteControlDisplay(
            /* [in] */ IIRemoteControlDisplay* rcd);

    private:
        CRemoteControlClient* mOwner;
    };

    class EventHandler
        : public HandlerBase
    {
    public:
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

    //===========================================================
    // Communication with IRemoteControlDisplay
    CARAPI_(void) DetachFromDisplay_syncCacheLock();

    CARAPI_(void) SendPlaybackState_syncCacheLock();

    CARAPI_(void) SendMetadata_syncCacheLock();

    CARAPI_(void) SendTransportControlFlags_syncCacheLock();

    CARAPI_(void) SendArtwork_syncCacheLock();

    CARAPI_(void) SendMetadataWithArtwork_syncCacheLock();

    static CARAPI_(AutoPtr<IIAudioService>) GetService();

    CARAPI_(void) SendAudioServiceNewPlaybackInfo_syncCacheLock(
        /* [in] */ Int32 what,
        /* [in] */ Int32 value);


    //===========================================================
    // Message handlers
    CARAPI_(void) OnNewInternalClientGen(
        /* [in] */ Int32 clientGeneration,
        /* [in] */ Int32 artWidth,
        /* [in] */ Int32 artHeight);

    CARAPI_(void) OnNewCurrentClientGen(
        /* [in] */ Int32 clientGeneration);

    CARAPI_(void) OnPlugDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd);

    CARAPI_(void) OnUnplugDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd);

    //===========================================================
    // Internal utilities

    /**
     * Scale a bitmap to fit the smallest dimension by uniformly scaling the incoming bitmap.
     * If the bitmap fits, then do nothing and return the original.
     *
     * @param bitmap
     * @param maxWidth
     * @param maxHeight
     * @return
     */

    CARAPI_(AutoPtr<IBitmap>) ScaleBitmapIfTooBig(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 maxWidth,
        /* [in] */ Int32 maxHeight);

    /**
     *  Fast routine to go through an array of allowed keys and return whether the key is part
     *  of that array
     * @param key the key value
     * @param validKeys the array of valid keys for a given type
     * @return true if the key is part of the array, false otherwise
     */
    static CARAPI_(Boolean) ValidTypeForKey(
        /* [in] */ Int32 key,
        /* [in] */ ArrayOf<Int32>* validKeys);

private:
    const static String TAG; // = "RemoteControlClient";

    const static Int32 PLAYBACK_TYPE_MIN;// = PLAYBACK_TYPE_LOCAL;
    const static Int32 PLAYBACK_TYPE_MAX;// = PLAYBACK_TYPE_REMOTE;

    static AutoPtr< ArrayOf<Int32> > METADATA_KEYS_TYPE_STRING;
    static AutoPtr< ArrayOf<Int32> > METADATA_KEYS_TYPE_LONG;

    Int32 mPlaybackType; // = PLAYBACK_TYPE_LOCAL;
    Int32 mPlaybackVolumeMax; // = DEFAULT_PLAYBACK_VOLUME;
    Int32 mPlaybackVolume; // = DEFAULT_PLAYBACK_VOLUME;
    Int32 mPlaybackVolumeHandling; // = DEFAULT_PLAYBACK_VOLUME_HANDLING;
    Int32 mPlaybackStream; // = AudioManager.STREAM_MUSIC;

    /**
     * Lock for all cached data
     */
    Object mCacheLock;

    /**
     * Cache for the playback state.
     * Access synchronized on mCacheLock
     */
    Int32 mPlaybackState; // = PLAYSTATE_NONE;

    /**
     * Time of last play state change
     * Access synchronized on mCacheLock
     */
    Int64 mPlaybackStateChangeTimeMs; // = 0;

    /**
     * Cache for the artwork bitmap.
     * Access synchronized on mCacheLock
     * Artwork and metadata are not kept in one Bundle because the bitmap sometimes needs to be
     * accessed to be resized, in which case a copy will be made. This would add overhead in
     * Bundle operations.
     */
    AutoPtr<IBitmap> mArtwork;

    Int32 ARTWORK_DEFAULT_SIZE; // = 256;
    Int32 ARTWORK_INVALID_SIZE; // = -1;

    Int32 mArtworkExpectedWidth; // = ARTWORK_DEFAULT_SIZE;
    Int32 mArtworkExpectedHeight; // = ARTWORK_DEFAULT_SIZE;

    /**
     * Cache for the transport control mask.
     * Access synchronized on mCacheLock
     */
    Int32 mTransportControlFlags; // = FLAGS_KEY_MEDIA_NONE;

    /**
     * Cache for the metadata strings.
     * Access synchronized on mCacheLock
     * This is re-initialized in apply() and so cannot be final.
     */
    AutoPtr<IBundle> mMetadata; // = new Bundle();

    /**
     * The current remote control client generation ID across the system
     */
    Int32 mCurrentClientGenId; // = -1;

    /**
     * The remote control client generation ID, the last time it was told it was the current RC.
     * If (mCurrentClientGenId == mInternalClientGenId) is true, it means that this remote control
     * client is the "focused" one, and that whenever this client's info is updated, it needs to
     * send it to the known IRemoteControlDisplay interfaces.
     */
    Int32 mInternalClientGenId; // = -2;

    /**
     * The media button intent description associated with this remote control client
     * (can / should include target component for intent handling)
     */
    AutoPtr<IPendingIntent> mRcMediaIntent;

    /**
     * The remote control display to which this client will send information.
     * NOTE: Only one IRemoteControlDisplay supported in this implementation
     */
    AutoPtr<IIRemoteControlDisplay> mRcDisplay;

    /**
     * The IRemoteControlClient implementation
     */
    AutoPtr<IIRemoteControlClient> mIRCC; // = new IRemoteControlClient.Stub()

    /**
     * Unique identifier of the RemoteControlStackEntry in AudioService with which
     * this RemoteControlClient is associated.
     */
    Int32 mRcseId; // = RCSE_ID_UNREGISTERED;

    AutoPtr<EventHandler> mEventHandler;

    const static Int32 MSG_REQUEST_PLAYBACK_STATE;// = 1;
    const static Int32 MSG_REQUEST_METADATA;// = 2;
    const static Int32 MSG_REQUEST_TRANSPORTCONTROL;// = 3;
    const static Int32 MSG_REQUEST_ARTWORK;// = 4;
    const static Int32 MSG_NEW_INTERNAL_CLIENT_GEN;// = 5;
    const static Int32 MSG_NEW_CURRENT_CLIENT_GEN;// = 6;
    const static Int32 MSG_PLUG_DISPLAY;// = 7;
    const static Int32 MSG_UNPLUG_DISPLAY;// = 8;

    //===========================================================
    // Communication with AudioService
    static AutoPtr<IIAudioService> sService;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CREMOTECONTROLCLIENT_H__
