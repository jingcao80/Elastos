/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.server.hdmi;

import static android.hardware.hdmi.HdmiControlManager.CLEAR_TIMER_STATUS_CEC_DISABLE;
import static android.hardware.hdmi.HdmiControlManager.CLEAR_TIMER_STATUS_CHECK_RECORDER_CONNECTION;
import static android.hardware.hdmi.HdmiControlManager.CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE;
import static android.hardware.hdmi.HdmiControlManager.ONE_TOUCH_RECORD_CEC_DISABLED;
import static android.hardware.hdmi.HdmiControlManager.ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION;
import static android.hardware.hdmi.HdmiControlManager.ONE_TOUCH_RECORD_FAIL_TO_RECORD_DISPLAYED_SCREEN;
import static android.hardware.hdmi.HdmiControlManager.OSD_MESSAGE_ARC_CONNECTED_INVALID_PORT;
import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_RESULT_EXTRA_CEC_DISABLED;
import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION;
import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE;
import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_ANALOGUE;
import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_DIGITAL;
import static android.hardware.hdmi.HdmiControlManager.TIMER_RECORDING_TYPE_EXTERNAL;

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiRecordSources;
using Elastos::Droid::Hardware::Hdmi::IHdmiTimerRecordSources;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::ISettings::Global::
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Droid::internal.annotations.GuardedBy;
using Elastos::Droid::internal.util.IndentingPrintWriter;
using Elastos::Droid::Server::Ihdmi.DeviceDiscoveryAction.DeviceDiscoveryCallback;
using Elastos::Droid::Server::Ihdmi.HdmiAnnotations.ServiceThreadOnly;
using Elastos::Droid::Server::Ihdmi.HdmiControlService.SendMessageCallback;

using Elastos::IO::IUnsupportedEncodingException;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

/**
 * Represent a logical device of type TV residing in Android system.
 */
final class HdmiCecLocalDeviceTv extends HdmiCecLocalDevice {
    private static const String TAG = "HdmiCecLocalDeviceTv";

    // Whether ARC is available or not. "TRUE" means that ARC is established between TV and
    // AVR as audio receiver.
    @ServiceThreadOnly
    private Boolean mArcEstablished = FALSE;

    // Whether ARC feature is enabled or not. The default value is TRUE.
    // TODO: once adding system setting for it, read the value to it.
    private Boolean mArcFeatureEnabled = TRUE;

    // Whether System audio mode is activated or not.
    // This becomes TRUE only when all system audio sequences are finished.
    @GuardedBy("mLock")
    private Boolean mSystemAudioActivated = FALSE;

    // The previous port id (input) before switching to the new one. This is remembered in order to
    // be able to switch to it upon receiving <Inactive Source> from currently active source.
    // This remains valid only when the active source was switched via one touch play operation
    // (either by TV or source device). Manual port switching invalidates this value to
    // Constants.PORT_INVALID, for which case <Inactive Source> does not do anything.
    @GuardedBy("mLock")
    private Int32 mPrevPortId;

    @GuardedBy("mLock")
    private Int32 mSystemAudioVolume = Constants.UNKNOWN_VOLUME;

    @GuardedBy("mLock")
    private Boolean mSystemAudioMute = FALSE;

    // Copy of mDeviceInfos to guarantee thread-safety.
    @GuardedBy("mLock")
    private List<HdmiDeviceInfo> mSafeAllDeviceInfos = Collections->EmptyList();
    // All external cec Input(source) devices. Does not include system audio device.
    @GuardedBy("mLock")
    private List<HdmiDeviceInfo> mSafeExternalInputs = Collections->EmptyList();

    // Map-like container of all cec devices including local ones.
    // device id is used as key of container.
    // This is not thread-safe. For external purpose use mSafeDeviceInfos.
    private final SparseArray<HdmiDeviceInfo> mDeviceInfos = new SparseArray<>();

    // If TRUE, TV going to standby mode puts other devices also to standby.
    private Boolean mAutoDeviceOff;

    // If TRUE, TV wakes itself up when receiving <Text/Image View On>.
    private Boolean mAutoWakeup;

    private final HdmiCecStandbyModeHandler mStandbyHandler;

    // If TRUE, do not do routing control/send active source for internal source.
    // Set to TRUE when the device was woken up by <Text/Image View On>.
    private Boolean mSkipRoutingControl;

    // Set of physical addresses of CEC switches on the CEC bus. Managed independently from
    // other CEC devices since they might not have logical address.
    private final ArraySet<Integer> mCecSwitches = new ArraySet<Integer>();

    HdmiCecLocalDeviceTv(HdmiControlService service) {
        Super(service, HdmiDeviceInfo.DEVICE_TV);
        mPrevPortId = Constants.INVALID_PORT_ID;
        mAutoDeviceOff = mService->ReadBooleanSetting(Global.HDMI_CONTROL_AUTO_DEVICE_OFF_ENABLED,
                TRUE);
        mAutoWakeup = mService->ReadBooleanSetting(Global.HDMI_CONTROL_AUTO_WAKEUP_ENABLED, TRUE);
        mStandbyHandler = new HdmiCecStandbyModeHandler(service, this);
    }

    //@Override
    @ServiceThreadOnly
    protected void OnAddressAllocated(Int32 logicalAddress, Int32 reason) {
        AssertRunOnServiceThread();
        mService->SendCecCommand(HdmiCecMessageBuilder->BuildReportPhysicalAddressCommand(
                mAddress, mService->GetPhysicalAddress(), mDeviceType));
        mService->SendCecCommand(HdmiCecMessageBuilder->BuildDeviceVendorIdCommand(
                mAddress, mService->GetVendorId()));
        mCecSwitches->Add(mService->GetPhysicalAddress());  // TV is a CEC switch too.
        mSkipRoutingControl = (reason == HdmiControlService.INITIATED_BY_WAKE_UP_MESSAGE);
        LaunchRoutingControl(reason != HdmiControlService.INITIATED_BY_ENABLE_CEC &&
                reason != HdmiControlService.INITIATED_BY_BOOT_UP);
        LaunchDeviceDiscovery();
        StartQueuedActions();
    }

    //@Override
    @ServiceThreadOnly
    protected Int32 GetPreferredAddress() {
        AssertRunOnServiceThread();
        return SystemProperties->GetInt(Constants.PROPERTY_PREFERRED_ADDRESS_TV,
                Constants.ADDR_UNREGISTERED);
    }

    //@Override
    @ServiceThreadOnly
    protected void SetPreferredAddress(Int32 addr) {
        AssertRunOnServiceThread();
        SystemProperties->Set(Constants.PROPERTY_PREFERRED_ADDRESS_TV, String->ValueOf(addr));
    }

    //@Override
    @ServiceThreadOnly
    Boolean DispatchMessage(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        if (mService->IsPowerStandby() && mStandbyHandler->HandleCommand(message)) {
            return TRUE;
        }
        return super->OnMessage(message);
    }

    /**
     * Performs the action 'device select', or 'one touch play' initiated by TV.
     *
     * @param id id of HDMI device to select
     * @param callback callback object to report the result with
     */
    @ServiceThreadOnly
    void DeviceSelect(Int32 id, IHdmiControlCallback callback) {
        AssertRunOnServiceThread();
        HdmiDeviceInfo targetDevice = mDeviceInfos->Get(id);
        if (targetDevice == NULL) {
            InvokeCallback(callback, HdmiControlManager.RESULT_TARGET_NOT_AVAILABLE);
            return;
        }
        Int32 targetAddress = targetDevice->GetLogicalAddress();
        ActiveSource active = GetActiveSource();
        if (active->IsValid() && targetAddress == active.logicalAddress) {
            InvokeCallback(callback, HdmiControlManager.RESULT_SUCCESS);
            return;
        }
        if (targetAddress == Constants.ADDR_INTERNAL) {
            HandleSelectInternalSource();
            // Switching to internal source is always successful even when CEC control is disabled.
            SetActiveSource(targetAddress, mService->GetPhysicalAddress());
            SetActivePath(mService->GetPhysicalAddress());
            InvokeCallback(callback, HdmiControlManager.RESULT_SUCCESS);
            return;
        }
        if (!mService->IsControlEnabled()) {
            SetActiveSource(targetDevice);
            InvokeCallback(callback, HdmiControlManager.RESULT_INCORRECT_MODE);
            return;
        }
        RemoveAction(DeviceSelectAction.class);
        AddAndStartAction(new DeviceSelectAction(this, targetDevice, callback));
    }

    @ServiceThreadOnly
    private void HandleSelectInternalSource() {
        AssertRunOnServiceThread();
        // Seq #18
        if (mService->IsControlEnabled() && mActiveSource.logicalAddress != mAddress) {
            UpdateActiveSource(mAddress, mService->GetPhysicalAddress());
            if (mSkipRoutingControl) {
                mSkipRoutingControl = FALSE;
                return;
            }
            HdmiCecMessage activeSource = HdmiCecMessageBuilder->BuildActiveSource(
                    mAddress, mService->GetPhysicalAddress());
            mService->SendCecCommand(activeSource);
        }
    }

    @ServiceThreadOnly
    void UpdateActiveSource(Int32 logicalAddress, Int32 physicalAddress) {
        AssertRunOnServiceThread();
        UpdateActiveSource(ActiveSource->Of(logicalAddress, physicalAddress));
    }

    @ServiceThreadOnly
    void UpdateActiveSource(ActiveSource newActive) {
        AssertRunOnServiceThread();
        // Seq #14
        if (mActiveSource->Equals(newActive)) {
            return;
        }
        SetActiveSource(newActive);
        Int32 logicalAddress = newActive.logicalAddress;
        if (GetCecDeviceInfo(logicalAddress) != NULL && logicalAddress != mAddress) {
            if (mService->PathToPortId(newActive.physicalAddress) == GetActivePortId()) {
                SetPrevPortId(GetActivePortId());
            }
            // TODO: Show the OSD banner related to the new active source device.
        } else {
            // TODO: If displayed, remove the OSD banner related to the previous
            //       active source device.
        }
    }

    Int32 GetPortId(Int32 physicalAddress) {
        return mService->PathToPortId(physicalAddress);
    }

    /**
     * Returns the previous port id kept to handle input switching on <Inactive Source>.
     */
    Int32 GetPrevPortId() {
        synchronized(mLock) {
            return mPrevPortId;
        }
    }

    /**
     * Sets the previous port id. INVALID_PORT_ID invalidates it, hence no actions will be
     * taken for <Inactive Source>.
     */
    void SetPrevPortId(Int32 portId) {
        synchronized(mLock) {
            mPrevPortId = portId;
        }
    }

    @ServiceThreadOnly
    void UpdateActiveInput(Int32 path, Boolean notifyInputChange) {
        AssertRunOnServiceThread();
        // Seq #15
        if (path == GetActivePath()) {
            return;
        }
        SetPrevPortId(GetActivePortId());
        SetActivePath(path);
        // TODO: Handle PAP/PIP case.
        // Show OSD port change banner
        if (notifyInputChange) {
            ActiveSource activeSource = GetActiveSource();
            HdmiDeviceInfo info = GetCecDeviceInfo(activeSource.logicalAddress);
            if (info == NULL) {
                info = new HdmiDeviceInfo(Constants.ADDR_INVALID, path, GetActivePortId(),
                        HdmiDeviceInfo.DEVICE_RESERVED, 0, NULL);
            }
            mService->InvokeInputChangeListener(info);
        }
    }

    @ServiceThreadOnly
    void DoManualPortSwitching(Int32 portId, IHdmiControlCallback callback) {
        AssertRunOnServiceThread();
        // Seq #20
        if (!mService->IsValidPortId(portId)) {
            InvokeCallback(callback, HdmiControlManager.RESULT_INCORRECT_MODE);
            return;
        }
        if (portId == GetActivePortId()) {
            InvokeCallback(callback, HdmiControlManager.RESULT_SUCCESS);
            return;
        }
        mActiveSource->Invalidate();
        if (!mService->IsControlEnabled()) {
            SetActivePortId(portId);
            InvokeCallback(callback, HdmiControlManager.RESULT_INCORRECT_MODE);
            return;
        }
        Int32 oldPath = GetActivePortId() != Constants.INVALID_PORT_ID
                ? mService->PortIdToPath(GetActivePortId()) : GetDeviceInfo()->GetPhysicalAddress();
        SetActivePath(oldPath);
        if (mSkipRoutingControl) {
            mSkipRoutingControl = FALSE;
            return;
        }
        Int32 newPath = mService->PortIdToPath(portId);
        HdmiCecMessage routingChange =
                HdmiCecMessageBuilder->BuildRoutingChange(mAddress, oldPath, newPath);
        mService->SendCecCommand(routingChange);
        RemoveAction(RoutingControlAction.class);
        AddAndStartAction(new RoutingControlAction(this, newPath, TRUE, callback));
    }

    @ServiceThreadOnly
    Int32 GetPowerStatus() {
        AssertRunOnServiceThread();
        return mService->GetPowerStatus();
    }

    /**
     * Sends key to a target CEC device.
     *
     * @param keyCode key code to send. Defined in {@link android.view.KeyEvent}.
     * @param isPressed TRUE if this is key press event
     */
    //@Override
    @ServiceThreadOnly
    protected void SendKeyEvent(Int32 keyCode, Boolean isPressed) {
        AssertRunOnServiceThread();
        if (!HdmiCecKeycode->IsSupportedKeycode(keyCode)) {
            Slogger::W(TAG, "Unsupported key: " + keyCode);
            return;
        }
        List<SendKeyAction> action = GetActions(SendKeyAction.class);
        if (!action->IsEmpty()) {
            action->Get(0).ProcessKeyEvent(keyCode, isPressed);
        } else {
            if (isPressed && GetActiveSource()->IsValid()) {
                Int32 logicalAddress = GetActiveSource().logicalAddress;
                AddAndStartAction(new SendKeyAction(this, logicalAddress, keyCode));
            } else {
                Slogger::W(TAG, "Discard key event: " + keyCode + " pressed:" + isPressed);
            }
        }
    }

    private static void InvokeCallback(IHdmiControlCallback callback, Int32 result) {
        if (callback == NULL) {
            return;
        }
        try {
            callback->OnComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Invoking callback failed:" + e);
        }
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleActiveSource(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        Int32 logicalAddress = message->GetSource();
        Int32 physicalAddress = HdmiUtils->TwoBytesToInt(message->GetParams());
        if (GetCecDeviceInfo(logicalAddress) == NULL) {
            HandleNewDeviceAtTheTailOfActivePath(physicalAddress);
        } else {
            ActiveSource activeSource = ActiveSource->Of(logicalAddress, physicalAddress);
            ActiveSourceHandler->Create(this, NULL).Process(activeSource);
        }
        return TRUE;
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleInactiveSource(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        // Seq #10

        // Ignore <Inactive Source> from non-active source device.
        if (GetActiveSource().logicalAddress != message->GetSource()) {
            return TRUE;
        }
        if (IsProhibitMode()) {
            return TRUE;
        }
        Int32 portId = GetPrevPortId();
        if (portId != Constants.INVALID_PORT_ID) {
            // TODO: Do this only if TV is not showing multiview like PIP/PAP.

            HdmiDeviceInfo inactiveSource = GetCecDeviceInfo(message->GetSource());
            if (inactiveSource == NULL) {
                return TRUE;
            }
            if (mService->PathToPortId(inactiveSource->GetPhysicalAddress()) == portId) {
                return TRUE;
            }
            // TODO: Switch the TV freeze mode off

            DoManualPortSwitching(portId, NULL);
            SetPrevPortId(Constants.INVALID_PORT_ID);
        }
        return TRUE;
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleRequestActiveSource(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        // Seq #19
        if (mAddress == GetActiveSource().logicalAddress) {
            mService->SendCecCommand(
                    HdmiCecMessageBuilder->BuildActiveSource(mAddress, GetActivePath()));
        }
        return TRUE;
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleGetMenuLanguage(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        if (!BroadcastMenuLanguage(mService->GetLanguage())) {
            Slogger::W(TAG, "Failed to respond to <Get Menu Language>: " + message->ToString());
        }
        return TRUE;
    }

    @ServiceThreadOnly
    Boolean BroadcastMenuLanguage(String language) {
        AssertRunOnServiceThread();
        HdmiCecMessage command = HdmiCecMessageBuilder->BuildSetMenuLanguageCommand(
                mAddress, language);
        if (command != NULL) {
            mService->SendCecCommand(command);
            return TRUE;
        }
        return FALSE;
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleReportPhysicalAddress(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        Int32 path = HdmiUtils->TwoBytesToInt(message->GetParams());
        Int32 address = message->GetSource();
        Int32 type = message->GetParams()[2];

        if (UpdateCecSwitchInfo(address, type, path)) return TRUE;

        // Ignore if [Device Discovery Action] is going on.
        if (HasAction(DeviceDiscoveryAction.class)) {
            Slogger::I(TAG, "Ignored while Device Discovery Action is in progress: " + message);
            return TRUE;
        }

        if (!IsInDeviceList(address, path)) {
            HandleNewDeviceAtTheTailOfActivePath(path);
        }
        StartNewDeviceAction(ActiveSource->Of(address, path));
        return TRUE;
    }

    //@Override
    protected Boolean HandleReportPowerStatus(HdmiCecMessage command) {
        Int32 newStatus = command->GetParams()[0] & 0xFF;
        UpdateDevicePowerStatus(command->GetSource(), newStatus);
        return TRUE;
    }

    //@Override
    protected Boolean HandleTimerStatus(HdmiCecMessage message) {
        // Do nothing.
        return TRUE;
    }

    //@Override
    protected Boolean HandleRecordStatus(HdmiCecMessage message) {
        // Do nothing.
        return TRUE;
    }

    Boolean UpdateCecSwitchInfo(Int32 address, Int32 type, Int32 path) {
        if (address == Constants.ADDR_UNREGISTERED
                && type == HdmiDeviceInfo.DEVICE_PURE_CEC_SWITCH) {
            mCecSwitches->Add(path);
            UpdateSafeDeviceInfoList();
            return TRUE;  // Pure switch does not need further processing. Return here.
        }
        if (type == HdmiDeviceInfo.DEVICE_AUDIO_SYSTEM) {
            mCecSwitches->Add(path);
        }
        return FALSE;
    }

    void StartNewDeviceAction(ActiveSource activeSource) {
        for (NewDeviceAction action : GetActions(NewDeviceAction.class)) {
            // If there is new device action which has the same logical address and path
            // ignore new request.
            // NewDeviceAction is created whenever it receives <Report Physical Address>.
            // And there is a chance starting NewDeviceAction for the same source.
            // Usually, new device sends <Report Physical Address> when it's plugged
            // in. However, TV can detect a new device from HotPlugDetectionAction,
            // which sends <Give Physical Address> to the source for newly detected
            // device.
            if (action->IsActionOf(activeSource)) {
                return;
            }
        }

        AddAndStartAction(new NewDeviceAction(this, activeSource.logicalAddress,
                activeSource.physicalAddress));
    }

    private void HandleNewDeviceAtTheTailOfActivePath(Int32 path) {
        // Seq #22
        if (IsTailOfActivePath(path, GetActivePath())) {
            RemoveAction(RoutingControlAction.class);
            Int32 newPath = mService->PortIdToPath(GetActivePortId());
            SetActivePath(newPath);
            mService->SendCecCommand(HdmiCecMessageBuilder->BuildRoutingChange(
                    mAddress, GetActivePath(), newPath));
            AddAndStartAction(new RoutingControlAction(this, newPath, FALSE, NULL));
        }
    }

    /**
     * Whether the given path is located in the tail of current active path.
     *
     * @param path to be tested
     * @param activePath current active path
     * @return TRUE if the given path is located in the tail of current active path; otherwise,
     *         FALSE
     */
    static Boolean IsTailOfActivePath(Int32 path, Int32 activePath) {
        // If active routing path is internal source, return FALSE.
        if (activePath == 0) {
            return FALSE;
        }
        for (Int32 i = 12; i >= 0; i -= 4) {
            Int32 curActivePath = (activePath >> i) & 0xF;
            if (curActivePath == 0) {
                return TRUE;
            } else {
                Int32 curPath = (path >> i) & 0xF;
                if (curPath != curActivePath) {
                    return FALSE;
                }
            }
        }
        return FALSE;
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleRoutingChange(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        // Seq #21
        Byte[] params = message->GetParams();
        Int32 currentPath = HdmiUtils->TwoBytesToInt(params);
        if (HdmiUtils->IsAffectingActiveRoutingPath(GetActivePath(), currentPath)) {
            mActiveSource->Invalidate();
            RemoveAction(RoutingControlAction.class);
            Int32 newPath = HdmiUtils->TwoBytesToInt(params, 2);
            AddAndStartAction(new RoutingControlAction(this, newPath, TRUE, NULL));
        }
        return TRUE;
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleReportAudioStatus(HdmiCecMessage message) {
        AssertRunOnServiceThread();

        Byte params[] = message->GetParams();
        Int32 mute = params[0] & 0x80;
        Int32 volume = params[0] & 0x7F;
        SetAudioStatus(mute == 0x80, volume);
        return TRUE;
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleTextViewOn(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        if (mService->IsPowerStandbyOrTransient() && mAutoWakeup) {
            mService->WakeUp();
        }
        return TRUE;
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleImageViewOn(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        // Currently, it's the same as <Text View On>.
        return HandleTextViewOn(message);
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleSetOsdName(HdmiCecMessage message) {
        Int32 source = message->GetSource();
        HdmiDeviceInfo deviceInfo = GetCecDeviceInfo(source);
        // If the device is not in device list, ignore it.
        if (deviceInfo == NULL) {
            Slogger::E(TAG, "No source device info for <Set Osd Name>." + message);
            return TRUE;
        }
        String osdName = NULL;
        try {
            osdName = new String(message->GetParams(), "US-ASCII");
        } catch (UnsupportedEncodingException e) {
            Slogger::E(TAG, "Invalid <Set Osd Name> request:" + message, e);
            return TRUE;
        }

        if (deviceInfo->GetDisplayName()->Equals(osdName)) {
            Slogger::I(TAG, "Ignore incoming <Set Osd Name> having same osd name:" + message);
            return TRUE;
        }

        AddCecDevice(new HdmiDeviceInfo(deviceInfo->GetLogicalAddress(),
                deviceInfo->GetPhysicalAddress(), deviceInfo->GetPortId(),
                deviceInfo->GetDeviceType(), deviceInfo->GetVendorId(), osdName));
        return TRUE;
    }

    @ServiceThreadOnly
    private void LaunchDeviceDiscovery() {
        AssertRunOnServiceThread();
        ClearDeviceInfoList();
        DeviceDiscoveryAction action = new DeviceDiscoveryAction(this,
                new DeviceDiscoveryCallback() {
                    //@Override
                    CARAPI OnDeviceDiscoveryDone(List<HdmiDeviceInfo> deviceInfos) {
                        for (HdmiDeviceInfo info : deviceInfos) {
                            AddCecDevice(info);
                        }

                        // Since we removed all devices when it's start and
                        // device discovery action does not poll local devices,
                        // we should put device info of local device manually here
                        for (HdmiCecLocalDevice device : mService->GetAllLocalDevices()) {
                            AddCecDevice(device->GetDeviceInfo());
                        }

                        AddAndStartAction(new HotplugDetectionAction(HdmiCecLocalDeviceTv.this));
                        AddAndStartAction(new PowerStatusMonitorAction(HdmiCecLocalDeviceTv.this));

                        // If there is AVR, initiate System Audio Auto initiation action,
                        // which turns on and off system audio according to last system
                        // audio setting.
                        HdmiDeviceInfo avr = GetAvrDeviceInfo();
                        if (avr != NULL) {
                            OnNewAvrAdded(avr);
                        }
                    }
                });
        AddAndStartAction(action);
    }

    @ServiceThreadOnly
    void OnNewAvrAdded(HdmiDeviceInfo avr) {
        AssertRunOnServiceThread();
        if (GetSystemAudioModeSetting()) {
            AddAndStartAction(new SystemAudioAutoInitiationAction(this, avr->GetLogicalAddress()));
        }
        if (IsArcFeatureEnabled()) {
            StartArcAction(TRUE);
        }
    }

    // Clear all device info.
    @ServiceThreadOnly
    private void ClearDeviceInfoList() {
        AssertRunOnServiceThread();
        for (HdmiDeviceInfo info : mSafeExternalInputs) {
            InvokeDeviceEventListener(info, HdmiControlManager.DEVICE_EVENT_REMOVE_DEVICE);
        }
        mDeviceInfos->Clear();
        UpdateSafeDeviceInfoList();
    }

    @ServiceThreadOnly
    // Seq #32
    void ChangeSystemAudioMode(Boolean enabled, IHdmiControlCallback callback) {
        AssertRunOnServiceThread();
        if (!mService->IsControlEnabled() || HasAction(DeviceDiscoveryAction.class)) {
            SetSystemAudioMode(FALSE, TRUE);
            InvokeCallback(callback, HdmiControlManager.RESULT_INCORRECT_MODE);
            return;
        }
        HdmiDeviceInfo avr = GetAvrDeviceInfo();
        if (avr == NULL) {
            SetSystemAudioMode(FALSE, TRUE);
            InvokeCallback(callback, HdmiControlManager.RESULT_TARGET_NOT_AVAILABLE);
            return;
        }

        AddAndStartAction(
                new SystemAudioActionFromTv(this, avr->GetLogicalAddress(), enabled, callback));
    }

    // # Seq 25
    void SetSystemAudioMode(Boolean on, Boolean updateSetting) {
        HdmiLogger->Debug("System Audio Mode change[old:%b new:%b]", mSystemAudioActivated, on);

        if (updateSetting) {
            mService->WriteBooleanSetting(Global.HDMI_SYSTEM_AUDIO_ENABLED, on);
        }
        UpdateAudioManagerForSystemAudio(on);
        synchronized(mLock) {
            if (mSystemAudioActivated != on) {
                mSystemAudioActivated = on;
                mService->AnnounceSystemAudioModeChange(on);
            }
        }
    }

    private void UpdateAudioManagerForSystemAudio(Boolean on) {
        Int32 device = mService->GetAudioManager()->SetHdmiSystemAudioSupported(on);
        HdmiLogger->Debug("[A]UpdateSystemAudio mode[on=%b] output=[%X]", on, device);
    }

    Boolean IsSystemAudioActivated() {
        if (GetAvrDeviceInfo() == NULL) {
            return FALSE;
        }
        synchronized(mLock) {
            return mSystemAudioActivated;
        }
    }

    Boolean GetSystemAudioModeSetting() {
        return mService->ReadBooleanSetting(Global.HDMI_SYSTEM_AUDIO_ENABLED, FALSE);
    }

    /**
     * Change ARC status into the given {@code enabled} status.
     *
     * @return {@code TRUE} if ARC was in "Enabled" status
     */
    @ServiceThreadOnly
    Boolean SetArcStatus(Boolean enabled) {
        AssertRunOnServiceThread();

        HdmiLogger->Debug("Set Arc Status[old:%b new:%b]", mArcEstablished, enabled);
        Boolean oldStatus = mArcEstablished;
        // 1. Enable/disable ARC circuit.
        mService->SetAudioReturnChannel(enabled);
        // 2. Notify arc status to audio service.
        NotifyArcStatusToAudioService(enabled);
        // 3. Update arc status;
        mArcEstablished = enabled;
        return oldStatus;
    }

    private void NotifyArcStatusToAudioService(Boolean enabled) {
        // Note that we don't set any name to ARC.
        mService->GetAudioManager()->SetWiredDeviceConnectionState(
                AudioSystem.DEVICE_OUT_HDMI_ARC,
                enabled ? 1 : 0, "");
    }

    /**
     * Returns whether ARC is enabled or not.
     */
    @ServiceThreadOnly
    Boolean IsArcEstabilished() {
        AssertRunOnServiceThread();
        return mArcFeatureEnabled && mArcEstablished;
    }

    @ServiceThreadOnly
    void ChangeArcFeatureEnabled(Boolean enabled) {
        AssertRunOnServiceThread();

        if (mArcFeatureEnabled != enabled) {
            mArcFeatureEnabled = enabled;
            if (enabled) {
                if (!mArcEstablished) {
                    StartArcAction(TRUE);
                }
            } else {
                if (mArcEstablished) {
                    StartArcAction(FALSE);
                }
            }
        }
    }

    @ServiceThreadOnly
    Boolean IsArcFeatureEnabled() {
        AssertRunOnServiceThread();
        return mArcFeatureEnabled;
    }

    @ServiceThreadOnly
    void StartArcAction(Boolean enabled) {
        AssertRunOnServiceThread();
        HdmiDeviceInfo info = GetAvrDeviceInfo();
        if (info == NULL) {
            Slogger::W(TAG, "Failed to start arc action; No AVR device.");
            return;
        }
        if (!CanStartArcUpdateAction(info->GetLogicalAddress(), enabled)) {
            Slogger::W(TAG, "Failed to start arc action; ARC configuration check failed.");
            if (enabled && !IsConnectedToArcPort(info->GetPhysicalAddress())) {
                DisplayOsd(OSD_MESSAGE_ARC_CONNECTED_INVALID_PORT);
            }
            return;
        }

        // Terminate opposite action and start action if not exist.
        if (enabled) {
            RemoveAction(RequestArcTerminationAction.class);
            if (!HasAction(RequestArcInitiationAction.class)) {
                AddAndStartAction(new RequestArcInitiationAction(this, info->GetLogicalAddress()));
            }
        } else {
            RemoveAction(RequestArcInitiationAction.class);
            if (!HasAction(RequestArcTerminationAction.class)) {
                AddAndStartAction(new RequestArcTerminationAction(this, info->GetLogicalAddress()));
            }
        }
    }

    private Boolean IsDirectConnectAddress(Int32 physicalAddress) {
        return (physicalAddress & Constants.ROUTING_PATH_TOP_MASK) == physicalAddress;
    }

    void SetAudioStatus(Boolean mute, Int32 volume) {
        synchronized(mLock) {
            mSystemAudioMute = mute;
            mSystemAudioVolume = volume;
            Int32 maxVolume = mService->GetAudioManager()->GetStreamMaxVolume(
                    AudioManager.STREAM_MUSIC);
            mService->SetAudioStatus(mute,
                    VolumeControlAction->ScaleToCustomVolume(volume, maxVolume));
            DisplayOsd(HdmiControlManager.OSD_MESSAGE_AVR_VOLUME_CHANGED,
                    mute ? HdmiControlManager.AVR_VOLUME_MUTED : volume);
        }
    }

    @ServiceThreadOnly
    void ChangeVolume(Int32 curVolume, Int32 delta, Int32 maxVolume) {
        AssertRunOnServiceThread();
        if (delta == 0 || !IsSystemAudioActivated()) {
            return;
        }

        Int32 targetVolume = curVolume + delta;
        Int32 cecVolume = VolumeControlAction->ScaleToCecVolume(targetVolume, maxVolume);
        synchronized(mLock) {
            // If new volume is the same as current system audio volume, just ignore it.
            // Note that UNKNOWN_VOLUME is not in range of cec volume scale.
            if (cecVolume == mSystemAudioVolume) {
                // Update tv volume with system volume value.
                mService->SetAudioStatus(FALSE,
                        VolumeControlAction->ScaleToCustomVolume(mSystemAudioVolume, maxVolume));
                return;
            }
        }

        List<VolumeControlAction> actions = GetActions(VolumeControlAction.class);
        if (actions->IsEmpty()) {
            AddAndStartAction(new VolumeControlAction(this,
                    GetAvrDeviceInfo()->GetLogicalAddress(), delta > 0));
        } else {
            actions->Get(0).HandleVolumeChange(delta > 0);
        }
    }

    @ServiceThreadOnly
    void ChangeMute(Boolean mute) {
        AssertRunOnServiceThread();
        HdmiLogger->Debug("[A]:Change mute:%b", mute);
        synchronized(mLock) {
            if (mSystemAudioMute == mute) {
                HdmiLogger->Debug("No need to change mute.");
                return;
            }
        }
        if (!IsSystemAudioActivated()) {
            HdmiLogger->Debug("[A]:System audio is not activated.");
            return;
        }

        // Remove existing volume action.
        RemoveAction(VolumeControlAction.class);
        SendUserControlPressedAndReleased(GetAvrDeviceInfo()->GetLogicalAddress(),
                mute ? HdmiCecKeycode.CEC_KEYCODE_MUTE_FUNCTION :
                        HdmiCecKeycode.CEC_KEYCODE_RESTORE_VOLUME_FUNCTION);
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleInitiateArc(HdmiCecMessage message) {
        AssertRunOnServiceThread();

        if (!CanStartArcUpdateAction(message->GetSource(), TRUE)) {
            mService->MaySendFeatureAbortCommand(message, Constants.ABORT_REFUSED);
            if (!IsConnectedToArcPort(message->GetSource())) {
                DisplayOsd(OSD_MESSAGE_ARC_CONNECTED_INVALID_PORT);
            }
            return TRUE;
        }

        // In case where <Initiate Arc> is started by <Request ARC Initiation>
        // need to clean up RequestArcInitiationAction.
        RemoveAction(RequestArcInitiationAction.class);
        SetArcTransmissionStateAction action = new SetArcTransmissionStateAction(this,
                message->GetSource(), TRUE);
        AddAndStartAction(action);
        return TRUE;
    }

    private Boolean CanStartArcUpdateAction(Int32 avrAddress, Boolean shouldCheckArcFeatureEnabled) {
        HdmiDeviceInfo avr = GetAvrDeviceInfo();
        if (avr != NULL
                && (avrAddress == avr->GetLogicalAddress())
                && IsConnectedToArcPort(avr->GetPhysicalAddress())
                && IsDirectConnectAddress(avr->GetPhysicalAddress())) {
            if (shouldCheckArcFeatureEnabled) {
                return IsArcFeatureEnabled();
            } else {
                return TRUE;
            }
        } else {
            return FALSE;
        }
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleTerminateArc(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        // In cast of termination, do not check ARC configuration in that AVR device
        // might be removed already.

        // In case where <Terminate Arc> is started by <Request ARC Termination>
        // need to clean up RequestArcInitiationAction.
        RemoveAction(RequestArcTerminationAction.class);
        SetArcTransmissionStateAction action = new SetArcTransmissionStateAction(this,
                message->GetSource(), FALSE);
        AddAndStartAction(action);
        return TRUE;
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleSetSystemAudioMode(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        if (!IsMessageForSystemAudio(message)) {
            HdmiLogger->Warning("Invalid <Set System Audio Mode> message:" + message);
            mService->MaySendFeatureAbortCommand(message, Constants.ABORT_REFUSED);
            return TRUE;
        }
        SystemAudioActionFromAvr action = new SystemAudioActionFromAvr(this,
                message->GetSource(), HdmiUtils->ParseCommandParamSystemAudioStatus(message), NULL);
        AddAndStartAction(action);
        return TRUE;
    }

    //@Override
    @ServiceThreadOnly
    protected Boolean HandleSystemAudioModeStatus(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        if (!IsMessageForSystemAudio(message)) {
            HdmiLogger->Warning("Invalid <System Audio Mode Status> message:" + message);
            // Ignore this message.
            return TRUE;
        }
        SetSystemAudioMode(HdmiUtils->ParseCommandParamSystemAudioStatus(message), TRUE);
        return TRUE;
    }

    // Seq #53
    //@Override
    @ServiceThreadOnly
    protected Boolean HandleRecordTvScreen(HdmiCecMessage message) {
        List<OneTouchRecordAction> actions = GetActions(OneTouchRecordAction.class);
        if (!actions->IsEmpty()) {
            // Assumes only one OneTouchRecordAction.
            OneTouchRecordAction action = actions->Get(0);
            if (action->GetRecorderAddress() != message->GetSource()) {
                AnnounceOneTouchRecordResult(
                        HdmiControlManager.ONE_TOUCH_RECORD_PREVIOUS_RECORDING_IN_PROGRESS);
            }
            return super->HandleRecordTvScreen(message);
        }

        Int32 recorderAddress = message->GetSource();
        Byte[] recordSource = mService->InvokeRecordRequestListener(recorderAddress);
        Int32 reason = StartOneTouchRecord(recorderAddress, recordSource);
        if (reason != Constants.ABORT_NO_ERROR) {
            mService->MaySendFeatureAbortCommand(message, reason);
        }
        return TRUE;
    }

    //@Override
    protected Boolean HandleTimerClearedStatus(HdmiCecMessage message) {
        Byte[] params = message->GetParams();
        Int32 timerClearedStatusData = params[0] & 0xFF;
        AnnounceTimerRecordingResult(timerClearedStatusData);
        return TRUE;
    }

    void AnnounceOneTouchRecordResult(Int32 result) {
        mService->InvokeOneTouchRecordResult(result);
    }

    void AnnounceTimerRecordingResult(Int32 result) {
        mService->InvokeTimerRecordingResult(result);
    }

    void AnnounceClearTimerRecordingResult(Int32 result) {
        mService->InvokeClearTimerRecordingResult(result);
    }

    private Boolean IsMessageForSystemAudio(HdmiCecMessage message) {
        return mService->IsControlEnabled()
                && message->GetSource() == Constants.ADDR_AUDIO_SYSTEM
                && (message->GetDestination() == Constants.ADDR_TV
                        || message->GetDestination() == Constants.ADDR_BROADCAST)
                && GetAvrDeviceInfo() != NULL;
    }

    /**
     * Add a new {@link HdmiDeviceInfo}. It returns old device info which has the same
     * logical address as new device info's.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     *
     * @param deviceInfo a new {@link HdmiDeviceInfo} to be added.
     * @return {@code NULL} if it is new device. Otherwise, returns old {@HdmiDeviceInfo}
     *         that has the same logical address as new one has.
     */
    @ServiceThreadOnly
    private HdmiDeviceInfo AddDeviceInfo(HdmiDeviceInfo deviceInfo) {
        AssertRunOnServiceThread();
        HdmiDeviceInfo oldDeviceInfo = GetCecDeviceInfo(deviceInfo->GetLogicalAddress());
        if (oldDeviceInfo != NULL) {
            RemoveDeviceInfo(deviceInfo->GetId());
        }
        mDeviceInfos->Append(deviceInfo->GetId(), deviceInfo);
        UpdateSafeDeviceInfoList();
        return oldDeviceInfo;
    }

    /**
     * Remove a device info corresponding to the given {@code logicalAddress}.
     * It returns removed {@link HdmiDeviceInfo} if exists.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     *
     * @param id id of device to be removed
     * @return removed {@link HdmiDeviceInfo} it exists. Otherwise, returns {@code NULL}
     */
    @ServiceThreadOnly
    private HdmiDeviceInfo RemoveDeviceInfo(Int32 id) {
        AssertRunOnServiceThread();
        HdmiDeviceInfo deviceInfo = mDeviceInfos->Get(id);
        if (deviceInfo != NULL) {
            mDeviceInfos->Remove(id);
        }
        UpdateSafeDeviceInfoList();
        return deviceInfo;
    }

    /**
     * Return a list of all {@link HdmiDeviceInfo}.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     * This is not thread-safe. For thread safety, call {@link #getSafeExternalInputsLocked} which
     * does not include local device.
     */
    @ServiceThreadOnly
    List<HdmiDeviceInfo> GetDeviceInfoList(Boolean includeLocalDevice) {
        AssertRunOnServiceThread();
        if (includeLocalDevice) {
            return HdmiUtils->SparseArrayToList(mDeviceInfos);
        } else {
            ArrayList<HdmiDeviceInfo> infoList = new ArrayList<>();
            for (Int32 i = 0; i < mDeviceInfos->Size(); ++i) {
                HdmiDeviceInfo info = mDeviceInfos->ValueAt(i);
                if (!IsLocalDeviceAddress(info->GetLogicalAddress())) {
                    infoList->Add(info);
                }
            }
            return infoList;
        }
    }

    /**
     * Return external input devices.
     */
    List<HdmiDeviceInfo> GetSafeExternalInputsLocked() {
        return mSafeExternalInputs;
    }

    @ServiceThreadOnly
    private void UpdateSafeDeviceInfoList() {
        AssertRunOnServiceThread();
        List<HdmiDeviceInfo> copiedDevices = HdmiUtils->SparseArrayToList(mDeviceInfos);
        List<HdmiDeviceInfo> externalInputs = GetInputDevices();
        synchronized(mLock) {
            mSafeAllDeviceInfos = copiedDevices;
            mSafeExternalInputs = externalInputs;
        }
    }

    /**
     * Return a list of external cec input (source) devices.
     *
     * <p>Note that this effectively excludes non-source devices like system audio,
     * secondary TV.
     */
    private List<HdmiDeviceInfo> GetInputDevices() {
        ArrayList<HdmiDeviceInfo> infoList = new ArrayList<>();
        for (Int32 i = 0; i < mDeviceInfos->Size(); ++i) {
            HdmiDeviceInfo info = mDeviceInfos->ValueAt(i);
            if (IsLocalDeviceAddress(info->GetLogicalAddress())) {
                continue;
            }
            if (info->IsSourceType() && !HideDevicesBehindLegacySwitch(info)) {
                infoList->Add(info);
            }
        }
        return infoList;
    }

    // Check if we are hiding CEC devices connected to a legacy (non-CEC) switch.
    // Returns TRUE if the policy is set to TRUE, and the device to check does not have
    // a parent CEC device (which should be the CEC-enabled switch) in the list.
    private Boolean HideDevicesBehindLegacySwitch(HdmiDeviceInfo info) {
        return HdmiConfig.HIDE_DEVICES_BEHIND_LEGACY_SWITCH
                && !IsConnectedToCecSwitch(info->GetPhysicalAddress(), mCecSwitches);
    }

    private static Boolean IsConnectedToCecSwitch(Int32 path, Collection<Integer> switches) {
        for (Int32 switchPath : switches) {
            if (IsParentPath(switchPath, path)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    private static Boolean IsParentPath(Int32 parentPath, Int32 childPath) {
        // (A000, AB00) (AB00, ABC0), (ABC0, ABCD)
        // If child's last non-zero nibble is removed, the result equals to the parent.
        for (Int32 i = 0; i <= 12; i += 4) {
            Int32 nibble = (childPath >> i) & 0xF;
            if (nibble != 0) {
                Int32 parentNibble = (parentPath >> i) & 0xF;
                return parentNibble == 0 && (childPath >> i+4) == (parentPath >> i+4);
            }
        }
        return FALSE;
    }

    private void InvokeDeviceEventListener(HdmiDeviceInfo info, Int32 status) {
        if (info->IsSourceType() && !HideDevicesBehindLegacySwitch(info)) {
            mService->InvokeDeviceEventListeners(info, status);
        }
    }

    @ServiceThreadOnly
    private Boolean IsLocalDeviceAddress(Int32 address) {
        AssertRunOnServiceThread();
        for (HdmiCecLocalDevice device : mService->GetAllLocalDevices()) {
            if (device->IsAddressOf(address)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    @ServiceThreadOnly
    HdmiDeviceInfo GetAvrDeviceInfo() {
        AssertRunOnServiceThread();
        return GetCecDeviceInfo(Constants.ADDR_AUDIO_SYSTEM);
    }

    /**
     * Return a {@link HdmiDeviceInfo} corresponding to the given {@code logicalAddress}.
     *
     * This is not thread-safe. For thread safety, call {@link #GetSafeCecDeviceInfo(Int32)}.
     *
     * @param logicalAddress logical address of the device to be retrieved
     * @return {@link HdmiDeviceInfo} matched with the given {@code logicalAddress}.
     *         Returns NULL if no logical address matched
     */
    @ServiceThreadOnly
    HdmiDeviceInfo GetCecDeviceInfo(Int32 logicalAddress) {
        AssertRunOnServiceThread();
        return mDeviceInfos->Get(HdmiDeviceInfo->IdForCecDevice(logicalAddress));
    }

    Boolean HasSystemAudioDevice() {
        return GetSafeAvrDeviceInfo() != NULL;
    }

    HdmiDeviceInfo GetSafeAvrDeviceInfo() {
        return GetSafeCecDeviceInfo(Constants.ADDR_AUDIO_SYSTEM);
    }

    /**
     * Thread safe version of {@link #GetCecDeviceInfo(Int32)}.
     *
     * @param logicalAddress logical address to be retrieved
     * @return {@link HdmiDeviceInfo} matched with the given {@code logicalAddress}.
     *         Returns NULL if no logical address matched
     */
    HdmiDeviceInfo GetSafeCecDeviceInfo(Int32 logicalAddress) {
        synchronized(mLock) {
            for (HdmiDeviceInfo info : mSafeAllDeviceInfos) {
                if (info->IsCecDevice() && info->GetLogicalAddress() == logicalAddress) {
                    return info;
                }
            }
            return NULL;
        }
    }

    /**
     * Called when a device is newly added or a new device is detected or
     * existing device is updated.
     *
     * @param info device info of a new device.
     */
    @ServiceThreadOnly
    final void AddCecDevice(HdmiDeviceInfo info) {
        AssertRunOnServiceThread();
        AddDeviceInfo(info);
        if (info->GetLogicalAddress() == mAddress) {
            // The addition of TV device itself should not be notified.
            return;
        }
        InvokeDeviceEventListener(info, HdmiControlManager.DEVICE_EVENT_ADD_DEVICE);
    }

    /**
     * Called when a device is removed or removal of device is detected.
     *
     * @param address a logical address of a device to be removed
     */
    @ServiceThreadOnly
    final void RemoveCecDevice(Int32 address) {
        AssertRunOnServiceThread();
        HdmiDeviceInfo info = RemoveDeviceInfo(HdmiDeviceInfo->IdForCecDevice(address));

        mCecMessageCache->FlushMessagesFrom(address);
        InvokeDeviceEventListener(info, HdmiControlManager.DEVICE_EVENT_REMOVE_DEVICE);
    }

    @ServiceThreadOnly
    void HandleRemoveActiveRoutingPath(Int32 path) {
        AssertRunOnServiceThread();
        // Seq #23
        if (IsTailOfActivePath(path, GetActivePath())) {
            RemoveAction(RoutingControlAction.class);
            Int32 newPath = mService->PortIdToPath(GetActivePortId());
            mService->SendCecCommand(HdmiCecMessageBuilder->BuildRoutingChange(
                    mAddress, GetActivePath(), newPath));
            mActiveSource->Invalidate();
            AddAndStartAction(new RoutingControlAction(this, GetActivePath(), TRUE, NULL));
        }
    }

    /**
     * Launch routing control process.
     *
     * @param routingForBootup TRUE if routing control is initiated due to One Touch Play
     *        or TV power on
     */
    @ServiceThreadOnly
    void LaunchRoutingControl(Boolean routingForBootup) {
        AssertRunOnServiceThread();
        // Seq #24
        if (GetActivePortId() != Constants.INVALID_PORT_ID) {
            if (!routingForBootup && !IsProhibitMode()) {
                RemoveAction(RoutingControlAction.class);
                Int32 newPath = mService->PortIdToPath(GetActivePortId());
                SetActivePath(newPath);
                mService->SendCecCommand(HdmiCecMessageBuilder->BuildRoutingChange(mAddress,
                        GetActivePath(), newPath));
                AddAndStartAction(new RoutingControlAction(this, GetActivePortId(),
                        routingForBootup, NULL));
            }
        } else {
            Int32 activePath = mService->GetPhysicalAddress();
            SetActivePath(activePath);
            if (!routingForBootup) {
                mService->SendCecCommand(HdmiCecMessageBuilder->BuildActiveSource(mAddress,
                        activePath));
            }
        }
    }

    /**
     * Returns the {@link HdmiDeviceInfo} instance whose physical address matches
     * the given routing path. CEC devices use routing path for its physical address to
     * describe the hierarchy of the devices in the network.
     *
     * @param path routing path or physical address
     * @return {@link HdmiDeviceInfo} if the matched info is found; otherwise NULL
     */
    @ServiceThreadOnly
    final HdmiDeviceInfo GetDeviceInfoByPath(Int32 path) {
        AssertRunOnServiceThread();
        for (HdmiDeviceInfo info : GetDeviceInfoList(FALSE)) {
            if (info->GetPhysicalAddress() == path) {
                return info;
            }
        }
        return NULL;
    }

    /**
     * Whether a device of the specified physical address and logical address exists
     * in a device info list. However, both are minimal condition and it could
     * be different device from the original one.
     *
     * @param logicalAddress logical address of a device to be searched
     * @param physicalAddress physical address of a device to be searched
     * @return TRUE if exist; otherwise FALSE
     */
    @ServiceThreadOnly
    private Boolean IsInDeviceList(Int32 logicalAddress, Int32 physicalAddress) {
        AssertRunOnServiceThread();
        HdmiDeviceInfo device = GetCecDeviceInfo(logicalAddress);
        if (device == NULL) {
            return FALSE;
        }
        return device->GetPhysicalAddress() == physicalAddress;
    }

    //@Override
    @ServiceThreadOnly
    void OnHotplug(Int32 portId, Boolean connected) {
        AssertRunOnServiceThread();

        if (!connected) {
            RemoveCecSwitches(portId);
        }
        // Tv device will have permanent HotplugDetectionAction.
        List<HotplugDetectionAction> hotplugActions = GetActions(HotplugDetectionAction.class);
        if (!hotplugActions->IsEmpty()) {
            // Note that hotplug action is single action running on a machine.
            // "pollAllDevicesNow" cleans up timer and start poll action immediately.
            // It covers seq #40, #43.
            hotplugActions->Get(0).PollAllDevicesNow();
        }
    }

    private void RemoveCecSwitches(Int32 portId) {
        Iterator<Integer> it = mCecSwitches->Iterator();
        while (!it->HasNext()) {
            Int32 path = it->Next();
            if (PathToPortId(path) == portId) {
                it->Remove();
            }
        }
    }

    @ServiceThreadOnly
    void SetAutoDeviceOff(Boolean enabled) {
        AssertRunOnServiceThread();
        mAutoDeviceOff = enabled;
    }

    @ServiceThreadOnly
    void SetAutoWakeup(Boolean enabled) {
        AssertRunOnServiceThread();
        mAutoWakeup = enabled;
    }

    @ServiceThreadOnly
    Boolean GetAutoWakeup() {
        AssertRunOnServiceThread();
        return mAutoWakeup;
    }

    //@Override
    @ServiceThreadOnly
    protected void DisableDevice(Boolean initiatedByCec, PendingActionClearedCallback callback) {
        super->DisableDevice(initiatedByCec, callback);
        AssertRunOnServiceThread();
        // Remove any repeated working actions.
        // HotplugDetectionAction will be reinstated during the wake up process.
        // HdmiControlService->OnWakeUp() -> InitializeLocalDevices() ->
        //     LocalDeviceTv->OnAddressAllocated() -> LaunchDeviceDiscovery().
        RemoveAction(DeviceDiscoveryAction.class);
        RemoveAction(HotplugDetectionAction.class);
        RemoveAction(PowerStatusMonitorAction.class);
        // Remove recording actions.
        RemoveAction(OneTouchRecordAction.class);
        RemoveAction(TimerRecordingAction.class);

        DisableSystemAudioIfExist();
        DisableArcIfExist();
        ClearDeviceInfoList();
        CheckIfPendingActionsCleared();
    }

    @ServiceThreadOnly
    private void DisableSystemAudioIfExist() {
        AssertRunOnServiceThread();
        if (GetAvrDeviceInfo() == NULL) {
            return;
        }

        // Seq #31.
        RemoveAction(SystemAudioActionFromAvr.class);
        RemoveAction(SystemAudioActionFromTv.class);
        RemoveAction(SystemAudioAutoInitiationAction.class);
        RemoveAction(SystemAudioStatusAction.class);
        RemoveAction(VolumeControlAction.class);

        // Turn off the mode but do not write it the settings, so that the next time TV powers on
        // the system audio mode setting can be restored automatically.
        SetSystemAudioMode(FALSE, FALSE);
    }

    @ServiceThreadOnly
    private void DisableArcIfExist() {
        AssertRunOnServiceThread();
        HdmiDeviceInfo avr = GetAvrDeviceInfo();
        if (avr == NULL) {
            return;
        }

        // Seq #44.
        RemoveAction(RequestArcInitiationAction.class);
        if (!HasAction(RequestArcTerminationAction.class) && IsArcEstabilished()) {
            AddAndStartAction(new RequestArcTerminationAction(this, avr->GetLogicalAddress()));
        }
    }

    //@Override
    @ServiceThreadOnly
    protected void OnStandby(Boolean initiatedByCec) {
        AssertRunOnServiceThread();
        // Seq #11
        if (!mService->IsControlEnabled()) {
            return;
        }
        if (!initiatedByCec && mAutoDeviceOff) {
            mService->SendCecCommand(HdmiCecMessageBuilder->BuildStandby(
                    mAddress, Constants.ADDR_BROADCAST));
        }
    }

    Boolean IsProhibitMode() {
        return mService->IsProhibitMode();
    }

    Boolean IsPowerStandbyOrTransient() {
        return mService->IsPowerStandbyOrTransient();
    }

    @ServiceThreadOnly
    void DisplayOsd(Int32 messageId) {
        AssertRunOnServiceThread();
        mService->DisplayOsd(messageId);
    }

    @ServiceThreadOnly
    void DisplayOsd(Int32 messageId, Int32 extra) {
        AssertRunOnServiceThread();
        mService->DisplayOsd(messageId, extra);
    }

    // Seq #54 and #55
    @ServiceThreadOnly
    Int32 StartOneTouchRecord(Int32 recorderAddress, Byte[] recordSource) {
        AssertRunOnServiceThread();
        if (!mService->IsControlEnabled()) {
            Slogger::W(TAG, "Can not start one touch record. CEC control is disabled.");
            AnnounceOneTouchRecordResult(ONE_TOUCH_RECORD_CEC_DISABLED);
            return Constants.ABORT_NOT_IN_CORRECT_MODE;
        }

        if (!CheckRecorder(recorderAddress)) {
            Slogger::W(TAG, "Invalid recorder address:" + recorderAddress);
            AnnounceOneTouchRecordResult(ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
            return Constants.ABORT_NOT_IN_CORRECT_MODE;
        }

        if (!CheckRecordSource(recordSource)) {
            Slogger::W(TAG, "Invalid record source." + Arrays->ToString(recordSource));
            AnnounceOneTouchRecordResult(ONE_TOUCH_RECORD_FAIL_TO_RECORD_DISPLAYED_SCREEN);
            return Constants.ABORT_UNABLE_TO_DETERMINE;
        }

        AddAndStartAction(new OneTouchRecordAction(this, recorderAddress, recordSource));
        Slogger::I(TAG, "Start new [One Touch Record]-Target:" + recorderAddress + ", recordSource:"
                + Arrays->ToString(recordSource));
        return Constants.ABORT_NO_ERROR;
    }

    @ServiceThreadOnly
    void StopOneTouchRecord(Int32 recorderAddress) {
        AssertRunOnServiceThread();
        if (!mService->IsControlEnabled()) {
            Slogger::W(TAG, "Can not stop one touch record. CEC control is disabled.");
            AnnounceOneTouchRecordResult(ONE_TOUCH_RECORD_CEC_DISABLED);
            return;
        }

        if (!CheckRecorder(recorderAddress)) {
            Slogger::W(TAG, "Invalid recorder address:" + recorderAddress);
            AnnounceOneTouchRecordResult(ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
            return;
        }

        // Remove one touch record action so that other one touch record can be started.
        RemoveAction(OneTouchRecordAction.class);
        mService->SendCecCommand(HdmiCecMessageBuilder->BuildRecordOff(mAddress, recorderAddress));
        Slogger::I(TAG, "Stop [One Touch Record]-Target:" + recorderAddress);
    }

    private Boolean CheckRecorder(Int32 recorderAddress) {
        HdmiDeviceInfo device = GetCecDeviceInfo(recorderAddress);
        return (device != NULL)
                && (HdmiUtils->GetTypeFromAddress(recorderAddress)
                        == HdmiDeviceInfo.DEVICE_RECORDER);
    }

    private Boolean CheckRecordSource(Byte[] recordSource) {
        return (recordSource != NULL) && HdmiRecordSources->CheckRecordSource(recordSource);
    }

    @ServiceThreadOnly
    void StartTimerRecording(Int32 recorderAddress, Int32 sourceType, Byte[] recordSource) {
        AssertRunOnServiceThread();
        if (!mService->IsControlEnabled()) {
            Slogger::W(TAG, "Can not start one touch record. CEC control is disabled.");
            AnnounceTimerRecordingResult(TIMER_RECORDING_RESULT_EXTRA_CEC_DISABLED);
            return;
        }

        if (!CheckRecorder(recorderAddress)) {
            Slogger::W(TAG, "Invalid recorder address:" + recorderAddress);
            AnnounceTimerRecordingResult(
                    TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
            return;
        }

        if (!CheckTimerRecordingSource(sourceType, recordSource)) {
            Slogger::W(TAG, "Invalid record source." + Arrays->ToString(recordSource));
            AnnounceTimerRecordingResult(
                    TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE);
            return;
        }

        AddAndStartAction(
                new TimerRecordingAction(this, recorderAddress, sourceType, recordSource));
        Slogger::I(TAG, "Start [Timer Recording]-Target:" + recorderAddress + ", SourceType:"
                + sourceType + ", RecordSource:" + Arrays->ToString(recordSource));
    }

    private Boolean CheckTimerRecordingSource(Int32 sourceType, Byte[] recordSource) {
        return (recordSource != NULL)
                && HdmiTimerRecordSources->CheckTimerRecordSource(sourceType, recordSource);
    }

    @ServiceThreadOnly
    void ClearTimerRecording(Int32 recorderAddress, Int32 sourceType, Byte[] recordSource) {
        AssertRunOnServiceThread();
        if (!mService->IsControlEnabled()) {
            Slogger::W(TAG, "Can not start one touch record. CEC control is disabled.");
            AnnounceClearTimerRecordingResult(CLEAR_TIMER_STATUS_CEC_DISABLE);
            return;
        }

        if (!CheckRecorder(recorderAddress)) {
            Slogger::W(TAG, "Invalid recorder address:" + recorderAddress);
            AnnounceClearTimerRecordingResult(CLEAR_TIMER_STATUS_CHECK_RECORDER_CONNECTION);
            return;
        }

        if (!CheckTimerRecordingSource(sourceType, recordSource)) {
            Slogger::W(TAG, "Invalid record source." + Arrays->ToString(recordSource));
            AnnounceClearTimerRecordingResult(CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE);
            return;
        }

        SendClearTimerMessage(recorderAddress, sourceType, recordSource);
    }

    private void SendClearTimerMessage(Int32 recorderAddress, Int32 sourceType, Byte[] recordSource) {
        HdmiCecMessage message = NULL;
        switch (sourceType) {
            case TIMER_RECORDING_TYPE_DIGITAL:
                message = HdmiCecMessageBuilder->BuildClearDigitalTimer(mAddress, recorderAddress,
                        recordSource);
                break;
            case TIMER_RECORDING_TYPE_ANALOGUE:
                message = HdmiCecMessageBuilder->BuildClearAnalogueTimer(mAddress, recorderAddress,
                        recordSource);
                break;
            case TIMER_RECORDING_TYPE_EXTERNAL:
                message = HdmiCecMessageBuilder->BuildClearExternalTimer(mAddress, recorderAddress,
                        recordSource);
                break;
            default:
                Slogger::W(TAG, "Invalid source type:" + recorderAddress);
                AnnounceClearTimerRecordingResult(CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE);
                return;

        }
        mService->SendCecCommand(message, new SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants.SEND_RESULT_SUCCESS) {
                    AnnounceClearTimerRecordingResult(
                            CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE);
                }
            }
        });
    }

    void UpdateDevicePowerStatus(Int32 logicalAddress, Int32 newPowerStatus) {
        HdmiDeviceInfo info = GetCecDeviceInfo(logicalAddress);
        if (info == NULL) {
            Slogger::W(TAG, "Can not update power status of non-existing device:" + logicalAddress);
            return;
        }

        if (info->GetDevicePowerStatus() == newPowerStatus) {
            return;
        }

        HdmiDeviceInfo newInfo = HdmiUtils->CloneHdmiDeviceInfo(info, newPowerStatus);
        // addDeviceInfo replaces old device info with new one if exists.
        AddDeviceInfo(newInfo);

        InvokeDeviceEventListener(newInfo, HdmiControlManager.DEVICE_EVENT_UPDATE_DEVICE);
    }

    //@Override
    protected Boolean HandleMenuStatus(HdmiCecMessage message) {
        // Do nothing and just return TRUE not to prevent from responding <Feature Abort>.
        return TRUE;
    }

    //@Override
    protected void SendStandby(Int32 deviceId) {
        HdmiDeviceInfo targetDevice = mDeviceInfos->Get(deviceId);
        if (targetDevice == NULL) {
            return;
        }
        Int32 targetAddress = targetDevice->GetLogicalAddress();
        mService->SendCecCommand(HdmiCecMessageBuilder->BuildStandby(mAddress, targetAddress));
    }

    //@Override
    protected void Dump(final IndentingPrintWriter pw) {
        super->Dump(pw);
        pw->Println("mArcEstablished: " + mArcEstablished);
        pw->Println("mArcFeatureEnabled: " + mArcFeatureEnabled);
        pw->Println("mSystemAudioActivated: " + mSystemAudioActivated);
        pw->Println("mSystemAudioMute: " + mSystemAudioMute);
        pw->Println("mAutoDeviceOff: " + mAutoDeviceOff);
        pw->Println("mAutoWakeup: " + mAutoWakeup);
        pw->Println("mSkipRoutingControl: " + mSkipRoutingControl);
        pw->Println("CEC devices:");
        pw->IncreaseIndent();
        for (HdmiDeviceInfo info : mSafeAllDeviceInfos) {
            pw->Println(info);
        }
        pw->DecreaseIndent();
    }
}
