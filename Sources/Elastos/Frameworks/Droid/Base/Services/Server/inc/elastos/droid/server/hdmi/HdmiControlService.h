
#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICONTROLSERVICE_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICONTROLSERVICE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/hdmi/HdmiCecController.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDevicePlayback.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageValidator.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Os.h>
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/Looper.h>

using Elastos::Core::IRunnable;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiHotplugEvent;
using Elastos::Droid::Hardware::Hdmi::IHdmiPortInfo;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlService;
using Elastos::Droid::Hardware::Hdmi::IIHdmiDeviceEventListener;
using Elastos::Droid::Hardware::Hdmi::IIHdmiHotplugEventListener;
using Elastos::Droid::Hardware::Hdmi::IIHdmiInputChangeListener;
using Elastos::Droid::Hardware::Hdmi::IIHdmiMhlVendorCommandListener;
using Elastos::Droid::Hardware::Hdmi::IIHdmiRecordListener;
using Elastos::Droid::Hardware::Hdmi::IIHdmiSystemAudioModeChangeListener;
using Elastos::Droid::Hardware::Hdmi::IIHdmiVendorCommandListener;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;
using Elastos::Droid::Server::SystemService;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class UnmodifiableSparseInt32Array;
class HdmiCecLocalDeviceTv;
class HdmiCecLocalDevicePlayback;
class UnmodifiableSparseArray;
class HdmiMhlControllerStub;
class HdmiCecController;
/**
 * Provides a service for sending and processing HDMI control messages,
 * HDMI-CEC and MHL control command, and providing the information on both standard.
 */
class HdmiControlService
    : public SystemService
    , public IHdmiControlService
{
public:
    class VendorCommandListenerRecord
        : public Object
        , public IProxyDeathRecipient
    {
        friend class HdmiControlService;

    public:
        CAR_INTERFACE_DECL()

        VendorCommandListenerRecord(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IIHdmiVendorCommandListener* listener,
            /* [in] */ Int32 deviceType);

        // @Override
        CARAPI ProxyDied();

    private:
        HdmiControlService* mHost;

        AutoPtr<IIHdmiVendorCommandListener> mListener;

        const Int32 mDeviceType;
    };

    class BinderService
        : public Object
        , public IIHdmiControlService
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        BinderService();

        CARAPI constructor(
            /* [in] */ IHdmiControlService* host);

        // @Override
        CARAPI GetSupportedTypes(
            /* [out, callee] */ ArrayOf<Int32>** result);

        // @Override
        CARAPI GetActiveSource(
            /* [out] */ IHdmiDeviceInfo** result);

        // @Override
        CARAPI DeviceSelect(
            /* [in] */ Int32 deviceId,
            /* [in] */ IIHdmiControlCallback* callback);

        // @Override
        CARAPI PortSelect(
            /* [in] */ Int32 portId,
            /* [in] */ IIHdmiControlCallback* callback);

        // @Override
        CARAPI SendKeyEvent(
            /* [in] */ Int32 deviceType,
            /* [in] */ Int32 keyCode,
            /* [in] */ Boolean isPressed);

        // @Override
        CARAPI OneTouchPlay(
            /* [in] */ IIHdmiControlCallback* callback);

        // @Override
        CARAPI QueryDisplayStatus(
            /* [in] */ IIHdmiControlCallback* callback);

        // @Override
        CARAPI AddHotplugEventListener(
            /* [in] */ IIHdmiHotplugEventListener* listener);

        // @Override
        CARAPI RemoveHotplugEventListener(
            /* [in] */ IIHdmiHotplugEventListener* listener);

        // @Override
        CARAPI AddDeviceEventListener(
            /* [in] */ IIHdmiDeviceEventListener* listener);

        // @Override
        CARAPI GetPortInfo(
            /* [out] */ IList** result);

        // @Override
        CARAPI CanChangeSystemAudioMode(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetSystemAudioMode(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI SetSystemAudioMode(
            /* [in] */ Boolean enabled,
            /* [in] */ IIHdmiControlCallback* callback);

        // @Override
        CARAPI AddSystemAudioModeChangeListener(
            /* [in] */ IIHdmiSystemAudioModeChangeListener* listener);

        // @Override
        CARAPI RemoveSystemAudioModeChangeListener(
            /* [in] */ IIHdmiSystemAudioModeChangeListener* listener);

        // @Override
        CARAPI SetInputChangeListener(
            /* [in] */ IIHdmiInputChangeListener* listener);

        // @Override
        CARAPI GetInputDevices(
            /* [out] */ IList** result);

        // @Override
        CARAPI SetSystemAudioVolume(
            /* [in] */ Int32 oldIndex,
            /* [in] */ Int32 newIndex,
            /* [in] */ Int32 maxIndex);

        // @Override
        CARAPI SetSystemAudioMute(
            /* [in] */ Boolean mute);

        // @Override
        CARAPI SetArcMode(
            /* [in] */ Boolean enabled);

        // @Override
        CARAPI SetProhibitMode(
            /* [in] */ Boolean enabled);

        // @Override
        CARAPI AddVendorCommandListener(
            /* [in] */ IIHdmiVendorCommandListener* listener,
            /* [in] */ Int32 deviceType);

        // @Override
        CARAPI SendVendorCommand(
            /* [in] */ Int32 deviceType,
            /* [in] */ Int32 targetAddress,
            /* [in] */ ArrayOf<Byte>* params,
            /* [in] */ Boolean hasVendorId);

        // @Override
        CARAPI SendStandby(
            /* [in] */ Int32 deviceType,
            /* [in] */ Int32 deviceId);

        // @Override
        CARAPI SetHdmiRecordListener(
            /* [in] */ IIHdmiRecordListener* listener);

        // @Override
        CARAPI StartOneTouchRecord(
            /* [in] */ Int32 recorderAddress,
            /* [in] */ ArrayOf<Byte>* recordSource);

        // @Override
        CARAPI StopOneTouchRecord(
            /* [in] */ Int32 recorderAddress);

        // @Override
        CARAPI StartTimerRecording(
            /* [in] */ Int32 recorderAddress,
            /* [in] */ Int32 sourceType,
            /* [in] */ ArrayOf<Byte>* recordSource);

        // @Override
        CARAPI ClearTimerRecording(
            /* [in] */ Int32 recorderAddress,
            /* [in] */ Int32 sourceType,
            /* [in] */ ArrayOf<Byte>* recordSource);

        // @Override
        CARAPI SendMhlVendorCommand(
            /* [in] */ Int32 portId,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 length,
            /* [in] */ ArrayOf<Byte>* data);

        // @Override
        CARAPI AddHdmiMhlVendorCommandListener(
            /* [in] */ IIHdmiMhlVendorCommandListener* listener);

        // @Override
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* writer,
            /* [in] */ ArrayOf<String>* args);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        HdmiControlService* mHost;
    };

private:
    class HdmiControlBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("HdmiControlService::HdmiControlBroadcastReceiver")

        HdmiControlBroadcastReceiver(
            /* [in] */ HdmiControlService* host);

        // @ServiceThreadOnly
        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        HdmiControlService* mHost;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("HdmiControlService::SettingsObserver")

        CARAPI constructor(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IHandler* handler);

        // onChange is set up to run in service thread.
        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        HdmiControlService* mHost;
    };

    class HdmiMhlVendorCommandListenerRecord
        : public Object
        , public IProxyDeathRecipient
    {
        friend class HdmiControlService;

    public:
        CAR_INTERFACE_DECL()

        HdmiMhlVendorCommandListenerRecord(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IIHdmiMhlVendorCommandListener* listener);

        // @Override
        CARAPI ProxyDied();

    private:
        HdmiControlService* mHost;

        AutoPtr<IIHdmiMhlVendorCommandListener> mListener;
    };

    // Record class that monitors the event of the caller of being killed. Used to clean up
    // the listener list and record list accordingly.
    class HotplugEventListenerRecord
        : public Object
        , public IProxyDeathRecipient
    {
        friend class HdmiControlService;

    public:
        CAR_INTERFACE_DECL()

        HotplugEventListenerRecord(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IIHdmiHotplugEventListener* listener);

        // @Override
        CARAPI ProxyDied();

    private:
        HdmiControlService* mHost;

        AutoPtr<IIHdmiHotplugEventListener> mListener;
    };

    class DeviceEventListenerRecord
        : public Object
        , public IProxyDeathRecipient
    {
        friend class HdmiControlService;

    public:
        CAR_INTERFACE_DECL()

        DeviceEventListenerRecord(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IIHdmiDeviceEventListener* listener);

        // @Override
        CARAPI ProxyDied();

    private:
        HdmiControlService* mHost;

        AutoPtr<IIHdmiDeviceEventListener> mListener;
    };

    class SystemAudioModeChangeListenerRecord
        : public Object
        , public IProxyDeathRecipient
    {
        friend class HdmiControlService;

    public:
        CAR_INTERFACE_DECL()

        SystemAudioModeChangeListenerRecord(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IIHdmiSystemAudioModeChangeListener* listener);

        // @Override
        CARAPI ProxyDied();

    private:
        HdmiControlService* mHost;

        AutoPtr<IIHdmiSystemAudioModeChangeListener> mListener;
    };

    class HdmiRecordListenerRecord
        : public Object
        , public IProxyDeathRecipient
    {
        friend class HdmiControlService;

    public:
        CAR_INTERFACE_DECL()

        HdmiRecordListenerRecord(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IIHdmiRecordListener* listener);

        // @Override
        CARAPI ProxyDied();

    private:
        HdmiControlService* mHost;

        AutoPtr<IIHdmiRecordListener> mListener;
    };

    class InputChangeListenerRecord
        : public Object
        , public IProxyDeathRecipient
    {
        friend class HdmiControlService;

    public:
        CAR_INTERFACE_DECL()

        InputChangeListenerRecord(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IIHdmiInputChangeListener* listener);

        // @Override
        CARAPI ProxyDied();

    private:
        HdmiControlService* mHost;
        AutoPtr<IIHdmiInputChangeListener> mListener;
    };

    class DeviceSelectRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        DeviceSelectRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 deviceId,
            /* [in] */ IIHdmiControlCallback* callback);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Int32 mDeviceId;
        AutoPtr<IIHdmiControlCallback> mCallback;
    };

    class PortSelectRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        PortSelectRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 portId,
            /* [in] */ IIHdmiControlCallback* callback);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Int32 mPortId;
        AutoPtr<IIHdmiControlCallback> mCallback;
    };

    class SendKeyEventRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SendKeyEventRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 deviceType,
            /* [in] */ Int32 keyCode,
            /* [in] */ Boolean isPressed);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Int32 mDeviceType;
        Int32 mKeyCode;
        Boolean mIsPressed;
    };

    class OneTouchPlayRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        OneTouchPlayRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IIHdmiControlCallback* callback);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        AutoPtr<IIHdmiControlCallback> mCallback;
    };

    class QueryDisplayStatusRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        QueryDisplayStatusRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IIHdmiControlCallback* callback);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        AutoPtr<IIHdmiControlCallback> mCallback;
    };

    class SetSystemAudioModeRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SetSystemAudioModeRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Boolean enabled,
            /* [in] */ IIHdmiControlCallback* callback);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Boolean mEnabled;
        AutoPtr<IIHdmiControlCallback> mCallback;
    };

    class SetSystemAudioVolumeRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SetSystemAudioVolumeRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 oldIndex,
            /* [in] */ Int32 newIndex,
            /* [in] */ Int32 maxIndex);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Int32 mOldIndex;
        Int32 mNewIndex;
        Int32 mMaxIndex;
    };

    class SetSystemAudioMuteRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SetSystemAudioMuteRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Boolean mute);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Boolean mMute;
    };

    class SetArcModeRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SetArcModeRunnable(
            /* [in] */ HdmiControlService* host);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
    };

    class SendVendorCommandRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SendVendorCommandRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 deviceType,
            /* [in] */ Int32 targetAddress,
            /* [in] */ ArrayOf<Byte>* params,
            /* [in] */ Boolean hasVendorId);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Int32 mDeviceType;
        Int32 mTargetAddress;
        AutoPtr<ArrayOf<Byte> > mParams;
        Boolean mHasVendorId;
    };

    class SendStandbyRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SendStandbyRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 deviceType,
            /* [in] */ Int32 deviceId);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Int32 mDeviceType;
        Int32 mDeviceId;
    };

    class StartOneTouchRecordRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        StartOneTouchRecordRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 recorderAddress,
            /* [in] */ ArrayOf<Byte>* recordSource);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Int32 mRecorderAddress;
        AutoPtr<ArrayOf<Byte> > mRecordSource;
    };

    class StopOneTouchRecordRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        StopOneTouchRecordRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 recorderAddress);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Int32 mRecorderAddress;
    };

    class StartTimerRecordingRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        StartTimerRecordingRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 recorderAddress,
            /* [in] */ Int32 sourceType,
            /* [in] */ ArrayOf<Byte>* recordSource);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Int32 mRecorderAddress;
        Int32 mSourceType;
        AutoPtr<ArrayOf<Byte> > mRecordSource;
    };

    class ClearTimerRecordingRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        ClearTimerRecordingRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 recorderAddress,
            /* [in] */ Int32 sourceType,
            /* [in] */ ArrayOf<Byte>* recordSource);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Int32 mRecorderAddress;
        Int32 mSourceType;
        AutoPtr<ArrayOf<Byte> > mRecordSource;
    };

    class SendMhlVendorCommandRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SendMhlVendorCommandRunnable(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 portId,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 length,
            /* [in] */ ArrayOf<Byte>* data);

        // @Override
        CARAPI Run();

    private:
        HdmiControlService* mHost;
        Int32 mPortId;
        Int32 mOffset;
        Int32 mLength;
        AutoPtr<ArrayOf<Byte> > mData;
    };

    class InnerSub_AllocateAddressCallback
        : public Object
        , public IHdmiCecControllerAllocateAddressCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_AllocateAddressCallback(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IHdmiCecLocalDevice* localDevice,
            /* [in] */ IArrayList* allocatingDevices,
            /* [in] */ IArrayList* allocatedDevices,
            /* [in] */ ArrayOf<Int32>* finished,
            /* [in] */ Int32 initiatedBy);

        // @Override
        CARAPI OnAllocated(
            /* [in] */ Int32 deviceType,
            /* [in] */ Int32 logicalAddress);

    private:
        HdmiControlService* mHost;
        AutoPtr<IHdmiCecLocalDevice> mLocalDevice;
        AutoPtr<IArrayList> mAllocatingDevices;
        AutoPtr<IArrayList> mAllocatedDevices;
        AutoPtr<ArrayOf<Int32> > mFinished;
        Int32 mInitiatedBy;
    };

    class OnStandbyPendingActionClearedCallback
        : public Object
        , public IHdmiCecLocalDevicePendingActionClearedCallback
    {
    public:
        CAR_INTERFACE_DECL()

        OnStandbyPendingActionClearedCallback(
            /* [in] */ HdmiControlService* host,
            /* [in] */ IList* devices);

        // @Override
        CARAPI OnCleared(
            /* [in] */ IHdmiCecLocalDevice* device);

    private:
        HdmiControlService* mHost;
        AutoPtr<IList> mDevices;
    };

    class SetControlEnabledPendingActionClearedCallback
        : public Object
        , public IHdmiCecLocalDevicePendingActionClearedCallback
    {
    public:
        CAR_INTERFACE_DECL()

        SetControlEnabledPendingActionClearedCallback(
            /* [in] */ HdmiControlService* host);

        // @Override
        CARAPI OnCleared(
            /* [in] */ IHdmiCecLocalDevice* device);

    private:
        HdmiControlService* mHost;
    };

    class InnerSub_IHdmiControlCallbackStub
        : public Object
        , public IBinder
        , public IIHdmiControlCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_IHdmiControlCallbackStub(
            /* [in] */ HdmiControlService* host,
            /* [in] */ Int32 lastInput);

        // @Override
        CARAPI OnComplete(
            /* [in] */ Int32 result);

        // for IBinder
        CARAPI ToString(
            /* [out] */ String* result);

    private:
        HdmiControlService* mHost;
        Int32 mLastInput;
    };

public:
    CAR_INTERFACE_DECL()

    HdmiControlService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    CARAPI ReadBooleanSetting(
        /* [in] */ const String& key,
        /* [in] */ Boolean defVal,
        /* [out] */ Boolean* result);

    CARAPI WriteBooleanSetting(
        /* [in] */ const String& key,
        /* [in] */ Boolean value);

    CARAPI GetPortInfo(
        /* [out] */ IList** result);

    /**
     * Returns HDMI port information for the given port id.
     *
     * @param portId HDMI port id
     * @return {@link HdmiPortInfo} for the given port
     */
    CARAPI GetPortInfo(
        /* [in] */ Int32 portId,
        /* [out] */ IHdmiPortInfo** result);

    /**
     * Returns the routing path (physical address) of the HDMI port for the given
     * port id.
     */
    CARAPI PortIdToPath(
        /* [in] */ Int32 portId,
        /* [out] */ Int32* result);

    /**
     * Returns the id of HDMI port located at the top of the hierarchy of
     * the specified routing path. For the routing path 0x1220 (1.2.2.0), for instance,
     * the port id to be returned is the ID associated with the port address
     * 0x1000 (1.0.0.0) which is the topmost path of the given routing path.
     */
    CARAPI PathToPortId(
        /* [in] */ Int32 path,
        /* [out] */ Int32* result);

    CARAPI IsValidPortId(
        /* [in] */ Int32 portId,
        /* [out] */ Boolean* result);

    /**
     * Returns {@link Looper} for IO operation.
     *
     * <p>Declared as package-private.
     */
    CARAPI GetIoLooper(
        /* [out] */ ILooper** result);

    /**
     * Returns {@link Looper} of main thread. Use this {@link Looper} instance
     * for tasks that are running on main service thread.
     *
     * <p>Declared as package-private.
     */
    CARAPI GetServiceLooper(
        /* [out] */ ILooper** result);

    /**
     * Returns physical address of the device.
     */
    CARAPI GetPhysicalAddress(
        /* [out] */ Int32* result);

    /**
     * Returns vendor id of CEC service.
     */
    CARAPI GetVendorId(
        /* [out] */ Int32* result);

    // @ServiceThreadOnly
    CARAPI GetDeviceInfo(
        /* [in] */ Int32 logicalAddress,
        /* [out] */ IHdmiDeviceInfo** result);

    /**
     * Returns version of CEC.
     */
    CARAPI GetCecVersion(
        /* [out] */ Int32* result);

    /**
     * Whether a device of the specified physical address is connected to ARC enabled port.
     */
    CARAPI IsConnectedToArcPort(
        /* [in] */ Int32 physicalAddress,
        /* [out] */ Boolean* result);

    CARAPI RunOnServiceThread(
        /* [in] */ IRunnable* runnable);

    CARAPI RunOnServiceThreadAtFrontOfQueue(
        /* [in] */ IRunnable* runnable);

    /**
     * Transmit a CEC command to CEC bus.
     *
     * @param command CEC command to send out
     * @param callback interface used to the result of send command
     */
    // @ServiceThreadOnly
    CARAPI SendCecCommand(
        /* [in] */ IHdmiCecMessage* command,
        /* [in] */ IHdmiControlServiceSendMessageCallback* callback);

    // @ServiceThreadOnly
    CARAPI SendCecCommand(
        /* [in] */ IHdmiCecMessage* command);

    /**
     * Send <Feature Abort> command on the given CEC message if possible.
     * If the aborted message is invalid, then it wont send the message.
     * @param command original command to be aborted
     * @param reason reason of feature abort
     */
    // @ServiceThreadOnly
    CARAPI MaySendFeatureAbortCommand(
        /* [in] */ IHdmiCecMessage* command,
        /* [in] */ Int32 reason);

    // @ServiceThreadOnly
    CARAPI HandleCecCommand(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI SetAudioReturnChannel(
        /* [in] */ Boolean enabled);

    /**
     * Called when a new hotplug event is issued.
     *
     * @param portId hdmi port number where hot plug event issued.
     * @param connected whether to be plugged in or not
     */
    // @ServiceThreadOnly
    CARAPI OnHotplug(
        /* [in] */ Int32 portId,
        /* [in] */ Boolean connected);

    /**
     * Poll all remote devices. It sends &lt;Polling Message&gt; to all remote
     * devices.
     *
     * @param callback an interface used to get a list of all remote devices' address
     * @param sourceAddress a logical address of source device where sends polling message
     * @param pickStrategy strategy how to pick polling candidates
     * @param retryCount the number of retry used to send polling message to remote devices
     * @throw IllegalArgumentException if {@code pickStrategy} is invalid value
     */
    // @ServiceThreadOnly
    CARAPI PollDevices(
        /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
        /* [in] */ Int32 sourceAddress,
        /* [in] */ Int32 pickStrategy,
        /* [in] */ Int32 retryCount);

    CARAPI GetAllLocalDevices(
        /* [out] */ IList** result);

    CARAPI GetServiceLock(
        /* [out] */ IInterface** result);

    CARAPI SetAudioStatus(
        /* [in] */ Boolean mute,
        /* [in] */ Int32 volume);

    CARAPI AnnounceSystemAudioModeChange(
        /* [in] */ Boolean enabled);

    // @ServiceThreadOnly
    CARAPI HandleMhlHotplugEvent(
        /* [in] */ Int32 portId,
        /* [in] */ Boolean connected);

    // @ServiceThreadOnly
    CARAPI HandleMhlBusModeChanged(
        /* [in] */ Int32 portId,
        /* [in] */ Int32 busmode);

    // @ServiceThreadOnly
    CARAPI HandleMhlBusOvercurrent(
        /* [in] */ Int32 portId,
        /* [in] */ Boolean on);

    // @ServiceThreadOnly
    CARAPI HandleMhlDeviceStatusChanged(
        /* [in] */ Int32 portId,
        /* [in] */ Int32 adopterId,
        /* [in] */ Int32 deviceId);

    CARAPI InvokeDeviceEventListeners(
        /* [in] */ IHdmiDeviceInfo* device,
        /* [in] */ Int32 status);

    CARAPI InvokeInputChangeListener(
        /* [in] */ IHdmiDeviceInfo* info);

    CARAPI InvokeRecordRequestListener(
        /* [in] */ Int32 recorderAddress,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI InvokeOneTouchRecordResult(
        /* [in] */ Int32 result);

    CARAPI InvokeTimerRecordingResult(
        /* [in] */ Int32 result);

    CARAPI InvokeClearTimerRecordingResult(
        /* [in] */ Int32 result);

    CARAPI IsTvDevice(
        /* [out] */ Boolean* result);

    CARAPI GetAudioManager(
        /* [out] */ IAudioManager** result);

    CARAPI IsControlEnabled(
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI GetPowerStatus(
        /* [out] */ Int32* result);

    // @ServiceThreadOnly
    CARAPI IsPowerOnOrTransient(
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI IsPowerStandbyOrTransient(
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI IsPowerStandby(
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI WakeUp();

    // @ServiceThreadOnly
    CARAPI Standby();

    // @ServiceThreadOnly
    CARAPI GetLanguage(
        /* [out] */ String* result);

    CARAPI InvokeVendorCommandListeners(
        /* [in] */ Int32 deviceType,
        /* [in] */ Int32 srcAddress,
        /* [in] */ ArrayOf<Byte>* params,
        /* [in] */ Boolean hasVendorId,
        /* [out] */ Boolean* result);

    CARAPI InvokeMhlVendorCommandListeners(
        /* [in] */ Int32 portId,
        /* [in] */ Int32 offest,
        /* [in] */ Int32 length,
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI IsProhibitMode(
        /* [out] */ Boolean* result);

    CARAPI SetProhibitMode(
        /* [in] */ Boolean enabled);

    // @ServiceThreadOnly
    CARAPI SetCecOption(
        /* [in] */ Int32 key,
        /* [in] */ Int32 value);

    // @ServiceThreadOnly
    CARAPI SetControlEnabled(
        /* [in] */ Boolean enabled);

    // @ServiceThreadOnly
    CARAPI SetActivePortId(
        /* [in] */ Int32 portId);

    // @ServiceThreadOnly
    CARAPI SetLastInputForMhl(
        /* [in] */ Int32 portId);

    // @ServiceThreadOnly
    CARAPI GetLastInputForMhl(
        /* [out] */ Int32* result);

    /**
     * Performs input change, routing control for MHL device.
     *
     * @param portId MHL port, or the last port to go back to if {@code contentOn} is false
     * @param contentOn {@code true} if RAP data is content on; otherwise false
     */
    // @ServiceThreadOnly
    CARAPI ChangeInputForMhl(
        /* [in] */ Int32 portId,
        /* [in] */ Boolean contentOn);

    CARAPI SetMhlInputChangeEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsMhlInputChangeEnabled(
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI DisplayOsd(
        /* [in] */ Int32 messageId);

    // @ServiceThreadOnly
    CARAPI DisplayOsd(
        /* [in] */ Int32 messageId,
        /* [in] */ Int32 extra);

private:
    static CARAPI GetInt32List(
        /* [in] */ const String& string,
        /* [out] */ IList** result);

    /**
     * Called when the initialization of local devices is complete.
     */
    CARAPI OnInitializeCecComplete();

    CARAPI RegisterContentObserver();

    static CARAPI ToInt32(
        /* [in] */ Boolean enabled,
        /* [out] */ Int32* result);

    CARAPI UnregisterSettingsObserver();

    CARAPI InitializeCec(
        /* [in] */ Int32 initiatedBy);

    // @ServiceThreadOnly
    CARAPI InitializeLocalDevices(
        /* [in] */ Int32 initiatedBy);

    // @ServiceThreadOnly
    CARAPI AllocateLogicalAddress(
        /* [in] */ IArrayList* allocatingDevices,
        /* [in] */ Int32 initiatedBy);

    // @ServiceThreadOnly
    CARAPI NotifyAddressAllocated(
        /* [in] */ IArrayList* devices,
        /* [in] */ Int32 initiatedBy);

    // keep them in one place.
    // @ServiceThreadOnly
    CARAPI InitPortInfo();

    CARAPI AssertRunOnServiceThread();

    // @ServiceThreadOnly
    CARAPI DispatchMessageToLocalDevice(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI CheckPollStrategy(
        /* [in] */ Int32 pickStrategy,
        /* [out] */ Int32* result);

    CARAPI CreateDeviceInfo(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 deviceType,
        /* [in] */ Int32 powerStatus,
        /* [out] */ IHdmiDeviceInfo** result);

    // @ServiceThreadOnly
    CARAPI UpdateSafeMhlInput();

    CARAPI GetMhlDevicesLocked(
        /* [out] */ IList** result);

    CARAPI EnforceAccessPermission();

    // @ServiceThreadOnly
    CARAPI OneTouchPlay(
        /* [in] */ IIHdmiControlCallback* callback);

    // @ServiceThreadOnly
    CARAPI QueryDisplayStatus(
        /* [in] */ IIHdmiControlCallback* callback);

    CARAPI AddHotplugEventListener(
        /* [in] */ IIHdmiHotplugEventListener* listener);

    CARAPI RemoveHotplugEventListener(
        /* [in] */ IIHdmiHotplugEventListener* listener);

    CARAPI AddDeviceEventListener(
        /* [in] */ IIHdmiDeviceEventListener* listener);

    CARAPI AddSystemAudioModeChangeListner(
        /* [in] */ IIHdmiSystemAudioModeChangeListener* listener);

    CARAPI RemoveSystemAudioModeChangeListener(
        /* [in] */ IIHdmiSystemAudioModeChangeListener* listener);

    CARAPI SetInputChangeListener(
        /* [in] */ IIHdmiInputChangeListener* listener);

    CARAPI SetHdmiRecordListener(
        /* [in] */ IIHdmiRecordListener* listener);

    CARAPI InvokeCallback(
        /* [in] */ IIHdmiControlCallback* callback,
        /* [in] */ Int32 result);

    CARAPI InvokeSystemAudioModeChangeLocked(
        /* [in] */ IIHdmiSystemAudioModeChangeListener* listener,
        /* [in] */ Boolean enabled);

    CARAPI AnnounceHotplugEvent(
        /* [in] */ Int32 portId,
        /* [in] */ Boolean connected);

    CARAPI InvokeHotplugEventListenerLocked(
        /* [in] */ IIHdmiHotplugEventListener* listener,
        /* [in] */ IHdmiHotplugEvent* event);

    CARAPI Tv(
        /* [out] */ IHdmiCecLocalDeviceTv** result);

    CARAPI Playback(
        /* [out] */ HdmiCecLocalDevicePlayback** result);

    // @ServiceThreadOnly
    CARAPI OnWakeUp();

    // @ServiceThreadOnly
    CARAPI OnStandby();

    // @ServiceThreadOnly
    CARAPI OnLanguageChanged(
        /* [in] */ const String& language);

    CARAPI DisableDevices(
        /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* callback);

    // @ServiceThreadOnly
    CARAPI ClearLocalDevices();

    // @ServiceThreadOnly
    CARAPI OnStandbyCompleted();

    CARAPI AddVendorCommandListener(
        /* [in] */ IIHdmiVendorCommandListener* listener,
        /* [in] */ Int32 deviceType);

    CARAPI AddHdmiMhlVendorCommandListener(
        /* [in] */ IIHdmiMhlVendorCommandListener* listener);

public:
    static const String PERMISSION;

    // The reason code to initiate intializeCec().
    static const Int32 INITIATED_BY_ENABLE_CEC;
    static const Int32 INITIATED_BY_BOOT_UP;
    static const Int32 INITIATED_BY_SCREEN_ON;
    static const Int32 INITIATED_BY_WAKE_UP_MESSAGE;
    static const Int32 INITIATED_BY_HOTPLUG;

private:
    static const String TAG;

    // A thread to handle synchronous IO of CEC and MHL control service.
    // Since all of CEC and MHL HAL interfaces processed in short time (< 200ms)
    // and sparse call it shares a thread to handle IO operations.
    AutoPtr<IHandlerThread> mIoThread;

    // Used to synchronize the access to the service.
    AutoPtr<IObject> mLock;

    // Type of logical devices hosted in the system. Stored in the unmodifiable list.
    AutoPtr<IList> mLocalDevices;

    // List of records for device event listener to handle the caller killed in action.
    // @GuardedBy("mLock")
    AutoPtr<IArrayList> mDeviceEventListenerRecords;

    // List of records for vendor command listener to handle the caller killed in action.
    // @GuardedBy("mLock")
    AutoPtr<IArrayList> mVendorCommandListenerRecords;

    // @GuardedBy("mLock")
    AutoPtr<InputChangeListenerRecord> mInputChangeListenerRecord;

    // @GuardedBy("mLock")
    AutoPtr<HdmiRecordListenerRecord> mRecordListenerRecord;

    // Set to true while HDMI control is enabled. If set to false, HDMI-CEC/MHL protocol
    // handling will be disabled and no request will be handled.
    // @GuardedBy("mLock")
    Boolean mHdmiControlEnabled;

    // Set to true while the service is in normal mode. While set to false, no input change is
    // allowed. Used for situations where input change can confuse users such as channel auto-scan,
    // system upgrade, etc., a.k.a. "prohibit mode".
    // @GuardedBy("mLock")
    Boolean mProhibitMode;

    // List of records for system audio mode change to handle the the caller killed in action.
    AutoPtr<IArrayList> mSystemAudioModeChangeListenerRecords;

    // Handler used to run a task in service thread.
    AutoPtr<IHandler> mHandler;

    AutoPtr<SettingsObserver> mSettingsObserver;

    AutoPtr<HdmiControlBroadcastReceiver> mHdmiControlBroadcastReceiver;

    // @Nullable
    AutoPtr<HdmiCecController> mCecController;

    // HDMI port information. Stored in the unmodifiable list to keep the static information
    // from being modified.
    AutoPtr<IList> mPortInfo;

    // Map from path(physical address) to port ID.
    AutoPtr<UnmodifiableSparseInt32Array> mPortIdMap;

    // Map from port ID to HdmiPortInfo.
    AutoPtr<UnmodifiableSparseArray> mPortInfoMap;

    // Map from port ID to HdmiDeviceInfo.
    AutoPtr<UnmodifiableSparseArray> mPortDeviceMap;

    AutoPtr<HdmiCecMessageValidator> mMessageValidator;

    // @ServiceThreadOnly
    Int32 mPowerStatus;

    // @ServiceThreadOnly
    String mLanguage;

    // @ServiceThreadOnly
    Boolean mStandbyMessageReceived;

    // @ServiceThreadOnly
    Boolean mWakeUpMessageReceived;

    // @ServiceThreadOnly
    Int32 mActivePortId;

    // Set to true while the input change by MHL is allowed.
    // @GuardedBy("mLock")
    Boolean mMhlInputChangeEnabled;

    // List of records for MHL Vendor command listener to handle the caller killed in action.
    // @GuardedBy("mLock")
    AutoPtr<IArrayList> mMhlVendorCommandListenerRecords;

    // @GuardedBy("mLock")
    AutoPtr<IList> mMhlDevices;

    // @Nullable
    AutoPtr<HdmiMhlControllerStub> mMhlController;

    // Last input port before switching to the MHL port. Should switch back to this port
    // when the mobile device sends the request one touch play with off.
    // Gets invalidated if we go to other port/input.
    // @ServiceThreadOnly
    Int32 mLastInputMhl;

    // List of records for hotplug event listener to handle the the caller killed in action.
    // @GuardedBy("mLock")
    AutoPtr<IArrayList> mHotplugEventListenerRecords;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICONTROLSERVICE_H__
