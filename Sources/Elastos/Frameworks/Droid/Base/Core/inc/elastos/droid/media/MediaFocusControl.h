#ifndef __ELASTOS_DROID_MEDIA_MEDIAFOCUSCONTROL_H__
#define __ELASTOS_DROID_MEDIA_MEDIAFOCUSCONTROL_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Speech.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/telephony/PhoneStateListener.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentOnFinished;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Telephony::PhoneStateListener;
using Elastos::Droid::View::IKeyEvent;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IStack;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * @hide
 *
 */
class MediaFocusControl
    : public Object
    , public IPendingIntentOnFinished
{
    friend class PlayerRecord;
private:
    class NotificationListenerObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("MediaFocusControl::NotificationListenerObserver")

        NotificationListenerObserver(
            /* [in] */ MediaFocusControl* host);

        CARAPI constructor();

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        MediaFocusControl* mHost;
    };

    class MediaEventHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("MediaFocusControl::MediaEventHandler")

        MediaEventHandler(
            /* [in] */ MediaFocusControl* host,
            /* [in] */ ILooper* looper)
                : Handler(looper)
                , mHost(host)
            {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        MediaFocusControl* mHost;
    };

    class MyPhoneStateListener
        : public PhoneStateListener
    {
    public:
        MyPhoneStateListener(
            /* [in] */ MediaFocusControl* host)
            : mHost(host)
        {}

        CARAPI OnCallStateChanged(
            /* [in] */ Int32 state,
            /* [in] */ const String& incomingNumber);

    private:
        MediaFocusControl* mHost;
    };

    class AudioFocusDeathHandler
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        AudioFocusDeathHandler(
            /* [in] */ MediaFocusControl* host,
            /* [in] */ IBinder* cb)
            : mHost(host)
            , mCb(cb)
        {}

        CARAPI ProxyDied();

        CARAPI GetBinder(
            /* [out] */ IBinder** result);

    private:
        MediaFocusControl* mHost;
        AutoPtr<IBinder> mCb; // To be notified of client's death
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("MediaFocusControl::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ MediaFocusControl* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        MediaFocusControl* mHost;
    };

    /**
     * A class to encapsulate all the information about a remote control display.
     * After instanciation, init() must always be called before the object is added in the list
     * of displays.
     * Before being removed from the list of displays, release() must always be called (otherwise
     * it will leak death handlers).
     */
    class DisplayInfoForServer
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        DisplayInfoForServer(
            /* [in] */ IIRemoteControlDisplay* rcd,
            /* [in] */ Int32 w,
            /* [in] */ Int32 h,
            /* [in] */ MediaFocusControl* host);

        CARAPI Init(
            /* [out] */ Boolean* result);

        CARAPI ReleaseResources();

        CARAPI ProxyDied();

    public:
        /** may never be null */
        AutoPtr<IIRemoteControlDisplay> mRcDisplay;
        AutoPtr<IBinder> mRcDisplayBinder;
        Int32 mArtworkExpectedWidth;
        Int32 mArtworkExpectedHeight;
        Boolean mWantsPositionSync;
        AutoPtr<IComponentName> mClientNotifListComp;
        Boolean mEnabled;
        MediaFocusControl* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    MediaFocusControl();

    virtual ~MediaFocusControl();

    CARAPI constructor(
        /* [in] */ ILooper* looper,
        /* [in] */ IContext* cntxt,
        /* [in] */ IAudioServiceVolumeController* volumeCtrl,
        /* [in] */ IAudioService* as);

    CARAPI OnSendFinished(
        /* [in] */ IPendingIntent* pendingIntent,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* resultExtras);

    CARAPI SetRemoteControlClientPlayItem(
        /* [in] */ Int64 uid,
        /* [in] */ Int32 scope);

    CARAPI GetRemoteControlClientNowPlayingEntries();

    CARAPI SetRemoteControlClientBrowsedPlayer();

    CARAPI_(Int32) GetCurrentAudioFocus();

    /** @see AudioManager#requestAudioFocus(AudioManager.OnAudioFocusChangeListener, int, int)  */
    CARAPI_(Int32) RequestAudioFocus(
        /* [in] */ Int32 mainStreamType,
        /* [in] */ Int32 focusChangeHint,
        /* [in] */ IBinder* cb,
        /* [in] */ IIAudioFocusDispatcher* fd,
        /* [in] */ const String& clientId,
        /* [in] */ const String& callingPackageName);

    /** @see AudioManager#abandonAudioFocus(AudioManager.OnAudioFocusChangeListener)  */
    CARAPI_(Int32) AbandonAudioFocus(
        /* [in] */ IIAudioFocusDispatcher* fl,
        /* [in] */ const String& clientId);

    CARAPI_(void) UnregisterAudioFocusClient(
        /* [in] */ const String& clientId);

    CARAPI_(void) DiscardAudioFocusOwner();

    CARAPI_(Boolean) RegisterRemoteController(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IComponentName* listenerComp);

    CARAPI_(Boolean) RegisterRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI_(void) OnSetRemoteControlClientPlayItem(
        /* [in] */ Int32 scope,
        /* [in] */ Int64 uid);

    CARAPI_(void) OnGetRemoteControlClientNowPlayingEntries();

    CARAPI_(void) OnSetRemoteControlClientBrowsedPlayer();

    /**
     * Unregister an IRemoteControlDisplay.
     * No effect if the IRemoteControlDisplay hasn't been successfully registered.
     * @see android.media.IAudioService#unregisterRemoteControlDisplay(android.media.IRemoteControlDisplay)
     * @param rcd the IRemoteControlDisplay to unregister. No effect if null.
     */
    CARAPI_(void) UnregisterRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd);

    /**
     * Update the size of the artwork used by an IRemoteControlDisplay.
     * @see android.media.IAudioService#remoteControlDisplayUsesBitmapSize(android.media.IRemoteControlDisplay, int, int)
     * @param rcd the IRemoteControlDisplay with the new artwork size requirement
     * @param w the maximum width of the expected bitmap. Negative or zero values indicate this
     *   display doesn't need to receive artwork.
     * @param h the maximum height of the expected bitmap. Negative or zero values indicate this
     *   display doesn't need to receive artwork.
     */
    CARAPI_(void) RemoteControlDisplayUsesBitmapSize(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    /**
     * Controls whether a remote control display needs periodic checks of the RemoteControlClient
     * playback position to verify that the estimated position has not drifted from the actual
     * position. By default the check is not performed.
     * The IRemoteControlDisplay must have been previously registered for this to have any effect.
     * @param rcd the IRemoteControlDisplay for which the anti-drift mechanism will be enabled
     *     or disabled. Not null.
     * @param wantsSync if true, RemoteControlClient instances which expose their playback position
     *     to the framework will regularly compare the estimated playback position with the actual
     *     position, and will update the IRemoteControlDisplay implementation whenever a drift is
     *     detected.
     */
    CARAPI_(void) RemoteControlDisplayWantsPlaybackPositionSync(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Boolean wantsSync);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw);

    CARAPI_(void) SetRemoteStreamVolume(
        /* [in] */ Int32 vol);

protected:
    /**
     * No-op if the key code for keyEvent is not a valid media key
     * (see {@link #isValidMediaKeyEvent(KeyEvent)})
     * @param keyEvent the key event to send
     */
    CARAPI_(void) DispatchMediaKeyEvent(
        /* [in] */ IKeyEvent* keyEvent);

    /**
     * No-op if the key code for keyEvent is not a valid media key
     * (see {@link #isValidMediaKeyEvent(KeyEvent)})
     * @param keyEvent the key event to send
     */
    CARAPI_(void) DispatchMediaKeyEventUnderWakelock(
        /* [in] */ IKeyEvent* keyEvent);

    /**
     * see AudioManager.registerMediaButtonIntent(PendingIntent pi, ComponentName c)
     * precondition: mediaIntent != null
     */
    CARAPI_(void) RegisterMediaButtonIntent(
        /* [in] */ IPendingIntent* mediaIntent,
        /* [in] */ IComponentName* eventReceiver,
        /* [in] */ IBinder* token);

    /**
     * see AudioManager.unregisterMediaButtonIntent(PendingIntent mediaIntent)
     * precondition: mediaIntent != null, eventReceiver != null
     */
    CARAPI_(void) UnregisterMediaButtonIntent(
        /* [in] */ IPendingIntent* mediaIntent);

    CARAPI_(void) UnregisterMediaButtonIntentAsync(
        /* [in] */ IPendingIntent* mediaIntent);

    /**
     * see AudioManager.registerMediaButtonEventReceiverForCalls(ComponentName c)
     * precondition: c != null
     */
    CARAPI_(void) RegisterMediaButtonEventReceiverForCalls(
        /* [in] */ IComponentName* c);

    /**
     * see AudioManager.unregisterMediaButtonEventReceiverForCalls()
     */
    CARAPI_(void) UnregisterMediaButtonEventReceiverForCalls();

    /**
     * see AudioManager.registerRemoteControlClient(ComponentName eventReceiver, ...)
     * @return the unique ID of the PlayerRecord associated with the RemoteControlClient
     * Note: using this method with rcClient == null is a way to "disable" the IRemoteControlClient
     *     without modifying the RC stack, but while still causing the display to refresh (will
     *     become blank as a result of this)
     */
    CARAPI_(Int32) RegisterRemoteControlClient(
        /* [in] */ IPendingIntent* mediaIntent,
        /* [in] */ IIRemoteControlClient* rcClient,
        /* [in] */ const String& callingPackageName);

    /**
     * see AudioManager.unregisterRemoteControlClient(PendingIntent pi, ...)
     * rcClient is guaranteed non-null
     */
    CARAPI_(void) UnregisterRemoteControlClient(
        /* [in] */ IPendingIntent* mediaIntent,
        /* [in] */ IIRemoteControlClient* rcClient);

    /**
     * Checks if a remote client is active on the supplied stream type. Update the remote stream
     * volume state if found and playing
     * @param streamType
     * @return false if no remote playing is currently playing
     */
    CARAPI_(Boolean) CheckUpdateRemoteStateIfActive(
        /* [in] */ Int32 streamType);

    /**
     * Returns true if the given playback state is considered "active", i.e. it describes a state
     * where playback is happening, or about to
     * @param playState the playback state to evaluate
     * @return true if active, false otherwise (inactive or unknown)
     */
    static CARAPI_(Boolean) IsPlaystateActive(
        /* [in] */ Int32 playState);

    CARAPI_(Int32) GetRemoteStreamMaxVolume();

    CARAPI_(Int32) GetRemoteStreamVolume();

    /**
     * Call to make AudioService reevaluate whether it's in a mode where remote players should
     * have their volume controlled. In this implementation this is only to reset whether
     * VolumePanel should display remote volumes
     */
    CARAPI_(void) PostReevaluateRemote();

private:
    /**
     * Checks a caller's authorization to register an IRemoteControlDisplay.
     * Authorization is granted if one of the following is true:
     * <ul>
     * <li>the caller has android.Manifest.permission.MEDIA_CONTENT_CONTROL permission</li>
     * <li>the caller's listener is one of the enabled notification listeners</li>
     * </ul>
     * @return RCD_REG_FAILURE if it's not safe to proceed with the IRemoteControlDisplay
     *     registration.
     */
    CARAPI_(Int32) CheckRcdRegistrationAuthorization(
        /* [in] */ IComponentName* listenerComp);

    CARAPI_(void) PostReevaluateRemoteControlDisplays();

    CARAPI_(void) OnReevaluateRemoteControlDisplays();

    /**
     * @param comp a non-null ComponentName
     * @param enabledArray may be null
     * @return
     */
    CARAPI_(Boolean) IsComponentInStringArray(
        /* [in] */ IComponentName* comp,
        /* [in] */ ArrayOf<String>* enabledArray);

    static CARAPI_(void) SendMsg(
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 msg,
        /* [in] */ Int32 existingMsgPolicy,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 delay);

    CARAPI_(void) NotifyTopOfAudioFocusStack();

    CARAPI_(void) PropagateFocusLossFromGain_syncAf(
        /* [in] */ Int32 focusGain);

    /**
     * Helper function:
     * Display in the log the current entries in the audio focus stack
     */
    CARAPI_(void) DumpFocusStack(
        /* [in] */ IPrintWriter* pw);

    /**
     * Helper function:
     * Called synchronized on mAudioFocusLock
     * Remove a focus listener from the focus stack.
     * @param clientToRemove the focus listener
     * @param signal if true and the listener was at the top of the focus stack, i.e. it was holding
     *   focus, notify the next item in the stack it gained focus.
     */
    CARAPI_(void) RemoveFocusStackEntry(
        /* [in] */ const String& clientToRemove,
        /* [in] */ Boolean signal);

    /**
     * Helper function:
     * Called synchronized on mAudioFocusLock
     * Remove focus listeners from the focus stack for a particular client when it has died.
     */
    CARAPI_(void) RemoveFocusStackEntryForClient(
        /* [in] */ IBinder* cb);

    /**
     * Helper function:
     * Returns true if the system is in a state where the focus can be reevaluated, false otherwise.
     */
    CARAPI_(Boolean) CanReassignAudioFocus(
        /* [in] */ const String& clientId);

    /**
     * Helper function:
     * Returns true if the system is in a state where the focus can be reevaluated , false otherwise.
     */
    CARAPI_(Boolean) CanReassignAudioFocusFromQchat(
        /* [in] */ Int32 streamType,
        /* [in] */ const String& clientId);

    CARAPI_(void) FilterMediaKeyEvent(
        /* [in] */ IKeyEvent* keyEvent,
        /* [in] */ Boolean needWakeLock);

    /**
     * Handles the dispatching of the media button events to the telephony package.
     * Precondition: mMediaReceiverForCalls != null
     * @param keyEvent a non-null KeyEvent whose key code is one of the supported media buttons
     * @param needWakeLock true if a PARTIAL_WAKE_LOCK needs to be held while this key event
     *     is dispatched.
     */
    CARAPI_(void) DispatchMediaKeyEventForCalls(
        /* [in] */ IKeyEvent* keyEvent,
        /* [in] */ Boolean needWakeLock);

    /**
     * Handles the dispatching of the media button events to one of the registered listeners,
     * or if there was none, broadcast an ACTION_MEDIA_BUTTON intent to the rest of the system.
     * @param keyEvent a non-null KeyEvent whose key code is one of the supported media buttons
     * @param needWakeLock true if a PARTIAL_WAKE_LOCK needs to be held while this key event
     *     is dispatched.
     */
    CARAPI_(void) DispatchMediaKeyEvent(
        /* [in] */ IKeyEvent* keyEvent,
        /* [in] */ Boolean needWakeLock);

    /**
     * Filter key events that may be used for voice-based interactions
     * @param keyEvent a non-null KeyEvent whose key code is that of one of the supported
     *    media buttons that can be used to trigger voice-based interactions.
     * @param needWakeLock true if a PARTIAL_WAKE_LOCK needs to be held while this key event
     *     is dispatched.
     */
    CARAPI_(void) FilterVoiceInputKeyEvent(
        /* [in] */ IKeyEvent* keyEvent,
        /* [in] */ Boolean needWakeLock);

    CARAPI_(void) SendSimulatedMediaButtonEvent(
        /* [in] */ IKeyEvent* originalKeyEvent,
        /* [in] */ Boolean needWakeLock);

    static CARAPI_(Boolean) IsValidMediaKeyEvent(
        /* [in] */ IKeyEvent* keyEvent);

    /**
     * Checks whether the given key code is one that can trigger the launch of voice-based
     *   interactions.
     * @param keyCode the key code associated with the key event
     * @return true if the key is one of the supported voice-based interaction triggers
     */
    static CARAPI_(Boolean) IsValidVoiceInputKeyCode(
        /* [in] */ Int32 keyCode);

    /**
     * Tell the system to start voice-based interactions / voice commands
     */
    CARAPI_(void) StartVoiceBasedInteractions(
        /* [in] */ Boolean needWakeLock);

    /**
     * Helper function:
     * Display in the log the current entries in the remote control focus stack
     */
    CARAPI_(void) DumpRCStack(
        /* [in] */ IPrintWriter* pw);

    /**
     * Helper function:
     * Display in the log the current entries in the remote control stack, focusing
     * on RemoteControlClient data
     */
    CARAPI_(void) DumpRCCStack(
        /* [in] */ IPrintWriter* pw);

    /**
     * Helper function:
     * Display in the log the current entries in the list of remote control displays
     */
    CARAPI_(void) DumpRCDList(
        /* [in] */ IPrintWriter* pw);

    /**
     * Helper function:
     * Push the new media button receiver "near" the top of the PlayerRecord stack.
     * "Near the top" is defined as:
     *   - at the top if the current PlayerRecord at the top is not playing
     *   - below the entries at the top of the stack that correspond to the playing PlayerRecord
     *     otherwise
     * Called synchronized on mPRStack
     * precondition: mediaIntent != null
     * @return true if the top of mPRStack was changed, false otherwise
     */
    CARAPI_(Boolean) PushMediaButtonReceiver_syncPrs(
        /* [in] */ IPendingIntent* mediaIntent,
        /* [in] */ IComponentName* target,
        /* [in] */ IBinder* token);

    /**
     * Helper function:
     * Remove the remote control receiver from the RC focus stack.
     * Called synchronized on mPRStack
     * precondition: pi != null
     */
    CARAPI_(void) RemoveMediaButtonReceiver_syncPrs(
        /* [in] */ IPendingIntent* pi);

    /**
     * Helper function:
     * Called synchronized on mPRStack
     */
    CARAPI_(Boolean) IsCurrentRcController(
        /* [in] */ IPendingIntent* pi);

    /**
     * Update the remote control displays with the new "focused" client generation
     */
    CARAPI_(void) SetNewRcClientOnDisplays_syncRcsCurrc(
        /* [in] */ Int32 newClientGeneration,
        /* [in] */ IPendingIntent* newMediaIntent,
        /* [in] */ Boolean clearing);

    /**
     * Update the remote control clients with the new "focused" client generation
     */
    CARAPI_(void) SetNewRcClientGenerationOnClients_syncRcsCurrc(
        /* [in] */ Int32 newClientGeneration);

    /**
     * Update the displays and clients with the new "focused" client generation and name
     * @param newClientGeneration the new generation value matching a client update
     * @param newMediaIntent the media button event receiver associated with the client.
     *    May be null, which implies there is no registered media button event receiver.
     * @param clearing true if the new client generation value maps to a remote control update
     *    where the display should be cleared.
     */
    CARAPI_(void) SetNewRcClient_syncRcsCurrc(
        /* [in] */ Int32 newClientGeneration,
        /* [in] */ IPendingIntent* newMediaIntent,
        /* [in] */ Boolean clearing);

    /**
     * Called when processing MSG_RCDISPLAY_CLEAR event
     */
    CARAPI_(void) OnRcDisplayClear();

    /**
     * Called when processing MSG_RCDISPLAY_UPDATE event
     */
    CARAPI_(void) OnRcDisplayUpdate(
        /* [in] */ IPlayerRecord* prse,
        /* [in] */ Int32 flags /* USED ?*/);

    /**
     * Called when processing MSG_RCDISPLAY_INIT_INFO event
     * Causes the current RemoteControlClient to send its info (metadata, playstate...) to
     *   a single RemoteControlDisplay, NOT all of them, as with MSG_RCDISPLAY_UPDATE.
     */
    CARAPI_(void) OnRcDisplayInitInfo(
        /* [in] */ IIRemoteControlDisplay* newRcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    /**
     * Helper function:
     * Called synchronized on mPRStack
     */
    CARAPI_(void) ClearRemoteControlDisplay_syncPrs();

    /**
     * Helper function for code readability: only to be called from
     *    checkUpdateRemoteControlDisplay_syncPrs() which checks the preconditions for
     *    this method.
     * Preconditions:
     *    - called synchronized on mPRStack
     *    - mPRStack.isEmpty() is false
     */
    CARAPI_(void) UpdateRemoteControlDisplay_syncPrs(
        /* [in] */ Int32 infoChangedFlags);

    /**
     * Helper function:
     * Called synchronized on mPRStack
     * Check whether the remote control display should be updated, triggers the update if required
     * @param infoChangedFlags the flags corresponding to the remote control client information
     *     that has changed, if applicable (checking for the update conditions might trigger a
     *     clear, rather than an update event).
     */
    CARAPI_(void) CheckUpdateRemoteControlDisplay_syncPrs(
        /* [in] */ Int32 infoChangedFlags);

    /**
     * Plug each registered display into the specified client
     * @param rcc, guaranteed non null
     */
    CARAPI_(void) PlugRemoteControlDisplaysIntoClient_syncPrs(
        /* [in] */ IIRemoteControlClient* rcc);

    CARAPI_(void) EnableRemoteControlDisplayForClient_syncRcStack(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Boolean enabled);

    /**
     * Is the remote control display interface already registered
     * @param rcd
     * @return true if the IRemoteControlDisplay is already in the list of displays
     */
    CARAPI_(Boolean) RcDisplayIsPluggedIn_syncRcStack(
        /* [in] */ IIRemoteControlDisplay* rcd);

    /**
     * Register an IRemoteControlDisplay.
     * Notify all IRemoteControlClient of the new display and cause the RemoteControlClient
     * at the top of the stack to update the new display with its information.
     * @see android.media.IAudioService#registerRemoteControlDisplay(android.media.IRemoteControlDisplay, int, int)
     * @param rcd the IRemoteControlDisplay to register. No effect if null.
     * @param w the maximum width of the expected bitmap. Negative or zero values indicate this
     *   display doesn't need to receive artwork.
     * @param h the maximum height of the expected bitmap. Negative or zero values indicate this
     *   display doesn't need to receive artwork.
     * @param listenerComp the component for the listener interface, may be null if it's not needed
     *   to verify it belongs to one of the enabled notification listeners
     */
    CARAPI_(void) RegisterRemoteControlDisplay_int(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IComponentName* listenerComp);

    // handler for MSG_RCC_NEW_VOLUME_OBS
    CARAPI_(void) OnRegisterVolumeObserverForRcc(
        /* [in] */ Int32 rccId,
        /* [in] */ IIRemoteVolumeObserver* rvo);

    CARAPI_(void) SendVolumeUpdateToRemote(
        /* [in] */ Int32 rccId,
        /* [in] */ Int32 direction);

    CARAPI_(void) OnReevaluateRemote();

public:
    AutoPtr<MyBroadcastReceiver> mKeyEventDone;

protected:
    /** Debug remote control client/display feature */
    static Boolean DEBUG_RC;
    /** Debug volumes */
    static Boolean DEBUG_VOL;

    static const String IN_VOICE_COMM_FOCUS_ID;

private:
    static const String TAG;

    /** Used to alter media button redirection when the phone is ringing. */
    Boolean mIsRinging;

    AutoPtr<IPowerManagerWakeLock> mMediaEventWakeLock;
    AutoPtr<MediaEventHandler> mEventHandler;
    IContext* mContext;
    AutoPtr<IContentResolver> mContentResolver;
    AutoPtr<IAudioServiceVolumeController> mVolumeController;
    AutoPtr<IAppOpsManager> mAppOps;
    AutoPtr<IKeyguardManager> mKeyguardManager;
    IAudioService* mAudioService;
    AutoPtr<NotificationListenerObserver> mNotifListenerObserver;

    static AutoPtr<IUri> ENABLED_NOTIFICATION_LISTENERS_URI;

    static const Int32 RCD_REG_FAILURE;
    static const Int32 RCD_REG_SUCCESS_PERMISSION;
    static const Int32 RCD_REG_SUCCESS_ENABLED_NOTIF;

    // event handler messages
    static const Int32 MSG_RCDISPLAY_CLEAR;
    static const Int32 MSG_RCDISPLAY_UPDATE;
    static const Int32 MSG_REEVALUATE_REMOTE;
    static const Int32 MSG_RCC_NEW_PLAYBACK_INFO;
    static const Int32 MSG_RCC_NEW_VOLUME_OBS;
    static const Int32 MSG_RCC_NEW_PLAYBACK_STATE;
    static const Int32 MSG_RCC_SEEK_REQUEST;
    static const Int32 MSG_RCC_UPDATE_METADATA;
    static const Int32 MSG_RCDISPLAY_INIT_INFO;
    static const Int32 MSG_REEVALUATE_RCD;
    static const Int32 MSG_UNREGISTER_MEDIABUTTONINTENT;
    static const Int32 MSG_RCC_SET_BROWSED_PLAYER;
    static const Int32 MSG_RCC_SET_PLAY_ITEM;
    static const Int32 MSG_RCC_GET_NOW_PLAYING_ENTRIES;

    // sendMsg() flags
    /** If the msg is already queued, replace it with this one. */
    static const Int32 SENDMSG_REPLACE;
    /** If the msg is already queued, ignore this one and leave the old. */
    static const Int32 SENDMSG_NOOP;
    /** If the msg is already queued, queue this one and leave the old. */
    static const Int32 SENDMSG_QUEUE;

    static Object mAudioFocusLock;

    static Object mRingingLock;

    AutoPtr<MyPhoneStateListener> mPhoneStateListener;

    AutoPtr<IStack> mFocusStack;

    static const String CLIENT_ID_QCHAT;

    /**
     * The different actions performed in response to a voice button key event.
     */
    static const Int32 VOICEBUTTON_ACTION_DISCARD_CURRENT_KEY_PRESS;
    static const Int32 VOICEBUTTON_ACTION_START_VOICE_INPUT;
    static const Int32 VOICEBUTTON_ACTION_SIMULATE_KEY_PRESS;

    Object mVoiceEventLock;
    Boolean mVoiceButtonDown;
    Boolean mVoiceButtonHandled;

    static const Int32 WAKELOCK_RELEASE_ON_FINISHED; //magic number

    // only set when wakelock was acquired, no need to check value when received
    static const String EXTRA_WAKELOCK_ACQUIRED;

    /**
     * Synchronization on mCurrentRcLock always inside a block synchronized on mPRStack
     */
    Object mCurrentRcLock;
    /**
     * The one remote control client which will receive a request for display information.
     * This object may be null.
     * Access protected by mCurrentRcLock.
     */
    AutoPtr<IIRemoteControlClient> mCurrentRcClient;
    /**
     * The PendingIntent associated with mCurrentRcClient. Its value is irrelevant
     * if mCurrentRcClient is null
     */
    AutoPtr<IPendingIntent> mCurrentRcClientIntent;

    static const Int32 RC_INFO_NONE;
    static const Int32 RC_INFO_ALL;

    /**
     * A monotonically increasing generation counter for mCurrentRcClient.
     * Only accessed with a lock on mCurrentRcLock.
     * No value wrap-around issues as we only act on equal values.
     */
    Int32 mCurrentRcClientGen;


    /**
     * Internal cache for the playback information of the RemoteControlClient whose volume gets to
     * be controlled by the volume keys ("main"), so we don't have to iterate over the RC stack
     * every time we need this info.
     */
    AutoPtr<IPlayerRecordRemotePlaybackState> mMainRemote;
    /**
     * Indicates whether the "main" RemoteControlClient is considered active.
     * Use synchronized on mMainRemote.
     */
    Boolean mMainRemoteIsActive;
    /**
     * Indicates whether there is remote playback going on. True even if there is no "active"
     * remote playback (mMainRemoteIsActive is false), but a RemoteControlClient has declared it
     * handles remote playback.
     * Use synchronized on mMainRemote.
     */
    Boolean mHasRemotePlayback;

    /**
     * The stack of remote control event receivers.
     * All read and write operations on mPRStack are synchronized.
     */
    AutoPtr<IStack> mPRStack;

    /**
     * The component the telephony package can register so telephony calls have priority to
     * handle media button events
     */
    AutoPtr<IComponentName> mMediaReceiverForCalls;

    /**
     * The remote control displays.
     * Access synchronized on mPRStack
     */
    AutoPtr<IArrayList> mRcDisplays;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_MEDIAFOCUSCONTROL_H__
