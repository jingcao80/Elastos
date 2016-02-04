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

import static android.hardware.hdmi.HdmiControlManager.DEVICE_EVENT_ADD_DEVICE;
import static android.hardware.hdmi.HdmiControlManager.DEVICE_EVENT_REMOVE_DEVICE;
import static com.android.server.hdmi.Constants.DISABLED;
import static com.android.server.hdmi.Constants.ENABLED;
import static com.android.server.hdmi.Constants.OPTION_CEC_AUTO_WAKEUP;
import static com.android.server.hdmi.Constants.OPTION_CEC_ENABLE;
import static com.android.server.hdmi.Constants.OPTION_CEC_SERVICE_CONTROL;
import static com.android.server.hdmi.Constants.OPTION_MHL_ENABLE;
import static com.android.server.hdmi.Constants.OPTION_MHL_INPUT_SWITCHING;
import static com.android.server.hdmi.Constants.OPTION_MHL_POWER_CHARGE;

using Elastos::Droid::annotation.Nullable;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
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
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings::Global::
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ISparseIntArray;

using Elastos::Droid::internal.annotations.GuardedBy;
using Elastos::Droid::internal.util.IndentingPrintWriter;
using Elastos::Droid::Server::ISystemService;
using Elastos::Droid::Server::Ihdmi.HdmiAnnotations.ServiceThreadOnly;
using Elastos::Droid::Server::Ihdmi.HdmiCecController.AllocateAddressCallback;
using Elastos::Droid::Server::Ihdmi.HdmiCecLocalDevice.ActiveSource;
using Elastos::Droid::Server::Ihdmi.HdmiCecLocalDevice.PendingActionClearedCallback;

import libcore.util.EmptyArray;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;

/**
 * Provides a service for sending and processing HDMI control messages,
 * HDMI-CEC and MHL control command, and providing the information on both standard.
 */
public class HdmiControlService extends SystemService {
    private static const String TAG = "HdmiControlService";

    static const String PERMISSION = "android.permission.HDMI_CEC";

    // The reason code to initiate IntializeCec().
    static const Int32 INITIATED_BY_ENABLE_CEC = 0;
    static const Int32 INITIATED_BY_BOOT_UP = 1;
    static const Int32 INITIATED_BY_SCREEN_ON = 2;
    static const Int32 INITIATED_BY_WAKE_UP_MESSAGE = 3;
    static const Int32 INITIATED_BY_HOTPLUG = 4;

    /**
     * Interface to report send result.
     */
    interface SendMessageCallback {
        /**
         * Called when {@link HdmiControlService#sendCecCommand} is completed.
         *
         * @param error result of send request.
         * <ul>
         * <li>{@link Constants#SEND_RESULT_SUCCESS}
         * <li>{@link Constants#SEND_RESULT_NAK}
         * <li>{@link Constants#SEND_RESULT_FAILURE}
         * </ul>
         */
        void OnSendCompleted(Int32 error);
    }

    /**
     * Interface to get a list of available logical devices.
     */
    interface DevicePollingCallback {
        /**
         * Called when device polling is finished.
         *
         * @param ackedAddress a list of logical addresses of available devices
         */
        void OnPollingFinished(List<Integer> ackedAddress);
    }

    private class HdmiControlBroadcastReceiver extends BroadcastReceiver {
        @ServiceThreadOnly
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            AssertRunOnServiceThread();
            switch (intent->GetAction()) {
                case IIntent::ACTION_SCREEN_OFF:
                    if (IsPowerOnOrTransient()) {
                        OnStandby();
                    }
                    break;
                case IIntent::ACTION_SCREEN_ON:
                    if (IsPowerStandbyOrTransient()) {
                        OnWakeUp();
                    }
                    break;
                case IIntent::ACTION_CONFIGURATION_CHANGED:
                    String language = Locale->GetDefault()->GetISO3Language();
                    if (!mLanguage->Equals(language)) {
                        OnLanguageChanged(language);
                    }
                    break;
            }
        }
    }

    // A thread to handle synchronous IO of CEC and MHL control service.
    // Since all of CEC and MHL HAL interfaces processed in short time (< 200ms)
    // and sparse call it shares a thread to handle IO operations.
    private final HandlerThread mIoThread = new HandlerThread("Hdmi Control Io Thread");

    // Used to synchronize the access to the service.
    private final Object mLock = new Object();

    // Type of logical devices hosted in the system. Stored in the unmodifiable list.
    private final List<Integer> mLocalDevices;

    // List of records for hotplug event listener to handle the the caller killed in action.
    @GuardedBy("mLock")
    private final ArrayList<HotplugEventListenerRecord> mHotplugEventListenerRecords =
            new ArrayList<>();

    // List of records for device event listener to handle the caller killed in action.
    @GuardedBy("mLock")
    private final ArrayList<DeviceEventListenerRecord> mDeviceEventListenerRecords =
            new ArrayList<>();

    // List of records for vendor command listener to handle the caller killed in action.
    @GuardedBy("mLock")
    private final ArrayList<VendorCommandListenerRecord> mVendorCommandListenerRecords =
            new ArrayList<>();

    @GuardedBy("mLock")
    private InputChangeListenerRecord mInputChangeListenerRecord;

    @GuardedBy("mLock")
    private HdmiRecordListenerRecord mRecordListenerRecord;

    // Set to TRUE while HDMI control is enabled. If set to FALSE, HDMI-CEC/MHL protocol
    // handling will be disabled and no request will be handled.
    @GuardedBy("mLock")
    private Boolean mHdmiControlEnabled;

    // Set to TRUE while the service is in normal mode. While set to FALSE, no input change is
    // allowed. Used for situations where input change can confuse users such as channel auto-scan,
    // system upgrade, etc., a.k.a. "prohibit mode".
    @GuardedBy("mLock")
    private Boolean mProhibitMode;

    // List of records for system audio mode change to handle the the caller killed in action.
    private final ArrayList<SystemAudioModeChangeListenerRecord>
            mSystemAudioModeChangeListenerRecords = new ArrayList<>();

    // Handler used to run a task in service thread.
    private final Handler mHandler = new Handler();

    private final SettingsObserver mSettingsObserver;

    private final HdmiControlBroadcastReceiver
            mHdmiControlBroadcastReceiver = new HdmiControlBroadcastReceiver();

    @Nullable
    private HdmiCecController mCecController;

    // HDMI port information. Stored in the unmodifiable list to keep the static information
    // from being modified.
    private List<HdmiPortInfo> mPortInfo;

    // Map from Path(physical address) to port ID.
    private UnmodifiableSparseIntArray mPortIdMap;

    // Map from port ID to HdmiPortInfo.
    private UnmodifiableSparseArray<HdmiPortInfo> mPortInfoMap;

    // Map from port ID to HdmiDeviceInfo.
    private UnmodifiableSparseArray<HdmiDeviceInfo> mPortDeviceMap;

    private HdmiCecMessageValidator mMessageValidator;

    @ServiceThreadOnly
    private Int32 mPowerStatus = HdmiControlManager.POWER_STATUS_STANDBY;

    @ServiceThreadOnly
    private String mLanguage = Locale->GetDefault()->GetISO3Language();

    @ServiceThreadOnly
    private Boolean mStandbyMessageReceived = FALSE;

    @ServiceThreadOnly
    private Boolean mWakeUpMessageReceived = FALSE;

    @ServiceThreadOnly
    private Int32 mActivePortId = Constants.INVALID_PORT_ID;

    // Set to TRUE while the input change by MHL is allowed.
    @GuardedBy("mLock")
    private Boolean mMhlInputChangeEnabled;

    // List of records for MHL Vendor command listener to handle the caller killed in action.
    @GuardedBy("mLock")
    private final ArrayList<HdmiMhlVendorCommandListenerRecord>
            mMhlVendorCommandListenerRecords = new ArrayList<>();

    @GuardedBy("mLock")
    private List<HdmiDeviceInfo> mMhlDevices;

    @Nullable
    private HdmiMhlControllerStub mMhlController;

    // Last input port before switching to the MHL port. Should switch back to this port
    // when the mobile device sends the request one touch play with off.
    // Gets invalidated if we go to other port/input.
    @ServiceThreadOnly
    private Int32 mLastInputMhl = Constants.INVALID_PORT_ID;

    public HdmiControlService(Context context) {
        Super(context);
        mLocalDevices = GetIntList(SystemProperties->Get(Constants.PROPERTY_DEVICE_TYPE));
        mSettingsObserver = new SettingsObserver(mHandler);
    }

    private static List<Integer> GetIntList(String string) {
        ArrayList<Integer> list = new ArrayList<>();
        TextUtils.SimpleStringSplitter splitter = new TextUtils->SimpleStringSplitter(',');
        splitter->SetString(string);
        for (String item : splitter) {
            try {
                list->Add(Integer->ParseInt(item));
            } catch (NumberFormatException e) {
                Slogger::W(TAG, "Can't parseInt: " + item);
            }
        }
        return Collections->UnmodifiableList(list);
    }

    //@Override
    CARAPI OnStart() {
        mIoThread->Start();
        mPowerStatus = HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON;
        mProhibitMode = FALSE;
        mHdmiControlEnabled = ReadBooleanSetting(Global.HDMI_CONTROL_ENABLED, TRUE);
        mMhlInputChangeEnabled = ReadBooleanSetting(Global.MHL_INPUT_SWITCHING_ENABLED, TRUE);

        mCecController = HdmiCecController->Create(this);
        if (mCecController != NULL) {
            // TODO: Remove this as soon as OEM's HAL implementation is corrected.
            mCecController->SetOption(OPTION_CEC_ENABLE, ENABLED);

            // TODO: load value for mHdmiControlEnabled from preference.
            if (mHdmiControlEnabled) {
                InitializeCec(INITIATED_BY_BOOT_UP);
            }
        } else {
            Slogger::I(TAG, "Device does not support HDMI-CEC.");
        }

        mMhlController = HdmiMhlControllerStub->Create(this);
        if (!mMhlController->IsReady()) {
            Slogger::I(TAG, "Device does not support MHL-control.");
        }
        mMhlDevices = Collections->EmptyList();

        InitPortInfo();
        mMessageValidator = new HdmiCecMessageValidator(this);
        PublishBinderService(Context.HDMI_CONTROL_SERVICE, new BinderService());

        // Register broadcast receiver for power state change.
        if (mCecController != NULL) {
            IntentFilter filter = new IntentFilter();
            filter->AddAction(IIntent::ACTION_SCREEN_OFF);
            filter->AddAction(IIntent::ACTION_SCREEN_ON);
            filter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);
            GetContext()->RegisterReceiver(mHdmiControlBroadcastReceiver, filter);
        }
    }

    /**
     * Called when the initialization of local devices is complete.
     */
    private void OnInitializeCecComplete() {
        if (mPowerStatus == HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON) {
            mPowerStatus = HdmiControlManager.POWER_STATUS_ON;
        }
        mWakeUpMessageReceived = FALSE;

        if (IsTvDevice()) {
            mCecController->SetOption(OPTION_CEC_AUTO_WAKEUP, ToInt(Tv()->GetAutoWakeup()));
            RegisterContentObserver();
        }
    }

    private void RegisterContentObserver() {
        ContentResolver resolver = GetContext()->GetContentResolver();
        String[] settings = new String[] {
                Global.HDMI_CONTROL_ENABLED,
                Global.HDMI_CONTROL_AUTO_WAKEUP_ENABLED,
                Global.HDMI_CONTROL_AUTO_DEVICE_OFF_ENABLED,
                Global.MHL_INPUT_SWITCHING_ENABLED,
                Global.MHL_POWER_CHARGE_ENABLED
        };
        for (String s : settings) {
            resolver->RegisterContentObserver(Global->GetUriFor(s), FALSE, mSettingsObserver,
                    UserHandle.USER_ALL);
        }
    }

    private class SettingsObserver extends ContentObserver {
        public SettingsObserver(Handler handler) {
            Super(handler);
        }

        // onChange is set up to run in service thread.
        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            String option = uri->GetLastPathSegment();
            Boolean enabled = ReadBooleanSetting(option, TRUE);
            switch (option) {
                case Global.HDMI_CONTROL_ENABLED:
                    SetControlEnabled(enabled);
                    break;
                case Global.HDMI_CONTROL_AUTO_WAKEUP_ENABLED:
                    Tv()->SetAutoWakeup(enabled);
                    SetCecOption(OPTION_CEC_AUTO_WAKEUP, ToInt(enabled));
                    break;
                case Global.HDMI_CONTROL_AUTO_DEVICE_OFF_ENABLED:
                    Tv()->SetAutoDeviceOff(enabled);
                    // No need to propagate to HAL.
                    break;
                case Global.MHL_INPUT_SWITCHING_ENABLED:
                    SetMhlInputChangeEnabled(enabled);
                    break;
                case Global.MHL_POWER_CHARGE_ENABLED:
                    mMhlController->SetOption(OPTION_MHL_POWER_CHARGE, ToInt(enabled));
                    break;
            }
        }
    }

    private static Int32 ToInt(Boolean enabled) {
        return enabled ? ENABLED : DISABLED;
    }

    Boolean ReadBooleanSetting(String key, Boolean defVal) {
        ContentResolver cr = GetContext()->GetContentResolver();
        return Global->GetInt(cr, key, ToInt(defVal)) == ENABLED;
    }

    void WriteBooleanSetting(String key, Boolean value) {
        ContentResolver cr = GetContext()->GetContentResolver();
        Global->PutInt(cr, key, ToInt(value));
    }

    private void UnregisterSettingsObserver() {
        GetContext()->GetContentResolver().UnregisterContentObserver(mSettingsObserver);
    }

    private void InitializeCec(Int32 initiatedBy) {
        mCecController->SetOption(OPTION_CEC_SERVICE_CONTROL, ENABLED);
        InitializeLocalDevices(initiatedBy);
    }

    @ServiceThreadOnly
    private void InitializeLocalDevices(final Int32 initiatedBy) {
        AssertRunOnServiceThread();
        // A container for [Device type, Local device info].
        ArrayList<HdmiCecLocalDevice> localDevices = new ArrayList<>();
        for (Int32 type : mLocalDevices) {
            HdmiCecLocalDevice localDevice = mCecController->GetLocalDevice(type);
            if (localDevice == NULL) {
                localDevice = HdmiCecLocalDevice->Create(this, type);
            }
            localDevice->Init();
            localDevices->Add(localDevice);
        }
        // It's now safe to flush existing local devices from mCecController since they were
        // already moved to 'localDevices'.
        ClearLocalDevices();
        AllocateLogicalAddress(localDevices, initiatedBy);
    }

    @ServiceThreadOnly
    private void AllocateLogicalAddress(final ArrayList<HdmiCecLocalDevice> allocatingDevices,
            final Int32 initiatedBy) {
        AssertRunOnServiceThread();
        mCecController->ClearLogicalAddress();
        final ArrayList<HdmiCecLocalDevice> allocatedDevices = new ArrayList<>();
        final Int32[] finished = new Int32[1];
        for (final HdmiCecLocalDevice localDevice : allocatingDevices) {
            mCecController->AllocateLogicalAddress(localDevice->GetType(),
                    localDevice->GetPreferredAddress(), new AllocateAddressCallback() {
                //@Override
                CARAPI OnAllocated(Int32 deviceType, Int32 logicalAddress) {
                    if (logicalAddress == Constants.ADDR_UNREGISTERED) {
                        Slogger::E(TAG, "Failed to allocate address:[device_type:" + deviceType + "]");
                    } else {
                        // Set POWER_STATUS_ON to all local devices because they share lifetime
                        // with system.
                        HdmiDeviceInfo deviceInfo = CreateDeviceInfo(logicalAddress, deviceType,
                                HdmiControlManager.POWER_STATUS_ON);
                        localDevice->SetDeviceInfo(deviceInfo);
                        mCecController->AddLocalDevice(deviceType, localDevice);
                        mCecController->AddLogicalAddress(logicalAddress);
                        allocatedDevices->Add(localDevice);
                    }

                    // Address allocation completed for all devices. Notify each device.
                    if (allocatingDevices->Size() == ++finished[0]) {
                        if (initiatedBy != INITIATED_BY_HOTPLUG) {
                            // In case of the hotplug we don't call OnInitializeCecComplete()
                            // since we reallocate the logical address only.
                            OnInitializeCecComplete();
                        }
                        NotifyAddressAllocated(allocatedDevices, initiatedBy);
                    }
                }
            });
        }
    }

    @ServiceThreadOnly
    private void NotifyAddressAllocated(ArrayList<HdmiCecLocalDevice> devices, Int32 initiatedBy) {
        AssertRunOnServiceThread();
        for (HdmiCecLocalDevice device : devices) {
            Int32 address = device->GetDeviceInfo()->GetLogicalAddress();
            device->HandleAddressAllocated(address, initiatedBy);
        }
    }

    // Initialize HDMI port information. Combine the information from CEC and MHL HAL and
    // keep them in one place.
    @ServiceThreadOnly
    private void InitPortInfo() {
        AssertRunOnServiceThread();
        HdmiPortInfo[] cecPortInfo = NULL;

        // CEC HAL provides majority of the info while MHL does only MHL support flag for
        // each port. Return empty array if CEC HAL didn't provide the info.
        if (mCecController != NULL) {
            cecPortInfo = mCecController->GetPortInfos();
        }
        if (cecPortInfo == NULL) {
            return;
        }

        SparseArray<HdmiPortInfo> portInfoMap = new SparseArray<>();
        SparseIntArray portIdMap = new SparseIntArray();
        SparseArray<HdmiDeviceInfo> portDeviceMap = new SparseArray<>();
        for (HdmiPortInfo info : cecPortInfo) {
            portIdMap->Put(info->GetAddress(), info->GetId());
            portInfoMap->Put(info->GetId(), info);
            portDeviceMap->Put(info->GetId(), new HdmiDeviceInfo(info->GetAddress(), info->GetId()));
        }
        mPortIdMap = new UnmodifiableSparseIntArray(portIdMap);
        mPortInfoMap = new UnmodifiableSparseArray<>(portInfoMap);
        mPortDeviceMap = new UnmodifiableSparseArray<>(portDeviceMap);

        HdmiPortInfo[] mhlPortInfo = mMhlController->GetPortInfos();
        ArraySet<Integer> mhlSupportedPorts = new ArraySet<Integer>(mhlPortInfo.length);
        for (HdmiPortInfo info : mhlPortInfo) {
            if (info->IsMhlSupported()) {
                mhlSupportedPorts->Add(info->GetId());
            }
        }

        // Build HDMI port info list with CEC port info plus MHL supported flag. We can just use
        // cec port info if we do not have have port that supports MHL.
        if (mhlSupportedPorts->IsEmpty()) {
            mPortInfo = Collections->UnmodifiableList(Arrays->AsList(cecPortInfo));
            return;
        }
        ArrayList<HdmiPortInfo> result = new ArrayList<>(cecPortInfo.length);
        for (HdmiPortInfo info : cecPortInfo) {
            if (mhlSupportedPorts->Contains(info->GetId())) {
                result->Add(new HdmiPortInfo(info->GetId(), info->GetType(), info->GetAddress(),
                        info->IsCecSupported(), TRUE, info->IsArcSupported()));
            } else {
                result->Add(info);
            }
        }
        mPortInfo = Collections->UnmodifiableList(result);
    }

    List<HdmiPortInfo> GetPortInfo() {
        return mPortInfo;
    }

    /**
     * Returns HDMI port information for the given port id.
     *
     * @param portId HDMI port id
     * @return {@link HdmiPortInfo} for the given port
     */
    HdmiPortInfo GetPortInfo(Int32 portId) {
        return mPortInfoMap->Get(portId, NULL);
    }

    /**
     * Returns the routing path (physical address) of the HDMI port for the given
     * port id.
     */
    Int32 PortIdToPath(Int32 portId) {
        HdmiPortInfo portInfo = GetPortInfo(portId);
        if (portInfo == NULL) {
            Slogger::E(TAG, "Cannot find the port info: " + portId);
            return Constants.INVALID_PHYSICAL_ADDRESS;
        }
        return portInfo->GetAddress();
    }

    /**
     * Returns the id of HDMI port located at the top of the hierarchy of
     * the specified routing path. For the routing path 0x1220 (1.2.2.0), for instance,
     * the port id to be returned is the ID associated with the port address
     * 0x1000 (1.0.0.0) which is the topmost path of the given routing path.
     */
    Int32 PathToPortId(Int32 path) {
        Int32 portAddress = path & Constants.ROUTING_PATH_TOP_MASK;
        return mPortIdMap->Get(portAddress, Constants.INVALID_PORT_ID);
    }

    Boolean IsValidPortId(Int32 portId) {
        return GetPortInfo(portId) != NULL;
    }

    /**
     * Returns {@link Looper} for IO operation.
     *
     * <p>Declared as package-private.
     */
    Looper GetIoLooper() {
        return mIoThread->GetLooper();
    }

    /**
     * Returns {@link Looper} of main thread. Use this {@link Looper} instance
     * for tasks that are running on main service thread.
     *
     * <p>Declared as package-private.
     */
    Looper GetServiceLooper() {
        return mHandler->GetLooper();
    }

    /**
     * Returns physical address of the device.
     */
    Int32 GetPhysicalAddress() {
        return mCecController->GetPhysicalAddress();
    }

    /**
     * Returns vendor id of CEC service.
     */
    Int32 GetVendorId() {
        return mCecController->GetVendorId();
    }

    @ServiceThreadOnly
    HdmiDeviceInfo GetDeviceInfo(Int32 logicalAddress) {
        AssertRunOnServiceThread();
        HdmiCecLocalDeviceTv tv = Tv();
        if (tv == NULL) {
            return NULL;
        }
        return tv->GetCecDeviceInfo(logicalAddress);
    }

    /**
     * Returns version of CEC.
     */
    Int32 GetCecVersion() {
        return mCecController->GetVersion();
    }

    /**
     * Whether a device of the specified physical address is connected to ARC enabled port.
     */
    Boolean IsConnectedToArcPort(Int32 physicalAddress) {
        Int32 portId = PathToPortId(physicalAddress);
        if (portId != Constants.INVALID_PORT_ID) {
            return mPortInfoMap->Get(portId).IsArcSupported();
        }
        return FALSE;
    }

    void RunOnServiceThread(Runnable runnable) {
        mHandler->Post(runnable);
    }

    void RunOnServiceThreadAtFrontOfQueue(Runnable runnable) {
        mHandler->PostAtFrontOfQueue(runnable);
    }

    private void AssertRunOnServiceThread() {
        if (Looper->MyLooper() != mHandler->GetLooper()) {
            throw new IllegalStateException("Should run on service thread.");
        }
    }

    /**
     * Transmit a CEC command to CEC bus.
     *
     * @param command CEC command to send out
     * @param callback interface used to the result of send command
     */
    @ServiceThreadOnly
    void SendCecCommand(HdmiCecMessage command, @Nullable SendMessageCallback callback) {
        AssertRunOnServiceThread();
        if (mMessageValidator->IsValid(command) == HdmiCecMessageValidator.OK) {
            mCecController->SendCommand(command, callback);
        } else {
            HdmiLogger->Error("Invalid message type:" + command);
            if (callback != NULL) {
                callback->OnSendCompleted(Constants.SEND_RESULT_FAILURE);
            }
        }
    }

    @ServiceThreadOnly
    void SendCecCommand(HdmiCecMessage command) {
        AssertRunOnServiceThread();
        SendCecCommand(command, NULL);
    }

    /**
     * Send <Feature Abort> command on the given CEC message if possible.
     * If the aborted message is invalid, then it wont send the message.
     * @param command original command to be aborted
     * @param reason reason of feature abort
     */
    @ServiceThreadOnly
    void MaySendFeatureAbortCommand(HdmiCecMessage command, Int32 reason) {
        AssertRunOnServiceThread();
        mCecController->MaySendFeatureAbortCommand(command, reason);
    }

    @ServiceThreadOnly
    Boolean HandleCecCommand(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        Int32 errorCode = mMessageValidator->IsValid(message);
        if (errorCode != HdmiCecMessageValidator.OK) {
            // We'll not response on the messages with the invalid source or destination.
            if (errorCode == HdmiCecMessageValidator.ERROR_PARAMETER) {
                MaySendFeatureAbortCommand(message, Constants.ABORT_INVALID_OPERAND);
            }
            return TRUE;
        }
        return DispatchMessageToLocalDevice(message);
    }

    void SetAudioReturnChannel(Boolean enabled) {
        mCecController->SetAudioReturnChannel(enabled);
    }

    @ServiceThreadOnly
    private Boolean DispatchMessageToLocalDevice(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        for (HdmiCecLocalDevice device : mCecController->GetLocalDeviceList()) {
            if (device->DispatchMessage(message)
                    && message->GetDestination() != Constants.ADDR_BROADCAST) {
                return TRUE;
            }
        }

        if (message->GetDestination() != Constants.ADDR_BROADCAST) {
            HdmiLogger->Warning("Unhandled cec command:" + message);
        }
        return FALSE;
    }

    /**
     * Called when a new hotplug event is issued.
     *
     * @param portId hdmi port number where hot plug event issued.
     * @param connected whether to be plugged in or not
     */
    @ServiceThreadOnly
    void OnHotplug(Int32 portId, Boolean connected) {
        AssertRunOnServiceThread();

        ArrayList<HdmiCecLocalDevice> localDevices = new ArrayList<>();
        for (Int32 type : mLocalDevices) {
            if (type == HdmiDeviceInfo.DEVICE_TV) {
                // Skip the reallocation of the logical address on TV.
                continue;
            }
            HdmiCecLocalDevice localDevice = mCecController->GetLocalDevice(type);
            if (localDevice == NULL) {
                localDevice = HdmiCecLocalDevice->Create(this, type);
                localDevice->Init();
            }
            localDevices->Add(localDevice);
        }
        AllocateLogicalAddress(localDevices, INITIATED_BY_HOTPLUG);

        for (HdmiCecLocalDevice device : mCecController->GetLocalDeviceList()) {
            device->OnHotplug(portId, connected);
        }
        AnnounceHotplugEvent(portId, connected);
    }

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
    @ServiceThreadOnly
    void PollDevices(DevicePollingCallback callback, Int32 sourceAddress, Int32 pickStrategy,
            Int32 retryCount) {
        AssertRunOnServiceThread();
        mCecController->PollDevices(callback, sourceAddress, CheckPollStrategy(pickStrategy),
                retryCount);
    }

    private Int32 CheckPollStrategy(Int32 pickStrategy) {
        Int32 strategy = pickStrategy & Constants.POLL_STRATEGY_MASK;
        if (strategy == 0) {
            throw new IllegalArgumentException("Invalid poll strategy:" + pickStrategy);
        }
        Int32 iterationStrategy = pickStrategy & Constants.POLL_ITERATION_STRATEGY_MASK;
        if (iterationStrategy == 0) {
            throw new IllegalArgumentException("Invalid iteration strategy:" + pickStrategy);
        }
        return strategy | iterationStrategy;
    }

    List<HdmiCecLocalDevice> GetAllLocalDevices() {
        AssertRunOnServiceThread();
        return mCecController->GetLocalDeviceList();
    }

    Object GetServiceLock() {
        return mLock;
    }

    void SetAudioStatus(Boolean mute, Int32 volume) {
        AudioManager audioManager = GetAudioManager();
        Boolean muted = audioManager->IsStreamMute(AudioManager.STREAM_MUSIC);
        if (mute) {
            if (!muted) {
                audioManager->SetStreamMute(AudioManager.STREAM_MUSIC, TRUE);
            }
        } else {
            if (muted) {
                audioManager->SetStreamMute(AudioManager.STREAM_MUSIC, FALSE);
            }
            // FLAG_HDMI_SYSTEM_AUDIO_VOLUME prevents audio manager from announcing
            // volume change notification back to hdmi control service.
            audioManager->SetStreamVolume(AudioManager.STREAM_MUSIC, volume,
                    AudioManager.FLAG_SHOW_UI | AudioManager.FLAG_HDMI_SYSTEM_AUDIO_VOLUME);
        }
    }

    void AnnounceSystemAudioModeChange(Boolean enabled) {
        synchronized(mLock) {
            for (SystemAudioModeChangeListenerRecord record :
                    mSystemAudioModeChangeListenerRecords) {
                InvokeSystemAudioModeChangeLocked(record.mListener, enabled);
            }
        }
    }

    private HdmiDeviceInfo CreateDeviceInfo(Int32 logicalAddress, Int32 deviceType, Int32 powerStatus) {
        // TODO: find better name instead of model name.
        String displayName = Build.MODEL;
        return new HdmiDeviceInfo(logicalAddress,
                GetPhysicalAddress(), PathToPortId(GetPhysicalAddress()), deviceType,
                GetVendorId(), displayName);
    }

    @ServiceThreadOnly
    void HandleMhlHotplugEvent(Int32 portId, Boolean connected) {
        AssertRunOnServiceThread();
        if (connected) {
            HdmiMhlLocalDeviceStub newDevice = new HdmiMhlLocalDeviceStub(this, portId);
            HdmiMhlLocalDeviceStub oldDevice = mMhlController->AddLocalDevice(newDevice);
            if (oldDevice != NULL) {
                oldDevice->OnDeviceRemoved();
                Slogger::I(TAG, "Old device of port " + portId + " is removed");
            }
        } else {
            HdmiMhlLocalDeviceStub device = mMhlController->RemoveLocalDevice(portId);
            if (device != NULL) {
                device->OnDeviceRemoved();
                // There is no explicit event for device removal.
                // Hence we remove the device on hotplug event.
                HdmiDeviceInfo deviceInfo = device->GetInfo();
                if (deviceInfo != NULL) {
                    InvokeDeviceEventListeners(deviceInfo, DEVICE_EVENT_REMOVE_DEVICE);
                    UpdateSafeMhlInput();
                }
            } else {
                Slogger::W(TAG, "No device to remove:[portId=" + portId);
            }
        }
        AnnounceHotplugEvent(portId, connected);
    }

    @ServiceThreadOnly
    void HandleMhlBusModeChanged(Int32 portId, Int32 busmode) {
        AssertRunOnServiceThread();
        HdmiMhlLocalDeviceStub device = mMhlController->GetLocalDevice(portId);
        if (device != NULL) {
            device->SetBusMode(busmode);
        } else {
            Slogger::W(TAG, "No mhl device exists for bus mode change[portId:" + portId +
                    ", busmode:" + busmode + "]");
        }
    }

    @ServiceThreadOnly
    void HandleMhlBusOvercurrent(Int32 portId, Boolean on) {
        AssertRunOnServiceThread();
        HdmiMhlLocalDeviceStub device = mMhlController->GetLocalDevice(portId);
        if (device != NULL) {
            device->OnBusOvercurrentDetected(on);
        } else {
            Slogger::W(TAG, "No mhl device exists for bus overcurrent event[portId:" + portId + "]");
        }
    }

    @ServiceThreadOnly
    void HandleMhlDeviceStatusChanged(Int32 portId, Int32 adopterId, Int32 deviceId) {
        AssertRunOnServiceThread();
        HdmiMhlLocalDeviceStub device = mMhlController->GetLocalDevice(portId);

        // Hotplug event should already have been called before device status change event.
        if (device != NULL) {
            device->SetDeviceStatusChange(adopterId, deviceId);
            InvokeDeviceEventListeners(device->GetInfo(), DEVICE_EVENT_ADD_DEVICE);
            UpdateSafeMhlInput();
        } else {
            Slogger::W(TAG, "No mhl device exists for device status event[portId:"
                    + portId + ", adopterId:" + adopterId + ", deviceId:" + deviceId + "]");
        }
    }

    @ServiceThreadOnly
    private void UpdateSafeMhlInput() {
        AssertRunOnServiceThread();
        List<HdmiDeviceInfo> inputs = Collections->EmptyList();
        SparseArray<HdmiMhlLocalDeviceStub> devices = mMhlController->GetAllLocalDevices();
        for (Int32 i = 0; i < devices->Size(); ++i) {
            HdmiMhlLocalDeviceStub device = devices->ValueAt(i);
            HdmiDeviceInfo info = device->GetInfo();
            if (info != NULL) {
                if (inputs->IsEmpty()) {
                    inputs = new ArrayList<>();
                }
                inputs->Add(device->GetInfo());
            }
        }
        synchronized(mLock) {
            mMhlDevices = inputs;
        }
    }

    private List<HdmiDeviceInfo> GetMhlDevicesLocked() {
        return mMhlDevices;
    }

    private class HdmiMhlVendorCommandListenerRecord implements IBinder.DeathRecipient {
        private final IHdmiMhlVendorCommandListener mListener;

        public HdmiMhlVendorCommandListenerRecord(IHdmiMhlVendorCommandListener listener) {
            mListener = listener;
        }

        //@Override
        CARAPI BinderDied() {
            mMhlVendorCommandListenerRecords->Remove(this);
        }
    }

    // Record class that monitors the event of the caller of being killed. Used to clean up
    // the listener list and record list accordingly.
    private final class HotplugEventListenerRecord implements IBinder.DeathRecipient {
        private final IHdmiHotplugEventListener mListener;

        public HotplugEventListenerRecord(IHdmiHotplugEventListener listener) {
            mListener = listener;
        }

        //@Override
        CARAPI BinderDied() {
            synchronized(mLock) {
                mHotplugEventListenerRecords->Remove(this);
            }
        }
    }

    private final class DeviceEventListenerRecord implements IBinder.DeathRecipient {
        private final IHdmiDeviceEventListener mListener;

        public DeviceEventListenerRecord(IHdmiDeviceEventListener listener) {
            mListener = listener;
        }

        //@Override
        CARAPI BinderDied() {
            synchronized(mLock) {
                mDeviceEventListenerRecords->Remove(this);
            }
        }
    }

    private final class SystemAudioModeChangeListenerRecord implements IBinder.DeathRecipient {
        private final IHdmiSystemAudioModeChangeListener mListener;

        public SystemAudioModeChangeListenerRecord(IHdmiSystemAudioModeChangeListener listener) {
            mListener = listener;
        }

        //@Override
        CARAPI BinderDied() {
            synchronized(mLock) {
                mSystemAudioModeChangeListenerRecords->Remove(this);
            }
        }
    }

    class VendorCommandListenerRecord implements IBinder.DeathRecipient {
        private final IHdmiVendorCommandListener mListener;
        private final Int32 mDeviceType;

        public VendorCommandListenerRecord(IHdmiVendorCommandListener listener, Int32 deviceType) {
            mListener = listener;
            mDeviceType = deviceType;
        }

        //@Override
        CARAPI BinderDied() {
            synchronized(mLock) {
                mVendorCommandListenerRecords->Remove(this);
            }
        }
    }

    private class HdmiRecordListenerRecord implements IBinder.DeathRecipient {
        private final IHdmiRecordListener mListener;

        public HdmiRecordListenerRecord(IHdmiRecordListener listener) {
            mListener = listener;
        }

        //@Override
        CARAPI BinderDied() {
            synchronized(mLock) {
                mRecordListenerRecord = NULL;
            }
        }
    }

    private void EnforceAccessPermission() {
        GetContext()->EnforceCallingOrSelfPermission(PERMISSION, TAG);
    }

    private final class BinderService extends IHdmiControlService.Stub {
        //@Override
        public Int32[] GetSupportedTypes() {
            EnforceAccessPermission();
            // mLocalDevices is an unmodifiable list - no lock necesary.
            Int32[] localDevices = new Int32[mLocalDevices->Size()];
            for (Int32 i = 0; i < localDevices.length; ++i) {
                localDevices[i] = mLocalDevices->Get(i);
            }
            return localDevices;
        }

        //@Override
        public HdmiDeviceInfo GetActiveSource() {
            EnforceAccessPermission();
            HdmiCecLocalDeviceTv tv = Tv();
            if (tv == NULL) {
                Slogger::W(TAG, "Local tv device not available");
                return NULL;
            }
            ActiveSource activeSource = tv->GetActiveSource();
            if (activeSource->IsValid()) {
                return new HdmiDeviceInfo(activeSource.logicalAddress,
                        activeSource.physicalAddress, HdmiDeviceInfo.PORT_INVALID,
                        HdmiDeviceInfo.DEVICE_INACTIVE, 0, "");
            }
            Int32 activePath = tv->GetActivePath();
            if (activePath != HdmiDeviceInfo.PATH_INVALID) {
                return new HdmiDeviceInfo(activePath, tv->GetActivePortId());
            }
            return NULL;
        }

        //@Override
        CARAPI DeviceSelect(final Int32 deviceId, final IHdmiControlCallback callback) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    if (callback == NULL) {
                        Slogger::E(TAG, "Callback cannot be NULL");
                        return;
                    }
                    HdmiCecLocalDeviceTv tv = Tv();
                    if (tv == NULL) {
                        Slogger::W(TAG, "Local tv device not available");
                        InvokeCallback(callback, HdmiControlManager.RESULT_SOURCE_NOT_AVAILABLE);
                        return;
                    }
                    HdmiMhlLocalDeviceStub device = mMhlController->GetLocalDeviceById(deviceId);
                    if (device != NULL) {
                        if (device->GetPortId() == tv->GetActivePortId()) {
                            InvokeCallback(callback, HdmiControlManager.RESULT_SUCCESS);
                            return;
                        }
                        // Upon selecting MHL device, we send RAP[Content On] to wake up
                        // the connected mobile device, start routing control to switch ports.
                        // callback is handled by MHL action.
                        device->TurnOn(callback);
                        tv->DoManualPortSwitching(device->GetPortId(), NULL);
                        return;
                    }
                    tv->DeviceSelect(deviceId, callback);
                }
            });
        }

        //@Override
        CARAPI PortSelect(final Int32 portId, final IHdmiControlCallback callback) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    if (callback == NULL) {
                        Slogger::E(TAG, "Callback cannot be NULL");
                        return;
                    }
                    HdmiCecLocalDeviceTv tv = Tv();
                    if (tv == NULL) {
                        Slogger::W(TAG, "Local tv device not available");
                        InvokeCallback(callback, HdmiControlManager.RESULT_SOURCE_NOT_AVAILABLE);
                        return;
                    }
                    tv->DoManualPortSwitching(portId, callback);
                }
            });
        }

        //@Override
        CARAPI SendKeyEvent(final Int32 deviceType, final Int32 keyCode, final Boolean isPressed) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    HdmiMhlLocalDeviceStub device = mMhlController->GetLocalDevice(mActivePortId);
                    if (device != NULL) {
                        device->SendKeyEvent(keyCode, isPressed);
                        return;
                    }
                    if (mCecController != NULL) {
                        HdmiCecLocalDevice localDevice = mCecController->GetLocalDevice(deviceType);
                        if (localDevice == NULL) {
                            Slogger::W(TAG, "Local device not available");
                            return;
                        }
                        localDevice->SendKeyEvent(keyCode, isPressed);
                    }
                }
            });
        }

        //@Override
        CARAPI OneTouchPlay(final IHdmiControlCallback callback) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    HdmiControlService.this->OneTouchPlay(callback);
                }
            });
        }

        //@Override
        CARAPI QueryDisplayStatus(final IHdmiControlCallback callback) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    HdmiControlService.this->QueryDisplayStatus(callback);
                }
            });
        }

        //@Override
        CARAPI AddHotplugEventListener(final IHdmiHotplugEventListener listener) {
            EnforceAccessPermission();
            HdmiControlService.this->AddHotplugEventListener(listener);
        }

        //@Override
        CARAPI RemoveHotplugEventListener(final IHdmiHotplugEventListener listener) {
            EnforceAccessPermission();
            HdmiControlService.this->RemoveHotplugEventListener(listener);
        }

        //@Override
        CARAPI AddDeviceEventListener(final IHdmiDeviceEventListener listener) {
            EnforceAccessPermission();
            HdmiControlService.this->AddDeviceEventListener(listener);
        }

        //@Override
        public List<HdmiPortInfo> GetPortInfo() {
            EnforceAccessPermission();
            return HdmiControlService.this->GetPortInfo();
        }

        //@Override
        public Boolean CanChangeSystemAudioMode() {
            EnforceAccessPermission();
            HdmiCecLocalDeviceTv tv = Tv();
            if (tv == NULL) {
                return FALSE;
            }
            return tv->HasSystemAudioDevice();
        }

        //@Override
        public Boolean GetSystemAudioMode() {
            EnforceAccessPermission();
            HdmiCecLocalDeviceTv tv = Tv();
            if (tv == NULL) {
                return FALSE;
            }
            return tv->IsSystemAudioActivated();
        }

        //@Override
        CARAPI SetSystemAudioMode(final Boolean enabled, final IHdmiControlCallback callback) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    HdmiCecLocalDeviceTv tv = Tv();
                    if (tv == NULL) {
                        Slogger::W(TAG, "Local tv device not available");
                        InvokeCallback(callback, HdmiControlManager.RESULT_SOURCE_NOT_AVAILABLE);
                        return;
                    }
                    tv->ChangeSystemAudioMode(enabled, callback);
                }
            });
        }

        //@Override
        CARAPI AddSystemAudioModeChangeListener(
                final IHdmiSystemAudioModeChangeListener listener) {
            EnforceAccessPermission();
            HdmiControlService.this->AddSystemAudioModeChangeListner(listener);
        }

        //@Override
        CARAPI RemoveSystemAudioModeChangeListener(
                final IHdmiSystemAudioModeChangeListener listener) {
            EnforceAccessPermission();
            HdmiControlService.this->RemoveSystemAudioModeChangeListener(listener);
        }

        //@Override
        CARAPI SetInputChangeListener(final IHdmiInputChangeListener listener) {
            EnforceAccessPermission();
            HdmiControlService.this->SetInputChangeListener(listener);
        }

        //@Override
        public List<HdmiDeviceInfo> GetInputDevices() {
            EnforceAccessPermission();
            // No need to hold the lock for obtaining TV device as the local device instance
            // is preserved while the HDMI control is enabled.
            HdmiCecLocalDeviceTv tv = Tv();
            synchronized(mLock) {
                List<HdmiDeviceInfo> cecDevices = (tv == NULL)
                        ? Collections.<HdmiDeviceInfo>EmptyList()
                        : tv->GetSafeExternalInputsLocked();
                return HdmiUtils->MergeToUnmodifiableList(cecDevices, GetMhlDevicesLocked());
            }
        }

        //@Override
        CARAPI SetSystemAudioVolume(final Int32 oldIndex, final Int32 newIndex,
                final Int32 maxIndex) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    HdmiCecLocalDeviceTv tv = Tv();
                    if (tv == NULL) {
                        Slogger::W(TAG, "Local tv device not available");
                        return;
                    }
                    tv->ChangeVolume(oldIndex, newIndex - oldIndex, maxIndex);
                }
            });
        }

        //@Override
        CARAPI SetSystemAudioMute(final Boolean mute) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    HdmiCecLocalDeviceTv tv = Tv();
                    if (tv == NULL) {
                        Slogger::W(TAG, "Local tv device not available");
                        return;
                    }
                    tv->ChangeMute(mute);
                }
            });
        }

        //@Override
        CARAPI SetArcMode(final Boolean enabled) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    HdmiCecLocalDeviceTv tv = Tv();
                    if (tv == NULL) {
                        Slogger::W(TAG, "Local tv device not available to change arc mode.");
                        return;
                    }
                }
            });
        }

        //@Override
        CARAPI SetProhibitMode(final Boolean enabled) {
            EnforceAccessPermission();
            if (!IsTvDevice()) {
                return;
            }
            HdmiControlService.this->SetProhibitMode(enabled);
        }

        //@Override
        CARAPI AddVendorCommandListener(final IHdmiVendorCommandListener listener,
                final Int32 deviceType) {
            EnforceAccessPermission();
            HdmiControlService.this->AddVendorCommandListener(listener, deviceType);
        }

        //@Override
        CARAPI SendVendorCommand(final Int32 deviceType, final Int32 targetAddress,
                final Byte[] params, final Boolean hasVendorId) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    HdmiCecLocalDevice device = mCecController->GetLocalDevice(deviceType);
                    if (device == NULL) {
                        Slogger::W(TAG, "Local device not available");
                        return;
                    }
                    if (hasVendorId) {
                        SendCecCommand(HdmiCecMessageBuilder->BuildVendorCommandWithId(
                                device->GetDeviceInfo()->GetLogicalAddress(), targetAddress,
                                GetVendorId(), params));
                    } else {
                        SendCecCommand(HdmiCecMessageBuilder->BuildVendorCommand(
                                device->GetDeviceInfo()->GetLogicalAddress(), targetAddress, params));
                    }
                }
            });
        }

        //@Override
        CARAPI SendStandby(final Int32 deviceType, final Int32 deviceId) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    HdmiCecLocalDevice device = mCecController->GetLocalDevice(deviceType);
                    if (device == NULL) {
                        Slogger::W(TAG, "Local device not available");
                        return;
                    }
                    device->SendStandby(deviceId);
                }
            });
        }

        //@Override
        CARAPI SetHdmiRecordListener(IHdmiRecordListener listener) {
            EnforceAccessPermission();
            HdmiControlService.this->SetHdmiRecordListener(listener);
        }

        //@Override
        CARAPI StartOneTouchRecord(final Int32 recorderAddress, final Byte[] recordSource) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    if (!IsTvDevice()) {
                        Slogger::W(TAG, "No TV is available.");
                        return;
                    }
                    Tv()->StartOneTouchRecord(recorderAddress, recordSource);
                }
            });
        }

        //@Override
        CARAPI StopOneTouchRecord(final Int32 recorderAddress) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    if (!IsTvDevice()) {
                        Slogger::W(TAG, "No TV is available.");
                        return;
                    }
                    Tv()->StopOneTouchRecord(recorderAddress);
                }
            });
        }

        //@Override
        CARAPI StartTimerRecording(final Int32 recorderAddress, final Int32 sourceType,
                final Byte[] recordSource) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    if (!IsTvDevice()) {
                        Slogger::W(TAG, "No TV is available.");
                        return;
                    }
                    Tv()->StartTimerRecording(recorderAddress, sourceType, recordSource);
                }
            });
        }

        //@Override
        CARAPI ClearTimerRecording(final Int32 recorderAddress, final Int32 sourceType,
                final Byte[] recordSource) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    if (!IsTvDevice()) {
                        Slogger::W(TAG, "No TV is available.");
                        return;
                    }
                    Tv()->ClearTimerRecording(recorderAddress, sourceType, recordSource);
                }
            });
        }

        //@Override
        CARAPI SendMhlVendorCommand(final Int32 portId, final Int32 offset, final Int32 length,
                final Byte[] data) {
            EnforceAccessPermission();
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    if (!IsControlEnabled()) {
                        Slogger::W(TAG, "Hdmi control is disabled.");
                        return ;
                    }
                    HdmiMhlLocalDeviceStub device = mMhlController->GetLocalDevice(portId);
                    if (device == NULL) {
                        Slogger::W(TAG, "Invalid port id:" + portId);
                        return;
                    }
                    mMhlController->SendVendorCommand(portId, offset, length, data);
                }
            });
        }

        //@Override
        CARAPI AddHdmiMhlVendorCommandListener(
                IHdmiMhlVendorCommandListener listener) {
            EnforceAccessPermission();
            HdmiControlService.this->AddHdmiMhlVendorCommandListener(listener);
        }

        //@Override
        protected void Dump(FileDescriptor fd, final PrintWriter writer, String[] args) {
            GetContext()->EnforceCallingOrSelfPermission(Manifest::permission::DUMP, TAG);
            final IndentingPrintWriter pw = new IndentingPrintWriter(writer, "  ");

            pw->Println("mHdmiControlEnabled: " + mHdmiControlEnabled);
            pw->Println("mProhibitMode: " + mProhibitMode);
            if (mCecController != NULL) {
                pw->Println("mCecController: ");
                pw->IncreaseIndent();
                mCecController->Dump(pw);
                pw->DecreaseIndent();
            }
            pw->Println("mPortInfo: ");
            pw->IncreaseIndent();
            for (HdmiPortInfo hdmiPortInfo : mPortInfo) {
                pw->Println("- " + hdmiPortInfo);
            }
            pw->DecreaseIndent();
            pw->Println("mPowerStatus: " + mPowerStatus);
        }
    }

    @ServiceThreadOnly
    private void OneTouchPlay(final IHdmiControlCallback callback) {
        AssertRunOnServiceThread();
        HdmiCecLocalDevicePlayback source = Playback();
        if (source == NULL) {
            Slogger::W(TAG, "Local playback device not available");
            InvokeCallback(callback, HdmiControlManager.RESULT_SOURCE_NOT_AVAILABLE);
            return;
        }
        source->OneTouchPlay(callback);
    }

    @ServiceThreadOnly
    private void QueryDisplayStatus(final IHdmiControlCallback callback) {
        AssertRunOnServiceThread();
        HdmiCecLocalDevicePlayback source = Playback();
        if (source == NULL) {
            Slogger::W(TAG, "Local playback device not available");
            InvokeCallback(callback, HdmiControlManager.RESULT_SOURCE_NOT_AVAILABLE);
            return;
        }
        source->QueryDisplayStatus(callback);
    }

    private void AddHotplugEventListener(IHdmiHotplugEventListener listener) {
        HotplugEventListenerRecord record = new HotplugEventListenerRecord(listener);
        try {
            listener->AsBinder()->LinkToDeath(record, 0);
        } catch (RemoteException e) {
            Slogger::W(TAG, "Listener already died");
            return;
        }
        synchronized(mLock) {
            mHotplugEventListenerRecords->Add(record);
        }
    }

    private void RemoveHotplugEventListener(IHdmiHotplugEventListener listener) {
        synchronized(mLock) {
            for (HotplugEventListenerRecord record : mHotplugEventListenerRecords) {
                if (record.mListener->AsBinder() == listener->AsBinder()) {
                    listener->AsBinder()->UnlinkToDeath(record, 0);
                    mHotplugEventListenerRecords->Remove(record);
                    break;
                }
            }
        }
    }

    private void AddDeviceEventListener(IHdmiDeviceEventListener listener) {
        DeviceEventListenerRecord record = new DeviceEventListenerRecord(listener);
        try {
            listener->AsBinder()->LinkToDeath(record, 0);
        } catch (RemoteException e) {
            Slogger::W(TAG, "Listener already died");
            return;
        }
        synchronized(mLock) {
            mDeviceEventListenerRecords->Add(record);
        }
    }

    void InvokeDeviceEventListeners(HdmiDeviceInfo device, Int32 status) {
        synchronized(mLock) {
            for (DeviceEventListenerRecord record : mDeviceEventListenerRecords) {
                try {
                    record.mListener->OnStatusChanged(device, status);
                } catch (RemoteException e) {
                    Slogger::E(TAG, "Failed to report device event:" + e);
                }
            }
        }
    }

    private void AddSystemAudioModeChangeListner(IHdmiSystemAudioModeChangeListener listener) {
        SystemAudioModeChangeListenerRecord record = new SystemAudioModeChangeListenerRecord(
                listener);
        try {
            listener->AsBinder()->LinkToDeath(record, 0);
        } catch (RemoteException e) {
            Slogger::W(TAG, "Listener already died");
            return;
        }
        synchronized(mLock) {
            mSystemAudioModeChangeListenerRecords->Add(record);
        }
    }

    private void RemoveSystemAudioModeChangeListener(IHdmiSystemAudioModeChangeListener listener) {
        synchronized(mLock) {
            for (SystemAudioModeChangeListenerRecord record :
                    mSystemAudioModeChangeListenerRecords) {
                if (record.mListener->AsBinder() == listener) {
                    listener->AsBinder()->UnlinkToDeath(record, 0);
                    mSystemAudioModeChangeListenerRecords->Remove(record);
                    break;
                }
            }
        }
    }

    private final class InputChangeListenerRecord implements IBinder.DeathRecipient {
        private final IHdmiInputChangeListener mListener;

        public InputChangeListenerRecord(IHdmiInputChangeListener listener) {
            mListener = listener;
        }

        //@Override
        CARAPI BinderDied() {
            synchronized(mLock) {
                mInputChangeListenerRecord = NULL;
            }
        }
    }

    private void SetInputChangeListener(IHdmiInputChangeListener listener) {
        synchronized(mLock) {
            mInputChangeListenerRecord = new InputChangeListenerRecord(listener);
            try {
                listener->AsBinder()->LinkToDeath(mInputChangeListenerRecord, 0);
            } catch (RemoteException e) {
                Slogger::W(TAG, "Listener already died");
                return;
            }
        }
    }

    void InvokeInputChangeListener(HdmiDeviceInfo info) {
        synchronized(mLock) {
            if (mInputChangeListenerRecord != NULL) {
                try {
                    mInputChangeListenerRecord.mListener->OnChanged(info);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Exception thrown by IHdmiInputChangeListener: " + e);
                }
            }
        }
    }

    private void SetHdmiRecordListener(IHdmiRecordListener listener) {
        synchronized(mLock) {
            mRecordListenerRecord = new HdmiRecordListenerRecord(listener);
            try {
                listener->AsBinder()->LinkToDeath(mRecordListenerRecord, 0);
            } catch (RemoteException e) {
                Slogger::W(TAG, "Listener already died.", e);
            }
        }
    }

    Byte[] InvokeRecordRequestListener(Int32 recorderAddress) {
        synchronized(mLock) {
            if (mRecordListenerRecord != NULL) {
                try {
                    return mRecordListenerRecord.mListener->GetOneTouchRecordSource(recorderAddress);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Failed to start record.", e);
                }
            }
            return EmptyArray.BYTE;
        }
    }

    void InvokeOneTouchRecordResult(Int32 result) {
        synchronized(mLock) {
            if (mRecordListenerRecord != NULL) {
                try {
                    mRecordListenerRecord.mListener->OnOneTouchRecordResult(result);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Failed to call onOneTouchRecordResult.", e);
                }
            }
        }
    }

    void InvokeTimerRecordingResult(Int32 result) {
        synchronized(mLock) {
            if (mRecordListenerRecord != NULL) {
                try {
                    mRecordListenerRecord.mListener->OnTimerRecordingResult(result);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Failed to call onTimerRecordingResult.", e);
                }
            }
        }
    }

    void InvokeClearTimerRecordingResult(Int32 result) {
        synchronized(mLock) {
            if (mRecordListenerRecord != NULL) {
                try {
                    mRecordListenerRecord.mListener->OnClearTimerRecordingResult(result);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Failed to call onClearTimerRecordingResult.", e);
                }
            }
        }
    }

    private void InvokeCallback(IHdmiControlCallback callback, Int32 result) {
        try {
            callback->OnComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Invoking callback failed:" + e);
        }
    }

    private void InvokeSystemAudioModeChangeLocked(IHdmiSystemAudioModeChangeListener listener,
            Boolean enabled) {
        try {
            listener->OnStatusChanged(enabled);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Invoking callback failed:" + e);
        }
    }

    private void AnnounceHotplugEvent(Int32 portId, Boolean connected) {
        HdmiHotplugEvent event = new HdmiHotplugEvent(portId, connected);
        synchronized(mLock) {
            for (HotplugEventListenerRecord record : mHotplugEventListenerRecords) {
                InvokeHotplugEventListenerLocked(record.mListener, event);
            }
        }
    }

    private void InvokeHotplugEventListenerLocked(IHdmiHotplugEventListener listener,
            HdmiHotplugEvent event) {
        try {
            listener->OnReceived(event);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Failed to report hotplug event:" + event->ToString(), e);
        }
    }

    private HdmiCecLocalDeviceTv Tv() {
        return (HdmiCecLocalDeviceTv) mCecController->GetLocalDevice(HdmiDeviceInfo.DEVICE_TV);
    }

    Boolean IsTvDevice() {
        return Tv() != NULL;
    }

    private HdmiCecLocalDevicePlayback Playback() {
        return (HdmiCecLocalDevicePlayback)
                mCecController->GetLocalDevice(HdmiDeviceInfo.DEVICE_PLAYBACK);
    }

    AudioManager GetAudioManager() {
        return (AudioManager) GetContext()->GetSystemService(Context.AUDIO_SERVICE);
    }

    Boolean IsControlEnabled() {
        synchronized(mLock) {
            return mHdmiControlEnabled;
        }
    }

    @ServiceThreadOnly
    Int32 GetPowerStatus() {
        AssertRunOnServiceThread();
        return mPowerStatus;
    }

    @ServiceThreadOnly
    Boolean IsPowerOnOrTransient() {
        AssertRunOnServiceThread();
        return mPowerStatus == HdmiControlManager.POWER_STATUS_ON
                || mPowerStatus == HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON;
    }

    @ServiceThreadOnly
    Boolean IsPowerStandbyOrTransient() {
        AssertRunOnServiceThread();
        return mPowerStatus == HdmiControlManager.POWER_STATUS_STANDBY
                || mPowerStatus == HdmiControlManager.POWER_STATUS_TRANSIENT_TO_STANDBY;
    }

    @ServiceThreadOnly
    Boolean IsPowerStandby() {
        AssertRunOnServiceThread();
        return mPowerStatus == HdmiControlManager.POWER_STATUS_STANDBY;
    }

    @ServiceThreadOnly
    void WakeUp() {
        AssertRunOnServiceThread();
        mWakeUpMessageReceived = TRUE;
        PowerManager pm = (PowerManager) GetContext()->GetSystemService(Context.POWER_SERVICE);
        pm->WakeUp(SystemClock->UptimeMillis());
        // PowerManger will send the broadcast IIntent::ACTION_SCREEN_ON and after this gets
        // the intent, the sequence will continue at OnWakeUp().
    }

    @ServiceThreadOnly
    void Standby() {
        AssertRunOnServiceThread();
        mStandbyMessageReceived = TRUE;
        PowerManager pm = (PowerManager) GetContext()->GetSystemService(Context.POWER_SERVICE);
        pm->GoToSleep(SystemClock->UptimeMillis(), PowerManager.GO_TO_SLEEP_REASON_HDMI, 0);
        // PowerManger will send the broadcast IIntent::ACTION_SCREEN_OFF and after this gets
        // the intent, the sequence will continue at OnStandby().
    }

    @ServiceThreadOnly
    private void OnWakeUp() {
        AssertRunOnServiceThread();
        mPowerStatus = HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON;
        if (mCecController != NULL) {
            if (mHdmiControlEnabled) {
                Int32 startReason = INITIATED_BY_SCREEN_ON;
                if (mWakeUpMessageReceived) {
                    startReason = INITIATED_BY_WAKE_UP_MESSAGE;
                }
                InitializeCec(startReason);
            }
        } else {
            Slogger::I(TAG, "Device does not support HDMI-CEC.");
        }
        // TODO: Initialize MHL local devices.
    }

    @ServiceThreadOnly
    private void OnStandby() {
        AssertRunOnServiceThread();
        mPowerStatus = HdmiControlManager.POWER_STATUS_TRANSIENT_TO_STANDBY;

        final List<HdmiCecLocalDevice> devices = GetAllLocalDevices();
        DisableDevices(new PendingActionClearedCallback() {
            //@Override
            CARAPI OnCleared(HdmiCecLocalDevice device) {
                Slogger::V(TAG, "On standby-action cleared:" + device.mDeviceType);
                devices->Remove(device);
                if (devices->IsEmpty()) {
                    OnStandbyCompleted();
                    // We will not clear local devices here, since some OEM/SOC will keep passing
                    // the received packets until the application processor enters to the sleep
                    // actually.
                }
            }
        });
    }

    @ServiceThreadOnly
    private void OnLanguageChanged(String language) {
        AssertRunOnServiceThread();
        mLanguage = language;

        if (IsTvDevice()) {
            Tv()->BroadcastMenuLanguage(language);
        }
    }

    @ServiceThreadOnly
    String GetLanguage() {
        AssertRunOnServiceThread();
        return mLanguage;
    }

    private void DisableDevices(PendingActionClearedCallback callback) {
        if (mCecController != NULL) {
            for (HdmiCecLocalDevice device : mCecController->GetLocalDeviceList()) {
                device->DisableDevice(mStandbyMessageReceived, callback);
            }
            if (IsTvDevice()) {
                UnregisterSettingsObserver();
            }
        }

        mMhlController->ClearAllLocalDevices();
    }

    @ServiceThreadOnly
    private void ClearLocalDevices() {
        AssertRunOnServiceThread();
        if (mCecController == NULL) {
            return;
        }
        mCecController->ClearLogicalAddress();
        mCecController->ClearLocalDevices();
    }

    @ServiceThreadOnly
    private void OnStandbyCompleted() {
        AssertRunOnServiceThread();
        Slogger::V(TAG, "onStandbyCompleted");

        if (mPowerStatus != HdmiControlManager.POWER_STATUS_TRANSIENT_TO_STANDBY) {
            return;
        }
        mPowerStatus = HdmiControlManager.POWER_STATUS_STANDBY;
        for (HdmiCecLocalDevice device : mCecController->GetLocalDeviceList()) {
            device->OnStandby(mStandbyMessageReceived);
        }
        mStandbyMessageReceived = FALSE;
        mCecController->SetOption(OPTION_CEC_SERVICE_CONTROL, DISABLED);
    }

    private void AddVendorCommandListener(IHdmiVendorCommandListener listener, Int32 deviceType) {
        VendorCommandListenerRecord record = new VendorCommandListenerRecord(listener, deviceType);
        try {
            listener->AsBinder()->LinkToDeath(record, 0);
        } catch (RemoteException e) {
            Slogger::W(TAG, "Listener already died");
            return;
        }
        synchronized(mLock) {
            mVendorCommandListenerRecords->Add(record);
        }
    }

    Boolean InvokeVendorCommandListeners(Int32 deviceType, Int32 srcAddress, Byte[] params,
            Boolean hasVendorId) {
        synchronized(mLock) {
            if (mVendorCommandListenerRecords->IsEmpty()) {
                return FALSE;
            }
            for (VendorCommandListenerRecord record : mVendorCommandListenerRecords) {
                if (record.mDeviceType != deviceType) {
                    continue;
                }
                try {
                    record.mListener->OnReceived(srcAddress, params, hasVendorId);
                } catch (RemoteException e) {
                    Slogger::E(TAG, "Failed to notify vendor command reception", e);
                }
            }
            return TRUE;
        }
    }

    private void AddHdmiMhlVendorCommandListener(IHdmiMhlVendorCommandListener listener) {
        HdmiMhlVendorCommandListenerRecord record =
                new HdmiMhlVendorCommandListenerRecord(listener);
        try {
            listener->AsBinder()->LinkToDeath(record, 0);
        } catch (RemoteException e) {
            Slogger::W(TAG, "Listener already died.");
            return;
        }

        synchronized(mLock) {
            mMhlVendorCommandListenerRecords->Add(record);
        }
    }

    void InvokeMhlVendorCommandListeners(Int32 portId, Int32 offest, Int32 length, Byte[] data) {
        synchronized(mLock) {
            for (HdmiMhlVendorCommandListenerRecord record : mMhlVendorCommandListenerRecords) {
                try {
                    record.mListener->OnReceived(portId, offest, length, data);
                } catch (RemoteException e) {
                    Slogger::E(TAG, "Failed to notify MHL vendor command", e);
                }
            }
        }
    }

    Boolean IsProhibitMode() {
        synchronized(mLock) {
            return mProhibitMode;
        }
    }

    void SetProhibitMode(Boolean enabled) {
        synchronized(mLock) {
            mProhibitMode = enabled;
        }
    }

    @ServiceThreadOnly
    void SetCecOption(Int32 key, Int32 value) {
        AssertRunOnServiceThread();
        mCecController->SetOption(key, value);
    }

    @ServiceThreadOnly
    void SetControlEnabled(Boolean enabled) {
        AssertRunOnServiceThread();

        Int32 value = ToInt(enabled);
        mCecController->SetOption(OPTION_CEC_ENABLE, value);
        mMhlController->SetOption(OPTION_MHL_ENABLE, value);

        synchronized(mLock) {
            mHdmiControlEnabled = enabled;
        }

        if (enabled) {
            InitializeCec(INITIATED_BY_ENABLE_CEC);
        } else {
            DisableDevices(new PendingActionClearedCallback() {
                //@Override
                CARAPI OnCleared(HdmiCecLocalDevice device) {
                    AssertRunOnServiceThread();
                    ClearLocalDevices();
                }
            });
        }
    }

    @ServiceThreadOnly
    void SetActivePortId(Int32 portId) {
        AssertRunOnServiceThread();
        mActivePortId = portId;

        // Resets last input for MHL, which stays valid only after the MHL device was selected,
        // and no further switching is done.
        SetLastInputForMhl(Constants.INVALID_PORT_ID);
    }

    @ServiceThreadOnly
    void SetLastInputForMhl(Int32 portId) {
        AssertRunOnServiceThread();
        mLastInputMhl = portId;
    }

    @ServiceThreadOnly
    Int32 GetLastInputForMhl() {
        AssertRunOnServiceThread();
        return mLastInputMhl;
    }

    /**
     * Performs input change, routing control for MHL device.
     *
     * @param portId MHL port, or the last port to go back to if {@code contentOn} is FALSE
     * @param contentOn {@code TRUE} if RAP data is content on; otherwise FALSE
     */
    @ServiceThreadOnly
    void ChangeInputForMhl(Int32 portId, Boolean contentOn) {
        AssertRunOnServiceThread();
        final Int32 lastInput = contentOn ? Tv()->GetActivePortId() : Constants.INVALID_PORT_ID;
        Tv()->DoManualPortSwitching(portId, new IHdmiControlCallback->Stub() {
            //@Override
            CARAPI OnComplete(Int32 result) throws RemoteException {
                // Keep the last input to switch back later when RAP[ContentOff] is received.
                // This effectively sets the port to invalid one if the switching is for
                // RAP[ContentOff].
                SetLastInputForMhl(lastInput);
            }
        });

        // MHL device is always directly connected to the port. Update the active port ID to avoid
        // unnecessary post-routing control task.
        Tv()->SetActivePortId(portId);

        // The port is either the MHL-enabled port where the mobile device is connected, or
        // the last port to go back to when turnoff command is received. Note that the last port
        // may not be the MHL-enabled one. In this case the device info to be passed to
        // input change listener should be the one describing the corresponding HDMI port.
        HdmiMhlLocalDeviceStub device = mMhlController->GetLocalDevice(portId);
        HdmiDeviceInfo info = (device != NULL && device->GetInfo() != NULL)
                ? device->GetInfo()
                : mPortDeviceMap->Get(portId);
        InvokeInputChangeListener(info);
    }

   void SetMhlInputChangeEnabled(Boolean enabled) {
       mMhlController->SetOption(OPTION_MHL_INPUT_SWITCHING, ToInt(enabled));

        synchronized(mLock) {
            mMhlInputChangeEnabled = enabled;
        }
    }

    Boolean IsMhlInputChangeEnabled() {
        synchronized(mLock) {
            return mMhlInputChangeEnabled;
        }
    }

    @ServiceThreadOnly
    void DisplayOsd(Int32 messageId) {
        AssertRunOnServiceThread();
        Intent intent = new Intent(HdmiControlManager.ACTION_OSD_MESSAGE);
        intent->PutExtra(HdmiControlManager.EXTRA_MESSAGE_ID, messageId);
        GetContext()->SendBroadcastAsUser(intent, UserHandle.ALL,
                HdmiControlService.PERMISSION);
    }

    @ServiceThreadOnly
    void DisplayOsd(Int32 messageId, Int32 extra) {
        AssertRunOnServiceThread();
        Intent intent = new Intent(HdmiControlManager.ACTION_OSD_MESSAGE);
        intent->PutExtra(HdmiControlManager.EXTRA_MESSAGE_ID, messageId);
        intent->PutExtra(HdmiControlManager.EXTRA_MESSAGE_EXTRAM_PARAM1, extra);
        GetContext()->SendBroadcastAsUser(intent, UserHandle.ALL,
                HdmiControlService.PERMISSION);
    }
}
