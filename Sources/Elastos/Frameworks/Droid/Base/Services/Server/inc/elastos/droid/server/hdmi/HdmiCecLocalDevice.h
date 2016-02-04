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

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyEvent;

using Elastos::Droid::internal.annotations.GuardedBy;
using Elastos::Droid::internal.util.IndentingPrintWriter;
using Elastos::Droid::Server::Ihdmi.HdmiAnnotations.ServiceThreadOnly;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IList;

/**
 * Class that models a logical CEC device hosted in this system. Handles initialization,
 * CEC commands that call for actions customized per device type.
 */
abstract class HdmiCecLocalDevice {
    private static const String TAG = "HdmiCecLocalDevice";

    private static const Int32 MSG_DISABLE_DEVICE_TIMEOUT = 1;
    private static const Int32 MSG_USER_CONTROL_RELEASE_TIMEOUT = 2;
    // Timeout in millisecond for device clean up (5s).
    // Normal actions timeout is 2s but some of them would have several sequence of timeout.
    private static const Int32 DEVICE_CLEANUP_TIMEOUT = 5000;
    // Within the timer, a received <User Control Pressed> will start "Press and Hold" behavior.
    // When it expires, we can assume <User Control Release> is received.
    private static const Int32 FOLLOWER_SAFETY_TIMEOUT = 550;

    protected final HdmiControlService mService;
    protected final Int32 mDeviceType;
    protected Int32 mAddress;
    protected Int32 mPreferredAddress;
    protected HdmiDeviceInfo mDeviceInfo;
    protected Int32 mLastKeycode = HdmiCecKeycode.UNSUPPORTED_KEYCODE;
    protected Int32 mLastKeyRepeatCount = 0;

    static class ActiveSource {
        Int32 logicalAddress;
        Int32 physicalAddress;

        public ActiveSource() {
            Invalidate();
        }
        public ActiveSource(Int32 logical, Int32 physical) {
            logicalAddress = logical;
            physicalAddress = physical;
        }
        public static ActiveSource Of(Int32 logical, Int32 physical) {
            return new ActiveSource(logical, physical);
        }
        public Boolean IsValid() {
            return HdmiUtils->IsValidAddress(logicalAddress);
        }
        CARAPI Invalidate() {
            logicalAddress = Constants.ADDR_INVALID;
            physicalAddress = Constants.INVALID_PHYSICAL_ADDRESS;
        }
        public Boolean Equals(Int32 logical, Int32 physical) {
            return logicalAddress == logical && physicalAddress == physical;
        }
        //@Override
        public Boolean Equals(Object obj) {
            if (obj instanceof ActiveSource) {
                ActiveSource that = (ActiveSource) obj;
                return that.logicalAddress == logicalAddress &&
                       that.physicalAddress == physicalAddress;
            }
            return FALSE;
        }
        //@Override
        public Int32 HashCode() {
            return logicalAddress * 29 + physicalAddress;
        }
        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            StringBuffer s = new StringBuffer();
            String logicalAddressString = (logicalAddress == Constants.ADDR_INVALID)
                    ? "invalid" : String->Format("0x%02x", logicalAddress);
            s->Append("logical_address: ").Append(logicalAddressString);
            String physicalAddressString = (physicalAddress == Constants.INVALID_PHYSICAL_ADDRESS)
                    ? "invalid" : String->Format("0x%04x", physicalAddress);
            s->Append(", physical_address: ").Append(physicalAddressString);
            return s->ToString();
        }
    }
    // Logical address of the active source.
    @GuardedBy("mLock")
    protected final ActiveSource mActiveSource = new ActiveSource();

    // Active routing path. Physical address of the active source but not all the time, such as
    // when the new active source does not claim itself to be one. Note that we don't keep
    // the active port id (or active input) since it can be gotten by {@link #PathToPortId(Int32)}.
    @GuardedBy("mLock")
    private Int32 mActiveRoutingPath;

    protected final HdmiCecMessageCache mCecMessageCache = new HdmiCecMessageCache();
    protected final Object mLock;

    // A collection of FeatureAction.
    // Note that access to this collection should happen in service thread.
    private final LinkedList<HdmiCecFeatureAction> mActions = new LinkedList<>();

    private final Handler mHandler = new Handler () {
        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case MSG_DISABLE_DEVICE_TIMEOUT:
                    HandleDisableDeviceTimeout();
                    break;
                case MSG_USER_CONTROL_RELEASE_TIMEOUT:
                    HandleUserControlReleased();
                    break;
            }
        }
    };

    /**
     * A callback interface to get notified when all pending action is cleared.
     * It can be called when timeout happened.
     */
    interface PendingActionClearedCallback {
        void OnCleared(HdmiCecLocalDevice device);
    }

    protected PendingActionClearedCallback mPendingActionClearedCallback;

    protected HdmiCecLocalDevice(HdmiControlService service, Int32 deviceType) {
        mService = service;
        mDeviceType = deviceType;
        mAddress = Constants.ADDR_UNREGISTERED;
        mLock = service->GetServiceLock();
    }

    // Factory method that returns HdmiCecLocalDevice of corresponding type.
    static HdmiCecLocalDevice Create(HdmiControlService service, Int32 deviceType) {
        switch (deviceType) {
        case HdmiDeviceInfo.DEVICE_TV:
            return new HdmiCecLocalDeviceTv(service);
        case HdmiDeviceInfo.DEVICE_PLAYBACK:
            return new HdmiCecLocalDevicePlayback(service);
        default:
            return NULL;
        }
    }

    @ServiceThreadOnly
    void Init() {
        AssertRunOnServiceThread();
        mPreferredAddress = GetPreferredAddress();
    }

    /**
     * Called once a logical address of the local device is allocated.
     */
    protected abstract void OnAddressAllocated(Int32 logicalAddress, Int32 reason);

    /**
     * Get the preferred logical address from system properties.
     */
    protected abstract Int32 GetPreferredAddress();

    /**
     * Set the preferred logical address to system properties.
     */
    protected abstract void SetPreferredAddress(Int32 addr);

    /**
     * Dispatch incoming message.
     *
     * @param message incoming message
     * @return TRUE if consumed a message; otherwise, return FALSE.
     */
    @ServiceThreadOnly
    Boolean DispatchMessage(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        Int32 dest = message->GetDestination();
        if (dest != mAddress && dest != Constants.ADDR_BROADCAST) {
            return FALSE;
        }
        // Cache incoming message. Note that it caches only white-listed one.
        mCecMessageCache->CacheMessage(message);
        return OnMessage(message);
    }

    @ServiceThreadOnly
    protected final Boolean OnMessage(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        if (DispatchMessageToAction(message)) {
            return TRUE;
        }
        switch (message->GetOpcode()) {
            case Constants.MESSAGE_ACTIVE_SOURCE:
                return HandleActiveSource(message);
            case Constants.MESSAGE_INACTIVE_SOURCE:
                return HandleInactiveSource(message);
            case Constants.MESSAGE_REQUEST_ACTIVE_SOURCE:
                return HandleRequestActiveSource(message);
            case Constants.MESSAGE_GET_MENU_LANGUAGE:
                return HandleGetMenuLanguage(message);
            case Constants.MESSAGE_GIVE_PHYSICAL_ADDRESS:
                return HandleGivePhysicalAddress();
            case Constants.MESSAGE_GIVE_OSD_NAME:
                return HandleGiveOsdName(message);
            case Constants.MESSAGE_GIVE_DEVICE_VENDOR_ID:
                return HandleGiveDeviceVendorId();
            case Constants.MESSAGE_GET_CEC_VERSION:
                return HandleGetCecVersion(message);
            case Constants.MESSAGE_REPORT_PHYSICAL_ADDRESS:
                return HandleReportPhysicalAddress(message);
            case Constants.MESSAGE_ROUTING_CHANGE:
                return HandleRoutingChange(message);
            case Constants.MESSAGE_ROUTING_INFORMATION:
                return HandleRoutingInformation(message);
            case Constants.MESSAGE_INITIATE_ARC:
                return HandleInitiateArc(message);
            case Constants.MESSAGE_TERMINATE_ARC:
                return HandleTerminateArc(message);
            case Constants.MESSAGE_SET_SYSTEM_AUDIO_MODE:
                return HandleSetSystemAudioMode(message);
            case Constants.MESSAGE_SYSTEM_AUDIO_MODE_STATUS:
                return HandleSystemAudioModeStatus(message);
            case Constants.MESSAGE_REPORT_AUDIO_STATUS:
                return HandleReportAudioStatus(message);
            case Constants.MESSAGE_STANDBY:
                return HandleStandby(message);
            case Constants.MESSAGE_TEXT_VIEW_ON:
                return HandleTextViewOn(message);
            case Constants.MESSAGE_IMAGE_VIEW_ON:
                return HandleImageViewOn(message);
            case Constants.MESSAGE_USER_CONTROL_PRESSED:
                return HandleUserControlPressed(message);
            case Constants.MESSAGE_USER_CONTROL_RELEASED:
                return HandleUserControlReleased();
            case Constants.MESSAGE_SET_STREAM_PATH:
                return HandleSetStreamPath(message);
            case Constants.MESSAGE_GIVE_DEVICE_POWER_STATUS:
                return HandleGiveDevicePowerStatus(message);
            case Constants.MESSAGE_MENU_REQUEST:
                return HandleMenuRequest(message);
            case Constants.MESSAGE_MENU_STATUS:
                return HandleMenuStatus(message);
            case Constants.MESSAGE_VENDOR_COMMAND:
                return HandleVendorCommand(message);
            case Constants.MESSAGE_VENDOR_COMMAND_WITH_ID:
                return HandleVendorCommandWithId(message);
            case Constants.MESSAGE_SET_OSD_NAME:
                return HandleSetOsdName(message);
            case Constants.MESSAGE_RECORD_TV_SCREEN:
                return HandleRecordTvScreen(message);
            case Constants.MESSAGE_TIMER_CLEARED_STATUS:
                return HandleTimerClearedStatus(message);
            case Constants.MESSAGE_REPORT_POWER_STATUS:
                return HandleReportPowerStatus(message);
            case Constants.MESSAGE_TIMER_STATUS:
                return HandleTimerStatus(message);
            case Constants.MESSAGE_RECORD_STATUS:
                return HandleRecordStatus(message);
            default:
                return FALSE;
        }
    }

    @ServiceThreadOnly
    private Boolean DispatchMessageToAction(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        for (HdmiCecFeatureAction action : mActions) {
            if (action->ProcessCommand(message)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    @ServiceThreadOnly
    protected Boolean HandleGivePhysicalAddress() {
        AssertRunOnServiceThread();

        Int32 physicalAddress = mService->GetPhysicalAddress();
        HdmiCecMessage cecMessage = HdmiCecMessageBuilder->BuildReportPhysicalAddressCommand(
                mAddress, physicalAddress, mDeviceType);
        mService->SendCecCommand(cecMessage);
        return TRUE;
    }

    @ServiceThreadOnly
    protected Boolean HandleGiveDeviceVendorId() {
        AssertRunOnServiceThread();
        Int32 vendorId = mService->GetVendorId();
        HdmiCecMessage cecMessage = HdmiCecMessageBuilder->BuildDeviceVendorIdCommand(
                mAddress, vendorId);
        mService->SendCecCommand(cecMessage);
        return TRUE;
    }

    @ServiceThreadOnly
    protected Boolean HandleGetCecVersion(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        Int32 version = mService->GetCecVersion();
        HdmiCecMessage cecMessage = HdmiCecMessageBuilder->BuildCecVersion(message->GetDestination(),
                message->GetSource(), version);
        mService->SendCecCommand(cecMessage);
        return TRUE;
    }

    @ServiceThreadOnly
    protected Boolean HandleActiveSource(HdmiCecMessage message) {
        return FALSE;
    }

    @ServiceThreadOnly
    protected Boolean HandleInactiveSource(HdmiCecMessage message) {
        return FALSE;
    }

    @ServiceThreadOnly
    protected Boolean HandleRequestActiveSource(HdmiCecMessage message) {
        return FALSE;
    }

    @ServiceThreadOnly
    protected Boolean HandleGetMenuLanguage(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        Slogger::W(TAG, "Only TV can handle <Get Menu Language>:" + message->ToString());
        // 'return FALSE' will cause to reply with <Feature Abort>.
        return FALSE;
    }

    @ServiceThreadOnly
    protected Boolean HandleGiveOsdName(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        // Note that since this method is called after logical address allocation is done,
        // mDeviceInfo should not be NULL.
        HdmiCecMessage cecMessage = HdmiCecMessageBuilder->BuildSetOsdNameCommand(
                mAddress, message->GetSource(), mDeviceInfo->GetDisplayName());
        if (cecMessage != NULL) {
            mService->SendCecCommand(cecMessage);
        } else {
            Slogger::W(TAG, "Failed to build <Get Osd Name>:" + mDeviceInfo->GetDisplayName());
        }
        return TRUE;
    }

    protected Boolean HandleRoutingChange(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleRoutingInformation(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleReportPhysicalAddress(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleSystemAudioModeStatus(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleSetSystemAudioMode(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleTerminateArc(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleInitiateArc(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleReportAudioStatus(HdmiCecMessage message) {
        return FALSE;
    }

    @ServiceThreadOnly
    protected Boolean HandleStandby(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        // Seq #12
        if (mService->IsControlEnabled() && !mService->IsProhibitMode()
                && mService->IsPowerOnOrTransient()) {
            mService->Standby();
            return TRUE;
        }
        return FALSE;
    }

    @ServiceThreadOnly
    protected Boolean HandleUserControlPressed(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        mHandler->RemoveMessages(MSG_USER_CONTROL_RELEASE_TIMEOUT);
        if (mService->IsPowerOnOrTransient() && IsPowerOffOrToggleCommand(message)) {
            mService->Standby();
            return TRUE;
        } else if (mService->IsPowerStandbyOrTransient() && IsPowerOnOrToggleCommand(message)) {
            mService->WakeUp();
            return TRUE;
        }

        final Int64 downTime = SystemClock->UptimeMillis();
        final Byte[] params = message->GetParams();
        // Note that we don't support parameterized keycode now.
        // TODO: translate parameterized keycode as well.
        final Int32 keycode = HdmiCecKeycode->CecKeyToAndroidKey(params[0]);
        Int32 keyRepeatCount = 0;
        if (mLastKeycode != HdmiCecKeycode.UNSUPPORTED_KEYCODE) {
            if (keycode == mLastKeycode) {
                keyRepeatCount = mLastKeyRepeatCount + 1;
            } else {
                InjectKeyEvent(downTime, KeyEvent.ACTION_UP, mLastKeycode, 0);
            }
        }
        mLastKeycode = keycode;
        mLastKeyRepeatCount = keyRepeatCount;

        if (keycode != HdmiCecKeycode.UNSUPPORTED_KEYCODE) {
            InjectKeyEvent(downTime, KeyEvent.ACTION_DOWN, keycode, keyRepeatCount);
            mHandler->SendMessageDelayed(Message->Obtain(mHandler, MSG_USER_CONTROL_RELEASE_TIMEOUT),
                    FOLLOWER_SAFETY_TIMEOUT);
            return TRUE;
        }
        return FALSE;
    }

    @ServiceThreadOnly
    protected Boolean HandleUserControlReleased() {
        AssertRunOnServiceThread();
        mHandler->RemoveMessages(MSG_USER_CONTROL_RELEASE_TIMEOUT);
        mLastKeyRepeatCount = 0;
        if (mLastKeycode != HdmiCecKeycode.UNSUPPORTED_KEYCODE) {
            final Int64 upTime = SystemClock->UptimeMillis();
            InjectKeyEvent(upTime, KeyEvent.ACTION_UP, mLastKeycode, 0);
            mLastKeycode = HdmiCecKeycode.UNSUPPORTED_KEYCODE;
            return TRUE;
        }
        return FALSE;
    }

    static void InjectKeyEvent(Int64 time, Int32 action, Int32 keycode, Int32 repeat) {
        KeyEvent keyEvent = KeyEvent->Obtain(time, time, action, keycode,
                repeat, 0, KeyCharacterMap.VIRTUAL_KEYBOARD, 0, KeyEvent.FLAG_FROM_SYSTEM,
                InputDevice.SOURCE_HDMI, NULL);
        InputManager->GetInstance()->InjectInputEvent(keyEvent,
                InputManager.INJECT_INPUT_EVENT_MODE_ASYNC);
        keyEvent->Recycle();
   }

    static Boolean IsPowerOnOrToggleCommand(HdmiCecMessage message) {
        Byte[] params = message->GetParams();
        return message->GetOpcode() == Constants.MESSAGE_USER_CONTROL_PRESSED
                && (params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER
                        || params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER_ON_FUNCTION
                        || params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER_TOGGLE_FUNCTION);
    }

    static Boolean IsPowerOffOrToggleCommand(HdmiCecMessage message) {
        Byte[] params = message->GetParams();
        return message->GetOpcode() == Constants.MESSAGE_USER_CONTROL_PRESSED
                && (params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER
                        || params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER_OFF_FUNCTION
                        || params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER_TOGGLE_FUNCTION);
    }

    protected Boolean HandleTextViewOn(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleImageViewOn(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleSetStreamPath(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleGiveDevicePowerStatus(HdmiCecMessage message) {
        mService->SendCecCommand(HdmiCecMessageBuilder->BuildReportPowerStatus(
                mAddress, message->GetSource(), mService->GetPowerStatus()));
        return TRUE;
    }

    protected Boolean HandleMenuRequest(HdmiCecMessage message) {
        // Always report menu active to receive Remote Control.
        mService->SendCecCommand(HdmiCecMessageBuilder->BuildReportMenuStatus(
                mAddress, message->GetSource(), Constants.MENU_STATE_ACTIVATED));
        return TRUE;
    }

    protected Boolean HandleMenuStatus(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleVendorCommand(HdmiCecMessage message) {
        if (!mService->InvokeVendorCommandListeners(mDeviceType, message->GetSource(),
                message->GetParams(), FALSE)) {
            // Vendor command listener may not have been registered yet. Respond with
            // <Feature Abort> [NOT_IN_CORRECT_MODE] so that the sender can try again later.
            mService->MaySendFeatureAbortCommand(message, Constants.ABORT_NOT_IN_CORRECT_MODE);
        }
        return TRUE;
    }

    protected Boolean HandleVendorCommandWithId(HdmiCecMessage message) {
        Byte[] params = message->GetParams();
        Int32 vendorId = HdmiUtils->ThreeBytesToInt(params);
        if (vendorId == mService->GetVendorId()) {
            if (!mService->InvokeVendorCommandListeners(mDeviceType, message->GetSource(), params,
                    TRUE)) {
                mService->MaySendFeatureAbortCommand(message, Constants.ABORT_NOT_IN_CORRECT_MODE);
            }
        } else if (message->GetDestination() != Constants.ADDR_BROADCAST &&
                message->GetSource() != Constants.ADDR_UNREGISTERED) {
            Slogger::V(TAG, "Wrong direct vendor command. Replying with <Feature Abort>");
            mService->MaySendFeatureAbortCommand(message, Constants.ABORT_UNRECOGNIZED_OPCODE);
        } else {
            Slogger::V(TAG, "Wrong broadcast vendor command. Ignoring");
        }
        return TRUE;
    }

    protected void SendStandby(Int32 deviceId) {
        // Do nothing.
    }

    protected Boolean HandleSetOsdName(HdmiCecMessage message) {
        // The default behavior of <Set Osd Name> is doing nothing.
        return TRUE;
    }

    protected Boolean HandleRecordTvScreen(HdmiCecMessage message) {
        // The default behavior of <Record TV Screen> is replying <Feature Abort> with
        // "Cannot provide source".
        mService->MaySendFeatureAbortCommand(message, Constants.ABORT_CANNOT_PROVIDE_SOURCE);
        return TRUE;
    }

    protected Boolean HandleTimerClearedStatus(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleReportPowerStatus(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleTimerStatus(HdmiCecMessage message) {
        return FALSE;
    }

    protected Boolean HandleRecordStatus(HdmiCecMessage message) {
        return FALSE;
    }

    @ServiceThreadOnly
    final void HandleAddressAllocated(Int32 logicalAddress, Int32 reason) {
        AssertRunOnServiceThread();
        mAddress = mPreferredAddress = logicalAddress;
        OnAddressAllocated(logicalAddress, reason);
        SetPreferredAddress(logicalAddress);
    }

    Int32 GetType() {
        return mDeviceType;
    }

    @ServiceThreadOnly
    HdmiDeviceInfo GetDeviceInfo() {
        AssertRunOnServiceThread();
        return mDeviceInfo;
    }

    @ServiceThreadOnly
    void SetDeviceInfo(HdmiDeviceInfo info) {
        AssertRunOnServiceThread();
        mDeviceInfo = info;
    }

    // Returns TRUE if the logical address is same as the argument.
    @ServiceThreadOnly
    Boolean IsAddressOf(Int32 addr) {
        AssertRunOnServiceThread();
        return addr == mAddress;
    }

    // Resets the logical address to Unregistered(15), meaning the logical device is invalid.
    @ServiceThreadOnly
    void ClearAddress() {
        AssertRunOnServiceThread();
        mAddress = Constants.ADDR_UNREGISTERED;
    }

    @ServiceThreadOnly
    void AddAndStartAction(final HdmiCecFeatureAction action) {
        AssertRunOnServiceThread();
        mActions->Add(action);
        if (mService->IsPowerStandbyOrTransient()) {
            Slogger::I(TAG, "Not ready to start action. Queued for deferred start:" + action);
            return;
        }
        action->Start();
    }

    @ServiceThreadOnly
    void StartQueuedActions() {
        AssertRunOnServiceThread();
        for (HdmiCecFeatureAction action : mActions) {
            if (!action->Started()) {
                Slogger::I(TAG, "Starting queued action:" + action);
                action->Start();
            }
        }
    }

    // See if we have an action of a given type in progress.
    @ServiceThreadOnly
    <T extends HdmiCecFeatureAction> Boolean HasAction(final Class<T> clazz) {
        AssertRunOnServiceThread();
        for (HdmiCecFeatureAction action : mActions) {
            if (action->GetClass()->Equals(clazz)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    // Returns all actions matched with given class type.
    @ServiceThreadOnly
    <T extends HdmiCecFeatureAction> List<T> GetActions(final Class<T> clazz) {
        AssertRunOnServiceThread();
        List<T> actions = Collections.<T>EmptyList();
        for (HdmiCecFeatureAction action : mActions) {
            if (action->GetClass()->Equals(clazz)) {
                if (actions->IsEmpty()) {
                    actions = new ArrayList<T>();
                }
                actions->Add((T) action);
            }
        }
        return actions;
    }

    /**
     * Remove the given {@link HdmiCecFeatureAction} object from the action queue.
     *
     * @param action {@link HdmiCecFeatureAction} to remove
     */
    @ServiceThreadOnly
    void RemoveAction(final HdmiCecFeatureAction action) {
        AssertRunOnServiceThread();
        action->Finish(FALSE);
        mActions->Remove(action);
        CheckIfPendingActionsCleared();
    }

    // Remove all actions matched with the given Class type.
    @ServiceThreadOnly
    <T extends HdmiCecFeatureAction> void RemoveAction(final Class<T> clazz) {
        AssertRunOnServiceThread();
        RemoveActionExcept(clazz, NULL);
    }

    // Remove all actions matched with the given Class type besides |exception|.
    @ServiceThreadOnly
    <T extends HdmiCecFeatureAction> void RemoveActionExcept(final Class<T> clazz,
            final HdmiCecFeatureAction exception) {
        AssertRunOnServiceThread();
        Iterator<HdmiCecFeatureAction> iter = mActions->Iterator();
        while (iter->HasNext()) {
            HdmiCecFeatureAction action = iter->Next();
            if (action != exception && action->GetClass()->Equals(clazz)) {
                action->Finish(FALSE);
                iter->Remove();
            }
        }
        CheckIfPendingActionsCleared();
    }

    protected void CheckIfPendingActionsCleared() {
        if (mActions->IsEmpty() && mPendingActionClearedCallback != NULL) {
            PendingActionClearedCallback callback = mPendingActionClearedCallback;
            // To prevent from calling the callback again during handling the callback itself.
            mPendingActionClearedCallback = NULL;
            callback->OnCleared(this);
        }
    }

    protected void AssertRunOnServiceThread() {
        if (Looper->MyLooper() != mService->GetServiceLooper()) {
            throw new IllegalStateException("Should run on service thread.");
        }
    }

    /**
     * Called when a hot-plug event issued.
     *
     * @param portId id of port where a hot-plug event happened
     * @param connected whether to connected or not on the event
     */
    void OnHotplug(Int32 portId, Boolean connected) {
    }

    final HdmiControlService GetService() {
        return mService;
    }

    @ServiceThreadOnly
    final Boolean IsConnectedToArcPort(Int32 path) {
        AssertRunOnServiceThread();
        return mService->IsConnectedToArcPort(path);
    }

    ActiveSource GetActiveSource() {
        synchronized(mLock) {
            return mActiveSource;
        }
    }

    void SetActiveSource(ActiveSource newActive) {
        SetActiveSource(newActive.logicalAddress, newActive.physicalAddress);
    }

    void SetActiveSource(HdmiDeviceInfo info) {
        SetActiveSource(info->GetLogicalAddress(), info->GetPhysicalAddress());
    }

    void SetActiveSource(Int32 logicalAddress, Int32 physicalAddress) {
        synchronized(mLock) {
            mActiveSource.logicalAddress = logicalAddress;
            mActiveSource.physicalAddress = physicalAddress;
        }
        mService->SetLastInputForMhl(Constants.INVALID_PORT_ID);
    }

    Int32 GetActivePath() {
        synchronized(mLock) {
            return mActiveRoutingPath;
        }
    }

    void SetActivePath(Int32 path) {
        synchronized(mLock) {
            mActiveRoutingPath = path;
        }
        mService->SetActivePortId(PathToPortId(path));
    }

    /**
     * Returns the ID of the active HDMI port. The active port is the one that has the active
     * routing path connected to it directly or indirectly under the device hierarchy.
     */
    Int32 GetActivePortId() {
        synchronized(mLock) {
            return mService->PathToPortId(mActiveRoutingPath);
        }
    }

    /**
     * Update the active port.
     *
     * @param portId the new active port id
     */
    void SetActivePortId(Int32 portId) {
        // We update active routing path instead, since we get the active port id from
        // the active routing path.
        SetActivePath(mService->PortIdToPath(portId));
    }

    @ServiceThreadOnly
    HdmiCecMessageCache GetCecMessageCache() {
        AssertRunOnServiceThread();
        return mCecMessageCache;
    }

    @ServiceThreadOnly
    Int32 PathToPortId(Int32 newPath) {
        AssertRunOnServiceThread();
        return mService->PathToPortId(newPath);
    }

    /**
     * Called when the system goes to standby mode.
     *
     * @param initiatedByCec TRUE if this power sequence is initiated
     *        by the reception the CEC messages like &lt;Standby&gt;
     */
    protected void OnStandby(Boolean initiatedByCec) {}

    /**
     * Disable device. {@code callback} is used to get notified when all pending
     * actions are completed or timeout is issued.
     *
     * @param initiatedByCec TRUE if this sequence is initiated
     *        by the reception the CEC messages like &lt;Standby&gt;
     * @param origialCallback callback interface to get notified when all pending actions are
     *        cleared
     */
    protected void DisableDevice(Boolean initiatedByCec,
            final PendingActionClearedCallback origialCallback) {
        mPendingActionClearedCallback = new PendingActionClearedCallback() {
            //@Override
            CARAPI OnCleared(HdmiCecLocalDevice device) {
                mHandler->RemoveMessages(MSG_DISABLE_DEVICE_TIMEOUT);
                origialCallback->OnCleared(device);
            }
        };
        mHandler->SendMessageDelayed(Message->Obtain(mHandler, MSG_DISABLE_DEVICE_TIMEOUT),
                DEVICE_CLEANUP_TIMEOUT);
    }

    @ServiceThreadOnly
    private void HandleDisableDeviceTimeout() {
        AssertRunOnServiceThread();

        // If all actions are not cleared in DEVICE_CLEANUP_TIMEOUT, enforce to finish them.
        // onCleard will be called at the last action's finish method.
        Iterator<HdmiCecFeatureAction> iter = mActions->Iterator();
        while (iter->HasNext()) {
            HdmiCecFeatureAction action = iter->Next();
            action->Finish(FALSE);
            iter->Remove();
        }
    }

    /**
     * Send a key event to other device.
     *
     * @param keyCode key code defined in {@link android.view.KeyEvent}
     * @param isPressed {@code TRUE} for key down event
     */
    protected void SendKeyEvent(Int32 keyCode, Boolean isPressed) {
        Slogger::W(TAG, "sendKeyEvent not implemented");
    }

    void SendUserControlPressedAndReleased(Int32 targetAddress, Int32 cecKeycode) {
        mService->SendCecCommand(HdmiCecMessageBuilder->BuildUserControlPressed(
                mAddress, targetAddress, cecKeycode));
        mService->SendCecCommand(HdmiCecMessageBuilder->BuildUserControlReleased(
                mAddress, targetAddress));
    }

    /**
     * Dump internal status of HdmiCecLocalDevice object.
     */
    protected void Dump(final IndentingPrintWriter pw) {
        pw->Println("mDeviceType: " + mDeviceType);
        pw->Println("mAddress: " + mAddress);
        pw->Println("mPreferredAddress: " + mPreferredAddress);
        pw->Println("mDeviceInfo: " + mDeviceInfo);
        pw->Println("mActiveSource: " + mActiveSource);
        pw->Println(String->Format("mActiveRoutingPath: 0x%04x", mActiveRoutingPath));
    }
}
