#ifndef _ELASTOS_DROID_MEDIA_REMOTECONTROLLER_H__
#define _ELASTOS_DROID_MEDIA_REMOTECONTROLLER_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/session/CMediaController.h"
#include "elastos/droid/media/MediaMetadataEditor.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Media::Session::EIID_IMediaControllerCallback;
using Elastos::Droid::Media::Session::CMediaController;
using Elastos::Droid::Media::Session::IMediaController;
using Elastos::Droid::Media::Session::IMediaControllerCallback;
using Elastos::Droid::Media::Session::IMediaSessionManager;
using Elastos::Droid::Media::Session::IMediaSessionManagerOnActiveSessionsChangedListener;
using Elastos::Droid::Media::Session::IPlaybackState;
using Elastos::Utility::Etl::List;

namespace Elastos{
namespace Droid {
namespace Media {

class RemoteController
    : public Object
    , public IRemoteController
{
private:
    //==================================================
    // Implementation of IRemoteControlDisplay interface
    class RcDisplay
        : public Object
        , public IIRemoteControlDisplay
    {
        friend class RemoteController;
    public:
        RcDisplay(
            /* [in] */ RemoteController* rc);

        ~RcDisplay();

        CAR_INTERFACE_DECL()

        CARAPI SetCurrentClientId(
            /* [in] */ Int32 genId,
            /* [in] */ IPendingIntent* clientMediaIntent,
            /* [in] */ Boolean clearing);

        CARAPI SetEnabled(
            /* [in] */ Boolean enabled);

        CARAPI SetPlaybackState(
            /* [in] */ Int32 genId,
            /* [in] */ Int32 state,
            /* [in] */ Int64 stateChangeTimeMs,
            /* [in] */ Int64 currentPosMs,
            /* [in] */ Float speed);

        CARAPI SetTransportControlInfo(
            /* [in] */ Int32 genId,
            /* [in] */ Int32 transportControlFlags,
            /* [in] */ Int32 posCapabilities);

        CARAPI SetMetadata(
            /* [in] */ Int32 genId,
            /* [in] */ IBundle* metadata);

        CARAPI SetArtwork(
            /* [in] */ Int32 genId,
            /* [in] */ IBitmap* artwork);

        CARAPI SetAllMetadata(
            /* [in] */ Int32 genId,
            /* [in] */ IBundle* metadata,
            /* [in] */ IBitmap* artwork);
    private:
        RemoteController* mController;
    };

    class MediaControllerCallback
        : public Object
        , public IMediaControllerCallback
    {
        friend class RemoteController;
    public:
        MediaControllerCallback(
            /* [in] */ RemoteController* host);

        ~MediaControllerCallback();

        CAR_INTERFACE_DECL()

        CARAPI OnPlaybackStateChanged(
            /* [in] */ IPlaybackState* state);

        CARAPI OnMetadataChanged(
            /* [in] */ IMediaMetadata* metadata);

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
        RemoteController* mHost;
    };

    /**
     * Listens for changes to the active session stack and replaces the
     * currently tracked session if it has changed.
     */
    class TopTransportSessionListener
        : public Object
        , public IMediaSessionManagerOnActiveSessionsChangedListener
    {
    public:
        TopTransportSessionListener(
            /* [in] */ RemoteController* host);

        ~TopTransportSessionListener();

        CAR_INTERFACE_DECL()

        CARAPI OnActiveSessionsChanged(
            /* [in] */ IList * controllers);

    private:
        RemoteController* mHost;
    };

    class EventHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("RemoteController::EventHandler")

        EventHandler(
            /* [in] */ RemoteController* host,
            /* [in] */ ILooper* looper);

        ~EventHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        RemoteController* mHost;
    };

    class PlaybackInfo
        : public Object
        , public IPlaybackInfo
    {
    public:
        PlaybackInfo(
            /* [in] */ Int32 state,
            /* [in] */ Int64 stateChangeTimeMs,
            /* [in] */ Int64 currentPosMs,
            /* [in] */ Float speed);

        ~PlaybackInfo();

        CAR_INTERFACE_DECL()

    public:
        Int32 mState;
        Int64 mStateChangeTimeMs;
        Int64 mCurrentPosMs;
        Float mSpeed;
    };

public:
    class MetadataEditor
        : public MediaMetadataEditor
        , public IRemoteControllerMetadataEditor
    {
        friend class RemoteController;
    public:
        CARAPI Apply();

        CAR_INTERFACE_DECL()

    protected:
        /**
         * @hide
         */
        MetadataEditor(
            /* [in] */ RemoteController* host);

        MetadataEditor(
            /* [in] */ RemoteController* host,
            /* [in] */ IBundle* metadata,
            /* [in] */ Int64 editableKeys);

    private:
        CARAPI_(void) CleanupBitmapFromBundle(
            /* [in] */ Int32 key);

    private:
        RemoteController* mHost;
    };

public:
    RemoteController();

    virtual ~RemoteController();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IRemoteControllerOnClientUpdateListener* updateListener);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IRemoteControllerOnClientUpdateListener* updateListener,
        /* [in] */ ILooper* looper);

    CARAPI GetRemoteControlClientPackageName(
        /* [out] */ String* result);

    CARAPI GetEstimatedMediaPosition(
        /* [out] */ Int64* result);

    CARAPI SendMediaKeyEvent(
        /* [in] */ IKeyEvent* keyEvent,
        /* [out] */ Boolean* result);

    CARAPI SeekTo(
        /* [in] */ Int64 timeMs,
        /* [out] */ Boolean* result);

    CARAPI SetRemoteControlClientPlayItem(
        /* [in] */ Int64 uid,
        /* [in] */ Int32 scope);

    CARAPI GetRemoteControlClientNowPlayingEntries();

    CARAPI SetRemoteControlClientBrowsedPlayer();

    CARAPI SetArtworkConfiguration(
        /* [in] */ Boolean wantBitmap,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ Boolean* result);

    CARAPI SetArtworkConfiguration(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ Boolean* result);

    CARAPI ClearArtworkConfiguration(
        /* [out] */ Boolean* result);

    CARAPI SetSynchronizationMode(
        /* [in] */ Int32 sync,
        /* [out] */ Boolean* result);

    CARAPI EditMetadata(
        /* [out] */ IRemoteControllerMetadataEditor** result);

    CARAPI StartListeningToSessions();

    CARAPI StopListeningToSessions();

    CARAPI OnFolderInfoBrowsedPlayer(
        /* [in] */ const String& stringUri);

    CARAPI OnNowPlayingEntriesUpdate(
        /* [in] */ ArrayOf<Int64>* playList);

    CARAPI OnNowPlayingContentChange();

    CARAPI OnSetPlayItemResponse(
        /* [in] */ Boolean success);

    CARAPI SetIsRegistered(
        /* [in] */ Boolean registered);

    CARAPI GetRcDisplay(
        /* [out] */ IIRemoteControlDisplay** result);

    CARAPI GetArtworkSize(
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI GetUpdateListener(
        /* [out] */ IRemoteControllerOnClientUpdateListener** result);

private:
    static CARAPI_(void) SendMsg(
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 msg,
        /* [in] */ Int32 existingMsgPolicy,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 delayMs);

    CARAPI_(void) OnNewPendingIntent(
        /* [in] */ Int32 genId,
        /* [in] */ IPendingIntent* pi);

    CARAPI_(void) OnNewPlaybackInfo(
        /* [in] */ Int32 genId,
        /* [in] */ PlaybackInfo* pi);

    CARAPI_(void) OnNewTransportInfo(
        /* [in] */ Int32 genId,
        /* [in] */ Int32 transportControlFlags);

    CARAPI_(void) OnNewMetadata(
        /* [in] */ Int32 genId,
        /* [in] */ IBundle* metadata);

    CARAPI_(void) OnClientChange(
        /* [in] */ Int32 genId,
        /* [in] */ Boolean clearing);

    CARAPI_(void) OnDisplayEnable(
        /* [in] */ Boolean enabled);

    CARAPI_(void) UpdateController(
        /* [in] */ IMediaController* controller);

    CARAPI_(void) OnNewPlaybackState(
        /* [in] */ IPlaybackState* state);

    CARAPI_(void) OnNewMediaMetadata(
        /* [in] */ IMediaMetadata* metadata);

private:
    const static Int32 MAX_BITMAP_DIMENSION;
    const static Int32 TRANSPORT_UNKNOWN;
    const static String TAG;
    const static Boolean DEBUG;
    const static Boolean USE_SESSIONS;
    /*const*/ static Object mGenLock;
    /*const*/ static Object mInfoLock;

    AutoPtr<RcDisplay> mRcd;
    AutoPtr<IContext> mContext;
    AutoPtr<IAudioManager> mAudioManager;
    Int32 mMaxBitmapDimension;
    AutoPtr<MetadataEditor> mMetadataEditor;

    AutoPtr<IMediaSessionManager> mSessionManager;
    AutoPtr<IMediaSessionManagerOnActiveSessionsChangedListener> mSessionListener;
    AutoPtr<IMediaControllerCallback> mSessionCb;

    /**
     * Synchronized on mGenLock
     */
    Int32 mClientGenerationIdCurrent;

    /**
     * Synchronized on mInfoLock
     */
    Boolean mIsRegistered;
    AutoPtr<IPendingIntent> mClientPendingIntentCurrent;
    AutoPtr<IRemoteControllerOnClientUpdateListener> mOnClientUpdateListener;
    AutoPtr<PlaybackInfo> mLastPlaybackInfo;
    Int32 mArtworkWidth;
    Int32 mArtworkHeight;
    Boolean mEnabled;
    // synchronized on mInfoLock, for USE_SESSION apis.
    AutoPtr<IMediaController> mCurrentSession;

    //==================================================
    // Event handling
    AutoPtr<EventHandler> mEventHandler;
    const static Int32 MSG_NEW_PENDING_INTENT;
    const static Int32 MSG_NEW_PLAYBACK_INFO;
    const static Int32 MSG_NEW_TRANSPORT_INFO;
    const static Int32 MSG_NEW_METADATA; // msg always has non-null obj parameter
    const static Int32 MSG_CLIENT_CHANGE;
    const static Int32 MSG_DISPLAY_ENABLE;
    const static Int32 MSG_NEW_PLAYBACK_STATE;
    const static Int32 MSG_NEW_MEDIA_METADATA;

    /** If the msg is already queued, replace it with this one. */
    static const Int32 SENDMSG_REPLACE;
    /** If the msg is already queued, ignore this one and leave the old. */
    static const Int32 SENDMSG_NOOP;
    /** If the msg is already queued, queue this one and leave the old. */
    static const Int32 SENDMSG_QUEUE;
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_REMOTECONTROLLER_H__
