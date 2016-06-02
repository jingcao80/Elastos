
#ifndef __ELASTOS_DROID_SERVER_TV_TVINPUTHARDWAREMANAGER_H__
#define __ELASTOS_DROID_SERVER_TV_TVINPUTHARDWAREMANAGER_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/tv/TvInputHal.h"
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Media.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Core::IInteger32;
using Elastos::Core::IRunnable;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiHotplugEvent;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlService;
using Elastos::Droid::Hardware::Hdmi::IIHdmiDeviceEventListener;
using Elastos::Droid::Hardware::Hdmi::IIHdmiHotplugEventListener;
using Elastos::Droid::Hardware::Hdmi::IIHdmiSystemAudioModeChangeListener;
using Elastos::Droid::Media::IAudioDevicePort;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioManagerOnAudioPortUpdateListener;
using Elastos::Droid::Media::IAudioPatch;
using Elastos::Droid::Media::IAudioPort;
using Elastos::Droid::Media::Tv::IITvInputHardware;
using Elastos::Droid::Media::Tv::IITvInputHardwareCallback;
using Elastos::Droid::Media::Tv::ITvInputHardwareInfo;
using Elastos::Droid::Media::Tv::ITvInputInfo;
using Elastos::Droid::Media::Tv::ITvStreamConfig;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ISurface;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Tv {

/**
 * A helper class for TvInputManagerService to handle TV input hardware.
 *
 * This class does a basic connection management and forwarding calls to TvInputHal which eventually
 * calls to tv_input HAL module.
 *
 * @hide
 */
class TvInputHardwareManager
    : public Object
    , public ITvInputHalCallback
{
private:
    class TvInputHardwareImpl;
    class Connection
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        Connection(
            /* [in] */ TvInputHardwareManager* host);

        CARAPI constructor(
            /* [in] */ ITvInputHardwareInfo* hardwareInfo);

        // *Locked methods assume TvInputHardwareManager.mLock is held.
        CARAPI ResetLocked(
            /* [in] */ TvInputHardwareImpl* hardware,
            /* [in] */ IITvInputHardwareCallback* callback,
            /* [in] */ ITvInputInfo* info,
            /* [in] */ IInteger32* callingUid,
            /* [in] */ IInteger32* resolvedUserId);

        CARAPI UpdateConfigsLocked(
            /* [in] */ ArrayOf<ITvStreamConfig*>* configs);

        CARAPI GetHardwareInfoLocked(
            /* [out] */ ITvInputHardwareInfo** result);

        CARAPI GetInfoLocked(
            /* [out] */ ITvInputInfo** result);

        CARAPI GetHardwareLocked(
            /* [out] */ IITvInputHardware** result);

        CARAPI GetHardwareImplLocked(
            /* [out] */ TvInputHardwareImpl** result);

        CARAPI GetCallbackLocked(
            /* [out] */ IITvInputHardwareCallback** result);

        CARAPI GetConfigsLocked(
            /* [out, callee] */ ArrayOf<ITvStreamConfig*>** result);

        CARAPI GetCallingUidLocked(
            /* [out] */ IInteger32** result);

        CARAPI GetResolvedUserIdLocked(
            /* [out] */ IInteger32** result);

        CARAPI SetOnFirstFrameCapturedLocked(
            /* [in] */ IRunnable* runnable);

        CARAPI GetOnFirstFrameCapturedLocked(
            /* [out] */ IRunnable** result);

        // @Override
        CARAPI ProxyDied();

    private:
        AutoPtr<ITvInputHardwareInfo> mHardwareInfo;

        AutoPtr<ITvInputInfo> mInfo;

        AutoPtr<TvInputHardwareImpl> mHardware;

        AutoPtr<IITvInputHardwareCallback> mCallback;

        ArrayOf<ITvStreamConfig*>* mConfigs;

        AutoPtr<IInteger32> mCallingUid;

        AutoPtr<IInteger32> mResolvedUserId;

        AutoPtr<IRunnable> mOnFirstFrameCaptured;

        TvInputHardwareManager* mHost;
    };

    class HdmiSystemAudioModeChangeListener;
    class InnerSub_Runnable;

    class TvInputHardwareImpl
        : public Object
        , public IITvInputHardware
        , public IBinder
    {
        friend class HdmiSystemAudioModeChangeListener;
        friend class InnerSub_Runnable;
        friend class TvInputHardwareManager;
    private:
        class InnerSub_AudioManagerOnAudioPortUpdateListener
            : public Object
            , public IAudioManagerOnAudioPortUpdateListener
        {
        public:
            CAR_INTERFACE_DECL()

            InnerSub_AudioManagerOnAudioPortUpdateListener(
                /* [in] */ TvInputHardwareImpl* host);

            // @Override
            CARAPI OnAudioPortListUpdate(
                /* [in] */ ArrayOf<IAudioPort*>* portList);

            // @Override
            CARAPI OnAudioPatchListUpdate(
                /* [in] */ ArrayOf<IAudioPatch*>* patchList);

            // @Override
            CARAPI OnServiceDied();

        private:
            TvInputHardwareImpl* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        TvInputHardwareImpl(
            /* [in] */ TvInputHardwareManager* host);

        CARAPI constructor(
            /* [in] */ ITvInputHardwareInfo* info);

        CARAPI ReleaseResources();

        // prevail.
        // @Override
        CARAPI SetSurface(
            /* [in] */ ISurface* surface,
            /* [in] */ ITvStreamConfig* config,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI SetStreamVolume(
            /* [in] */ Float volume);

        // @Override
        CARAPI DispatchKeyEventToHdmi(
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OverrideAudioSink(
            /* [in] */ Int32 audioType,
            /* [in] */ const String& audioAddress,
            /* [in] */ Int32 samplingRate,
            /* [in] */ Int32 channelMask,
            /* [in] */ Int32 format);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        CARAPI FindAudioSinkFromAudioPolicy(
            /* [out] */ IAudioDevicePort** result);

        CARAPI FindAudioDevicePort(
            /* [in] */ Int32 type,
            /* [in] */ const String& address,
            /* [out] */ IAudioDevicePort** result);

        /**
         * Update audio configuration (source, sink, patch) all up to current state.
         */
        CARAPI UpdateAudioConfigLocked();

        CARAPI StartCapture(
            /* [in] */ ISurface* surface,
            /* [in] */ ITvStreamConfig* config,
            /* [out] */ Boolean* result);

        CARAPI StopCapture(
            /* [in] */ ITvStreamConfig* config,
            /* [out] */ Boolean* result);

        CARAPI UpdateAudioSourceLocked(
            /* [out] */ Boolean* result);

        CARAPI UpdateAudioSinkLocked(
            /* [out] */ Boolean* result);

        CARAPI HandleAudioSinkUpdated();

    private:
        TvInputHardwareManager* mHost;

        AutoPtr<IObject> mImplLock;

        AutoPtr<IAudioManagerOnAudioPortUpdateListener> mAudioListener;

        AutoPtr<ITvInputHardwareInfo> mInfo;

        Boolean mReleased;

        Int32 mOverrideAudioType;

        String mOverrideAudioAddress;

        AutoPtr<IAudioDevicePort> mAudioSource;

        AutoPtr<IAudioDevicePort> mAudioSink;

        AutoPtr<IAudioPatch> mAudioPatch;

        Float mCommittedVolume;

        Float mVolume;

        AutoPtr<ITvStreamConfig> mActiveConfig;

        Int32 mDesiredSamplingRate;

        Int32 mDesiredChannelMask;

        Int32 mDesiredFormat;
    };

    class HdmiHotplugEventListener;
    class HdmiDeviceEventListener;
    class ListenerHandler
        : public Handler
    {
        friend class HdmiHotplugEventListener;
        friend class HdmiDeviceEventListener;
        friend class TvInputHardwareManager;
    public:
        TO_STRING_IMPL("TvInputHardwareManager::ListenerHandler")

        ListenerHandler(
            /* [in] */ TvInputHardwareManager* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        TvInputHardwareManager* mHost;

        static const Int32 STATE_CHANGED;

        static const Int32 HARDWARE_DEVICE_ADDED;

        static const Int32 HARDWARE_DEVICE_REMOVED;

        static const Int32 HDMI_DEVICE_ADDED;

        static const Int32 HDMI_DEVICE_REMOVED;

        static const Int32 HDMI_DEVICE_UPDATED;
    };

    // Listener implementations for HdmiControlService
    class HdmiHotplugEventListener
        : public Object
        , public IIHdmiHotplugEventListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        HdmiHotplugEventListener(
            /* [in] */ TvInputHardwareManager* host);

        // @Override
        CARAPI OnReceived(
            /* [in] */ IHdmiHotplugEvent* event);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        TvInputHardwareManager* mHost;
    };

    class HdmiDeviceEventListener
        : public Object
        , public IIHdmiDeviceEventListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        HdmiDeviceEventListener(
            /* [in] */ TvInputHardwareManager* host);

        // @Override
        CARAPI OnStatusChanged(
            /* [in] */ IHdmiDeviceInfo* deviceInfo,
            /* [in] */ Int32 status);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        CARAPI FindHdmiDeviceInfo(
            /* [in] */ Int32 id,
            /* [out] */ IHdmiDeviceInfo** result);

    private:
        TvInputHardwareManager* mHost;
    };

    class HdmiSystemAudioModeChangeListener
        : public Object
        , public IIHdmiSystemAudioModeChangeListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        HdmiSystemAudioModeChangeListener(
            /* [in] */ TvInputHardwareManager* host);

        // @Override
        CARAPI OnStatusChanged(
            /* [in] */ Boolean enabled);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        TvInputHardwareManager* mHost;
    };

    class InnerSub_Runnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_Runnable(
            /* [in] */ TvInputHardwareImpl* hardwareImpl,
            /* [in] */ ITvStreamConfig* config);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<TvInputHardwareImpl> mHardwareImpl;
        AutoPtr<ITvStreamConfig> mConfig;
    };

public:
    CAR_INTERFACE_DECL()

    TvInputHardwareManager();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ITvInputHardwareManagerListener* listener);

    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

    // @Override
    CARAPI OnDeviceAvailable(
        /* [in] */ ITvInputHardwareInfo* info,
        /* [in] */ ArrayOf<ITvStreamConfig*>* configs);

    // @Override
    CARAPI OnDeviceUnavailable(
        /* [in] */ Int32 deviceId);

    // @Override
    CARAPI OnStreamConfigurationChanged(
        /* [in] */ Int32 deviceId,
        /* [in] */ ArrayOf<ITvStreamConfig*>* configs);

    // @Override
    CARAPI OnFirstFrameCaptured(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 streamId);

    CARAPI GetHardwareList(
        /* [out] */ IList** result);

    CARAPI GetHdmiDeviceList(
        /* [out] */ IList** result);

    CARAPI AddHardwareTvInput(
        /* [in] */ Int32 deviceId,
        /* [in] */ ITvInputInfo* info);

    CARAPI AddHdmiTvInput(
        /* [in] */ Int32 id,
        /* [in] */ ITvInputInfo* info);

    CARAPI RemoveTvInput(
        /* [in] */ const String& inputId);

    /**
     * Create a TvInputHardware object with a specific deviceId. One service at a time can access
     * the object, and if more than one process attempts to create hardware with the same deviceId,
     * the latest service will get the object and all the other hardware are released. The
     * release is notified via ITvInputHardwareCallback.onReleased().
     */
    CARAPI AcquireHardware(
        /* [in] */ Int32 deviceId,
        /* [in] */ IITvInputHardwareCallback* callback,
        /* [in] */ ITvInputInfo* info,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 resolvedUserId,
        /* [out] */ IITvInputHardware** result);

    /**
     * Release the specified hardware.
     */
    CARAPI ReleaseHardware(
        /* [in] */ Int32 deviceId,
        /* [in] */ IITvInputHardware* hardware,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 resolvedUserId);

    /**
     * Get the list of TvStreamConfig which is buffered mode.
     */
    CARAPI GetAvailableTvStreamConfigList(
        /* [in] */ const String& inputId,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 resolvedUserId,
        /* [out] */ IList** result);

    /**
     * Take a snapshot of the given TV input into the provided Surface.
     */
    CARAPI CaptureFrame(
        /* [in] */ const String& inputId,
        /* [in] */ ISurface* surface,
        /* [in] */ ITvStreamConfig* config,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 resolvedUserId,
        /* [out] */ Boolean* result);

private:
    CARAPI BuildHardwareListLocked();

    CARAPI_(Boolean) CheckUidChangedLocked(
        /* [in] */ Connection* connection,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 resolvedUserId);

    CARAPI ConvertConnectedToState(
        /* [in] */ Boolean connected,
        /* [out] */ Int32* result);

    static CARAPI IndexOfEqualValue(
        /* [in] */ ISparseArray* map,
        /* [in] */ IInterface* value,
        /* [out] */ Int32* result);

    CARAPI FindHardwareInfoForHdmiPortLocked(
        /* [in] */ Int32 port,
        /* [out] */ ITvInputHardwareInfo** result);

    CARAPI FindDeviceIdForInputIdLocked(
        /* [in] */ const String& inputId,
        /* [out] */ Int32* result);

    CARAPI ProcessPendingHdmiDeviceEventsLocked();

private:
    static const String TAG;

    AutoPtr<ITvInputHardwareManagerListener> mListener;
    AutoPtr<TvInputHal> mHal;
    AutoPtr<ISparseArray> mConnections;
    AutoPtr<IList> mHardwareList;
    AutoPtr<IList> mHdmiDeviceList;
    /* A map from a device ID to the matching TV input ID. */
    AutoPtr<ISparseArray> mHardwareInputIdMap;
    /* A map from a HDMI logical address to the matching TV input ID. */
    AutoPtr<ISparseArray> mHdmiInputIdMap;
    AutoPtr<IMap> mInputMap;

    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IIHdmiControlService> mHdmiControlService;
    AutoPtr<IIHdmiHotplugEventListener> mHdmiHotplugEventListener;
    AutoPtr<IIHdmiDeviceEventListener> mHdmiDeviceEventListener;
    AutoPtr<IIHdmiSystemAudioModeChangeListener> mHdmiSystemAudioModeChangeListener;

    // TODO: Should handle STANDBY case.
    AutoPtr<ISparseBooleanArray> mHdmiStateMap;
    AutoPtr<IList> mPendingHdmiDeviceEvents;

    // Calls to mListener should happen here.
    AutoPtr<IHandler> mHandler;

    AutoPtr<IObject> mLock;
};

} // namespace Tv
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TV_TVINPUTHARDWAREMANAGER_H__
