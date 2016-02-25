
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include <elastos/utility/Arrays.h>
#include "elastos/droid/server/hdmi/Constants.h"

// import static android.hardware.hdmi.HdmiControlManager.DEVICE_EVENT_ADD_DEVICE;
// import static android.hardware.hdmi.HdmiControlManager.DEVICE_EVENT_REMOVE_DEVICE;
// import static com.android.server.hdmi.Constants.DISABLED;
// import static com.android.server.hdmi.Constants.ENABLED;
// import static com.android.server.hdmi.Constants.OPTION_CEC_AUTO_WAKEUP;
// import static com.android.server.hdmi.Constants.OPTION_CEC_ENABLE;
// import static com.android.server.hdmi.Constants.OPTION_CEC_SERVICE_CONTROL;
// import static com.android.server.hdmi.Constants.OPTION_MHL_ENABLE;
// import static com.android.server.hdmi.Constants.OPTION_MHL_INPUT_SWITCHING;
// import static com.android.server.hdmi.Constants.OPTION_MHL_POWER_CHARGE;
// import libcore.util.EmptyArray;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiPortInfo;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Hardware::Hdmi::IIHdmiInputChangeListener;
using Elastos::Droid::Hardware::Hdmi::IIHdmiMhlVendorCommandListener;
using Elastos::Droid::Hardware::Hdmi::IIHdmiRecordListener;
using Elastos::Droid::Hardware::Hdmi::IIHdmiVendorCommandListener;
// using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IPowerManager;
// using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
// using Elastos::Droid::Provider::ISettings::Global;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Server::ISystemService;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollections;
using Elastos::Utility::ILocale;
using Elastos::Droid::Hardware::Hdmi::EIID_IIHdmiControlService;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// HdmiControlService::VendorCommandListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::VendorCommandListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::VendorCommandListenerRecord::VendorCommandListenerRecord(
    /* [in] */ IIHdmiVendorCommandListener* listener,
    /* [in] */ Int32 deviceType)
    : mListener(listener)
    , mDeviceType(deviceType)
{}

ECode HdmiControlService::VendorCommandListenerRecord::ProxyDied()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                synchronized (mLock) {
                    mVendorCommandListenerRecords.remove(this);
                }

#endif
}

//=============================================================================
// HdmiControlService::HdmiControlBroadcastReceiver
//=============================================================================
ECode HdmiControlService::HdmiControlBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                assertRunOnServiceThread();
                switch (intent.getAction()) {
                    case Intent.ACTION_SCREEN_OFF:
                        if (isPowerOnOrTransient()) {
                            onStandby();
                        }
                        break;
                    case Intent.ACTION_SCREEN_ON:
                        if (isPowerStandbyOrTransient()) {
                            onWakeUp();
                        }
                        break;
                    case Intent.ACTION_CONFIGURATION_CHANGED:
                        String language = Locale.getDefault().getISO3Language();
                        if (!mLanguage.equals(language)) {
                            onLanguageChanged(language);
                        }
                        break;
                }

#endif
}

//=============================================================================
// HdmiControlService::SettingsObserver
//=============================================================================
HdmiControlService::SettingsObserver::SettingsObserver(
    /* [in] */ IHandler* handler)
{
    ContentObserver::constructor(handler);
}

ECode HdmiControlService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                String option = uri.getLastPathSegment();
                boolean enabled = readBooleanSetting(option, true);
                switch (option) {
                    case Global.HDMI_CONTROL_ENABLED:
                        setControlEnabled(enabled);
                        break;
                    case Global.HDMI_CONTROL_AUTO_WAKEUP_ENABLED:
                        tv().setAutoWakeup(enabled);
                        setCecOption(OPTION_CEC_AUTO_WAKEUP, toInt(enabled));
                        break;
                    case Global.HDMI_CONTROL_AUTO_DEVICE_OFF_ENABLED:
                        tv().setAutoDeviceOff(enabled);
                        // No need to propagate to HAL.
                        break;
                    case Global.MHL_INPUT_SWITCHING_ENABLED:
                        setMhlInputChangeEnabled(enabled);
                        break;
                    case Global.MHL_POWER_CHARGE_ENABLED:
                        mMhlController.setOption(OPTION_MHL_POWER_CHARGE, toInt(enabled));
                        break;
                }

#endif
}

//=============================================================================
// HdmiControlService::HdmiMhlVendorCommandListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::HdmiMhlVendorCommandListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::HdmiMhlVendorCommandListenerRecord::HdmiMhlVendorCommandListenerRecord(
    /* [in] */ IIHdmiMhlVendorCommandListener* listener)
    : mListener(listener)
{

}

ECode HdmiControlService::HdmiMhlVendorCommandListenerRecord::ProxyDied()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                mMhlVendorCommandListenerRecords.remove(this);

#endif
}

//=============================================================================
// HdmiControlService::HotplugEventListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::HotplugEventListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::HotplugEventListenerRecord::HotplugEventListenerRecord(
    /* [in] */ IIHdmiHotplugEventListener* listener)
    : mListener(listener)
{}

ECode HdmiControlService::HotplugEventListenerRecord::ProxyDied()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                synchronized (mLock) {
                    mHotplugEventListenerRecords.remove(this);
                }

#endif
}

//=============================================================================
// HdmiControlService::DeviceEventListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::DeviceEventListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::DeviceEventListenerRecord::DeviceEventListenerRecord(
    /* [in] */ IIHdmiDeviceEventListener* listener)
    : mListener(listener)
{}

ECode HdmiControlService::DeviceEventListenerRecord::ProxyDied()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                synchronized (mLock) {
                    mDeviceEventListenerRecords.remove(this);
                }

#endif
}

//=============================================================================
// HdmiControlService::SystemAudioModeChangeListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::SystemAudioModeChangeListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::SystemAudioModeChangeListenerRecord::SystemAudioModeChangeListenerRecord(
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener)
    : mListener(listener)
{}

ECode HdmiControlService::SystemAudioModeChangeListenerRecord::ProxyDied()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                synchronized (mLock) {
                    mSystemAudioModeChangeListenerRecords.remove(this);
                }

#endif
}

//=============================================================================
// HdmiControlService::HdmiRecordListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::HdmiRecordListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::HdmiRecordListenerRecord::HdmiRecordListenerRecord(
    /* [in] */ IIHdmiRecordListener* listener)
    : mListener(listener)
{}

ECode HdmiControlService::HdmiRecordListenerRecord::ProxyDied()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                synchronized (mLock) {
                    mRecordListenerRecord = null;
                }

#endif
}

//=============================================================================
// HdmiControlService::BinderService
//=============================================================================
CAR_INTERFACE_IMPL_2(HdmiControlService::BinderService, Object, IIHdmiControlService, IBinder)

ECode HdmiControlService::BinderService::GetSupportedTypes(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                // mLocalDevices is an unmodifiable list - no lock necesary.
                int[] localDevices = new int[mLocalDevices.size()];
                for (int i = 0; i < localDevices.length; ++i) {
                    localDevices[i] = mLocalDevices.get(i);
                }
                return localDevices;

#endif
}

ECode HdmiControlService::BinderService::GetActiveSource(
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiCecLocalDeviceTv tv = tv();
                if (tv == null) {
                    Slog.w(TAG, "Local tv device not available");
                    return null;
                }
                ActiveSource activeSource = tv.getActiveSource();
                if (activeSource.isValid()) {
                    return new HdmiDeviceInfo(activeSource.logicalAddress,
                            activeSource.physicalAddress, HdmiDeviceInfo.PORT_INVALID,
                            HdmiDeviceInfo.DEVICE_INACTIVE, 0, "");
                }
                int activePath = tv.getActivePath();
                if (activePath != HdmiDeviceInfo.PATH_INVALID) {
                    return new HdmiDeviceInfo(activePath, tv.getActivePortId());
                }
                return null;

#endif
}

ECode HdmiControlService::BinderService::DeviceSelect(
    /* [in] */ Int32 deviceId,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        if (callback == null) {
                            Slog.e(TAG, "Callback cannot be null");
                            return;
                        }
                        HdmiCecLocalDeviceTv tv = tv();
                        if (tv == null) {
                            Slog.w(TAG, "Local tv device not available");
                            invokeCallback(callback, HdmiControlManager.RESULT_SOURCE_NOT_AVAILABLE);
                            return;
                        }
                        HdmiMhlLocalDeviceStub device = mMhlController.getLocalDeviceById(deviceId);
                        if (device != null) {
                            if (device.getPortId() == tv.getActivePortId()) {
                                invokeCallback(callback, HdmiControlManager.RESULT_SUCCESS);
                                return;
                            }
                            // Upon selecting MHL device, we send RAP[Content On] to wake up
                            // the connected mobile device, start routing control to switch ports.
                            // callback is handled by MHL action.
                            device.turnOn(callback);
                            tv.doManualPortSwitching(device.getPortId(), null);
                            return;
                        }
                        tv.deviceSelect(deviceId, callback);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::PortSelect(
    /* [in] */ Int32 portId,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        if (callback == null) {
                            Slog.e(TAG, "Callback cannot be null");
                            return;
                        }
                        HdmiCecLocalDeviceTv tv = tv();
                        if (tv == null) {
                            Slog.w(TAG, "Local tv device not available");
                            invokeCallback(callback, HdmiControlManager.RESULT_SOURCE_NOT_AVAILABLE);
                            return;
                        }
                        tv.doManualPortSwitching(portId, callback);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::SendKeyEvent(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 keyCode,
    /* [in] */ Boolean isPressed)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        HdmiMhlLocalDeviceStub device = mMhlController.getLocalDevice(mActivePortId);
                        if (device != null) {
                            device.sendKeyEvent(keyCode, isPressed);
                            return;
                        }
                        if (mCecController != null) {
                            HdmiCecLocalDevice localDevice = mCecController.getLocalDevice(deviceType);
                            if (localDevice == null) {
                                Slog.w(TAG, "Local device not available");
                                return;
                            }
                            localDevice.sendKeyEvent(keyCode, isPressed);
                        }
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::OneTouchPlay(
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        HdmiControlService.this.oneTouchPlay(callback);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::QueryDisplayStatus(
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        HdmiControlService.this.queryDisplayStatus(callback);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::AddHotplugEventListener(
    /* [in] */ IIHdmiHotplugEventListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiControlService.this.addHotplugEventListener(listener);

#endif
}

ECode HdmiControlService::BinderService::RemoveHotplugEventListener(
    /* [in] */ IIHdmiHotplugEventListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiControlService.this.removeHotplugEventListener(listener);

#endif
}

ECode HdmiControlService::BinderService::AddDeviceEventListener(
    /* [in] */ IIHdmiDeviceEventListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiControlService.this.addDeviceEventListener(listener);

#endif
}

ECode HdmiControlService::BinderService::GetPortInfo(
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                return HdmiControlService.this.getPortInfo();

#endif
}

ECode HdmiControlService::BinderService::CanChangeSystemAudioMode(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiCecLocalDeviceTv tv = tv();
                if (tv == null) {
                    return false;
                }
                return tv.hasSystemAudioDevice();

#endif
}

ECode HdmiControlService::BinderService::GetSystemAudioMode(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiCecLocalDeviceTv tv = tv();
                if (tv == null) {
                    return false;
                }
                return tv.isSystemAudioActivated();

#endif
}

ECode HdmiControlService::BinderService::SetSystemAudioMode(
    /* [in] */ Boolean enabled,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        HdmiCecLocalDeviceTv tv = tv();
                        if (tv == null) {
                            Slog.w(TAG, "Local tv device not available");
                            invokeCallback(callback, HdmiControlManager.RESULT_SOURCE_NOT_AVAILABLE);
                            return;
                        }
                        tv.changeSystemAudioMode(enabled, callback);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::AddSystemAudioModeChangeListener(
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiControlService.this.addSystemAudioModeChangeListner(listener);

#endif
}

ECode HdmiControlService::BinderService::RemoveSystemAudioModeChangeListener(
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiControlService.this.removeSystemAudioModeChangeListener(listener);

#endif
}

ECode HdmiControlService::BinderService::SetInputChangeListener(
    /* [in] */ IIHdmiInputChangeListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiControlService.this.setInputChangeListener(listener);

#endif
}

ECode HdmiControlService::BinderService::GetInputDevices(
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                // No need to hold the lock for obtaining TV device as the local device instance
                // is preserved while the HDMI control is enabled.
                HdmiCecLocalDeviceTv tv = tv();
                synchronized (mLock) {
                    List<HdmiDeviceInfo> cecDevices = (tv == null)
                            ? Collections.<HdmiDeviceInfo>emptyList()
                            : tv.getSafeExternalInputsLocked();
                    return HdmiUtils.mergeToUnmodifiableList(cecDevices, getMhlDevicesLocked());
                }

#endif
}

ECode HdmiControlService::BinderService::SetSystemAudioVolume(
    /* [in] */ Int32 oldIndex,
    /* [in] */ Int32 newIndex,
    /* [in] */ Int32 maxIndex)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        HdmiCecLocalDeviceTv tv = tv();
                        if (tv == null) {
                            Slog.w(TAG, "Local tv device not available");
                            return;
                        }
                        tv.changeVolume(oldIndex, newIndex - oldIndex, maxIndex);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::SetSystemAudioMute(
    /* [in] */ Boolean mute)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        HdmiCecLocalDeviceTv tv = tv();
                        if (tv == null) {
                            Slog.w(TAG, "Local tv device not available");
                            return;
                        }
                        tv.changeMute(mute);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::SetArcMode(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        HdmiCecLocalDeviceTv tv = tv();
                        if (tv == null) {
                            Slog.w(TAG, "Local tv device not available to change arc mode.");
                            return;
                        }
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::SetProhibitMode(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                if (!isTvDevice()) {
                    return;
                }
                HdmiControlService.this.setProhibitMode(enabled);

#endif
}

ECode HdmiControlService::BinderService::AddVendorCommandListener(
    /* [in] */ IIHdmiVendorCommandListener* listener,
    /* [in] */ Int32 deviceType)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiControlService.this.addVendorCommandListener(listener, deviceType);

#endif
}

ECode HdmiControlService::BinderService::SendVendorCommand(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 targetAddress,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Boolean hasVendorId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        HdmiCecLocalDevice device = mCecController.getLocalDevice(deviceType);
                        if (device == null) {
                            Slog.w(TAG, "Local device not available");
                            return;
                        }
                        if (hasVendorId) {
                            sendCecCommand(HdmiCecMessageBuilder.buildVendorCommandWithId(
                                    device.getDeviceInfo().getLogicalAddress(), targetAddress,
                                    getVendorId(), params));
                        } else {
                            sendCecCommand(HdmiCecMessageBuilder.buildVendorCommand(
                                    device.getDeviceInfo().getLogicalAddress(), targetAddress, params));
                        }
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::SendStandby(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 deviceId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        HdmiCecLocalDevice device = mCecController.getLocalDevice(deviceType);
                        if (device == null) {
                            Slog.w(TAG, "Local device not available");
                            return;
                        }
                        device.sendStandby(deviceId);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::SetHdmiRecordListener(
    /* [in] */ IIHdmiRecordListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiControlService.this.setHdmiRecordListener(listener);

#endif
}

ECode HdmiControlService::BinderService::StartOneTouchRecord(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        if (!isTvDevice()) {
                            Slog.w(TAG, "No TV is available.");
                            return;
                        }
                        tv().startOneTouchRecord(recorderAddress, recordSource);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::StopOneTouchRecord(
    /* [in] */ Int32 recorderAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        if (!isTvDevice()) {
                            Slog.w(TAG, "No TV is available.");
                            return;
                        }
                        tv().stopOneTouchRecord(recorderAddress);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::StartTimerRecording(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        if (!isTvDevice()) {
                            Slog.w(TAG, "No TV is available.");
                            return;
                        }
                        tv().startTimerRecording(recorderAddress, sourceType, recordSource);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::ClearTimerRecording(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        if (!isTvDevice()) {
                            Slog.w(TAG, "No TV is available.");
                            return;
                        }
                        tv().clearTimerRecording(recorderAddress, sourceType, recordSource);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::SendMhlVendorCommand(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Byte>* data)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        if (!isControlEnabled()) {
                            Slog.w(TAG, "Hdmi control is disabled.");
                            return ;
                        }
                        HdmiMhlLocalDeviceStub device = mMhlController.getLocalDevice(portId);
                        if (device == null) {
                            Slog.w(TAG, "Invalid port id:" + portId);
                            return;
                        }
                        mMhlController.sendVendorCommand(portId, offset, length, data);
                    }
                });

#endif
}

ECode HdmiControlService::BinderService::AddHdmiMhlVendorCommandListener(
    /* [in] */ IIHdmiMhlVendorCommandListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                enforceAccessPermission();
                HdmiControlService.this.addHdmiMhlVendorCommandListener(listener);

#endif
}

ECode HdmiControlService::BinderService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                getContext().enforceCallingOrSelfPermission(android.Manifest.permission.DUMP, TAG);
                final IndentingPrintWriter pw = new IndentingPrintWriter(writer, "  ");
                pw.println("mHdmiControlEnabled: " + mHdmiControlEnabled);
                pw.println("mProhibitMode: " + mProhibitMode);
                if (mCecController != null) {
                    pw.println("mCecController: ");
                    pw.increaseIndent();
                    mCecController.dump(pw);
                    pw.decreaseIndent();
                }
                pw.println("mPortInfo: ");
                pw.increaseIndent();
                for (HdmiPortInfo hdmiPortInfo : mPortInfo) {
                    pw.println("- " + hdmiPortInfo);
                }
                pw.decreaseIndent();
                pw.println("mPowerStatus: " + mPowerStatus);

#endif
}

ECode HdmiControlService::BinderService::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

//=============================================================================
// HdmiControlService::InputChangeListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::InputChangeListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::InputChangeListenerRecord::InputChangeListenerRecord(
    /* [in] */ IIHdmiInputChangeListener* listener)
    : mListener(listener)
{}

ECode HdmiControlService::InputChangeListenerRecord::ProxyDied()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                synchronized (mLock) {
                    mInputChangeListenerRecord = null;
                }

#endif
}

//=============================================================================
// HdmiControlService
//=============================================================================
const String HdmiControlService::TAG("HdmiControlService");
const String HdmiControlService::PERMISSION("android.permission.HDMI_CEC");
const Int32 HdmiControlService::INITIATED_BY_ENABLE_CEC = 0;
const Int32 HdmiControlService::INITIATED_BY_BOOT_UP = 1;
const Int32 HdmiControlService::INITIATED_BY_SCREEN_ON = 2;
const Int32 HdmiControlService::INITIATED_BY_WAKE_UP_MESSAGE = 3;
const Int32 HdmiControlService::INITIATED_BY_HOTPLUG = 4;

HdmiControlService::HdmiControlService()
    : mHdmiControlEnabled(FALSE)
    , mProhibitMode(FALSE)
    , mPowerStatus(IHdmiControlManager::POWER_STATUS_STANDBY)
    , mStandbyMessageReceived(FALSE)
    , mWakeUpMessageReceived(FALSE)
    , mActivePortId(Constants::INVALID_PORT_ID)
    , mMhlInputChangeEnabled(FALSE)
    , mLastInputMhl(Constants::INVALID_PORT_ID)
{
#if 0 // TODO: Translate codes below
    mIoThread = new HandlerThread("Hdmi Control Io Thread");
    mLock = new Object();
    mDeviceEventListenerRecords = new ArrayList<>();
    mVendorCommandListenerRecords = new ArrayList<>();
    mSystemAudioModeChangeListenerRecords = new ArrayList<>();
    mHandler = new Handler();
    mHdmiControlBroadcastReceiver = new HdmiControlBroadcastReceiver();
    mLanguage = Locale.getDefault().getISO3Language();
    mMhlVendorCommandListenerRecords = new ArrayList<>();
    mHotplugEventListenerRecords = new ArrayList<>();
#endif
}

ECode HdmiControlService::constructor(
    /* [in] */ IContext* context)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super(context);
        mLocalDevices = getIntList(SystemProperties.get(Constants::PROPERTY_DEVICE_TYPE));
        mSettingsObserver = new SettingsObserver(mHandler);

#endif
}

ECode HdmiControlService::GetIntList(
    /* [in] */ const String& string,
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ArrayList<Integer> list = new ArrayList<>();
        TextUtils.SimpleStringSplitter splitter = new TextUtils.SimpleStringSplitter(',');
        splitter.setString(string);
        for (String item : splitter) {
            try {
                list.add(Integer.parseInt(item));
            } catch (NumberFormatException e) {
                Slogger::W(TAG, "Can't parseInt: " + item);
            }
        }
        return Collections.unmodifiableList(list);

#endif
}

ECode HdmiControlService::OnStart()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mIoThread.start();
        mPowerStatus = HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON;
        mProhibitMode = false;
        mHdmiControlEnabled = readBooleanSetting(Global.HDMI_CONTROL_ENABLED, true);
        mMhlInputChangeEnabled = readBooleanSetting(Global.MHL_INPUT_SWITCHING_ENABLED, true);
        mCecController = HdmiCecController.create(this);
        if (mCecController != NULL) {
            // TODO: Remove this as soon as OEM's HAL implementation is corrected.
            mCecController.setOption(OPTION_CEC_ENABLE, ENABLED);
            // TODO: load value for mHdmiControlEnabled from preference.
            if (mHdmiControlEnabled) {
                initializeCec(INITIATED_BY_BOOT_UP);
            }
        } else {
            Slogger::I(TAG, "Device does not support HDMI-CEC.");
        }
        mMhlController = HdmiMhlControllerStub.create(this);
        if (!mMhlController.isReady()) {
            Slogger::I(TAG, "Device does not support MHL-control.");
        }
        mMhlDevices = Collections.emptyList();
        initPortInfo();
        mMessageValidator = new HdmiCecMessageValidator(this);
        publishBinderService(Context.HDMI_CONTROL_SERVICE, new BinderService());
        // Register broadcast receiver for power state change.
        if (mCecController != NULL) {
            IntentFilter filter = new IntentFilter();
            filter.addAction(Intent.ACTION_SCREEN_OFF);
            filter.addAction(Intent.ACTION_SCREEN_ON);
            filter.addAction(Intent.ACTION_CONFIGURATION_CHANGED);
            getContext().registerReceiver(mHdmiControlBroadcastReceiver, filter);
        }

#endif
}

ECode HdmiControlService::OnInitializeCecComplete()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mPowerStatus == HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON) {
            mPowerStatus = HdmiControlManager.POWER_STATUS_ON;
        }
        mWakeUpMessageReceived = false;
        if (isTvDevice()) {
            mCecController.setOption(OPTION_CEC_AUTO_WAKEUP, toInt(tv().getAutoWakeup()));
            registerContentObserver();
        }

#endif
}

ECode HdmiControlService::RegisterContentObserver()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ContentResolver resolver = getContext().getContentResolver();
        String[] settings = new String[] {
                Global.HDMI_CONTROL_ENABLED,
                Global.HDMI_CONTROL_AUTO_WAKEUP_ENABLED,
                Global.HDMI_CONTROL_AUTO_DEVICE_OFF_ENABLED,
                Global.MHL_INPUT_SWITCHING_ENABLED,
                Global.MHL_POWER_CHARGE_ENABLED
        };
        for (String s : settings) {
            resolver.registerContentObserver(Global.getUriFor(s), false, mSettingsObserver,
                    UserHandle.USER_ALL);
        }

#endif
}

ECode HdmiControlService::ToInt(
    /* [in] */ Boolean enabled,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return enabled ? ENABLED : DISABLED;

#endif
}

ECode HdmiControlService::ReadBooleanSetting(
    /* [in] */ const String& key,
    /* [in] */ Boolean defVal,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ContentResolver cr = getContext().getContentResolver();
        return Global.getInt(cr, key, toInt(defVal)) == ENABLED;

#endif
}

ECode HdmiControlService::WriteBooleanSetting(
    /* [in] */ const String& key,
    /* [in] */ Boolean value)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ContentResolver cr = getContext().getContentResolver();
        Global.putInt(cr, key, toInt(value));

#endif
}

ECode HdmiControlService::UnregisterSettingsObserver()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        getContext().getContentResolver().unregisterContentObserver(mSettingsObserver);

#endif
}

ECode HdmiControlService::InitializeCec(
    /* [in] */ Int32 initiatedBy)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mCecController.setOption(OPTION_CEC_SERVICE_CONTROL, ENABLED);
        initializeLocalDevices(initiatedBy);

#endif
}

ECode HdmiControlService::InitializeLocalDevices(
    /* [in] */ Int32 initiatedBy)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // A container for [Device type, Local device info].
        ArrayList<HdmiCecLocalDevice> localDevices = new ArrayList<>();
        for (int type : mLocalDevices) {
            HdmiCecLocalDevice localDevice = mCecController.getLocalDevice(type);
            if (localDevice == NULL) {
                localDevice = HdmiCecLocalDevice.create(this, type);
            }
            localDevice.init();
            localDevices.add(localDevice);
        }
        // It's now safe to flush existing local devices from mCecController since they were
        // already moved to 'localDevices'.
        clearLocalDevices();
        allocateLogicalAddress(localDevices, initiatedBy);

#endif
}

ECode HdmiControlService::AllocateLogicalAddress(
    /* [in] */ IArrayList* allocatingDevices,
    /* [in] */ Int32 initiatedBy)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mCecController.clearLogicalAddress();
        final ArrayList<HdmiCecLocalDevice> allocatedDevices = new ArrayList<>();
        final int[] finished = new int[1];
        for (final HdmiCecLocalDevice localDevice : allocatingDevices) {
            mCecController.allocateLogicalAddress(localDevice.getType(),
                    localDevice.getPreferredAddress(), new AllocateAddressCallback() {
                @Override
                public void onAllocated(int deviceType, int logicalAddress) {
                    if (logicalAddress == Constants::ADDR_UNREGISTERED) {
                        Slogger::E(TAG, "Failed to allocate address:[device_type:" + deviceType + "]");
                    } else {
                        // Set POWER_STATUS_ON to all local devices because they share lifetime
                        // with system.
                        HdmiDeviceInfo deviceInfo = createDeviceInfo(logicalAddress, deviceType,
                                HdmiControlManager.POWER_STATUS_ON);
                        localDevice.setDeviceInfo(deviceInfo);
                        mCecController.addLocalDevice(deviceType, localDevice);
                        mCecController.addLogicalAddress(logicalAddress);
                        allocatedDevices.add(localDevice);
                    }
                    // Address allocation completed for all devices. Notify each device.
                    if (allocatingDevices.size() == ++finished[0]) {
                        if (initiatedBy != INITIATED_BY_HOTPLUG) {
                            // In case of the hotplug we don't call onInitializeCecComplete()
                            // since we reallocate the logical address only.
                            onInitializeCecComplete();
                        }
                        notifyAddressAllocated(allocatedDevices, initiatedBy);
                    }
                }
            });
        }

#endif
}

ECode HdmiControlService::NotifyAddressAllocated(
    /* [in] */ IArrayList* devices,
    /* [in] */ Int32 initiatedBy)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        for (HdmiCecLocalDevice device : devices) {
            int address = device.getDeviceInfo().getLogicalAddress();
            device.handleAddressAllocated(address, initiatedBy);
        }

#endif
}

ECode HdmiControlService::InitPortInfo()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiPortInfo[] cecPortInfo = NULL;
        // CEC HAL provides majority of the info while MHL does only MHL support flag for
        // each port. Return empty array if CEC HAL didn't provide the info.
        if (mCecController != NULL) {
            cecPortInfo = mCecController.getPortInfos();
        }
        if (cecPortInfo == NULL) {
            return;
        }
        SparseArray<HdmiPortInfo> portInfoMap = new SparseArray<>();
        SparseIntArray portIdMap = new SparseIntArray();
        SparseArray<HdmiDeviceInfo> portDeviceMap = new SparseArray<>();
        for (HdmiPortInfo info : cecPortInfo) {
            portIdMap.put(info.getAddress(), info.getId());
            portInfoMap.put(info.getId(), info);
            portDeviceMap.put(info.getId(), new HdmiDeviceInfo(info.getAddress(), info.getId()));
        }
        mPortIdMap = new UnmodifiableSparseIntArray(portIdMap);
        mPortInfoMap = new UnmodifiableSparseArray<>(portInfoMap);
        mPortDeviceMap = new UnmodifiableSparseArray<>(portDeviceMap);
        HdmiPortInfo[] mhlPortInfo = mMhlController.getPortInfos();
        ArraySet<Integer> mhlSupportedPorts = new ArraySet<Integer>(mhlPortInfo.length);
        for (HdmiPortInfo info : mhlPortInfo) {
            if (info.isMhlSupported()) {
                mhlSupportedPorts.add(info.getId());
            }
        }
        // Build HDMI port info list with CEC port info plus MHL supported flag. We can just use
        // cec port info if we do not have have port that supports MHL.
        if (mhlSupportedPorts.isEmpty()) {
            mPortInfo = Collections.unmodifiableList(Arrays.asList(cecPortInfo));
            return;
        }
        ArrayList<HdmiPortInfo> result = new ArrayList<>(cecPortInfo.length);
        for (HdmiPortInfo info : cecPortInfo) {
            if (mhlSupportedPorts.contains(info.getId())) {
                result.add(new HdmiPortInfo(info.getId(), info.getType(), info.getAddress(),
                        info.isCecSupported(), true, info.isArcSupported()));
            } else {
                result.add(info);
            }
        }
        mPortInfo = Collections.unmodifiableList(result);

#endif
}

ECode HdmiControlService::GetPortInfo(
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mPortInfo;

#endif
}

ECode HdmiControlService::GetPortInfo(
    /* [in] */ Int32 portId,
    /* [out] */ IHdmiPortInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mPortInfoMap.get(portId, NULL);

#endif
}

ECode HdmiControlService::PortIdToPath(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiPortInfo portInfo = getPortInfo(portId);
        if (portInfo == NULL) {
            Slogger::E(TAG, "Cannot find the port info: " + portId);
            return Constants::INVALID_PHYSICAL_ADDRESS;
        }
        return portInfo.getAddress();

#endif
}

ECode HdmiControlService::PathToPortId(
    /* [in] */ Int32 path,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int portAddress = path & Constants::ROUTING_PATH_TOP_MASK;
        return mPortIdMap.get(portAddress, Constants::INVALID_PORT_ID);

#endif
}

ECode HdmiControlService::IsValidPortId(
    /* [in] */ Int32 portId,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return getPortInfo(portId) != NULL;

#endif
}

ECode HdmiControlService::GetIoLooper(
    /* [out] */ ILooper** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mIoThread.getLooper();

#endif
}

ECode HdmiControlService::GetServiceLooper(
    /* [out] */ ILooper** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mHandler.getLooper();

#endif
}

ECode HdmiControlService::GetPhysicalAddress(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mCecController.getPhysicalAddress();

#endif
}

ECode HdmiControlService::GetVendorId(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mCecController.getVendorId();

#endif
}

ECode HdmiControlService::GetDeviceInfo(
    /* [in] */ Int32 logicalAddress,
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiCecLocalDeviceTv tv = tv();
        if (tv == NULL) {
            return NULL;
        }
        return tv.getCecDeviceInfo(logicalAddress);

#endif
}

ECode HdmiControlService::GetCecVersion(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mCecController.getVersion();

#endif
}

ECode HdmiControlService::IsConnectedToArcPort(
    /* [in] */ Int32 physicalAddress,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int portId = pathToPortId(physicalAddress);
        if (portId != Constants::INVALID_PORT_ID) {
            return mPortInfoMap.get(portId).isArcSupported();
        }
        return false;

#endif
}

ECode HdmiControlService::RunOnServiceThread(
    /* [in] */ IRunnable* runnable)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mHandler.post(runnable);

#endif
}

ECode HdmiControlService::RunOnServiceThreadAtFrontOfQueue(
    /* [in] */ IRunnable* runnable)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mHandler.postAtFrontOfQueue(runnable);

#endif
}

ECode HdmiControlService::AssertRunOnServiceThread()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (Looper.myLooper() != mHandler.getLooper()) {
            throw new IllegalStateException("Should run on service thread.");
        }

#endif
}

ECode HdmiControlService::SendCecCommand(
    /* [in] */ IHdmiCecMessage* command,
    /* [in] */ IHdmiControlServiceSendMessageCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (mMessageValidator.isValid(command) == HdmiCecMessageValidator.OK) {
            mCecController.sendCommand(command, callback);
        } else {
            HdmiLogger.error("Invalid message type:" + command);
            if (callback != NULL) {
                callback.onSendCompleted(Constants::SEND_RESULT_FAILURE);
            }
        }

#endif
}

ECode HdmiControlService::SendCecCommand(
    /* [in] */ IHdmiCecMessage* command)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        sendCecCommand(command, NULL);

#endif
}

ECode HdmiControlService::MaySendFeatureAbortCommand(
    /* [in] */ IHdmiCecMessage* command,
    /* [in] */ Int32 reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mCecController.maySendFeatureAbortCommand(command, reason);

#endif
}

ECode HdmiControlService::HandleCecCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int errorCode = mMessageValidator.isValid(message);
        if (errorCode != HdmiCecMessageValidator.OK) {
            // We'll not response on the messages with the invalid source or destination.
            if (errorCode == HdmiCecMessageValidator.ERROR_PARAMETER) {
                maySendFeatureAbortCommand(message, Constants::ABORT_INVALID_OPERAND);
            }
            return true;
        }
        return dispatchMessageToLocalDevice(message);

#endif
}

ECode HdmiControlService::SetAudioReturnChannel(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mCecController.setAudioReturnChannel(enabled);

#endif
}

ECode HdmiControlService::DispatchMessageToLocalDevice(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        for (HdmiCecLocalDevice device : mCecController.getLocalDeviceList()) {
            if (device.dispatchMessage(message)
                    && message.getDestination() != Constants::ADDR_BROADCAST) {
                return true;
            }
        }
        if (message.getDestination() != Constants::ADDR_BROADCAST) {
            HdmiLogger.warning("Unhandled cec command:" + message);
        }
        return false;

#endif
}

ECode HdmiControlService::OnHotplug(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        ArrayList<HdmiCecLocalDevice> localDevices = new ArrayList<>();
        for (int type : mLocalDevices) {
            if (type == HdmiDeviceInfo.DEVICE_TV) {
                // Skip the reallocation of the logical address on TV.
                continue;
            }
            HdmiCecLocalDevice localDevice = mCecController.getLocalDevice(type);
            if (localDevice == NULL) {
                localDevice = HdmiCecLocalDevice.create(this, type);
                localDevice.init();
            }
            localDevices.add(localDevice);
        }
        allocateLogicalAddress(localDevices, INITIATED_BY_HOTPLUG);
        for (HdmiCecLocalDevice device : mCecController.getLocalDeviceList()) {
            device.onHotplug(portId, connected);
        }
        announceHotplugEvent(portId, connected);

#endif
}

ECode HdmiControlService::PollDevices(
    /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
    /* [in] */ Int32 sourceAddress,
    /* [in] */ Int32 pickStrategy,
    /* [in] */ Int32 retryCount)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mCecController.pollDevices(callback, sourceAddress, checkPollStrategy(pickStrategy),
                retryCount);

#endif
}

ECode HdmiControlService::CheckPollStrategy(
    /* [in] */ Int32 pickStrategy,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int strategy = pickStrategy & Constants::POLL_STRATEGY_MASK;
        if (strategy == 0) {
            throw new IllegalArgumentException("Invalid poll strategy:" + pickStrategy);
        }
        int iterationStrategy = pickStrategy & Constants::POLL_ITERATION_STRATEGY_MASK;
        if (iterationStrategy == 0) {
            throw new IllegalArgumentException("Invalid iteration strategy:" + pickStrategy);
        }
        return strategy | iterationStrategy;

#endif
}

ECode HdmiControlService::GetAllLocalDevices(
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mCecController.getLocalDeviceList();

#endif
}

ECode HdmiControlService::GetServiceLock(
    /* [out] */ IObject** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mLock;

#endif
}

ECode HdmiControlService::SetAudioStatus(
    /* [in] */ Boolean mute,
    /* [in] */ Int32 volume)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AudioManager audioManager = getAudioManager();
        boolean muted = audioManager.isStreamMute(AudioManager.STREAM_MUSIC);
        if (mute) {
            if (!muted) {
                audioManager.setStreamMute(AudioManager.STREAM_MUSIC, true);
            }
        } else {
            if (muted) {
                audioManager.setStreamMute(AudioManager.STREAM_MUSIC, false);
            }
            // FLAG_HDMI_SYSTEM_AUDIO_VOLUME prevents audio manager from announcing
            // volume change notification back to hdmi control service.
            audioManager.setStreamVolume(AudioManager.STREAM_MUSIC, volume,
                    AudioManager.FLAG_SHOW_UI | AudioManager.FLAG_HDMI_SYSTEM_AUDIO_VOLUME);
        }

#endif
}

ECode HdmiControlService::AnnounceSystemAudioModeChange(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            for (SystemAudioModeChangeListenerRecord record :
                    mSystemAudioModeChangeListenerRecords) {
                invokeSystemAudioModeChangeLocked(record.mListener, enabled);
            }
        }

#endif
}

ECode HdmiControlService::CreateDeviceInfo(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 powerStatus,
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // TODO: find better name instead of model name.
        String displayName = Build.MODEL;
        return new HdmiDeviceInfo(logicalAddress,
                getPhysicalAddress(), pathToPortId(getPhysicalAddress()), deviceType,
                getVendorId(), displayName);

#endif
}

ECode HdmiControlService::HandleMhlHotplugEvent(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (connected) {
            HdmiMhlLocalDeviceStub newDevice = new HdmiMhlLocalDeviceStub(this, portId);
            HdmiMhlLocalDeviceStub oldDevice = mMhlController.addLocalDevice(newDevice);
            if (oldDevice != NULL) {
                oldDevice.onDeviceRemoved();
                Slogger::I(TAG, "Old device of port " + portId + " is removed");
            }
        } else {
            HdmiMhlLocalDeviceStub device = mMhlController.removeLocalDevice(portId);
            if (device != NULL) {
                device.onDeviceRemoved();
                // There is no explicit event for device removal.
                // Hence we remove the device on hotplug event.
                HdmiDeviceInfo deviceInfo = device.getInfo();
                if (deviceInfo != NULL) {
                    invokeDeviceEventListeners(deviceInfo, DEVICE_EVENT_REMOVE_DEVICE);
                    updateSafeMhlInput();
                }
            } else {
                Slogger::W(TAG, "No device to remove:[portId=" + portId);
            }
        }
        announceHotplugEvent(portId, connected);

#endif
}

ECode HdmiControlService::HandleMhlBusModeChanged(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 busmode)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiMhlLocalDeviceStub device = mMhlController.getLocalDevice(portId);
        if (device != NULL) {
            device.setBusMode(busmode);
        } else {
            Slogger::W(TAG, "No mhl device exists for bus mode change[portId:" + portId +
                    ", busmode:" + busmode + "]");
        }

#endif
}

ECode HdmiControlService::HandleMhlBusOvercurrent(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean on)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiMhlLocalDeviceStub device = mMhlController.getLocalDevice(portId);
        if (device != NULL) {
            device.onBusOvercurrentDetected(on);
        } else {
            Slogger::W(TAG, "No mhl device exists for bus overcurrent event[portId:" + portId + "]");
        }

#endif
}

ECode HdmiControlService::HandleMhlDeviceStatusChanged(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 adopterId,
    /* [in] */ Int32 deviceId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiMhlLocalDeviceStub device = mMhlController.getLocalDevice(portId);
        // Hotplug event should already have been called before device status change event.
        if (device != NULL) {
            device.setDeviceStatusChange(adopterId, deviceId);
            invokeDeviceEventListeners(device.getInfo(), DEVICE_EVENT_ADD_DEVICE);
            updateSafeMhlInput();
        } else {
            Slogger::W(TAG, "No mhl device exists for device status event[portId:"
                    + portId + ", adopterId:" + adopterId + ", deviceId:" + deviceId + "]");
        }

#endif
}

ECode HdmiControlService::UpdateSafeMhlInput()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        List<HdmiDeviceInfo> inputs = Collections.emptyList();
        SparseArray<HdmiMhlLocalDeviceStub> devices = mMhlController.getAllLocalDevices();
        for (int i = 0; i < devices.size(); ++i) {
            HdmiMhlLocalDeviceStub device = devices.valueAt(i);
            HdmiDeviceInfo info = device.getInfo();
            if (info != NULL) {
                if (inputs.isEmpty()) {
                    inputs = new ArrayList<>();
                }
                inputs.add(device.getInfo());
            }
        }
        synchronized (mLock) {
            mMhlDevices = inputs;
        }

#endif
}

ECode HdmiControlService::GetMhlDevicesLocked(
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mMhlDevices;

#endif
}

ECode HdmiControlService::EnforceAccessPermission()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        getContext().enforceCallingOrSelfPermission(PERMISSION, TAG);

#endif
}

ECode HdmiControlService::OneTouchPlay(
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiCecLocalDevicePlayback source = playback();
        if (source == NULL) {
            Slogger::W(TAG, "Local playback device not available");
            invokeCallback(callback, HdmiControlManager.RESULT_SOURCE_NOT_AVAILABLE);
            return;
        }
        source.oneTouchPlay(callback);

#endif
}

ECode HdmiControlService::QueryDisplayStatus(
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiCecLocalDevicePlayback source = playback();
        if (source == NULL) {
            Slogger::W(TAG, "Local playback device not available");
            invokeCallback(callback, HdmiControlManager.RESULT_SOURCE_NOT_AVAILABLE);
            return;
        }
        source.queryDisplayStatus(callback);

#endif
}

ECode HdmiControlService::AddHotplugEventListener(
    /* [in] */ IIHdmiHotplugEventListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HotplugEventListenerRecord record = new HotplugEventListenerRecord(listener);
        try {
            listener.asBinder().linkToDeath(record, 0);
        } catch (RemoteException e) {
            Slogger::W(TAG, "Listener already died");
            return;
        }
        synchronized (mLock) {
            mHotplugEventListenerRecords.add(record);
        }

#endif
}

ECode HdmiControlService::RemoveHotplugEventListener(
    /* [in] */ IIHdmiHotplugEventListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            for (HotplugEventListenerRecord record : mHotplugEventListenerRecords) {
                if (record.mListener.asBinder() == listener.asBinder()) {
                    listener.asBinder().unlinkToDeath(record, 0);
                    mHotplugEventListenerRecords.remove(record);
                    break;
                }
            }
        }

#endif
}

ECode HdmiControlService::AddDeviceEventListener(
    /* [in] */ IIHdmiDeviceEventListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        DeviceEventListenerRecord record = new DeviceEventListenerRecord(listener);
        try {
            listener.asBinder().linkToDeath(record, 0);
        } catch (RemoteException e) {
            Slogger::W(TAG, "Listener already died");
            return;
        }
        synchronized (mLock) {
            mDeviceEventListenerRecords.add(record);
        }

#endif
}

ECode HdmiControlService::InvokeDeviceEventListeners(
    /* [in] */ IHdmiDeviceInfo* device,
    /* [in] */ Int32 status)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            for (DeviceEventListenerRecord record : mDeviceEventListenerRecords) {
                try {
                    record.mListener.onStatusChanged(device, status);
                } catch (RemoteException e) {
                    Slogger::E(TAG, "Failed to report device event:" + e);
                }
            }
        }

#endif
}

ECode HdmiControlService::AddSystemAudioModeChangeListner(
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SystemAudioModeChangeListenerRecord record = new SystemAudioModeChangeListenerRecord(
                listener);
        try {
            listener.asBinder().linkToDeath(record, 0);
        } catch (RemoteException e) {
            Slogger::W(TAG, "Listener already died");
            return;
        }
        synchronized (mLock) {
            mSystemAudioModeChangeListenerRecords.add(record);
        }

#endif
}

ECode HdmiControlService::RemoveSystemAudioModeChangeListener(
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            for (SystemAudioModeChangeListenerRecord record :
                    mSystemAudioModeChangeListenerRecords) {
                if (record.mListener.asBinder() == listener) {
                    listener.asBinder().unlinkToDeath(record, 0);
                    mSystemAudioModeChangeListenerRecords.remove(record);
                    break;
                }
            }
        }

#endif
}

ECode HdmiControlService::SetInputChangeListener(
    /* [in] */ IIHdmiInputChangeListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            mInputChangeListenerRecord = new InputChangeListenerRecord(listener);
            try {
                listener.asBinder().linkToDeath(mInputChangeListenerRecord, 0);
            } catch (RemoteException e) {
                Slogger::W(TAG, "Listener already died");
                return;
            }
        }

#endif
}

ECode HdmiControlService::InvokeInputChangeListener(
    /* [in] */ IHdmiDeviceInfo* info)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            if (mInputChangeListenerRecord != NULL) {
                try {
                    mInputChangeListenerRecord.mListener.onChanged(info);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Exception thrown by IHdmiInputChangeListener: " + e);
                }
            }
        }

#endif
}

ECode HdmiControlService::SetHdmiRecordListener(
    /* [in] */ IIHdmiRecordListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            mRecordListenerRecord = new HdmiRecordListenerRecord(listener);
            try {
                listener.asBinder().linkToDeath(mRecordListenerRecord, 0);
            } catch (RemoteException e) {
                Slogger::W(TAG, "Listener already died.", e);
            }
        }

#endif
}

ECode HdmiControlService::InvokeRecordRequestListener(
    /* [in] */ Int32 recorderAddress,
    /* [out, callee] */ ArrayOf<Byte>* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            if (mRecordListenerRecord != NULL) {
                try {
                    return mRecordListenerRecord.mListener.getOneTouchRecordSource(recorderAddress);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Failed to start record.", e);
                }
            }
            return EmptyArray.BYTE;
        }

#endif
}

ECode HdmiControlService::InvokeOneTouchRecordResult(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            if (mRecordListenerRecord != NULL) {
                try {
                    mRecordListenerRecord.mListener.onOneTouchRecordResult(result);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Failed to call onOneTouchRecordResult.", e);
                }
            }
        }

#endif
}

ECode HdmiControlService::InvokeTimerRecordingResult(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            if (mRecordListenerRecord != NULL) {
                try {
                    mRecordListenerRecord.mListener.onTimerRecordingResult(result);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Failed to call onTimerRecordingResult.", e);
                }
            }
        }

#endif
}

ECode HdmiControlService::InvokeClearTimerRecordingResult(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            if (mRecordListenerRecord != NULL) {
                try {
                    mRecordListenerRecord.mListener.onClearTimerRecordingResult(result);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Failed to call onClearTimerRecordingResult.", e);
                }
            }
        }

#endif
}

ECode HdmiControlService::InvokeCallback(
    /* [in] */ IIHdmiControlCallback* callback,
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        try {
            callback.onComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Invoking callback failed:" + e);
        }

#endif
}

ECode HdmiControlService::InvokeSystemAudioModeChangeLocked(
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener,
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        try {
            listener.onStatusChanged(enabled);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Invoking callback failed:" + e);
        }

#endif
}

ECode HdmiControlService::AnnounceHotplugEvent(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiHotplugEvent event = new HdmiHotplugEvent(portId, connected);
        synchronized (mLock) {
            for (HotplugEventListenerRecord record : mHotplugEventListenerRecords) {
                invokeHotplugEventListenerLocked(record.mListener, event);
            }
        }

#endif
}

ECode HdmiControlService::InvokeHotplugEventListenerLocked(
    /* [in] */ IIHdmiHotplugEventListener* listener,
    /* [in] */ IHdmiHotplugEvent* event)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        try {
            listener.onReceived(event);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Failed to report hotplug event:" + event.toString(), e);
        }

#endif
}

ECode HdmiControlService::Tv(
    /* [out] */ HdmiCecLocalDeviceTv** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (HdmiCecLocalDeviceTv) mCecController.getLocalDevice(HdmiDeviceInfo.DEVICE_TV);

#endif
}

ECode HdmiControlService::IsTvDevice(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return tv() != NULL;

#endif
}

ECode HdmiControlService::Playback(
    /* [out] */ HdmiCecLocalDevicePlayback** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (HdmiCecLocalDevicePlayback)
                mCecController.getLocalDevice(HdmiDeviceInfo.DEVICE_PLAYBACK);

#endif
}

ECode HdmiControlService::GetAudioManager(
    /* [out] */ IAudioManager** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (AudioManager) getContext().getSystemService(Context.AUDIO_SERVICE);

#endif
}

ECode HdmiControlService::IsControlEnabled(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            return mHdmiControlEnabled;
        }

#endif
}

ECode HdmiControlService::GetPowerStatus(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mPowerStatus;

#endif
}

ECode HdmiControlService::IsPowerOnOrTransient(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mPowerStatus == HdmiControlManager.POWER_STATUS_ON
                || mPowerStatus == HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON;

#endif
}

ECode HdmiControlService::IsPowerStandbyOrTransient(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mPowerStatus == HdmiControlManager.POWER_STATUS_STANDBY
                || mPowerStatus == HdmiControlManager.POWER_STATUS_TRANSIENT_TO_STANDBY;

#endif
}

ECode HdmiControlService::IsPowerStandby(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mPowerStatus == HdmiControlManager.POWER_STATUS_STANDBY;

#endif
}

ECode HdmiControlService::WakeUp()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mWakeUpMessageReceived = true;
        PowerManager pm = (PowerManager) getContext().getSystemService(Context.POWER_SERVICE);
        pm.wakeUp(SystemClock.uptimeMillis());
        // PowerManger will send the broadcast Intent.ACTION_SCREEN_ON and after this gets
        // the intent, the sequence will continue at onWakeUp().

#endif
}

ECode HdmiControlService::Standby()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mStandbyMessageReceived = true;
        PowerManager pm = (PowerManager) getContext().getSystemService(Context.POWER_SERVICE);
        pm.goToSleep(SystemClock.uptimeMillis(), PowerManager.GO_TO_SLEEP_REASON_HDMI, 0);
        // PowerManger will send the broadcast Intent.ACTION_SCREEN_OFF and after this gets
        // the intent, the sequence will continue at onStandby().

#endif
}

ECode HdmiControlService::OnWakeUp()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mPowerStatus = HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON;
        if (mCecController != NULL) {
            if (mHdmiControlEnabled) {
                int startReason = INITIATED_BY_SCREEN_ON;
                if (mWakeUpMessageReceived) {
                    startReason = INITIATED_BY_WAKE_UP_MESSAGE;
                }
                initializeCec(startReason);
            }
        } else {
            Slogger::I(TAG, "Device does not support HDMI-CEC.");
        }
        // TODO: Initialize MHL local devices.

#endif
}

ECode HdmiControlService::OnStandby()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mPowerStatus = HdmiControlManager.POWER_STATUS_TRANSIENT_TO_STANDBY;
        final List<HdmiCecLocalDevice> devices = getAllLocalDevices();
        disableDevices(new PendingActionClearedCallback() {
            @Override
            public void onCleared(HdmiCecLocalDevice device) {
                Slogger::V(TAG, "On standby-action cleared:" + device.mDeviceType);
                devices.remove(device);
                if (devices.isEmpty()) {
                    onStandbyCompleted();
                    // We will not clear local devices here, since some OEM/SOC will keep passing
                    // the received packets until the application processor enters to the sleep
                    // actually.
                }
            }
        });

#endif
}

ECode HdmiControlService::OnLanguageChanged(
    /* [in] */ const String& language)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mLanguage = language;
        if (isTvDevice()) {
            tv().broadcastMenuLanguage(language);
        }

#endif
}

ECode HdmiControlService::GetLanguage(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mLanguage;

#endif
}

ECode HdmiControlService::DisableDevices(
    /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mCecController != NULL) {
            for (HdmiCecLocalDevice device : mCecController.getLocalDeviceList()) {
                device.disableDevice(mStandbyMessageReceived, callback);
            }
            if (isTvDevice()) {
                unregisterSettingsObserver();
            }
        }
        mMhlController.clearAllLocalDevices();

#endif
}

ECode HdmiControlService::ClearLocalDevices()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (mCecController == NULL) {
            return;
        }
        mCecController.clearLogicalAddress();
        mCecController.clearLocalDevices();

#endif
}

ECode HdmiControlService::OnStandbyCompleted()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        Slogger::V(TAG, "onStandbyCompleted");
        if (mPowerStatus != HdmiControlManager.POWER_STATUS_TRANSIENT_TO_STANDBY) {
            return;
        }
        mPowerStatus = HdmiControlManager.POWER_STATUS_STANDBY;
        for (HdmiCecLocalDevice device : mCecController.getLocalDeviceList()) {
            device.onStandby(mStandbyMessageReceived);
        }
        mStandbyMessageReceived = false;
        mCecController.setOption(OPTION_CEC_SERVICE_CONTROL, DISABLED);

#endif
}

ECode HdmiControlService::AddVendorCommandListener(
    /* [in] */ IIHdmiVendorCommandListener* listener,
    /* [in] */ Int32 deviceType)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        VendorCommandListenerRecord record = new VendorCommandListenerRecord(listener, deviceType);
        try {
            listener.asBinder().linkToDeath(record, 0);
        } catch (RemoteException e) {
            Slogger::W(TAG, "Listener already died");
            return;
        }
        synchronized (mLock) {
            mVendorCommandListenerRecords.add(record);
        }

#endif
}

ECode HdmiControlService::InvokeVendorCommandListeners(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 srcAddress,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Boolean hasVendorId,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            if (mVendorCommandListenerRecords.isEmpty()) {
                return false;
            }
            for (VendorCommandListenerRecord record : mVendorCommandListenerRecords) {
                if (record.mDeviceType != deviceType) {
                    continue;
                }
                try {
                    record.mListener.onReceived(srcAddress, params, hasVendorId);
                } catch (RemoteException e) {
                    Slogger::E(TAG, "Failed to notify vendor command reception", e);
                }
            }
            return true;
        }

#endif
}

ECode HdmiControlService::AddHdmiMhlVendorCommandListener(
    /* [in] */ IIHdmiMhlVendorCommandListener* listener)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiMhlVendorCommandListenerRecord record =
                new HdmiMhlVendorCommandListenerRecord(listener);
        try {
            listener.asBinder().linkToDeath(record, 0);
        } catch (RemoteException e) {
            Slogger::W(TAG, "Listener already died.");
            return;
        }
        synchronized (mLock) {
            mMhlVendorCommandListenerRecords.add(record);
        }

#endif
}

ECode HdmiControlService::InvokeMhlVendorCommandListeners(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 offest,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Byte>* data)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            for (HdmiMhlVendorCommandListenerRecord record : mMhlVendorCommandListenerRecords) {
                try {
                    record.mListener.onReceived(portId, offest, length, data);
                } catch (RemoteException e) {
                    Slogger::E(TAG, "Failed to notify MHL vendor command", e);
                }
            }
        }

#endif
}

ECode HdmiControlService::IsProhibitMode(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            return mProhibitMode;
        }

#endif
}

ECode HdmiControlService::SetProhibitMode(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            mProhibitMode = enabled;
        }

#endif
}

ECode HdmiControlService::SetCecOption(
    /* [in] */ Int32 key,
    /* [in] */ Int32 value)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mCecController.setOption(key, value);

#endif
}

ECode HdmiControlService::SetControlEnabled(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int value = toInt(enabled);
        mCecController.setOption(OPTION_CEC_ENABLE, value);
        mMhlController.setOption(OPTION_MHL_ENABLE, value);
        synchronized (mLock) {
            mHdmiControlEnabled = enabled;
        }
        if (enabled) {
            initializeCec(INITIATED_BY_ENABLE_CEC);
        } else {
            disableDevices(new PendingActionClearedCallback() {
                @Override
                public void onCleared(HdmiCecLocalDevice device) {
                    assertRunOnServiceThread();
                    clearLocalDevices();
                }
            });
        }

#endif
}

ECode HdmiControlService::SetActivePortId(
    /* [in] */ Int32 portId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mActivePortId = portId;
        // Resets last input for MHL, which stays valid only after the MHL device was selected,
        // and no further switching is done.
        setLastInputForMhl(Constants::INVALID_PORT_ID);

#endif
}

ECode HdmiControlService::SetLastInputForMhl(
    /* [in] */ Int32 portId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mLastInputMhl = portId;

#endif
}

ECode HdmiControlService::GetLastInputForMhl(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mLastInputMhl;

#endif
}

ECode HdmiControlService::ChangeInputForMhl(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean contentOn)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        final int lastInput = contentOn ? tv().getActivePortId() : Constants::INVALID_PORT_ID;
        tv().doManualPortSwitching(portId, new IHdmiControlCallback.Stub() {
            @Override
            public void onComplete(int result) throws RemoteException {
                // Keep the last input to switch back later when RAP[ContentOff] is received.
                // This effectively sets the port to invalid one if the switching is for
                // RAP[ContentOff].
                setLastInputForMhl(lastInput);
            }
        });
        // MHL device is always directly connected to the port. Update the active port ID to avoid
        // unnecessary post-routing control task.
        tv().setActivePortId(portId);
        // The port is either the MHL-enabled port where the mobile device is connected, or
        // the last port to go back to when turnoff command is received. Note that the last port
        // may not be the MHL-enabled one. In this case the device info to be passed to
        // input change listener should be the one describing the corresponding HDMI port.
        HdmiMhlLocalDeviceStub device = mMhlController.getLocalDevice(portId);
        HdmiDeviceInfo info = (device != NULL && device.getInfo() != NULL)
                ? device.getInfo()
                : mPortDeviceMap.get(portId);
        invokeInputChangeListener(info);

#endif
}

ECode HdmiControlService::SetMhlInputChangeEnabled(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
       mMhlController.setOption(OPTION_MHL_INPUT_SWITCHING, toInt(enabled));
        synchronized (mLock) {
            mMhlInputChangeEnabled = enabled;
        }

#endif
}

ECode HdmiControlService::IsMhlInputChangeEnabled(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            return mMhlInputChangeEnabled;
        }

#endif
}

ECode HdmiControlService::DisplayOsd(
    /* [in] */ Int32 messageId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        Intent intent = new Intent(HdmiControlManager.ACTION_OSD_MESSAGE);
        intent.putExtra(HdmiControlManager.EXTRA_MESSAGE_ID, messageId);
        getContext().sendBroadcastAsUser(intent, UserHandle.ALL,
                HdmiControlService.PERMISSION);

#endif
}

ECode HdmiControlService::DisplayOsd(
    /* [in] */ Int32 messageId,
    /* [in] */ Int32 extra)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        Intent intent = new Intent(HdmiControlManager.ACTION_OSD_MESSAGE);
        intent.putExtra(HdmiControlManager.EXTRA_MESSAGE_ID, messageId);
        intent.putExtra(HdmiControlManager.EXTRA_MESSAGE_EXTRAM_PARAM1, extra);
        getContext().sendBroadcastAsUser(intent, UserHandle.ALL,
                HdmiControlService.PERMISSION);

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
