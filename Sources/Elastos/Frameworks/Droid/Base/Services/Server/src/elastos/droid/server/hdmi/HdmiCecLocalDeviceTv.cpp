
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include <elastos/utility/Arrays.h>
#include <Elastos.Droid.Hardware.h>

// import static android.hardware.hdmi.HdmiControlManager.CLEAR_TIMER_STATUS_CEC_DISABLE;
// import static android.hardware.hdmi.HdmiControlManager.CLEAR_TIMER_STATUS_CHECK_RECORDER_CONNECTION;
// import static android.hardware.hdmi.HdmiControlManager.CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE;
// import static android.hardware.hdmi.HdmiControlManager.ONE_TOUCH_RECORD_CEC_DISABLED;
// import static android.hardware.hdmi.HdmiControlManager.ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION;
// import static android.hardware.hdmi.HdmiControlManager.ONE_TOUCH_RECORD_FAIL_TO_RECORD_DISPLAYED_SCREEN;
// import static android.hardware.hdmi.HdmiControlManager.OSD_MESSAGE_ARC_CONNECTED_INVALID_PORT;
// import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_RESULT_EXTRA_CEC_DISABLED;
// import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION;
// import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE;
// import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_ANALOGUE;
// import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_DIGITAL;
// import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_EXTERNAL;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiRecordSources;
using Elastos::Droid::Hardware::Hdmi::IHdmiTimerRecordSources;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Server::Hdmi::IDeviceDiscoveryActionDeviceDiscoveryCallback;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String HdmiCecLocalDeviceTv::TAG("HdmiCecLocalDeviceTv");

HdmiCecLocalDeviceTv::HdmiCecLocalDeviceTv()
    : mArcEstablished(FALSE)
    , mArcFeatureEnabled(true)
    , mSystemAudioActivated(FALSE)
    , mAutoDeviceOff(FALSE)
    , mAutoWakeup(FALSE)
    , mSkipRoutingControl(FALSE)
{}

ECode HdmiCecLocalDeviceTv::constructor(
    /* [in] */ HdmiControlService* service)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super(service, HdmiDeviceInfo.DEVICE_TV);
        mPrevPortId = Constants::INVALID_PORT_ID;
        mAutoDeviceOff = mService.readBooleanSetting(Global.HDMI_CONTROL_AUTO_DEVICE_OFF_ENABLED,
                true);
        mAutoWakeup = mService.readBooleanSetting(Global.HDMI_CONTROL_AUTO_WAKEUP_ENABLED, true);
        mStandbyHandler = new HdmiCecStandbyModeHandler(service, this);

#endif
}

ECode HdmiCecLocalDeviceTv::OnAddressAllocated(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mService.sendCecCommand(HdmiCecMessageBuilder.buildReportPhysicalAddressCommand(
                mAddress, mService.getPhysicalAddress(), mDeviceType));
        mService.sendCecCommand(HdmiCecMessageBuilder.buildDeviceVendorIdCommand(
                mAddress, mService.getVendorId()));
        mCecSwitches.add(mService.getPhysicalAddress());  // TV is a CEC switch too.
        mSkipRoutingControl = (reason == HdmiControlService.INITIATED_BY_WAKE_UP_MESSAGE);
        launchRoutingControl(reason != HdmiControlService.INITIATED_BY_ENABLE_CEC &&
                reason != HdmiControlService.INITIATED_BY_BOOT_UP);
        launchDeviceDiscovery();
        startQueuedActions();

#endif
}

ECode HdmiCecLocalDeviceTv::GetPreferredAddress(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return SystemProperties.getInt(Constants::PROPERTY_PREFERRED_ADDRESS_TV,
                Constants::ADDR_UNREGISTERED);

#endif
}

ECode HdmiCecLocalDeviceTv::SetPreferredAddress(
    /* [in] */ Int32 addr)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        SystemProperties.set(Constants::PROPERTY_PREFERRED_ADDRESS_TV, String.valueOf(addr));

#endif
}

ECode HdmiCecLocalDeviceTv::DispatchMessage(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (mService.isPowerStandby() && mStandbyHandler.handleCommand(message)) {
            return true;
        }
        return super.onMessage(message);

#endif
}

ECode HdmiCecLocalDeviceTv::DeviceSelect(
    /* [in] */ Int32 id,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiDeviceInfo targetDevice = mDeviceInfos.get(id);
        if (targetDevice == NULL) {
            invokeCallback(callback, HdmiControlManager.RESULT_TARGET_NOT_AVAILABLE);
            return;
        }
        int targetAddress = targetDevice.getLogicalAddress();
        ActiveSource active = getActiveSource();
        if (active.isValid() && targetAddress == active.logicalAddress) {
            invokeCallback(callback, HdmiControlManager.RESULT_SUCCESS);
            return;
        }
        if (targetAddress == Constants::ADDR_INTERNAL) {
            handleSelectInternalSource();
            // Switching to internal source is always successful even when CEC control is disabled.
            setActiveSource(targetAddress, mService.getPhysicalAddress());
            setActivePath(mService.getPhysicalAddress());
            invokeCallback(callback, HdmiControlManager.RESULT_SUCCESS);
            return;
        }
        if (!mService.isControlEnabled()) {
            setActiveSource(targetDevice);
            invokeCallback(callback, HdmiControlManager.RESULT_INCORRECT_MODE);
            return;
        }
        removeAction(DeviceSelectAction.class);
        addAndStartAction(new DeviceSelectAction(this, targetDevice, callback));

#endif
}

ECode HdmiCecLocalDeviceTv::HandleSelectInternalSource()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Seq #18
        if (mService.isControlEnabled() && mActiveSource.logicalAddress != mAddress) {
            updateActiveSource(mAddress, mService.getPhysicalAddress());
            if (mSkipRoutingControl) {
                mSkipRoutingControl = false;
                return;
            }
            HdmiCecMessage activeSource = HdmiCecMessageBuilder.buildActiveSource(
                    mAddress, mService.getPhysicalAddress());
            mService.sendCecCommand(activeSource);
        }

#endif
}

ECode HdmiCecLocalDeviceTv::UpdateActiveSource(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 physicalAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        updateActiveSource(ActiveSource.of(logicalAddress, physicalAddress));

#endif
}

ECode HdmiCecLocalDeviceTv::UpdateActiveSource(
    /* [in] */ HdmiCecLocalDevice::ActiveSource* newActive)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Seq #14
        if (mActiveSource.equals(newActive)) {
            return;
        }
        setActiveSource(newActive);
        int logicalAddress = newActive.logicalAddress;
        if (getCecDeviceInfo(logicalAddress) != NULL && logicalAddress != mAddress) {
            if (mService.pathToPortId(newActive.physicalAddress) == getActivePortId()) {
                setPrevPortId(getActivePortId());
            }
            // TODO: Show the OSD banner related to the new active source device.
        } else {
            // TODO: If displayed, remove the OSD banner related to the previous
            //       active source device.
        }

#endif
}

ECode HdmiCecLocalDeviceTv::GetPortId(
    /* [in] */ Int32 physicalAddress,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mService.pathToPortId(physicalAddress);

#endif
}

ECode HdmiCecLocalDeviceTv::GetPrevPortId(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            return mPrevPortId;
        }

#endif
}

ECode HdmiCecLocalDeviceTv::SetPrevPortId(
    /* [in] */ Int32 portId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            mPrevPortId = portId;
        }

#endif
}

ECode HdmiCecLocalDeviceTv::UpdateActiveInput(
    /* [in] */ Int32 path,
    /* [in] */ Boolean notifyInputChange)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Seq #15
        if (path == getActivePath()) {
            return;
        }
        setPrevPortId(getActivePortId());
        setActivePath(path);
        // TODO: Handle PAP/PIP case.
        // Show OSD port change banner
        if (notifyInputChange) {
            ActiveSource activeSource = getActiveSource();
            HdmiDeviceInfo info = getCecDeviceInfo(activeSource.logicalAddress);
            if (info == NULL) {
                info = new HdmiDeviceInfo(Constants::ADDR_INVALID, path, getActivePortId(),
                        HdmiDeviceInfo.DEVICE_RESERVED, 0, NULL);
            }
            mService.invokeInputChangeListener(info);
        }

#endif
}

ECode HdmiCecLocalDeviceTv::DoManualPortSwitching(
    /* [in] */ Int32 portId,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Seq #20
        if (!mService.isValidPortId(portId)) {
            invokeCallback(callback, HdmiControlManager.RESULT_INCORRECT_MODE);
            return;
        }
        if (portId == getActivePortId()) {
            invokeCallback(callback, HdmiControlManager.RESULT_SUCCESS);
            return;
        }
        mActiveSource.invalidate();
        if (!mService.isControlEnabled()) {
            setActivePortId(portId);
            invokeCallback(callback, HdmiControlManager.RESULT_INCORRECT_MODE);
            return;
        }
        int oldPath = getActivePortId() != Constants::INVALID_PORT_ID
                ? mService.portIdToPath(getActivePortId()) : getDeviceInfo().getPhysicalAddress();
        setActivePath(oldPath);
        if (mSkipRoutingControl) {
            mSkipRoutingControl = false;
            return;
        }
        int newPath = mService.portIdToPath(portId);
        HdmiCecMessage routingChange =
                HdmiCecMessageBuilder.buildRoutingChange(mAddress, oldPath, newPath);
        mService.sendCecCommand(routingChange);
        removeAction(RoutingControlAction.class);
        addAndStartAction(new RoutingControlAction(this, newPath, true, callback));

#endif
}

ECode HdmiCecLocalDeviceTv::GetPowerStatus(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mService.getPowerStatus();

#endif
}

ECode HdmiCecLocalDeviceTv::SendKeyEvent(
    /* [in] */ Int32 keyCode,
    /* [in] */ Boolean isPressed)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (!HdmiCecKeycode.isSupportedKeycode(keyCode)) {
            Slogger::W(TAG, "Unsupported key: " + keyCode);
            return;
        }
        List<SendKeyAction> action = getActions(SendKeyAction.class);
        if (!action.isEmpty()) {
            action.get(0).processKeyEvent(keyCode, isPressed);
        } else {
            if (isPressed && getActiveSource().isValid()) {
                int logicalAddress = getActiveSource().logicalAddress;
                addAndStartAction(new SendKeyAction(this, logicalAddress, keyCode));
            } else {
                Slogger::W(TAG, "Discard key event: " + keyCode + " pressed:" + isPressed);
            }
        }

#endif
}

ECode HdmiCecLocalDeviceTv::InvokeCallback(
    /* [in] */ IIHdmiControlCallback* callback,
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (callback == NULL) {
            return;
        }
        try {
            callback.onComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Invoking callback failed:" + e);
        }

#endif
}

ECode HdmiCecLocalDeviceTv::HandleActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int logicalAddress = message.getSource();
        int physicalAddress = HdmiUtils.twoBytesToInt(message.getParams());
        if (getCecDeviceInfo(logicalAddress) == NULL) {
            handleNewDeviceAtTheTailOfActivePath(physicalAddress);
        } else {
            ActiveSource activeSource = ActiveSource.of(logicalAddress, physicalAddress);
            ActiveSourceHandler.create(this, NULL).process(activeSource);
        }
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleInactiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Seq #10
        // Ignore <Inactive Source> from non-active source device.
        if (getActiveSource().logicalAddress != message.getSource()) {
            return true;
        }
        if (isProhibitMode()) {
            return true;
        }
        int portId = getPrevPortId();
        if (portId != Constants::INVALID_PORT_ID) {
            // TODO: Do this only if TV is not showing multiview like PIP/PAP.
            HdmiDeviceInfo inactiveSource = getCecDeviceInfo(message.getSource());
            if (inactiveSource == NULL) {
                return true;
            }
            if (mService.pathToPortId(inactiveSource.getPhysicalAddress()) == portId) {
                return true;
            }
            // TODO: Switch the TV freeze mode off
            doManualPortSwitching(portId, NULL);
            setPrevPortId(Constants::INVALID_PORT_ID);
        }
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleRequestActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Seq #19
        if (mAddress == getActiveSource().logicalAddress) {
            mService.sendCecCommand(
                    HdmiCecMessageBuilder.buildActiveSource(mAddress, getActivePath()));
        }
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleGetMenuLanguage(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (!broadcastMenuLanguage(mService.getLanguage())) {
            Slogger::W(TAG, "Failed to respond to <Get Menu Language>: " + message.toString());
        }
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::BroadcastMenuLanguage(
    /* [in] */ const String& language,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiCecMessage command = HdmiCecMessageBuilder.buildSetMenuLanguageCommand(
                mAddress, language);
        if (command != NULL) {
            mService.sendCecCommand(command);
            return true;
        }
        return false;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleReportPhysicalAddress(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int path = HdmiUtils.twoBytesToInt(message.getParams());
        int address = message.getSource();
        int type = message.getParams()[2];
        if (updateCecSwitchInfo(address, type, path)) return true;
        // Ignore if [Device Discovery Action] is going on.
        if (hasAction(DeviceDiscoveryAction.class)) {
            Slogger::I(TAG, "Ignored while Device Discovery Action is in progress: " + message);
            return true;
        }
        if (!isInDeviceList(address, path)) {
            handleNewDeviceAtTheTailOfActivePath(path);
        }
        startNewDeviceAction(ActiveSource.of(address, path));
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleReportPowerStatus(
    /* [in] */ IHdmiCecMessage* command,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int newStatus = command.getParams()[0] & 0xFF;
        updateDevicePowerStatus(command.getSource(), newStatus);
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleTimerStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Do nothing.
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleRecordStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Do nothing.
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::UpdateCecSwitchInfo(
    /* [in] */ Int32 address,
    /* [in] */ Int32 type,
    /* [in] */ Int32 path,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (address == Constants::ADDR_UNREGISTERED
                && type == HdmiDeviceInfo.DEVICE_PURE_CEC_SWITCH) {
            mCecSwitches.add(path);
            updateSafeDeviceInfoList();
            return true;  // Pure switch does not need further processing. Return here.
        }
        if (type == HdmiDeviceInfo.DEVICE_AUDIO_SYSTEM) {
            mCecSwitches.add(path);
        }
        return false;

#endif
}

ECode HdmiCecLocalDeviceTv::StartNewDeviceAction(
    /* [in] */ HdmiCecLocalDevice::ActiveSource* activeSource)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (NewDeviceAction action : getActions(NewDeviceAction.class)) {
            // If there is new device action which has the same logical address and path
            // ignore new request.
            // NewDeviceAction is created whenever it receives <Report Physical Address>.
            // And there is a chance starting NewDeviceAction for the same source.
            // Usually, new device sends <Report Physical Address> when it's plugged
            // in. However, TV can detect a new device from HotPlugDetectionAction,
            // which sends <Give Physical Address> to the source for newly detected
            // device.
            if (action.isActionOf(activeSource)) {
                return;
            }
        }
        addAndStartAction(new NewDeviceAction(this, activeSource.logicalAddress,
                activeSource.physicalAddress));

#endif
}

ECode HdmiCecLocalDeviceTv::HandleNewDeviceAtTheTailOfActivePath(
    /* [in] */ Int32 path)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Seq #22
        if (isTailOfActivePath(path, getActivePath())) {
            removeAction(RoutingControlAction.class);
            int newPath = mService.portIdToPath(getActivePortId());
            setActivePath(newPath);
            mService.sendCecCommand(HdmiCecMessageBuilder.buildRoutingChange(
                    mAddress, getActivePath(), newPath));
            addAndStartAction(new RoutingControlAction(this, newPath, false, NULL));
        }

#endif
}

ECode HdmiCecLocalDeviceTv::IsTailOfActivePath(
    /* [in] */ Int32 path,
    /* [in] */ Int32 activePath,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // If active routing path is internal source, return false.
        if (activePath == 0) {
            return false;
        }
        for (int i = 12; i >= 0; i -= 4) {
            int curActivePath = (activePath >> i) & 0xF;
            if (curActivePath == 0) {
                return true;
            } else {
                int curPath = (path >> i) & 0xF;
                if (curPath != curActivePath) {
                    return false;
                }
            }
        }
        return false;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleRoutingChange(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Seq #21
        byte[] params = message.getParams();
        int currentPath = HdmiUtils.twoBytesToInt(params);
        if (HdmiUtils.isAffectingActiveRoutingPath(getActivePath(), currentPath)) {
            mActiveSource.invalidate();
            removeAction(RoutingControlAction.class);
            int newPath = HdmiUtils.twoBytesToInt(params, 2);
            addAndStartAction(new RoutingControlAction(this, newPath, true, NULL));
        }
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleReportAudioStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        byte params[] = message.getParams();
        int mute = params[0] & 0x80;
        int volume = params[0] & 0x7F;
        setAudioStatus(mute == 0x80, volume);
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleTextViewOn(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (mService.isPowerStandbyOrTransient() && mAutoWakeup) {
            mService.wakeUp();
        }
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleImageViewOn(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Currently, it's the same as <Text View On>.
        return handleTextViewOn(message);

#endif
}

ECode HdmiCecLocalDeviceTv::HandleSetOsdName(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int source = message.getSource();
        HdmiDeviceInfo deviceInfo = getCecDeviceInfo(source);
        // If the device is not in device list, ignore it.
        if (deviceInfo == NULL) {
            Slogger::E(TAG, "No source device info for <Set Osd Name>." + message);
            return true;
        }
        String osdName = NULL;
        try {
            osdName = new String(message.getParams(), "US-ASCII");
        } catch (UnsupportedEncodingException e) {
            Slogger::E(TAG, "Invalid <Set Osd Name> request:" + message, e);
            return true;
        }
        if (deviceInfo.getDisplayName().equals(osdName)) {
            Slogger::I(TAG, "Ignore incoming <Set Osd Name> having same osd name:" + message);
            return true;
        }
        addCecDevice(new HdmiDeviceInfo(deviceInfo.getLogicalAddress(),
                deviceInfo.getPhysicalAddress(), deviceInfo.getPortId(),
                deviceInfo.getDeviceType(), deviceInfo.getVendorId(), osdName));
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::LaunchDeviceDiscovery()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        clearDeviceInfoList();
        DeviceDiscoveryAction action = new DeviceDiscoveryAction(this,
                new DeviceDiscoveryCallback() {
                    @Override
                    public void onDeviceDiscoveryDone(List<HdmiDeviceInfo> deviceInfos) {
                        for (HdmiDeviceInfo info : deviceInfos) {
                            addCecDevice(info);
                        }
                        // Since we removed all devices when it's start and
                        // device discovery action does not poll local devices,
                        // we should put device info of local device manually here
                        for (HdmiCecLocalDevice device : mService.getAllLocalDevices()) {
                            addCecDevice(device.getDeviceInfo());
                        }
                        addAndStartAction(new HotplugDetectionAction(HdmiCecLocalDeviceTv.this));
                        addAndStartAction(new PowerStatusMonitorAction(HdmiCecLocalDeviceTv.this));
                        // If there is AVR, initiate System Audio Auto initiation action,
                        // which turns on and off system audio according to last system
                        // audio setting.
                        HdmiDeviceInfo avr = getAvrDeviceInfo();
                        if (avr != NULL) {
                            onNewAvrAdded(avr);
                        }
                    }
                });
        addAndStartAction(action);

#endif
}

ECode HdmiCecLocalDeviceTv::OnNewAvrAdded(
    /* [in] */ IHdmiDeviceInfo* avr)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (getSystemAudioModeSetting()) {
            addAndStartAction(new SystemAudioAutoInitiationAction(this, avr.getLogicalAddress()));
        }
        if (isArcFeatureEnabled()) {
            startArcAction(true);
        }

#endif
}

ECode HdmiCecLocalDeviceTv::ClearDeviceInfoList()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        for (HdmiDeviceInfo info : mSafeExternalInputs) {
            invokeDeviceEventListener(info, HdmiControlManager.DEVICE_EVENT_REMOVE_DEVICE);
        }
        mDeviceInfos.clear();
        updateSafeDeviceInfoList();

#endif
}

ECode HdmiCecLocalDeviceTv::ChangeSystemAudioMode(
    /* [in] */ Boolean enabled,
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (!mService.isControlEnabled() || hasAction(DeviceDiscoveryAction.class)) {
            setSystemAudioMode(false, true);
            invokeCallback(callback, HdmiControlManager.RESULT_INCORRECT_MODE);
            return;
        }
        HdmiDeviceInfo avr = getAvrDeviceInfo();
        if (avr == NULL) {
            setSystemAudioMode(false, true);
            invokeCallback(callback, HdmiControlManager.RESULT_TARGET_NOT_AVAILABLE);
            return;
        }
        addAndStartAction(
                new SystemAudioActionFromTv(this, avr.getLogicalAddress(), enabled, callback));

#endif
}

ECode HdmiCecLocalDeviceTv::SetSystemAudioMode(
    /* [in] */ Boolean on,
    /* [in] */ Boolean updateSetting)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiLogger.debug("System Audio Mode change[old:%b new:%b]", mSystemAudioActivated, on);
        if (updateSetting) {
            mService.writeBooleanSetting(Global.HDMI_SYSTEM_AUDIO_ENABLED, on);
        }
        updateAudioManagerForSystemAudio(on);
        synchronized (mLock) {
            if (mSystemAudioActivated != on) {
                mSystemAudioActivated = on;
                mService.announceSystemAudioModeChange(on);
            }
        }

#endif
}

ECode HdmiCecLocalDeviceTv::UpdateAudioManagerForSystemAudio(
    /* [in] */ Boolean on)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int device = mService.getAudioManager().setHdmiSystemAudioSupported(on);
        HdmiLogger.debug("[A]UpdateSystemAudio mode[on=%b] output=[%X]", on, device);

#endif
}

ECode HdmiCecLocalDeviceTv::IsSystemAudioActivated(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (getAvrDeviceInfo() == NULL) {
            return false;
        }
        synchronized (mLock) {
            return mSystemAudioActivated;
        }

#endif
}

ECode HdmiCecLocalDeviceTv::GetSystemAudioModeSetting(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mService.readBooleanSetting(Global.HDMI_SYSTEM_AUDIO_ENABLED, false);

#endif
}

ECode HdmiCecLocalDeviceTv::SetArcStatus(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiLogger.debug("Set Arc Status[old:%b new:%b]", mArcEstablished, enabled);
        boolean oldStatus = mArcEstablished;
        // 1. Enable/disable ARC circuit.
        mService.setAudioReturnChannel(enabled);
        // 2. Notify arc status to audio service.
        notifyArcStatusToAudioService(enabled);
        // 3. Update arc status;
        mArcEstablished = enabled;
        return oldStatus;

#endif
}

ECode HdmiCecLocalDeviceTv::NotifyArcStatusToAudioService(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Note that we don't set any name to ARC.
        mService.getAudioManager().setWiredDeviceConnectionState(
                AudioSystem.DEVICE_OUT_HDMI_ARC,
                enabled ? 1 : 0, "");

#endif
}

ECode HdmiCecLocalDeviceTv::IsArcEstabilished(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mArcFeatureEnabled && mArcEstablished;

#endif
}

ECode HdmiCecLocalDeviceTv::ChangeArcFeatureEnabled(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (mArcFeatureEnabled != enabled) {
            mArcFeatureEnabled = enabled;
            if (enabled) {
                if (!mArcEstablished) {
                    startArcAction(true);
                }
            } else {
                if (mArcEstablished) {
                    startArcAction(false);
                }
            }
        }

#endif
}

ECode HdmiCecLocalDeviceTv::IsArcFeatureEnabled(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mArcFeatureEnabled;

#endif
}

ECode HdmiCecLocalDeviceTv::StartArcAction(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiDeviceInfo info = getAvrDeviceInfo();
        if (info == NULL) {
            Slogger::W(TAG, "Failed to start arc action; No AVR device.");
            return;
        }
        if (!canStartArcUpdateAction(info.getLogicalAddress(), enabled)) {
            Slogger::W(TAG, "Failed to start arc action; ARC configuration check failed.");
            if (enabled && !isConnectedToArcPort(info.getPhysicalAddress())) {
                displayOsd(OSD_MESSAGE_ARC_CONNECTED_INVALID_PORT);
            }
            return;
        }
        // Terminate opposite action and start action if not exist.
        if (enabled) {
            removeAction(RequestArcTerminationAction.class);
            if (!hasAction(RequestArcInitiationAction.class)) {
                addAndStartAction(new RequestArcInitiationAction(this, info.getLogicalAddress()));
            }
        } else {
            removeAction(RequestArcInitiationAction.class);
            if (!hasAction(RequestArcTerminationAction.class)) {
                addAndStartAction(new RequestArcTerminationAction(this, info.getLogicalAddress()));
            }
        }

#endif
}

ECode HdmiCecLocalDeviceTv::IsDirectConnectAddress(
    /* [in] */ Int32 physicalAddress,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (physicalAddress & Constants::ROUTING_PATH_TOP_MASK) == physicalAddress;

#endif
}

ECode HdmiCecLocalDeviceTv::SetAudioStatus(
    /* [in] */ Boolean mute,
    /* [in] */ Int32 volume)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            mSystemAudioMute = mute;
            mSystemAudioVolume = volume;
            int maxVolume = mService.getAudioManager().getStreamMaxVolume(
                    AudioManager.STREAM_MUSIC);
            mService.setAudioStatus(mute,
                    VolumeControlAction.scaleToCustomVolume(volume, maxVolume));
            displayOsd(HdmiControlManager.OSD_MESSAGE_AVR_VOLUME_CHANGED,
                    mute ? HdmiControlManager.AVR_VOLUME_MUTED : volume);
        }

#endif
}

ECode HdmiCecLocalDeviceTv::ChangeVolume(
    /* [in] */ Int32 curVolume,
    /* [in] */ Int32 delta,
    /* [in] */ Int32 maxVolume)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (delta == 0 || !isSystemAudioActivated()) {
            return;
        }
        int targetVolume = curVolume + delta;
        int cecVolume = VolumeControlAction.scaleToCecVolume(targetVolume, maxVolume);
        synchronized (mLock) {
            // If new volume is the same as current system audio volume, just ignore it.
            // Note that UNKNOWN_VOLUME is not in range of cec volume scale.
            if (cecVolume == mSystemAudioVolume) {
                // Update tv volume with system volume value.
                mService.setAudioStatus(false,
                        VolumeControlAction.scaleToCustomVolume(mSystemAudioVolume, maxVolume));
                return;
            }
        }
        List<VolumeControlAction> actions = getActions(VolumeControlAction.class);
        if (actions.isEmpty()) {
            addAndStartAction(new VolumeControlAction(this,
                    getAvrDeviceInfo().getLogicalAddress(), delta > 0));
        } else {
            actions.get(0).handleVolumeChange(delta > 0);
        }

#endif
}

ECode HdmiCecLocalDeviceTv::ChangeMute(
    /* [in] */ Boolean mute)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiLogger.debug("[A]:Change mute:%b", mute);
        synchronized (mLock) {
            if (mSystemAudioMute == mute) {
                HdmiLogger.debug("No need to change mute.");
                return;
            }
        }
        if (!isSystemAudioActivated()) {
            HdmiLogger.debug("[A]:System audio is not activated.");
            return;
        }
        // Remove existing volume action.
        removeAction(VolumeControlAction.class);
        sendUserControlPressedAndReleased(getAvrDeviceInfo().getLogicalAddress(),
                mute ? HdmiCecKeycode.CEC_KEYCODE_MUTE_FUNCTION :
                        HdmiCecKeycode.CEC_KEYCODE_RESTORE_VOLUME_FUNCTION);

#endif
}

ECode HdmiCecLocalDeviceTv::HandleInitiateArc(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (!canStartArcUpdateAction(message.getSource(), true)) {
            mService.maySendFeatureAbortCommand(message, Constants::ABORT_REFUSED);
            if (!isConnectedToArcPort(message.getSource())) {
                displayOsd(OSD_MESSAGE_ARC_CONNECTED_INVALID_PORT);
            }
            return true;
        }
        // In case where <Initiate Arc> is started by <Request ARC Initiation>
        // need to clean up RequestArcInitiationAction.
        removeAction(RequestArcInitiationAction.class);
        SetArcTransmissionStateAction action = new SetArcTransmissionStateAction(this,
                message.getSource(), true);
        addAndStartAction(action);
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::CanStartArcUpdateAction(
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean shouldCheckArcFeatureEnabled,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiDeviceInfo avr = getAvrDeviceInfo();
        if (avr != NULL
                && (avrAddress == avr.getLogicalAddress())
                && isConnectedToArcPort(avr.getPhysicalAddress())
                && isDirectConnectAddress(avr.getPhysicalAddress())) {
            if (shouldCheckArcFeatureEnabled) {
                return isArcFeatureEnabled();
            } else {
                return true;
            }
        } else {
            return false;
        }

#endif
}

ECode HdmiCecLocalDeviceTv::HandleTerminateArc(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // In cast of termination, do not check ARC configuration in that AVR device
        // might be removed already.
        // In case where <Terminate Arc> is started by <Request ARC Termination>
        // need to clean up RequestArcInitiationAction.
        removeAction(RequestArcTerminationAction.class);
        SetArcTransmissionStateAction action = new SetArcTransmissionStateAction(this,
                message.getSource(), false);
        addAndStartAction(action);
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleSetSystemAudioMode(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (!isMessageForSystemAudio(message)) {
            HdmiLogger.warning("Invalid <Set System Audio Mode> message:" + message);
            mService.maySendFeatureAbortCommand(message, Constants::ABORT_REFUSED);
            return true;
        }
        SystemAudioActionFromAvr action = new SystemAudioActionFromAvr(this,
                message.getSource(), HdmiUtils.parseCommandParamSystemAudioStatus(message), NULL);
        addAndStartAction(action);
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleSystemAudioModeStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (!isMessageForSystemAudio(message)) {
            HdmiLogger.warning("Invalid <System Audio Mode Status> message:" + message);
            // Ignore this message.
            return true;
        }
        setSystemAudioMode(HdmiUtils.parseCommandParamSystemAudioStatus(message), true);
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleRecordTvScreen(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        List<OneTouchRecordAction> actions = getActions(OneTouchRecordAction.class);
        if (!actions.isEmpty()) {
            // Assumes only one OneTouchRecordAction.
            OneTouchRecordAction action = actions.get(0);
            if (action.getRecorderAddress() != message.getSource()) {
                announceOneTouchRecordResult(
                        HdmiControlManager.ONE_TOUCH_RECORD_PREVIOUS_RECORDING_IN_PROGRESS);
            }
            return super.handleRecordTvScreen(message);
        }
        int recorderAddress = message.getSource();
        byte[] recordSource = mService.invokeRecordRequestListener(recorderAddress);
        int reason = startOneTouchRecord(recorderAddress, recordSource);
        if (reason != Constants::ABORT_NO_ERROR) {
            mService.maySendFeatureAbortCommand(message, reason);
        }
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::HandleTimerClearedStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] params = message.getParams();
        int timerClearedStatusData = params[0] & 0xFF;
        announceTimerRecordingResult(timerClearedStatusData);
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::AnnounceOneTouchRecordResult(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mService.invokeOneTouchRecordResult(result);

#endif
}

ECode HdmiCecLocalDeviceTv::AnnounceTimerRecordingResult(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mService.invokeTimerRecordingResult(result);

#endif
}

ECode HdmiCecLocalDeviceTv::AnnounceClearTimerRecordingResult(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mService.invokeClearTimerRecordingResult(result);

#endif
}

ECode HdmiCecLocalDeviceTv::IsMessageForSystemAudio(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mService.isControlEnabled()
                && message.getSource() == Constants::ADDR_AUDIO_SYSTEM
                && (message.getDestination() == Constants::ADDR_TV
                        || message.getDestination() == Constants::ADDR_BROADCAST)
                && getAvrDeviceInfo() != NULL;

#endif
}

ECode HdmiCecLocalDeviceTv::AddDeviceInfo(
    /* [in] */ IHdmiDeviceInfo* deviceInfo,
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiDeviceInfo oldDeviceInfo = getCecDeviceInfo(deviceInfo.getLogicalAddress());
        if (oldDeviceInfo != NULL) {
            removeDeviceInfo(deviceInfo.getId());
        }
        mDeviceInfos.append(deviceInfo.getId(), deviceInfo);
        updateSafeDeviceInfoList();
        return oldDeviceInfo;

#endif
}

ECode HdmiCecLocalDeviceTv::RemoveDeviceInfo(
    /* [in] */ Int32 id,
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiDeviceInfo deviceInfo = mDeviceInfos.get(id);
        if (deviceInfo != NULL) {
            mDeviceInfos.remove(id);
        }
        updateSafeDeviceInfoList();
        return deviceInfo;

#endif
}

ECode HdmiCecLocalDeviceTv::GetDeviceInfoList(
    /* [in] */ Boolean includeLocalDevice,
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (includeLocalDevice) {
            return HdmiUtils.sparseArrayToList(mDeviceInfos);
        } else {
            ArrayList<HdmiDeviceInfo> infoList = new ArrayList<>();
            for (int i = 0; i < mDeviceInfos.size(); ++i) {
                HdmiDeviceInfo info = mDeviceInfos.valueAt(i);
                if (!isLocalDeviceAddress(info.getLogicalAddress())) {
                    infoList.add(info);
                }
            }
            return infoList;
        }

#endif
}

ECode HdmiCecLocalDeviceTv::GetSafeExternalInputsLocked(
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mSafeExternalInputs;

#endif
}

ECode HdmiCecLocalDeviceTv::UpdateSafeDeviceInfoList()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        List<HdmiDeviceInfo> copiedDevices = HdmiUtils.sparseArrayToList(mDeviceInfos);
        List<HdmiDeviceInfo> externalInputs = getInputDevices();
        synchronized (mLock) {
            mSafeAllDeviceInfos = copiedDevices;
            mSafeExternalInputs = externalInputs;
        }

#endif
}

ECode HdmiCecLocalDeviceTv::GetInputDevices(
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ArrayList<HdmiDeviceInfo> infoList = new ArrayList<>();
        for (int i = 0; i < mDeviceInfos.size(); ++i) {
            HdmiDeviceInfo info = mDeviceInfos.valueAt(i);
            if (isLocalDeviceAddress(info.getLogicalAddress())) {
                continue;
            }
            if (info.isSourceType() && !hideDevicesBehindLegacySwitch(info)) {
                infoList.add(info);
            }
        }
        return infoList;

#endif
}

ECode HdmiCecLocalDeviceTv::HideDevicesBehindLegacySwitch(
    /* [in] */ IHdmiDeviceInfo* info,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return HdmiConfig.HIDE_DEVICES_BEHIND_LEGACY_SWITCH
                && !isConnectedToCecSwitch(info.getPhysicalAddress(), mCecSwitches);

#endif
}

ECode HdmiCecLocalDeviceTv::IsConnectedToCecSwitch(
    /* [in] */ Int32 path,
    /* [in] */ ICollection* switches,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (int switchPath : switches) {
            if (isParentPath(switchPath, path)) {
                return true;
            }
        }
        return false;

#endif
}

ECode HdmiCecLocalDeviceTv::IsParentPath(
    /* [in] */ Int32 parentPath,
    /* [in] */ Int32 childPath,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // (A000, AB00) (AB00, ABC0), (ABC0, ABCD)
        // If child's last non-zero nibble is removed, the result equals to the parent.
        for (int i = 0; i <= 12; i += 4) {
            int nibble = (childPath >> i) & 0xF;
            if (nibble != 0) {
                int parentNibble = (parentPath >> i) & 0xF;
                return parentNibble == 0 && (childPath >> i+4) == (parentPath >> i+4);
            }
        }
        return false;

#endif
}

ECode HdmiCecLocalDeviceTv::InvokeDeviceEventListener(
    /* [in] */ IHdmiDeviceInfo* info,
    /* [in] */ Int32 status)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (info.isSourceType() && !hideDevicesBehindLegacySwitch(info)) {
            mService.invokeDeviceEventListeners(info, status);
        }

#endif
}

ECode HdmiCecLocalDeviceTv::IsLocalDeviceAddress(
    /* [in] */ Int32 address,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        for (HdmiCecLocalDevice device : mService.getAllLocalDevices()) {
            if (device.isAddressOf(address)) {
                return true;
            }
        }
        return false;

#endif
}

ECode HdmiCecLocalDeviceTv::GetAvrDeviceInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return getCecDeviceInfo(Constants::ADDR_AUDIO_SYSTEM);

#endif
}

ECode HdmiCecLocalDeviceTv::GetCecDeviceInfo(
    /* [in] */ Int32 logicalAddress,
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mDeviceInfos.get(HdmiDeviceInfo.idForCecDevice(logicalAddress));

#endif
}

ECode HdmiCecLocalDeviceTv::HasSystemAudioDevice(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return getSafeAvrDeviceInfo() != NULL;

#endif
}

ECode HdmiCecLocalDeviceTv::GetSafeAvrDeviceInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return getSafeCecDeviceInfo(Constants::ADDR_AUDIO_SYSTEM);

#endif
}

ECode HdmiCecLocalDeviceTv::GetSafeCecDeviceInfo(
    /* [in] */ Int32 logicalAddress,
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            for (HdmiDeviceInfo info : mSafeAllDeviceInfos) {
                if (info.isCecDevice() && info.getLogicalAddress() == logicalAddress) {
                    return info;
                }
            }
            return NULL;
        }

#endif
}

ECode HdmiCecLocalDeviceTv::AddCecDevice(
    /* [in] */ IHdmiDeviceInfo* info)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        addDeviceInfo(info);
        if (info.getLogicalAddress() == mAddress) {
            // The addition of TV device itself should not be notified.
            return;
        }
        invokeDeviceEventListener(info, HdmiControlManager.DEVICE_EVENT_ADD_DEVICE);

#endif
}

ECode HdmiCecLocalDeviceTv::RemoveCecDevice(
    /* [in] */ Int32 address)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiDeviceInfo info = removeDeviceInfo(HdmiDeviceInfo.idForCecDevice(address));
        mCecMessageCache.flushMessagesFrom(address);
        invokeDeviceEventListener(info, HdmiControlManager.DEVICE_EVENT_REMOVE_DEVICE);

#endif
}

ECode HdmiCecLocalDeviceTv::HandleRemoveActiveRoutingPath(
    /* [in] */ Int32 path)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Seq #23
        if (isTailOfActivePath(path, getActivePath())) {
            removeAction(RoutingControlAction.class);
            int newPath = mService.portIdToPath(getActivePortId());
            mService.sendCecCommand(HdmiCecMessageBuilder.buildRoutingChange(
                    mAddress, getActivePath(), newPath));
            mActiveSource.invalidate();
            addAndStartAction(new RoutingControlAction(this, getActivePath(), true, NULL));
        }

#endif
}

ECode HdmiCecLocalDeviceTv::LaunchRoutingControl(
    /* [in] */ Boolean routingForBootup)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Seq #24
        if (getActivePortId() != Constants::INVALID_PORT_ID) {
            if (!routingForBootup && !isProhibitMode()) {
                removeAction(RoutingControlAction.class);
                int newPath = mService.portIdToPath(getActivePortId());
                setActivePath(newPath);
                mService.sendCecCommand(HdmiCecMessageBuilder.buildRoutingChange(mAddress,
                        getActivePath(), newPath));
                addAndStartAction(new RoutingControlAction(this, getActivePortId(),
                        routingForBootup, NULL));
            }
        } else {
            int activePath = mService.getPhysicalAddress();
            setActivePath(activePath);
            if (!routingForBootup) {
                mService.sendCecCommand(HdmiCecMessageBuilder.buildActiveSource(mAddress,
                        activePath));
            }
        }

#endif
}

ECode HdmiCecLocalDeviceTv::GetDeviceInfoByPath(
    /* [in] */ Int32 path,
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        for (HdmiDeviceInfo info : getDeviceInfoList(false)) {
            if (info.getPhysicalAddress() == path) {
                return info;
            }
        }
        return NULL;

#endif
}

ECode HdmiCecLocalDeviceTv::IsInDeviceList(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 physicalAddress,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiDeviceInfo device = getCecDeviceInfo(logicalAddress);
        if (device == NULL) {
            return false;
        }
        return device.getPhysicalAddress() == physicalAddress;

#endif
}

ECode HdmiCecLocalDeviceTv::OnHotplug(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (!connected) {
            removeCecSwitches(portId);
        }
        // Tv device will have permanent HotplugDetectionAction.
        List<HotplugDetectionAction> hotplugActions = getActions(HotplugDetectionAction.class);
        if (!hotplugActions.isEmpty()) {
            // Note that hotplug action is single action running on a machine.
            // "pollAllDevicesNow" cleans up timer and start poll action immediately.
            // It covers seq #40, #43.
            hotplugActions.get(0).pollAllDevicesNow();
        }

#endif
}

ECode HdmiCecLocalDeviceTv::RemoveCecSwitches(
    /* [in] */ Int32 portId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Iterator<Integer> it = mCecSwitches.iterator();
        while (!it.hasNext()) {
            int path = it.next();
            if (pathToPortId(path) == portId) {
                it.remove();
            }
        }

#endif
}

ECode HdmiCecLocalDeviceTv::SetAutoDeviceOff(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mAutoDeviceOff = enabled;

#endif
}

ECode HdmiCecLocalDeviceTv::SetAutoWakeup(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mAutoWakeup = enabled;

#endif
}

ECode HdmiCecLocalDeviceTv::GetAutoWakeup(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mAutoWakeup;

#endif
}

ECode HdmiCecLocalDeviceTv::DisableDevice(
    /* [in] */ Boolean initiatedByCec,
    /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super.disableDevice(initiatedByCec, callback);
        assertRunOnServiceThread();
        // Remove any repeated working actions.
        // HotplugDetectionAction will be reinstated during the wake up process.
        // HdmiControlService.onWakeUp() -> initializeLocalDevices() ->
        //     LocalDeviceTv.onAddressAllocated() -> launchDeviceDiscovery().
        removeAction(DeviceDiscoveryAction.class);
        removeAction(HotplugDetectionAction.class);
        removeAction(PowerStatusMonitorAction.class);
        // Remove recording actions.
        removeAction(OneTouchRecordAction.class);
        removeAction(TimerRecordingAction.class);
        disableSystemAudioIfExist();
        disableArcIfExist();
        clearDeviceInfoList();
        checkIfPendingActionsCleared();

#endif
}

ECode HdmiCecLocalDeviceTv::DisableSystemAudioIfExist()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (getAvrDeviceInfo() == NULL) {
            return;
        }
        // Seq #31.
        removeAction(SystemAudioActionFromAvr.class);
        removeAction(SystemAudioActionFromTv.class);
        removeAction(SystemAudioAutoInitiationAction.class);
        removeAction(SystemAudioStatusAction.class);
        removeAction(VolumeControlAction.class);
        // Turn off the mode but do not write it the settings, so that the next time TV powers on
        // the system audio mode setting can be restored automatically.
        setSystemAudioMode(false, false);

#endif
}

ECode HdmiCecLocalDeviceTv::DisableArcIfExist()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiDeviceInfo avr = getAvrDeviceInfo();
        if (avr == NULL) {
            return;
        }
        // Seq #44.
        removeAction(RequestArcInitiationAction.class);
        if (!hasAction(RequestArcTerminationAction.class) && isArcEstabilished()) {
            addAndStartAction(new RequestArcTerminationAction(this, avr.getLogicalAddress()));
        }

#endif
}

ECode HdmiCecLocalDeviceTv::OnStandby(
    /* [in] */ Boolean initiatedByCec)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Seq #11
        if (!mService.isControlEnabled()) {
            return;
        }
        if (!initiatedByCec && mAutoDeviceOff) {
            mService.sendCecCommand(HdmiCecMessageBuilder.buildStandby(
                    mAddress, Constants::ADDR_BROADCAST));
        }

#endif
}

ECode HdmiCecLocalDeviceTv::IsProhibitMode(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mService.isProhibitMode();

#endif
}

ECode HdmiCecLocalDeviceTv::IsPowerStandbyOrTransient(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mService.isPowerStandbyOrTransient();

#endif
}

ECode HdmiCecLocalDeviceTv::DisplayOsd(
    /* [in] */ Int32 messageId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mService.displayOsd(messageId);

#endif
}

ECode HdmiCecLocalDeviceTv::DisplayOsd(
    /* [in] */ Int32 messageId,
    /* [in] */ Int32 extra)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mService.displayOsd(messageId, extra);

#endif
}

ECode HdmiCecLocalDeviceTv::StartOneTouchRecord(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ ArrayOf<Byte>* recordSource,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (!mService.isControlEnabled()) {
            Slogger::W(TAG, "Can not start one touch record. CEC control is disabled.");
            announceOneTouchRecordResult(ONE_TOUCH_RECORD_CEC_DISABLED);
            return Constants::ABORT_NOT_IN_CORRECT_MODE;
        }
        if (!checkRecorder(recorderAddress)) {
            Slogger::W(TAG, "Invalid recorder address:" + recorderAddress);
            announceOneTouchRecordResult(ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
            return Constants::ABORT_NOT_IN_CORRECT_MODE;
        }
        if (!checkRecordSource(recordSource)) {
            Slogger::W(TAG, "Invalid record source." + Arrays.toString(recordSource));
            announceOneTouchRecordResult(ONE_TOUCH_RECORD_FAIL_TO_RECORD_DISPLAYED_SCREEN);
            return Constants::ABORT_UNABLE_TO_DETERMINE;
        }
        addAndStartAction(new OneTouchRecordAction(this, recorderAddress, recordSource));
        Slogger::I(TAG, "Start new [One Touch Record]-Target:" + recorderAddress + ", recordSource:"
                + Arrays.toString(recordSource));
        return Constants::ABORT_NO_ERROR;

#endif
}

ECode HdmiCecLocalDeviceTv::StopOneTouchRecord(
    /* [in] */ Int32 recorderAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (!mService.isControlEnabled()) {
            Slogger::W(TAG, "Can not stop one touch record. CEC control is disabled.");
            announceOneTouchRecordResult(ONE_TOUCH_RECORD_CEC_DISABLED);
            return;
        }
        if (!checkRecorder(recorderAddress)) {
            Slogger::W(TAG, "Invalid recorder address:" + recorderAddress);
            announceOneTouchRecordResult(ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
            return;
        }
        // Remove one touch record action so that other one touch record can be started.
        removeAction(OneTouchRecordAction.class);
        mService.sendCecCommand(HdmiCecMessageBuilder.buildRecordOff(mAddress, recorderAddress));
        Slogger::I(TAG, "Stop [One Touch Record]-Target:" + recorderAddress);

#endif
}

ECode HdmiCecLocalDeviceTv::CheckRecorder(
    /* [in] */ Int32 recorderAddress,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiDeviceInfo device = getCecDeviceInfo(recorderAddress);
        return (device != NULL)
                && (HdmiUtils.getTypeFromAddress(recorderAddress)
                        == HdmiDeviceInfo.DEVICE_RECORDER);

#endif
}

ECode HdmiCecLocalDeviceTv::CheckRecordSource(
    /* [in] */ ArrayOf<Byte>* recordSource,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (recordSource != NULL) && HdmiRecordSources.checkRecordSource(recordSource);

#endif
}

ECode HdmiCecLocalDeviceTv::StartTimerRecording(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (!mService.isControlEnabled()) {
            Slogger::W(TAG, "Can not start one touch record. CEC control is disabled.");
            announceTimerRecordingResult(TIMER_RECORDING_RESULT_EXTRA_CEC_DISABLED);
            return;
        }
        if (!checkRecorder(recorderAddress)) {
            Slogger::W(TAG, "Invalid recorder address:" + recorderAddress);
            announceTimerRecordingResult(
                    TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
            return;
        }
        if (!checkTimerRecordingSource(sourceType, recordSource)) {
            Slogger::W(TAG, "Invalid record source." + Arrays.toString(recordSource));
            announceTimerRecordingResult(
                    TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE);
            return;
        }
        addAndStartAction(
                new TimerRecordingAction(this, recorderAddress, sourceType, recordSource));
        Slogger::I(TAG, "Start [Timer Recording]-Target:" + recorderAddress + ", SourceType:"
                + sourceType + ", RecordSource:" + Arrays.toString(recordSource));

#endif
}

ECode HdmiCecLocalDeviceTv::CheckTimerRecordingSource(
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (recordSource != NULL)
                && HdmiTimerRecordSources.checkTimerRecordSource(sourceType, recordSource);

#endif
}

ECode HdmiCecLocalDeviceTv::ClearTimerRecording(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (!mService.isControlEnabled()) {
            Slogger::W(TAG, "Can not start one touch record. CEC control is disabled.");
            announceClearTimerRecordingResult(CLEAR_TIMER_STATUS_CEC_DISABLE);
            return;
        }
        if (!checkRecorder(recorderAddress)) {
            Slogger::W(TAG, "Invalid recorder address:" + recorderAddress);
            announceClearTimerRecordingResult(CLEAR_TIMER_STATUS_CHECK_RECORDER_CONNECTION);
            return;
        }
        if (!checkTimerRecordingSource(sourceType, recordSource)) {
            Slogger::W(TAG, "Invalid record source." + Arrays.toString(recordSource));
            announceClearTimerRecordingResult(CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE);
            return;
        }
        sendClearTimerMessage(recorderAddress, sourceType, recordSource);

#endif
}

ECode HdmiCecLocalDeviceTv::SendClearTimerMessage(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiCecMessage message = NULL;
        switch (sourceType) {
            case TIMER_RECORDING_TYPE_DIGITAL:
                message = HdmiCecMessageBuilder.buildClearDigitalTimer(mAddress, recorderAddress,
                        recordSource);
                break;
            case TIMER_RECORDING_TYPE_ANALOGUE:
                message = HdmiCecMessageBuilder.buildClearAnalogueTimer(mAddress, recorderAddress,
                        recordSource);
                break;
            case TIMER_RECORDING_TYPE_EXTERNAL:
                message = HdmiCecMessageBuilder.buildClearExternalTimer(mAddress, recorderAddress,
                        recordSource);
                break;
            default:
                Slogger::W(TAG, "Invalid source type:" + recorderAddress);
                announceClearTimerRecordingResult(CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE);
                return;
        }
        mService.sendCecCommand(message, new SendMessageCallback() {
            @Override
            public void onSendCompleted(int error) {
                if (error != Constants::SEND_RESULT_SUCCESS) {
                    announceClearTimerRecordingResult(
                            CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE);
                }
            }
        });

#endif
}

ECode HdmiCecLocalDeviceTv::UpdateDevicePowerStatus(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 newPowerStatus)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiDeviceInfo info = getCecDeviceInfo(logicalAddress);
        if (info == NULL) {
            Slogger::W(TAG, "Can not update power status of non-existing device:" + logicalAddress);
            return;
        }
        if (info.getDevicePowerStatus() == newPowerStatus) {
            return;
        }
        HdmiDeviceInfo newInfo = HdmiUtils.cloneHdmiDeviceInfo(info, newPowerStatus);
        // addDeviceInfo replaces old device info with new one if exists.
        addDeviceInfo(newInfo);
        invokeDeviceEventListener(newInfo, HdmiControlManager.DEVICE_EVENT_UPDATE_DEVICE);

#endif
}

ECode HdmiCecLocalDeviceTv::HandleMenuStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Do nothing and just return true not to prevent from responding <Feature Abort>.
        return true;

#endif
}

ECode HdmiCecLocalDeviceTv::SendStandby(
    /* [in] */ Int32 deviceId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiDeviceInfo targetDevice = mDeviceInfos.get(deviceId);
        if (targetDevice == NULL) {
            return;
        }
        int targetAddress = targetDevice.getLogicalAddress();
        mService.sendCecCommand(HdmiCecMessageBuilder.buildStandby(mAddress, targetAddress));

#endif
}

ECode HdmiCecLocalDeviceTv::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super.dump(pw);
        pw.println("mArcEstablished: " + mArcEstablished);
        pw.println("mArcFeatureEnabled: " + mArcFeatureEnabled);
        pw.println("mSystemAudioActivated: " + mSystemAudioActivated);
        pw.println("mSystemAudioMute: " + mSystemAudioMute);
        pw.println("mAutoDeviceOff: " + mAutoDeviceOff);
        pw.println("mAutoWakeup: " + mAutoWakeup);
        pw.println("mSkipRoutingControl: " + mSkipRoutingControl);
        pw.println("CEC devices:");
        pw.increaseIndent();
        for (HdmiDeviceInfo info : mSafeAllDeviceInfos) {
            pw.println(info);
        }
        pw.decreaseIndent();

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
