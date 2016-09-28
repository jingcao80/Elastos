#ifndef __ELASTOS_DROID_MEDIA_CAUDIOSERVICE_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOSERVICE_H__

#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Media_CAudioService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/MediaFocusControl.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/view/OrientationEventListener.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::Bluetooth::IBluetoothA2dp;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothProfileServiceListener;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiPlaybackClient;
using Elastos::Droid::Hardware::Hdmi::IHdmiPlaybackClientDisplayStatusCallback;
using Elastos::Droid::Hardware::Hdmi::IHdmiTvClient;
using Elastos::Droid::Media::AudioPolicy::IAudioPolicyConfig;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerTouchExplorationStateChangeListener;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::OrientationEventListener;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::Thread;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::IConcurrentHashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * The implementation of the volume manager service.
 * <p>
 * This implementation focuses on delivering a responsive UI. Most methods are
 * asynchronous to external calls. For example, the task of setting a volume
 * will update our internal state, but in a separate thread will set the system
 * volume and later persist to the database. Similarly, setting the ringer mode
 * will update the state and broadcast a change and in a separate thread later
 * persist the ringer mode.
 *
 * @hide
 */
CarClass(CAudioService)
    , public Object
    , public IAudioService
    , public IIAudioService
    , public IBinder
{
public:
    // StreamVolumeCommand contains the information needed to defer the process of
    // setStreamVolume() in case the user has to acknowledge the safe volume warning message.
    class StreamVolumeCommand
        : public Object
    {
    public:
        StreamVolumeCommand(
            /* [in] */ Int32 streamType,
            /* [in] */ Int32 index,
            /* [in] */ Int32 flags,
            /* [in] */ Int32 device)
            : mStreamType(streamType)
            , mIndex(index)
            , mFlags(flags)
            , mDevice(device)
        {}

        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Int32 mStreamType;
        Int32 mIndex;
        Int32 mFlags;
        Int32 mDevice;
    };

    class LoadSoundEffectReply
        : public Object
    {
    public:
        LoadSoundEffectReply()
            : mStatus(1)
        {}

    public:
        Int32 mStatus;
    };

    class SoundPoolListenerThread
         : public Thread
    {
    public:
        SoundPoolListenerThread(
            /* [in] */ CAudioService* host)
            : mHost(host)
        {
            constructor(String("SoundPoolListenerThread"));
        }

        CARAPI Run();

    private:
        CAudioService* mHost;
    };

    class VolumeStreamState
        : public Object
        , public IAudioServiceVolumeStreamState
    {
    public:
        class VolumeDeathHandler
            : public Object
            , public IProxyDeathRecipient
        {
        public:
            CAR_INTERFACE_DECL()

            VolumeDeathHandler(
                /* [in] */ VolumeStreamState* host,
                /* [in] */ IBinder* cb)
                : mCallback(cb)
                , mMuteCount(0)
                , mHost(host)
            {}

            CARAPI ProxyDied();

            // must be called while synchronized on parent VolumeStreamState
            CARAPI Mute(
                /* [in] */ Boolean state);

        public:
            AutoPtr<IBinder> mCallback; // To be notified of client's death
            Int32 mMuteCount; // Number of active mutes for this client

        private:
            VolumeStreamState* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        VolumeStreamState(
            /* [in] */ CAudioService* host,
            /* [in] */ const String& settingName,
            /* [in] */ Int32 streamType);

        CARAPI GetSettingNameForDevice(
            /* [in] */ Int32 device,
            /* [out] */ String* result);

        CARAPI ReadSettings();

        CARAPI ApplyDeviceVolume(
            /* [in] */ Int32 device);

        CARAPI ApplyAllVolumes();

        CARAPI AdjustIndex(
            /* [in] */ Int32 deltaIndex,
            /* [in] */ Int32 device,
            /* [out] */ Boolean* result);

        CARAPI SetIndex(
            /* [in] */ Int32 index,
            /* [in] */ Int32 device,
            /* [out] */ Boolean* result);

        CARAPI GetIndex(
            /* [in] */ Int32 device,
            /* [out] */ Int32* result);

        CARAPI GetMaxIndex(
            /* [out] */ Int32* result);

        CARAPI SetAllIndexes(
            /* [in] */ IAudioServiceVolumeStreamState* srcStream);

        CARAPI SetAllIndexesToMax();

        CARAPI Mute(
            /* [in] */ IBinder* cb,
            /* [in] */ Boolean state);

        CARAPI GetStreamType(
            /* [out] */ Int32* result);

        CARAPI CheckFixedVolumeDevices();

        CARAPI_(Boolean) IsMuted();

        CARAPI_(void) Dump(
            /* [in] */ IPrintWriter* pw);

    private:
        CARAPI_(Int32) GetValidIndex(
            /* [in] */ Int32 index);

        CARAPI_(Int32) MuteCount();


        // only called by mute() which is already synchronized
        CARAPI_(AutoPtr<VolumeDeathHandler>) GetDeathHandler(
            /* [in] */ IBinder* cb,
            /* [in] */ Boolean state);

    public:
        Int32 mIndexMax;
        Int32 mStreamType;
        //TODO: AutoPtr<IConcurrentHashMap> mIndex;
        AutoPtr<IHashMap> mIndex;
        AutoPtr<IArrayList> mDeathHandlers; //handles mute/solo clients death

    private:
        CAudioService* mHost;

        String mVolumeIndexSettingName;
    };

    class VolumeController
        : public Object
        , public IAudioServiceVolumeController
    {
    public:
        CAR_INTERFACE_DECL()

        VolumeController(
            /* [in] */ CAudioService* host);

        CARAPI SetController(
            /* [in] */ IIVolumeController* controller);

        CARAPI LoadSettings(
            /* [in] */ IContentResolver* cr);

        CARAPI SuppressAdjustment(
            /* [in] */ Int32 resolvedStream,
            /* [in] */ Int32 flags,
            /* [out] */ Boolean* result);

        CARAPI SetVisible(
            /* [in] */ Boolean visible);

        CARAPI IsSameBinder(
            /* [in] */ IIVolumeController* controller,
            /* [out] */ Boolean* result);

        CARAPI AsBinder(
            /* [out] */ IBinder** result);

        CARAPI PostDisplaySafeVolumeWarning(
            /* [in] */ Int32 flags);

        CARAPI PostVolumeChanged(
            /* [in] */ Int32 streamType,
            /* [in] */ Int32 flags);

        CARAPI PostMasterVolumeChanged(
            /* [in] */ Int32 flags);

        CARAPI PostMasterMuteChanged(
            /* [in] */ Int32 flags);

        CARAPI SetLayoutDirection(
            /* [in] */ Int32 layoutDirection);

        CARAPI PostDismiss();

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        static CARAPI_(AutoPtr<IBinder>) Binder(
            /* [in] */ IIVolumeController* controller);

    private:
        CAudioService* mHost;
        static const String TAG;

        AutoPtr<IIVolumeController> mController;
        Boolean mVisible;
        Int64 mNextLongPress;
        Int32 mLongPressTimeout;
    };

    /**
     * Interface for system components to get some extra functionality through
     * LocalServices.
     */
    class AudioServiceInternal
        : public Object
        , public IAudioManagerInternal
    {
    public:
        CAR_INTERFACE_DECL()

        AudioServiceInternal(
            /* [in] */ CAudioService* host)
            : mHost(host)
        {}

        CARAPI AdjustSuggestedStreamVolumeForUid(
            /* [in] */ Int32 streamType,
            /* [in] */ Int32 direction,
            /* [in] */ Int32 flags,
            /* [in] */ const String& callingPackage,
            /* [in] */ Int32 uid);

        CARAPI AdjustStreamVolumeForUid(
            /* [in] */ Int32 streamType,
            /* [in] */ Int32 direction,
            /* [in] */ Int32 flags,
            /* [in] */ const String& callingPackage,
            /* [in] */ Int32 uid);

        CARAPI SetStreamVolumeForUid(
            /* [in] */ Int32 streamType,
            /* [in] */ Int32 direction,
            /* [in] */ Int32 flags,
            /* [in] */ const String& callingPackage,
            /* [in] */ Int32 uid);

    private:
        CAudioService* mHost;
    };

    class AudioPolicyProxy
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        AudioPolicyProxy(
            /* [in] */ CAudioService* host,
            /* [in] */ IAudioPolicyConfig* config,
            /* [in] */ IBinder* cb)
            : mToken(cb)
            , mConfig(config)
            , mHost(host)
        {}

        CARAPI ProxyDied();

    public:
        AutoPtr<IBinder> mToken;
        AutoPtr<IAudioPolicyConfig> mConfig;

    private:
        static const String TAG;
        CAudioService* mHost;
    };

private:
    class AudioOrientationEventListener
        : public OrientationEventListener
    {
    public:
        CARAPI constructor(
            /* [in] */ CAudioService* host,
            /* [in] */ IContext* context);

        CARAPI Run();

    private:
        CAudioService* mHost;
    };

    class ForceControlStreamClient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        ForceControlStreamClient(
            /* [in] */ CAudioService* host,
            /* [in] */ IBinder* cb);

        CARAPI ProxyDied();

        CARAPI ReleaseResources();

    private:
        CAudioService* mHost;
        AutoPtr<IBinder> mCb; // To be notified of client's death
    };

    class RmtSbmxFullVolDeathHandler
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        RmtSbmxFullVolDeathHandler(
            /* [in] */ CAudioService* host,
            /* [in] */ IBinder* cb);

        CARAPI ProxyDied();

        CARAPI IsHandlerFor(
            /* [in] */ IBinder* cb,
            /* [out] */ Boolean* result);

        CARAPI Forget();

    private:
        CAudioService* mHost;
        AutoPtr<IBinder> mICallback; // To be notified of client's death
    };

    class SetModeDeathHandler
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        SetModeDeathHandler(
            /* [in] */ CAudioService* host,
            /* [in] */ IBinder* cb,
            /* [in] */ Int32 pid)
            : mPid(pid)
            , mHost(host)
            , mCb(cb)
            , mMode(IAudioSystem::MODE_NORMAL)
        {}

        CARAPI ProxyDied();

        CARAPI GetPid(
            /* [out] */ Int32* result);

        CARAPI SetMode(
            /* [in] */ Int32 mode);

        CARAPI GetMode(
            /* [out] */ Int32* result);

        CARAPI GetBinder(
            /* [out] */ IBinder** result);

    public:
        Int32 mPid;

    private:
        CAudioService* mHost;
        AutoPtr<IBinder> mCb; // To be notified of client's death
        Int32 mMode; // Current mode set by this client
    };

    class SoundPoolCallback
        : public Object
        , public ISoundPoolOnLoadCompleteListener
    {
    public:
        CAR_INTERFACE_DECL()

        SoundPoolCallback(
            /* [in] */ CAudioService* host);

        CARAPI OnLoadComplete(
            /* [in] */ ISoundPool* soundPool,
            /* [in] */ Int32 sampleId,
            /* [in] */ Int32 status);

        CARAPI Status(
            /* [out] */ Int32* result);

        CARAPI SetSamples(
            /* [in] */ ArrayOf<Int32>* samples);

    private:
        CAudioService* mHost;
        Int32 mStatus; // 1 means neither error nor last sample loaded yet
        AutoPtr<IList> mSamples;
    };

    class ScoClient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        ScoClient(
            /* [in] */ CAudioService* host,
            /* [in] */ IBinder* cb);

        CARAPI ProxyDied();

        CARAPI IncCount(
            /* [in] */ Int32 scoAudioMode);

        CARAPI DecCount();

        CARAPI ClearCount(
            /* [in] */ Boolean stopSco);

        CARAPI GetCount(
            /* [out] */ Int32* result);

        CARAPI GetBinder(
            /* [out] */ IBinder** result);

        CARAPI GetPid(
            /* [out] */ Int32* result);

        CARAPI TotalCount(
            /* [out] */ Int32* result);

    private:
        CARAPI_(void) RequestScoState(
            /* [in] */ Int32 state,
            /* [in] */ Int32 scoAudioMode);

    private:
        CAudioService* mHost;
        AutoPtr<IBinder> mCb; // To be notified of client's death
        Int32 mCreatorPid;
        Int32 mStartcount; // number of SCO connections started by this client
    };

    /** Thread that handles native AudioSystem control. */
    class AudioSystemThread
         : public Thread
    {
    public:
        AudioSystemThread(
            /* [in] */ CAudioService* host)
            : mHost(host)
        {
            constructor(String("AudioSystemThread"));
        }

        CARAPI Run();

    private:
        CAudioService* mHost;
    };

    /** Handles internal volume messages in separate volume thread. */
    class AudioHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CAudioService::AudioHandler")

        AudioHandler(
            /* [in] */ CAudioService* host)
            : mHost(host)
        {
        }

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI_(void) CleanupPlayer(
            /* [in] */ IMediaPlayer* mp);

    private:
        CARAPI_(void) SetDeviceVolume(
            /* [in] */ VolumeStreamState* streamState,
            /* [in] */ Int32 device);

        CARAPI_(void) SetAllVolumes(
            /* [in] */ VolumeStreamState* streamState);

        CARAPI_(void) PersistVolume(
            /* [in] */ VolumeStreamState* streamState,
            /* [in] */ Int32 device);

        CARAPI_(void) PersistRingerMode(
            /* [in] */ Int32 ringerMode);

        CARAPI_(Boolean) OnLoadSoundEffects();

        /**
         *  Unloads samples from the sound pool.
         *  This method can be called to free some memory when
         *  sound effects are disabled.
         */
        CARAPI_(void) OnUnloadSoundEffects();

        CARAPI_(void) OnPlaySoundEffect(
            /* [in] */ Int32 effectType,
            /* [in] */ Int32 volume);

        CARAPI_(void) SetForceUse(
            /* [in] */ Int32 usage,
            /* [in] */ Int32 config);

        CARAPI_(void) OnPersistSafeVolumeState(
            /* [in] */ Int32 state);

    private:
        CAudioService* mHost;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CAudioService::SettingsObserver")

        SettingsObserver(
            /* [in] */ CAudioService* host);

        CARAPI constructor();

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CAudioService* mHost;
    };

    /**
     * Receiver for misc intent broadcasts the Phone app cares about.
     */
    class AudioServiceBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CAudioService::AudioServiceBroadcastReceiver")

        AudioServiceBroadcastReceiver(
            /* [in] */ CAudioService* host)
                : mHost(host)
            {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CAudioService* mHost;
    };

    //==========================================================================================
    // Hdmi Cec system audio mode.
    // If Hdmi Cec's system audio mode is on, audio service should notify volume change
    // to HdmiControlService so that audio recevier can handle volume change.
    //==========================================================================================
    class MyDisplayStatusCallback
        : public Object
        , public IHdmiPlaybackClientDisplayStatusCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyDisplayStatusCallback(
            /* [in] */ CAudioService* host)
                : mHost(host)
            {}

        CARAPI OnComplete(
            /* [in] */ Int32 status);

    private:
        CAudioService* mHost;
    };

    //==========================================================================================
    // Accessibility: taking touch exploration into account for selecting the default
    //   stream override timeout when adjusting volume
    //==========================================================================================
    class StreamOverride
        : public Object
        , public IAccessibilityManagerTouchExplorationStateChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        StreamOverride()
        {}

        CARAPI OnTouchExplorationStateChanged(
            /* [in] */ Boolean enabled);

        static CARAPI Init(
            /* [in] */ IContext* ctxt);

    private:
        static CARAPI_(void) UpdateDefaultStreamOverrideDelay(
            /* [in] */ Boolean touchExploreEnabled);

    public:
        static Int32 sDelayMs;

    private:
        // AudioService.getActiveStreamType() will return:
        // - STREAM_NOTIFICATION on tablets during this period after a notification stopped
        // - STREAM_MUSIC on phones during this period after music or talkback/voice search prompt
        // stopped
        static const Int32 DEFAULT_STREAM_TYPE_OVERRIDE_DELAY_MS;
        static const Int32 TOUCH_EXPLORE_STREAM_TYPE_OVERRIDE_DELAY_MS;
    };

    class MyAudioSystemCallback
        : public Object
        , public IAudioSystemErrorCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyAudioSystemCallback(
            /* [in] */ CAudioService* host)
                : mHost(host)
            {}

        CARAPI OnError(
            /* [in] */ Int32 error);

    private:
        CAudioService* mHost;
    };

    class AudioHandlerOnCompletionListener
        : public Object
        , public IMediaPlayerOnCompletionListener
    {
    public:
        CAR_INTERFACE_DECL()

        AudioHandlerOnCompletionListener(
            /* [in] */ AudioHandler* host)
                : mHost(host)
            {}

        CARAPI OnCompletion(
            /* [in] */ IMediaPlayer* mp);

    private:
        AudioHandler* mHost;
    };

    class AudioHandlerOnErrorListener
        : public Object
        , public IMediaPlayerOnErrorListener
    {
    public:
        CAR_INTERFACE_DECL()

        AudioHandlerOnErrorListener(
            /* [in] */ AudioHandler* host)
                : mHost(host)
            {}

        CARAPI OnError(
            /* [in] */ IMediaPlayer* mp,
            /* [in] */ Int32 what,
            /* [in] */ Int32 extra,
            /* [out] */ Boolean* result);

    private:
        AudioHandler* mHost;
    };

    class SetVolumeControllerDeathRecipient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        SetVolumeControllerDeathRecipient(
            /* [in] */ CAudioService* host,
            /* [in] */ IIVolumeController* controller)
            : mHost(host)
            , mController(controller)
        {}

        CARAPI ProxyDied();

    private:
        CAudioService* mHost;
        AutoPtr<IIVolumeController> mController;
    };

    class MediaPlayerInfo
        : public Object
    {
    public:
        MediaPlayerInfo(
            /* [in] */ const String& packageName,
            /* [in] */ Boolean isfocussed);

        CARAPI IsFocussed(
            /* [out] */ Boolean* result);

        CARAPI SetFocus(
            /* [in] */ Boolean focus);

        CARAPI GetPackageName(
            /* [out] */ String* result);

    private:
        String mPackageName;
        Boolean mIsfocussed;
    };

    class BluetoothProfileServiceListener
        : public Object
        , public IBluetoothProfileServiceListener
    {
    public:
        CAR_INTERFACE_DECL()

        BluetoothProfileServiceListener(
            /* [in] */ CAudioService* host)
            : mHost(host)
        {}

        CARAPI OnServiceConnected(
            /* [in] */ Int32 profile,
            /* [in] */ IBluetoothProfile* proxy);

        CARAPI OnServiceDisconnected(
            /* [in] */ Int32 profile);

    private:
        CAudioService* mHost;
    };

public:
    CAudioService();

    virtual ~CAudioService();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SystemReady();

    CARAPI OnSystemReady();

    CARAPI AddMediaPlayerAndUpdateRemoteController(
        /* [in] */ const String& packageName);

    CARAPI UpdateRemoteControllerOnExistingMediaPlayers();

    CARAPI RemoveMediaPlayerAndUpdateRemoteController(
        /* [in] */ const String& packageName);

    static CARAPI StreamToString(
        /* [in] */ Int32 stream,
        /* [out] */ String* result);

    ///////////////////////////////////////////////////////////////////////////
    // IPC methods
    ///////////////////////////////////////////////////////////////////////////
    /** @see AudioManager#adjustVolume(int, int) */
    CARAPI AdjustSuggestedStreamVolume(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 suggestedStreamType,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage);

    /** @see AudioManager#adjustStreamVolume(int, int, int) */
    CARAPI AdjustStreamVolume(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage);

    /** @see AudioManager#adjustMasterVolume(int, int) */
    CARAPI AdjustMasterVolume(
        /* [in] */ Int32 steps,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage);

    /** @see AudioManager#setStreamVolume(int, int, int) */
    CARAPI SetStreamVolume(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 index,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage);

    /** @see AudioManager#forceVolumeControlStream(int) */
    CARAPI ForceVolumeControlStream(
        /* [in] */ Int32 streamType,
        /* [in] */ IBinder* cb);

    /** @see AudioManager#setStreamSolo(int, Boolean) */
    CARAPI SetStreamSolo(
        /* [in] */ Int32 streamType,
        /* [in] */ Boolean state,
        /* [in] */ IBinder* cb);

    /** @see AudioManager#setStreamMute(int, Boolean) */
    CARAPI SetStreamMute(
        /* [in] */ Int32 streamType,
        /* [in] */ Boolean state,
        /* [in] */ IBinder* cb);

    /** get stream mute state. */
    CARAPI IsStreamMute(
        /* [in] */ Int32 streamType,
        /* [out] */ Boolean* result);

    CARAPI ForceRemoteSubmixFullVolume(
        /* [in] */ Boolean startForcing,
        /* [in] */ IBinder* cb);

    /** @see AudioManager#setMasterMute(Boolean, int) */
    CARAPI SetMasterMute(
        /* [in] */ Boolean state,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage,
        /* [in] */ IBinder* cb);

    /** get master mute state. */
    CARAPI IsMasterMute(
        /* [out] */ Boolean* result);

    /** @see AudioManager#getStreamVolume(int) */
    CARAPI GetStreamVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    CARAPI GetMasterVolume(
        /* [out] */ Int32* result);

    CARAPI SetMasterVolume(
        /* [in] */ Int32 volume,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage);

    /** @see AudioManager#getStreamMaxVolume(int) */
    CARAPI GetStreamMaxVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    CARAPI GetMasterMaxVolume(
        /* [out] */ Int32* result);

    /** Get last audible volume before stream was muted. */
    CARAPI GetLastAudibleStreamVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    /** Get last audible master volume before it was muted. */
    CARAPI GetLastAudibleMasterVolume(
        /* [out] */ Int32* result);

    /** @see AudioManager#getMasterStreamType()  */
    CARAPI GetMasterStreamType(
        /* [out] */ Int32* result);

    /** @see AudioManager#setMicrophoneMute(Boolean) */
    CARAPI SetMicrophoneMute(
        /* [in] */ Boolean on,
        /* [in] */ const String& callingPackage);

    /** @see AudioManager#getRingerMode() */
    CARAPI GetRingerMode(
        /* [out] */ Int32* result);

    /** @see AudioManager#setRingerMode(int) */
    CARAPI SetRingerMode(
        /* [in] */ Int32 ringerMode,
        /* [in] */ Boolean checkZen);

    /** @see AudioManager#shouldVibrate(int) */
    CARAPI ShouldVibrate(
        /* [in] */ Int32 vibrateType,
        /* [out] */ Boolean* result);

    /** @see AudioManager#getVibrateSetting(int) */
    CARAPI GetVibrateSetting(
        /* [in] */ Int32 vibrateType,
        /* [out] */ Int32* result);

    /** @see AudioManager#setVibrateSetting(int, int) */
    CARAPI SetVibrateSetting(
        /* [in] */ Int32 vibrateType,
        /* [in] */ Int32 vibrateSetting);

    /**
     * @see #setVibrateSetting(int, int)
     */
    static CARAPI GetValueForVibrateSetting(
        /* [in] */ Int32 existingValue,
        /* [in] */ Int32 vibrateType,
        /* [in] */ Int32 vibrateSetting,
        /* [out] */ Int32* result);

    /** @see AudioManager#setMode(int) */
    CARAPI SetMode(
        /* [in] */ Int32 mode,
        /* [in] */ IBinder* cb);

    /** @see AudioManager#getMode() */
    CARAPI GetMode(
        /* [out] */ Int32* result);

    /** @see AudioManager#playSoundEffect(int) */
    CARAPI PlaySoundEffect(
        /* [in] */ Int32 effectType);

    /** @see AudioManager#playSoundEffect(int, float) */
    CARAPI PlaySoundEffectVolume(
        /* [in] */ Int32 effectType,
        /* [in] */ Float volume);

    /**
     * Loads samples into the soundpool.
     * This method must be called at first when sound effects are enabled
     */
    CARAPI LoadSoundEffects(
        /* [out] */ Boolean* result);

    /**
     *  Unloads samples from the sound pool.
     *  This method can be called to free some memory when
     *  sound effects are disabled.
     */
    CARAPI UnloadSoundEffects();

    /** @see AudioManager#reloadAudioSettings() */
    CARAPI ReloadAudioSettings();

    /** @see AudioManager#setSpeakerphoneOn(Boolean) */
    CARAPI SetSpeakerphoneOn(
        /* [in] */ Boolean on);

    /** @see AudioManager#isSpeakerphoneOn() */
    CARAPI IsSpeakerphoneOn(
        /* [out] */ Boolean* result);

    /** @see AudioManager#setBluetoothScoOn(Boolean) */
    CARAPI SetBluetoothScoOn(
        /* [in] */ Boolean on);

    /** @see AudioManager#isBluetoothScoOn() */
    CARAPI IsBluetoothScoOn(
        /* [out] */ Boolean* result);

    /** @see AudioManager#setBluetoothA2dpOn(Boolean) */
    CARAPI SetBluetoothA2dpOn(
        /* [in] */ Boolean on);

    /** @see AudioManager#isBluetoothA2dpOn() */
    CARAPI IsBluetoothA2dpOn(
        /* [out] */ Boolean* result);

    /** @see AudioManager#startBluetoothSco() */
    CARAPI StartBluetoothSco(
        /* [in] */ IBinder* cb,
        /* [in] */ Int32 targetSdkVersion);

    /** @see AudioManager#startBluetoothScoVirtualCall() */
    CARAPI StartBluetoothScoVirtualCall(
        /* [in] */ IBinder* cb);

    /** @see AudioManager#stopBluetoothSco() */
    CARAPI StopBluetoothSco(
        /* [in] */ IBinder* cb);

    CARAPI ClearAllScoClients(
        /* [in] */ Int32 exceptPid,
        /* [in] */ Boolean stopSco);

    CARAPI IsStreamAffectedByMute(
        /* [in] */ Int32 streamType,
        /* [out] */ Boolean* result);

    CARAPI SetWiredDeviceConnectionState(
        /* [in] */ Int32 device,
        /* [in] */ Int32 state,
        /* [in] */ const String& name);

    CARAPI SetBluetoothA2dpDeviceConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 state,
        /* [in] */ Int32 profile,
        /* [out] */ Int32* result);

    CARAPI AvrcpSupportsAbsoluteVolume(
        /* [in] */ const String& address,
        /* [in] */ Boolean support);

    //==========================================================================================
    // RemoteControlDisplay / RemoteControlClient / Remote info
    //==========================================================================================
    CARAPI RegisterRemoteController(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IComponentName* listenerComp,
        /* [out] */ Boolean* result);

    CARAPI RegisterRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [out] */ Boolean* result);

    CARAPI UnregisterRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd);

    CARAPI RemoteControlDisplayUsesBitmapSize(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI RemoteControlDisplayWantsPlaybackPositionSync(
        /* [in] */ IIRemoteControlDisplay* rcd,
        /* [in] */ Boolean wantsSync);

    CARAPI SetRemoteControlClientPlayItem(
        /* [in] */ Int64 uid,
        /* [in] */ Int32 scope);

    CARAPI GetRemoteControlClientNowPlayingEntries();

    CARAPI SetRemoteControlClientBrowsedPlayer();

    //==========================================================================================
    // Audio Focus
    //==========================================================================================
    CARAPI RequestAudioFocus(
        /* [in] */ Int32 mainStreamType,
        /* [in] */ Int32 durationHint,
        /* [in] */ IBinder* cb,
        /* [in] */ IIAudioFocusDispatcher* fd,
        /* [in] */ const String& clientId,
        /* [in] */ const String& callingPackageName,
        /* [out] */ Int32* result);

    CARAPI AbandonAudioFocus(
        /* [in] */ IIAudioFocusDispatcher* fd,
        /* [in] */ const String& clientId,
        /* [out] */ Int32* result);

    CARAPI UnregisterAudioFocusClient(
        /* [in] */ const String& clientId);

    CARAPI GetCurrentAudioFocus(
        /* [out] */ Int32* result);

    //==========================================================================================
    // Device orientation
    //==========================================================================================
    // Handles request to override default use of A2DP for media.
    CARAPI SetBluetoothA2dpOnInt(
        /* [in] */ Boolean on);

    //==========================================================================================
    // Camera shutter sound policy.
    // config_camera_sound_forced configuration option in config.xml defines if the camera shutter
    // sound is forced (sound even if the device is in silent mode) or not. This option is false by
    // default and can be overridden by country specific overlay in values-mccXXX/config.xml.
    //==========================================================================================
    // called by android.hardware.Camera to populate CameraInfo.canDisableShutterSound
    CARAPI IsCameraSoundForced(
        /* [out] */ Boolean* result);

    //==========================================================================================
    // Audio policy management
    //==========================================================================================
    CARAPI RegisterAudioPolicy(
        /* [in] */ IAudioPolicyConfig* policyConfig,
        /* [in] */ IBinder* cb,
        /* [out] */ Boolean* result);

    CARAPI UnregisterAudioPolicyAsync(
        /* [in] */ IBinder* cb);

    CARAPI StartBluetoothScoInt(
        /* [in] */ IBinder* cb,
        /* [in] */ Int32 scoAudioMode);

    CARAPI IsStreamAffectedByRingerMode(
        /* [in] */ Int32 streamType,
        /* [out] */ Boolean* result);

    CARAPI UpdateRingerModeAffectedStreams(
        /* [out] */ Boolean* result);

    CARAPI CheckAudioSettingsPermission(
        /* [in] */ const String& method,
        /* [out] */ Boolean* result);

    CARAPI SetRemoteStreamVolume(
        /* [in] */ Int32 index);

    CARAPI SetRingtonePlayer(
        /* [in] */ IIRingtonePlayer* player);

    CARAPI GetRingtonePlayer(
        /* [out] */ IIRingtonePlayer** result);

    CARAPI StartWatchingRoutes(
        /* [in] */ IIAudioRoutesObserver* observer,
        /* [out] */ IAudioRoutesInfo** result);

    CARAPI DisableSafeMediaVolume();

    CARAPI SetHdmiSystemAudioSupported(
        /* [in] */ Boolean on,
        /* [out] */ Int32* result);

    CARAPI IsHdmiSystemAudioSupported(
        /* [out] */ Boolean* result);

    CARAPI SetVolumeController(
        /* [in] */ IIVolumeController* controller);

    CARAPI NotifyVolumeControllerVisible(
        /* [in] */ IIVolumeController* controller,
        /* [in] */ Boolean visible);

    static CARAPI GetMaxStreamVolume(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(Boolean) IsPlatformVoice();

    CARAPI_(Boolean) IsPlatformTelevision();

    CARAPI_(void) CreateAudioSystemThread();

    /** Waits for the volume handler to be created by the other thread. */
    CARAPI_(void) WaitForAudioHandlerCreation();

    CARAPI_(void) CheckAllAliasStreamVolumes();

    CARAPI_(void) CheckAllFixedVolumeDevices();

    CARAPI_(void) CheckAllFixedVolumeDevices(
        /* [in] */ Int32 streamType);

    CARAPI_(void) CreateStreamStates();

    CARAPI_(void) DumpStreamStates(
        /* [in] */ IPrintWriter* pw);

    CARAPI_(void) UpdateStreamVolumeAlias(
        /* [in] */ Boolean updateVolumes);

    CARAPI_(void) ReadDockAudioSettings(
        /* [in] */ IContentResolver* cr);

    CARAPI_(void) ReadPersistedSettings();

    CARAPI_(Int32) RescaleIndex(
        /* [in] */ Int32 index,
        /* [in] */ Int32 srcStream,
        /* [in] */ Int32 dstStream);

    CARAPI_(void) AdjustSuggestedStreamVolume(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 suggestedStreamType,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 uid);

    CARAPI_(void) AdjustStreamVolume(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 uid);

    CARAPI_(void) OnSetStreamVolume(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 index,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 device);

    CARAPI_(void) SetStreamVolume(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 index,
        /* [in] */ Int32 flags,
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 uid);

    CARAPI_(Int32) FindVolumeDelta(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 volume);

    CARAPI_(void) SendBroadcastToAll(
        /* [in] */ IIntent* intent);

    CARAPI_(void) SendStickyBroadcastToAll(
        /* [in] */ IIntent* intent);

    // UI update and Broadcast Intent
    CARAPI_(void) SendVolumeUpdate(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 oldIndex,
        /* [in] */ Int32 index,
        /* [in] */ Int32 flags);

    // UI update and Broadcast Intent
    CARAPI_(void) SendMasterVolumeUpdate(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 oldVolume,
        /* [in] */ Int32 newVolume);

    // UI update and Broadcast Intent
    CARAPI_(void) SendMasterMuteUpdate(
        /* [in] */ Boolean muted,
        /* [in] */ Int32 flags);

    CARAPI_(void) BroadcastMasterMuteStatus(
        /* [in] */ Boolean muted);

    /**
     * Sets the stream state's index, and posts a message to set system volume.
     * This will not call out to the UI. Assumes a valid stream type.
     *
     * @param streamType Type of the stream
     * @param index Desired volume index of the stream
     * @param device the device whose volume must be changed
     * @param force If true, set the volume even if the desired volume is same
     * as the current volume.
     */
    CARAPI_(void) SetStreamVolumeInt(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 index,
        /* [in] */ Int32 device,
        /* [in] */ Boolean force);

    /**
     * call must be synchronized on mRmtSbmxFullVolDeathHandlers
     * @return true if there is a registered death handler, false otherwise */
    CARAPI_(Boolean) DiscardRmtSbmxFullVolDeathHandlerFor(
        /* [in] */ IBinder* cb);

    /** call synchronized on mRmtSbmxFullVolDeathHandlers */
    CARAPI_(Boolean) HasRmtSbmxFullVolDeathHandlerFor(
        /* [in] */ IBinder* cb);

    CARAPI_(void) DoSetMasterVolume(
        /* [in] */ Float volume,
        /* [in] */ Int32 flags);

    CARAPI EnsureValidRingerMode(
        /* [in] */ Int32 ringerMode);

    CARAPI_(void) CheckZen(
        /* [in] */ Int32 ringerMode);

    CARAPI_(void) SetRingerModeInt(
        /* [in] */ Int32 ringerMode,
        /* [in] */ Boolean persist);

    CARAPI_(void) RestoreMasterVolume();

    // must be called synchronized on mSetModeDeathHandlers
    // setModeInt() returns a valid PID if the audio mode was successfully set to
    // any mode other than NORMAL.
    CARAPI_(Int32) SetModeInt(
        /* [in] */ Int32 mode,
        /* [in] */ IBinder* cb,
        /* [in] */ Int32 pid);

    CARAPI_(void) LoadTouchSoundAssetDefaults();

    CARAPI_(void) LoadTouchSoundAssets();

    CARAPI_(void) ReadAudioSettings(
        /* [in] */ Boolean userSwitch);

    CARAPI_(void) CheckScoAudioState();

    CARAPI_(AutoPtr<ScoClient>) GetScoClient(
        /* [in] */ IBinder* cb,
        /* [in] */ Boolean create);

    CARAPI_(Boolean) GetBluetoothHeadset();

    CARAPI_(void) DisconnectBluetoothSco(
        /* [in] */ Int32 exceptPid);

    CARAPI_(void) ResetBluetoothSco();

    CARAPI_(void) BroadcastScoConnectionState(
        /* [in] */ Int32 state);

    CARAPI_(void) OnBroadcastScoConnectionState(
        /* [in] */ Int32 state);

    CARAPI_(void) OnCheckMusicActive();

    CARAPI_(void) SaveMusicActiveMs();

    CARAPI_(void) OnConfigureSafeVolume(
        /* [in] */ Boolean force);

    /**
     * Checks if the adjustment should change ringer mode instead of just
     * adjusting volume. If so, this will set the proper ringer mode and volume
     * indices on the stream states.
     */
    CARAPI_(Int32) CheckForRingerModeChange(
        /* [in] */ Int32 oldIndex,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 step);

    CARAPI_(Boolean) IsStreamMutedByRingerMode(
        /* [in] */ Int32 streamType);

    CARAPI EnsureValidDirection(
        /* [in] */ Int32 direction);

    CARAPI EnsureValidSteps(
        /* [in] */ Int32 steps);

    CARAPI EnsureValidStreamType(
        /* [in] */ Int32 streamType);

    CARAPI_(Boolean) IsInCommunication();

    /**
     * For code clarity for getActiveStreamType(int)
     * @param delay_ms max time since last STREAM_MUSIC activity to consider
     * @return true if STREAM_MUSIC is active in streams handled by AudioFlinger now or
     *     in the last "delay_ms" ms.
     */
    CARAPI_(Boolean) IsAfMusicActiveRecently(
        /* [in] */ Int32 delay_ms);

    CARAPI_(Int32) GetActiveStreamType(
        /* [in] */ Int32 suggestedStreamType);

    CARAPI_(void) BroadcastRingerMode(
        /* [in] */ Int32 ringerMode);

    CARAPI_(void) BroadcastVibrateSetting(
        /* [in] */ Int32 vibrateType);

    // Message helper methods
    /**
     * Queue a message on the given handler's message queue, after acquiring the service wake lock.
     * Note that the wake lock needs to be released after the message has been handled.
     */
    CARAPI_(void) QueueMsgUnderWakeLock(
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 msg,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 delay);

    static CARAPI_(void) SendMsg(
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 msg,
        /* [in] */ Int32 existingMsgPolicy,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 delay);

    CARAPI_(Int32) GetDeviceForStream(
        /* [in] */ Int32 stream);

    // must be called synchronized on mConnectedDevices
    CARAPI_(void) MakeA2dpDeviceAvailable(
        /* [in] */ const String& address);

    CARAPI_(void) OnSendBecomingNoisyIntent();

    // must be called synchronized on mConnectedDevices
    CARAPI_(void) MakeA2dpDeviceUnavailableNow(
        /* [in] */ const String& address);

    // must be called synchronized on mConnectedDevices
    CARAPI_(void) MakeA2dpDeviceUnavailableLater(
        /* [in] */ const String& address);

    // must be called synchronized on mConnectedDevices
    CARAPI_(void) MakeA2dpSrcAvailable(
        /* [in] */ const String& address);

    // must be called synchronized on mConnectedDevices
    CARAPI_(void) MakeA2dpSrcUnavailable(
        /* [in] */ const String& address);

    // must be called synchronized on mConnectedDevices
    CARAPI_(void) CancelA2dpDeviceTimeout();

    // must be called synchronized on mConnectedDevices
    CARAPI_(Boolean) HasScheduledA2dpDockTimeout();

    CARAPI_(void) OnSetA2dpSinkConnectionState(
        /* [in] */ IBluetoothDevice* btDevice,
        /* [in] */ Int32 state);

    CARAPI_(void) OnSetA2dpSourceConnectionState(
        /* [in] */ IBluetoothDevice* btDevice,
        /* [in] */ Int32 state);

    CARAPI_(Boolean) HandleDeviceConnection(
        /* [in] */ Boolean connected,
        /* [in] */ Int32 device,
        /* [in] */ const String& params);

    // must be called before removing the device from mConnectedDevices
    CARAPI_(Int32) CheckSendBecomingNoisyIntent(
        /* [in] */ Int32 device,
        /* [in] */ Int32 state);

    CARAPI_(void) SendDeviceConnectionIntent(
        /* [in] */ Int32 device,
        /* [in] */ Int32 state,
        /* [in] */ const String& name);

    CARAPI_(void) StartMusicPlayer();

    CARAPI_(void) OnSetWiredDeviceConnectionState(
        /* [in] */ Int32 device,
        /* [in] */ Int32 state,
        /* [in] */ const String& name);

    CARAPI_(void) ConfigureHdmiPlugIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 state);

    //==========================================================================================
    // Device orientation
    //==========================================================================================
    /**
     * Handles device configuration changes that may map to a change in the orientation
     * or orientation.
     * Monitoring orientation and rotation is optional, and is defined by the definition and value
     * of the "ro.audio.monitorOrientation" and "ro.audio.monitorRotation" system properties.
     */
    CARAPI_(void) HandleConfigurationChanged(
        /* [in] */ IContext* context);

    CARAPI_(void) SetOrientationForAudioSystem();

    CARAPI_(void) SetRotationForAudioSystem();

    CARAPI_(void) SetSafeMediaVolumeEnabled(
        /* [in] */ Boolean on);

    CARAPI_(void) EnforceSafeMediaVolume();

    CARAPI_(Boolean) CheckSafeMediaVolume(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 index,
        /* [in] */ Int32 device);

    CARAPI_(void) DumpRingerMode(
        /* [in] */ IPrintWriter* pw);

    static CARAPI_(String) SafeMediaVolumeStateToString(
        /* [in] */ IInteger32* state);

    // Inform AudioFlinger of our device's low RAM attribute
    static CARAPI_(void) ReadAndSetLowRamDevice();

    CARAPI EnforceSelfOrSystemUI(
        /* [in] */ const String& action);

    static CARAPI_(Boolean) InitStatic();

public:
    // Devices for which the volume is fixed and VolumePanel slider should be disabled
    Int32 mFixedVolumeDevices;

    Int32 mFullVolumeDevices;

    // Devices which removal triggers intent ACTION_AUDIO_BECOMING_NOISY. The intent is only
    // sent if none of these devices is connected.
    Int32 mBecomingNoisyIntentDevices;

protected:
    /** Debug audio mode */
    static const Boolean DEBUG_MODE;
    /** Debug volumes */
    static const Boolean DEBUG_VOL;

private:
    static const String TAG;

    /** debug calls to media session apis */
    static const Boolean DEBUG_SESSIONS;

    /** Allow volume changes to set ringer mode to silent? */
    static const Boolean VOLUME_SETS_RINGER_MODE_SILENT;

    /** In silent mode, are volume adjustments (raises) prevented? */
    static const Boolean PREVENT_VOLUME_ADJUSTMENT_IF_SILENT;

    /** How long to delay before persisting a change in volume/ringer mode. */
    static const Int32 PERSIST_DELAY;

    /**
     * Only used in the result from {@link #checkForRingerModeChange(int, int, int)}
     */
    static const Int32 FLAG_ADJUST_VOLUME;

    AutoPtr<IContext> mContext;
    AutoPtr<IContentResolver> mContentResolver;
    AutoPtr<IAppOpsManager> mAppOps;

    // the platform has no specific capabilities
    static const Int32 PLATFORM_DEFAULT;
    // the platform is voice call capable (a phone)
    static const Int32 PLATFORM_VOICE;
    // the platform is a television or a set-top box
    static const Int32 PLATFORM_TELEVISION;
    // the platform type affects volume and silent mode behavior
    Int32 mPlatformType;

    static AutoPtr<IList> mMediaPlayers;

    /** The controller for the volume UI. */
    AutoPtr<VolumeController> mVolumeController;

    // sendMsg() flags
    /** If the msg is already queued, replace it with this one. */
    static const Int32 SENDMSG_REPLACE;
    /** If the msg is already queued, ignore this one and leave the old. */
    static const Int32 SENDMSG_NOOP;
    /** If the msg is already queued, queue this one and leave the old. */
    static const Int32 SENDMSG_QUEUE;

    // AudioHandler messages
    static const Int32 MSG_SET_DEVICE_VOLUME;
    static const Int32 MSG_PERSIST_VOLUME;
    static const Int32 MSG_PERSIST_MASTER_VOLUME;
    static const Int32 MSG_PERSIST_RINGER_MODE;
    static const Int32 MSG_MEDIA_SERVER_DIED;
    static const Int32 MSG_PLAY_SOUND_EFFECT;
    static const Int32 MSG_BTA2DP_DOCK_TIMEOUT;
    static const Int32 MSG_LOAD_SOUND_EFFECTS;
    static const Int32 MSG_SET_FORCE_USE;
    static const Int32 MSG_BT_HEADSET_CNCT_FAILED;
    static const Int32 MSG_SET_ALL_VOLUMES;
    static const Int32 MSG_PERSIST_MASTER_VOLUME_MUTE;
    static const Int32 MSG_REPORT_NEW_ROUTES;
    static const Int32 MSG_SET_FORCE_BT_A2DP_USE;
    static const Int32 MSG_CHECK_MUSIC_ACTIVE;
    static const Int32 MSG_BROADCAST_AUDIO_BECOMING_NOISY;
    static const Int32 MSG_CONFIGURE_SAFE_MEDIA_VOLUME;
    static const Int32 MSG_CONFIGURE_SAFE_MEDIA_VOLUME_FORCED;
    static const Int32 MSG_PERSIST_SAFE_VOLUME_STATE;
    static const Int32 MSG_BROADCAST_BT_CONNECTION_STATE;
    static const Int32 MSG_UNLOAD_SOUND_EFFECTS;
    static const Int32 MSG_SYSTEM_READY;
    static const Int32 MSG_PERSIST_MUSIC_ACTIVE_MS;
    static const Int32 MSG_PERSIST_MICROPHONE_MUTE;
    // start of messages handled under wakelock
    //   these messages can only be queued, i.e. sent with queueMsgUnderWakeLock(),
    //   and not with sendMsg(..., ..., SENDMSG_QUEUE, ...)
    static const Int32 MSG_SET_WIRED_DEVICE_CONNECTION_STATE;
    static const Int32 MSG_SET_A2DP_SRC_CONNECTION_STATE;
    static const Int32 MSG_SET_A2DP_SINK_CONNECTION_STATE;
    // end of messages handled under wakelock

    static const Int32 BTA2DP_DOCK_TIMEOUT_MILLIS;
    // Timeout for connection to bluetooth headset service
    static const Int32 BT_HEADSET_CNCT_TIMEOUT_MS;

    /** @see AudioSystemThread */
    AutoPtr<AudioSystemThread> mAudioSystemThread;
    /** @see AudioHandler */
    AutoPtr<AudioHandler> mAudioHandler;
    /** @see VolumeStreamState */
    AutoPtr<ArrayOf<VolumeStreamState*> > mStreamStates;
    AutoPtr<SettingsObserver> mSettingsObserver;

    Int32 mMode;
    // protects mRingerMode
    Object mSettingsLock;

    AutoPtr<ISoundPool> mSoundPool;
    Object mSoundEffectsLock;
    static const Int32 NUM_SOUNDPOOL_CHANNELS;

    // Internally master volume is a float in the 0.0 - 1.0 range,
    // but to support integer based AudioManager API we translate it to 0 - 100
    static const Int32 MAX_MASTER_VOLUME;

    // Maximum volume adjust steps allowed in a single batch call.
    static const Int32 MAX_BATCH_VOLUME_ADJUST_STEPS;

    /* Sound effect file names  */
    static const String SOUND_EFFECTS_PATH;
    static AutoPtr<IList> SOUND_EFFECT_FILES;

    /* Sound effect file name mapping sound effect id (AudioManager.FX_xxx) to
     * file index in SOUND_EFFECT_FILES[] (first column) and indicating if effect
     * uses soundpool (second column) */
    Int32 SOUND_EFFECT_FILES_MAP[IAudioManager::NUM_SOUND_EFFECTS][2];

   /** @hide Maximum volume index values for audio streams */
    static AutoPtr<ArrayOf<Int32> > MAX_STREAM_VOLUME;

    /* mStreamVolumeAlias[] indicates for each stream if it uses the volume settings
     * of another stream: This avoids multiplying the volume settings for hidden
     * stream types that follow other stream behavior for volume settings
     * NOTE: do not create loops in aliases!
     * Some streams alias to different streams according to device category (phone or tablet) or
     * use case (in call vs off call...). See updateStreamVolumeAlias() for more details.
     *  mStreamVolumeAlias contains STREAM_VOLUME_ALIAS_VOICE aliases for a voice capable device
     *  (phone), STREAM_VOLUME_ALIAS_TELEVISION for a television or set-top box and
     *  STREAM_VOLUME_ALIAS_DEFAULT for other devices (e.g. tablets).*/
    AutoPtr<ArrayOf<Int32> > STREAM_VOLUME_ALIAS_VOICE;

    AutoPtr<ArrayOf<Int32> > STREAM_VOLUME_ALIAS_TELEVISION;

    AutoPtr<ArrayOf<Int32> > STREAM_VOLUME_ALIAS_DEFAULT;

    AutoPtr<ArrayOf<Int32> > mStreamVolumeAlias;

    /**
     * Map AudioSystem.STREAM_* constants to app ops.  This should be used
     * after mapping through mStreamVolumeAlias.
     */
    static AutoPtr<ArrayOf<Int32> > STEAM_VOLUME_OPS;

    Boolean mUseFixedVolume;

    // stream names used by dumpStreamStates()
    static AutoPtr<ArrayOf<String> > STREAM_NAMES;

    Boolean mLinkNotificationWithVolume;

    AutoPtr<MyAudioSystemCallback> mAudioSystemCallback;

    /**
     * Current ringer mode from one of {@link AudioManager#RINGER_MODE_NORMAL},
     * {@link AudioManager#RINGER_MODE_SILENT}, or
     * {@link AudioManager#RINGER_MODE_VIBRATE}.
     */
    // protected by mSettingsLock
    Int32 mRingerMode;

    /** @see System#MODE_RINGER_STREAMS_AFFECTED */
    Int32 mRingerModeAffectedStreams;

    // Streams currently muted by ringer mode
    Int32 mRingerModeMutedStreams;

    /** @see System#MUTE_STREAMS_AFFECTED */
    Int32 mMuteAffectedStreams;

    /**
     * NOTE: setVibrateSetting(), getVibrateSetting(), shouldVibrate() are deprecated.
     * mVibrateSetting is just maintained during deprecation period but vibration policy is
     * now only controlled by mHasVibrator and mRingerMode
     */
    Int32 mVibrateSetting;

    // Is there a vibrator
    Boolean mHasVibrator;

    // Broadcast receiver for device connections intent broadcasts
    AutoPtr<IBroadcastReceiver> mReceiver;

    // Devices currently connected
    AutoPtr<IHashMap> mConnectedDevices;

    // Forced device usage for communications
    Int32 mForcedUseForComm;

    // True if we have master volume support
    Boolean mUseMasterVolume;

    AutoPtr<ArrayOf<Int32> > mMasterVolumeRamp;

    // List of binder death handlers for setMode() client processes.
    // The last process to have called setMode() is at the top of the list.
    AutoPtr<IArrayList> mSetModeDeathHandlers;

    // List of clients having issued a SCO start request
    AutoPtr<IArrayList> mScoClients;

    // BluetoothHeadset API to control SCO connection
    AutoPtr<IBluetoothHeadset> mBluetoothHeadset;

    // Bluetooth headset device
    AutoPtr<IBluetoothDevice> mBluetoothHeadsetDevice;

    // Indicate if SCO audio connection is currently active and if the initiator is
    // audio service (internal) or bluetooth headset (external)
    Int32 mScoAudioState;
    // SCO audio state is not active
    static const Int32 SCO_STATE_INACTIVE;
    // SCO audio activation request waiting for headset service to connect
    static const Int32 SCO_STATE_ACTIVATE_REQ;
    // SCO audio state is active or starting due to a request from AudioManager API
    static const Int32 SCO_STATE_ACTIVE_INTERNAL;
    // SCO audio deactivation request waiting for headset service to connect
    static const Int32 SCO_STATE_DEACTIVATE_REQ;

    // SCO audio state is active due to an action in BT handsfree (either voice recognition or
    // in call audio)
    static const Int32 SCO_STATE_ACTIVE_EXTERNAL;
    // Deactivation request for all SCO connections (initiated by audio mode change)
    // waiting for headset service to connect
    static const Int32 SCO_STATE_DEACTIVATE_EXT_REQ;

    // Indicates the mode used for SCO audio connection. The mode is virtual call if the request
    // originated from an app targeting an API version before JB MR2 and raw audio after that.
    Int32 mScoAudioMode;
    // SCO audio mode is undefined
    static const Int32 SCO_MODE_UNDEFINED;
    // SCO audio mode is virtual voice call (BluetoothHeadset.startScoUsingVirtualVoiceCall())
    static const Int32 SCO_MODE_VIRTUAL_CALL;
    // SCO audio mode is raw audio (BluetoothHeadset.connectAudio())
    static const Int32 SCO_MODE_RAW;
    // SCO audio mode is Voice Recognition (BluetoothHeadset.startVoiceRecognition())
    static const Int32 SCO_MODE_VR;

    static const Int32 SCO_MODE_MAX;

    // Current connection state indicated by bluetooth headset
    Int32 mScoConnectionState;

    // true if boot sequence has been completed
    Boolean mSystemReady;
    // listener for SoundPool sample load completion indication
    AutoPtr<SoundPoolCallback> mSoundPoolCallBack;
    // thread for SoundPool listener
    AutoPtr<SoundPoolListenerThread> mSoundPoolListenerThread;
    // message looper for SoundPool listener
    AutoPtr<ILooper> mSoundPoolLooper;
    // volume applied to sound played with playSoundEffect()
    static Int32 sSoundEffectVolumeDb;
    // previous volume adjustment direction received by checkForRingerModeChange()
    Int32 mPrevVolDirection;
    // Keyguard manager proxy
    AutoPtr<IKeyguardManager> mKeyguardManager;
    // mVolumeControlStream is set by VolumePanel to temporarily force the stream type which volume
    // is controlled by Vol keys.
    Int32  mVolumeControlStream;
    Object mForceControlStreamLock;
    // VolumePanel is currently the only client of forceVolumeControlStream() and runs in system
    // server process so in theory it is not necessary to monitor the client death.
    // However it is good to be ready for future evolutions.
    AutoPtr<ForceControlStreamClient> mForceControlStreamClient;
    // Used to play ringtones outside system_server
    /*volatile*/ AutoPtr<IIRingtonePlayer> mRingtonePlayer;

    Int32 mDeviceOrientation;
    Int32 mDeviceRotation;

    // Request to override default use of A2DP for media.
    Boolean mBluetoothA2dpEnabled;
    Object mBluetoothA2dpEnabledLock;

    // Monitoring of audio routes.  Protected by mCurAudioRoutes.
    AutoPtr<IAudioRoutesInfo> mCurAudioRoutes;
    AutoPtr<IRemoteCallbackList> mRoutesObservers;

    // TODO merge orientation and rotation
    Boolean mMonitorOrientation;
    Boolean mMonitorRotation;

    Boolean mDockAudioMediaEnabled;

    Int32 mDockState;

    // Used when safe volume warning message display is requested by setStreamVolume(). In this
    // case, the new requested volume, stream type and device are stored in mPendingVolumeCommand
    // and used later when/if disableSafeMediaVolume() is called.
    AutoPtr<StreamVolumeCommand> mPendingVolumeCommand;

    AutoPtr<IPowerManagerWakeLock> mAudioEventWakeLock;

    AutoPtr<MediaFocusControl> mMediaFocusControl;

    // Reference to BluetoothA2dp to query for AbsoluteVolume.
    AutoPtr<IBluetoothA2dp> mA2dp;
    Object mA2dpAvrcpLock;
    // If absolute volume is supported in AVRCP device
    Boolean mAvrcpAbsVolSupported;

    AutoPtr<AudioOrientationEventListener> mOrientationListener;

    Int32 mRmtSbmxFullVolRefCount;
    AutoPtr<IArrayList> mRmtSbmxFullVolDeathHandlers;

    //==========================================================================================
    // Sound Effects
    //==========================================================================================

    static const String TAG_AUDIO_ASSETS;
    static const String ATTR_VERSION;
    static const String TAG_GROUP;
    static const String ATTR_GROUP_NAME;
    static const String TAG_ASSET;
    static const String ATTR_ASSET_ID;
    static const String ATTR_ASSET_FILE;

    static const String ASSET_FILE_VERSION;
    static const String GROUP_TOUCH_SOUNDS;

    static const Int32 SOUND_EFFECTS_LOAD_TIMEOUT_MS;

    AutoPtr<IBluetoothProfileServiceListener> mBluetoothProfileServiceListener; // = new BluetoothProfile.ServiceListener();

    /* cache of the address of the last dock the device was connected to */
    String mDockAddress;

    //==========================================================================================
    // Safe media volume management.
    // MUSIC stream volume level is limited when headphones are connected according to safety
    // regulation. When the user attempts to raise the volume above the limit, a warning is
    // displayed and the user has to acknowlegde before the volume is actually changed.
    // The volume index corresponding to the limit is stored in config_safe_media_volume_index
    // property. Platforms with a different limit must set this property accordingly in their
    // overlay.
    //==========================================================================================

    // mSafeMediaVolumeState indicates whether the media volume is limited over headphones.
    // It is SAFE_MEDIA_VOLUME_NOT_CONFIGURED at boot time until a network service is connected
    // or the configure time is elapsed. It is then set to SAFE_MEDIA_VOLUME_ACTIVE or
    // SAFE_MEDIA_VOLUME_DISABLED according to country option. If not SAFE_MEDIA_VOLUME_DISABLED, it
    // can be set to SAFE_MEDIA_VOLUME_INACTIVE by calling AudioService.disableSafeMediaVolume()
    // (when user opts out).
    static const Int32 SAFE_MEDIA_VOLUME_NOT_CONFIGURED;
    static const Int32 SAFE_MEDIA_VOLUME_DISABLED;
    static const Int32 SAFE_MEDIA_VOLUME_INACTIVE;
    static const Int32 SAFE_MEDIA_VOLUME_ACTIVE;
    AutoPtr<IInteger32> mSafeMediaVolumeState;
    Object mSafeMediaVolumeStateLock;

    Int32 mMcc;
    // mSafeMediaVolumeIndex is the cached value of config_safe_media_volume_index property
    Int32 mSafeMediaVolumeIndex;
    // mSafeMediaVolumeDevices lists the devices for which safe media volume is enforced,
    Int32 mSafeMediaVolumeDevices;

    // mMusicActiveMs is the cumulative time of music activity since safe volume was disabled.
    // When this time reaches UNSAFE_VOLUME_MUSIC_ACTIVE_MS_MAX, the safe media volume is re-enabled
    // automatically. mMusicActiveMs is rounded to a multiple of MUSIC_ACTIVE_POLL_PERIOD_MS.
    Int32 mMusicActiveMs;
    static const Int32 UNSAFE_VOLUME_MUSIC_ACTIVE_MS_MAX;
    static const Int32 MUSIC_ACTIVE_POLL_PERIOD_MS;
    static const Int32 SAFE_VOLUME_CONFIGURE_TIMEOUT_MS;

    // If HDMI-CEC system audio is supported
    Boolean mHdmiSystemAudioSupported;
    // Set only when device is tv.
    AutoPtr<IHdmiTvClient> mHdmiTvClient;
    // true if the device has system feature PackageManager.FEATURE_TELEVISION.
    // cached HdmiControlManager interface
    AutoPtr<IHdmiControlManager> mHdmiManager;
    // Set only when device is a set-top box.
    AutoPtr<IHdmiPlaybackClient> mHdmiPlaybackClient;
    // true if we are a set-top box, an HDMI sink is connected and it supports CEC.
    Boolean mHdmiCecSink;

    AutoPtr<MyDisplayStatusCallback> mHdmiDisplayStatusCallback; // = new MyDisplayStatusCallback();

    // cached value of com.android.internal.R.bool.config_camera_sound_forced
    AutoPtr<IBoolean> mCameraSoundForced;

    static AutoPtr<ArrayOf<String> > RINGER_MODE_NAMES;

    AutoPtr<IHashMap> mAudioPolicies;

    static Boolean sInit;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOSERVICE_H__
