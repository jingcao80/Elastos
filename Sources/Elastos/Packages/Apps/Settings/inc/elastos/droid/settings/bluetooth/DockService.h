/*
 * Copyright (C) 2009 The Android Open Source Project
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

package com.android.settings.bluetooth;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Bluetooth::LocalBluetoothProfileManager::IServiceListener;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IService;
using Elastos::Droid::Bluetooth::IBluetoothA2dp;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;

using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

public class DockService extends Service implements ServiceListener {

    private static const String TAG = "DockService";

    static const Boolean DEBUG = FALSE;

    // Time allowed for the device to be undocked and redocked without severing
    // the bluetooth connection
    private static const Int64 UNDOCKED_GRACE_PERIOD = 1000;

    // Time allowed for the device to be undocked and redocked without turning
    // off Bluetooth
    private static const Int64 DISABLE_BT_GRACE_PERIOD = 2000;

    // Msg for user wanting the UI to setup the dock
    private static const Int32 MSG_TYPE_SHOW_UI = 111;

    // Msg for device docked event
    private static const Int32 MSG_TYPE_DOCKED = 222;

    // Msg for device undocked event
    private static const Int32 MSG_TYPE_UNDOCKED_TEMPORARY = 333;

    // Msg for undocked command to be process after UNDOCKED_GRACE_PERIOD millis
    // since MSG_TYPE_UNDOCKED_TEMPORARY
    private static const Int32 MSG_TYPE_UNDOCKED_PERMANENT = 444;

    // Msg for disabling bt after DISABLE_BT_GRACE_PERIOD millis since
    // MSG_TYPE_UNDOCKED_PERMANENT
    private static const Int32 MSG_TYPE_DISABLE_BT = 555;

    private static const String SHARED_PREFERENCES_NAME = "dock_settings";

    private static const String KEY_DISABLE_BT_WHEN_UNDOCKED = "disable_bt_when_undock";

    private static const String KEY_DISABLE_BT = "disable_bt";

    private static const String KEY_CONNECT_RETRY_COUNT = "connect_retry_count";

    /*
     * If disconnected unexpectedly, reconnect up to 6 times. Each profile counts
     * as one time so it's only 3 times for both profiles on the car dock.
     */
    private static const Int32 MAX_CONNECT_RETRY = 6;

    private static const Int32 INVALID_STARTID = -100;

    // Created in OnCreate()
    private volatile Looper mServiceLooper;
    private volatile ServiceHandler mServiceHandler;
    private Runnable mRunnable;
    private LocalBluetoothAdapter mLocalAdapter;
    private CachedBluetoothDeviceManager mDeviceManager;
    private LocalBluetoothProfileManager mProfileManager;

    // Normally set after getting a docked event and unset when the connection
    // is severed. One exception is that mDevice could be NULL if the service
    // was started after the docked event.
    private BluetoothDevice mDevice;

    // Created and used for the duration of the dialog
    private AlertDialog mDialog;
    private LocalBluetoothProfile[] mProfiles;
    private Boolean[] mCheckedItems;
    private Int32 mStartIdAssociatedWithDialog;

    // Set while BT is being enabled.
    private BluetoothDevice mPendingDevice;
    private Int32 mPendingStartId;
    private Int32 mPendingTurnOnStartId = INVALID_STARTID;
    private Int32 mPendingTurnOffStartId = INVALID_STARTID;

    private CheckBox mAudioMediaCheckbox;

    //@Override
    CARAPI OnCreate() {
        if (DEBUG) Logger::D(TAG, "onCreate");

        LocalBluetoothManager manager = LocalBluetoothManager->GetInstance(this);
        if (manager == NULL) {
            Logger::E(TAG, "Can't get LocalBluetoothManager: exiting");
            return;
        }

        mLocalAdapter = manager->GetBluetoothAdapter();
        mDeviceManager = manager->GetCachedDeviceManager();
        mProfileManager = manager->GetProfileManager();
        if (mProfileManager == NULL) {
            Logger::E(TAG, "Can't get LocalBluetoothProfileManager: exiting");
            return;
        }

        HandlerThread thread = new HandlerThread("DockService");
        thread->Start();

        mServiceLooper = thread->GetLooper();
        mServiceHandler = new ServiceHandler(mServiceLooper);
    }

    //@Override
    CARAPI OnDestroy() {
        if (DEBUG) Logger::D(TAG, "onDestroy");
        mRunnable = NULL;
        if (mDialog != NULL) {
            mDialog->Dismiss();
            mDialog = NULL;
        }
        if (mProfileManager != NULL) {
            mProfileManager->RemoveServiceListener(this);
        }
        if (mServiceLooper != NULL) {
            mServiceLooper->Quit();
        }

        mLocalAdapter = NULL;
        mDeviceManager = NULL;
        mProfileManager = NULL;
        mServiceLooper = NULL;
        mServiceHandler = NULL;
    }

    //@Override
    public IBinder OnBind(Intent intent) {
        // not supported
        return NULL;
    }

    private SharedPreferences GetPrefs() {
        return GetSharedPreferences(SHARED_PREFERENCES_NAME, MODE_PRIVATE);
    }

    //@Override
    public Int32 OnStartCommand(Intent intent, Int32 flags, Int32 startId) {
        if (DEBUG) Logger::D(TAG, "onStartCommand startId: " + startId + " flags: " + flags);

        if (intent == NULL) {
            // Nothing to process, stop.
            if (DEBUG) Logger::D(TAG, "START_NOT_STICKY - intent is NULL.");

            // NOTE: We MUST not call StopSelf() directly, since we need to
            // make sure the wake lock acquired by the Receiver is released.
            DockEventReceiver->FinishStartingService(this, startId);
            return START_NOT_STICKY;
        }

        if (BluetoothAdapter.ACTION_STATE_CHANGED->Equals(intent->GetAction())) {
            HandleBtStateChange(intent, startId);
            return START_NOT_STICKY;
        }

        /*
         * This assumes that the intent sender has checked that this is a dock
         * and that the intent is for a disconnect
         */
        final SharedPreferences prefs = GetPrefs();
        if (BluetoothHeadset.ACTION_CONNECTION_STATE_CHANGED->Equals(intent->GetAction())) {
            BluetoothDevice disconnectedDevice = intent
                    .GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            Int32 retryCount = prefs->GetInt(KEY_CONNECT_RETRY_COUNT, 0);
            if (retryCount < MAX_CONNECT_RETRY) {
                prefs->Edit()->PutInt(KEY_CONNECT_RETRY_COUNT, retryCount + 1).Apply();
                HandleUnexpectedDisconnect(disconnectedDevice, mProfileManager->GetHeadsetProfile(), startId);
            }
            return START_NOT_STICKY;
        } else if (BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED->Equals(intent->GetAction())) {
            BluetoothDevice disconnectedDevice = intent
                    .GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            Int32 retryCount = prefs->GetInt(KEY_CONNECT_RETRY_COUNT, 0);
            if (retryCount < MAX_CONNECT_RETRY) {
                prefs->Edit()->PutInt(KEY_CONNECT_RETRY_COUNT, retryCount + 1).Apply();
                HandleUnexpectedDisconnect(disconnectedDevice, mProfileManager->GetA2dpProfile(), startId);
            }
            return START_NOT_STICKY;
        }

        Message msg = ParseIntent(intent);
        if (msg == NULL) {
            // Bad intent
            if (DEBUG) Logger::D(TAG, "START_NOT_STICKY - Bad intent.");
            DockEventReceiver->FinishStartingService(this, startId);
            return START_NOT_STICKY;
        }

        if (msg.what == MSG_TYPE_DOCKED) {
            prefs->Edit()->Remove(KEY_CONNECT_RETRY_COUNT).Apply();
        }

        msg.arg2 = startId;
        ProcessMessage(msg);

        return START_NOT_STICKY;
    }

    private final class ServiceHandler extends Handler {
        private ServiceHandler(Looper looper) {
            Super(looper);
        }

        //@Override
        CARAPI HandleMessage(Message msg) {
            ProcessMessage(msg);
        }
    }

    // This method gets messages from both onStartCommand and mServiceHandler/mServiceLooper
    private synchronized void ProcessMessage(Message msg) {
        Int32 msgType = msg.what;
        final Int32 state = msg.arg1;
        final Int32 startId = msg.arg2;
        BluetoothDevice device = NULL;
        if (msg.obj != NULL) {
            device = (BluetoothDevice) msg.obj;
        }

        If(DEBUG) Logger::D(TAG, "processMessage: " + msgType + " state: " + state + " device = "
                + (device == NULL ? "NULL" : device->ToString()));

        Boolean deferFinishCall = FALSE;

        switch (msgType) {
            case MSG_TYPE_SHOW_UI:
                if (device != NULL) {
                    CreateDialog(device, state, startId);
                }
                break;

            case MSG_TYPE_DOCKED:
                deferFinishCall = MsgTypeDocked(device, state, startId);
                break;

            case MSG_TYPE_UNDOCKED_PERMANENT:
                deferFinishCall = MsgTypeUndockedPermanent(device, startId);
                break;

            case MSG_TYPE_UNDOCKED_TEMPORARY:
                MsgTypeUndockedTemporary(device, state, startId);
                break;

            case MSG_TYPE_DISABLE_BT:
                deferFinishCall = MsgTypeDisableBluetooth(startId);
                break;
        }

        if (mDialog == NULL && mPendingDevice == NULL && msgType != MSG_TYPE_UNDOCKED_TEMPORARY
                && !deferFinishCall) {
            // NOTE: We MUST not call StopSelf() directly, since we need to
            // make sure the wake lock acquired by the Receiver is released.
            DockEventReceiver->FinishStartingService(this, startId);
        }
    }

    private Boolean MsgTypeDisableBluetooth(Int32 startId) {
        if (DEBUG) {
            Logger::D(TAG, "BT DISABLE");
        }
        final SharedPreferences prefs = GetPrefs();
        if (mLocalAdapter->Disable()) {
            prefs->Edit()->Remove(KEY_DISABLE_BT_WHEN_UNDOCKED).Apply();
            return FALSE;
        } else {
            // Disable() returned an error. Persist a flag to disable BT later
            prefs->Edit()->PutBoolean(KEY_DISABLE_BT, TRUE).Apply();
            mPendingTurnOffStartId = startId;
            If(DEBUG) {
                Logger::D(TAG, "disable failed. try again later " + startId);
            }
            return TRUE;
        }
    }

    private void MsgTypeUndockedTemporary(BluetoothDevice device, Int32 state,
            Int32 startId) {
        // Undocked event received. Queue a delayed msg to sever connection
        Message newMsg = mServiceHandler->ObtainMessage(MSG_TYPE_UNDOCKED_PERMANENT, state,
                startId, device);
        mServiceHandler->SendMessageDelayed(newMsg, UNDOCKED_GRACE_PERIOD);
    }

    private Boolean MsgTypeUndockedPermanent(BluetoothDevice device, Int32 startId) {
        // Grace period passed. Disconnect.
        HandleUndocked(device);
        if (device != NULL) {
            final SharedPreferences prefs = GetPrefs();

            if (DEBUG) {
                Logger::D(TAG, "DISABLE_BT_WHEN_UNDOCKED = "
                        + prefs->GetBoolean(KEY_DISABLE_BT_WHEN_UNDOCKED, FALSE));
            }

            if (prefs->GetBoolean(KEY_DISABLE_BT_WHEN_UNDOCKED, FALSE)) {
                if (HasOtherConnectedDevices(device)) {
                    // Don't disable BT if something is connected
                    prefs->Edit()->Remove(KEY_DISABLE_BT_WHEN_UNDOCKED).Apply();
                } else {
                    // BT was disabled when we first docked
                    if (DEBUG) {
                        Logger::D(TAG, "QUEUED BT DISABLE");
                    }
                    // Queue a delayed msg to disable BT
                    Message newMsg = mServiceHandler->ObtainMessage(
                            MSG_TYPE_DISABLE_BT, 0, startId, NULL);
                    mServiceHandler->SendMessageDelayed(newMsg,
                            DISABLE_BT_GRACE_PERIOD);
                    return TRUE;
                }
            }
        }
        return FALSE;
    }

    private Boolean MsgTypeDocked(BluetoothDevice device, final Int32 state,
            final Int32 startId) {
        if (DEBUG) {
            // TODO figure out why hasMsg always returns FALSE if device
            // is supplied
            Logger::D(TAG, "1 Has undock perm msg = "
                    + mServiceHandler->HasMessages(MSG_TYPE_UNDOCKED_PERMANENT, mDevice));
            Logger::D(TAG, "2 Has undock perm msg = "
                    + mServiceHandler->HasMessages(MSG_TYPE_UNDOCKED_PERMANENT, device));
        }

        mServiceHandler->RemoveMessages(MSG_TYPE_UNDOCKED_PERMANENT);
        mServiceHandler->RemoveMessages(MSG_TYPE_DISABLE_BT);
        GetPrefs()->Edit().Remove(KEY_DISABLE_BT).Apply();

        if (device != NULL) {
            if (!device->Equals(mDevice)) {
                if (mDevice != NULL) {
                    // Not expected. Cleanup/undock existing
                    HandleUndocked(mDevice);
                }

                mDevice = device;

                // Register first in case LocalBluetoothProfileManager
                // becomes ready after isManagerReady is called and it
                // would be too late to register a service listener.
                mProfileManager->AddServiceListener(this);
                if (mProfileManager->IsManagerReady()) {
                    HandleDocked(device, state, startId);
                    // Not needed after all
                    mProfileManager->RemoveServiceListener(this);
                } else {
                    final BluetoothDevice d = device;
                    mRunnable = new Runnable() {
                        CARAPI Run() {
                            HandleDocked(d, state, startId);  // FIXME: WTF runnable here?
                        }
                    };
                    return TRUE;
                }
            }
        } else {
            // display dialog to enable dock for media audio only in the case of low end docks and
            // if not already selected by user
            Int32 dockAudioMediaEnabled = Settings::Global::>GetInt(GetContentResolver(),
                    Settings::Global::DOCK_AUDIO_MEDIA_ENABLED, -1);
            if (dockAudioMediaEnabled == -1 &&
                    state == Intent.EXTRA_DOCK_STATE_LE_DESK) {
                HandleDocked(NULL, state, startId);
                return TRUE;
            }
        }
        return FALSE;
    }

    synchronized Boolean HasOtherConnectedDevices(BluetoothDevice dock) {
        Collection<CachedBluetoothDevice> cachedDevices = mDeviceManager->GetCachedDevicesCopy();
        Set<BluetoothDevice> btDevices = mLocalAdapter->GetBondedDevices();
        if (btDevices == NULL || cachedDevices == NULL || btDevices->IsEmpty()) {
            return FALSE;
        }
        If(DEBUG) {
            Logger::D(TAG, "btDevices = " + btDevices->Size());
            Logger::D(TAG, "cachedDeviceUIs = " + cachedDevices->Size());
        }

        for (CachedBluetoothDevice deviceUI : cachedDevices) {
            BluetoothDevice btDevice = deviceUI->GetDevice();
            if (!btDevice->Equals(dock) && btDevices->Contains(btDevice) && deviceUI
                    .IsConnected()) {
                If(DEBUG) Logger::D(TAG, "connected deviceUI = " + deviceUI->GetName());
                return TRUE;
            }
        }
        return FALSE;
    }

    private Message ParseIntent(Intent intent) {
        BluetoothDevice device = intent->GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
        Int32 state = intent->GetIntExtra(Intent.EXTRA_DOCK_STATE, -1234);

        if (DEBUG) {
            Logger::D(TAG, "Action: " + intent->GetAction() + " State:" + state
                    + " Device: " + (device == NULL ? "NULL" : device->GetAliasName()));
        }

        Int32 msgType;
        switch (state) {
            case Intent.EXTRA_DOCK_STATE_UNDOCKED:
                msgType = MSG_TYPE_UNDOCKED_TEMPORARY;
                break;
            case Intent.EXTRA_DOCK_STATE_DESK:
            case Intent.EXTRA_DOCK_STATE_HE_DESK:
            case Intent.EXTRA_DOCK_STATE_CAR:
                if (device == NULL) {
                    Logger::W(TAG, "device is NULL");
                    return NULL;
                }
                /// Fall Through ///
            case Intent.EXTRA_DOCK_STATE_LE_DESK:
                if (DockEventReceiver.ACTION_DOCK_SHOW_UI->Equals(intent->GetAction())) {
                    if (device == NULL) {
                        Logger::W(TAG, "device is NULL");
                        return NULL;
                    }
                    msgType = MSG_TYPE_SHOW_UI;
                } else {
                    msgType = MSG_TYPE_DOCKED;
                }
                break;
            default:
                return NULL;
        }

        return mServiceHandler->ObtainMessage(msgType, state, 0, device);
    }

    private void CreateDialog(BluetoothDevice device,
            Int32 state, Int32 startId) {
        if (mDialog != NULL) {
            // Shouldn't normally happen
            mDialog->Dismiss();
            mDialog = NULL;
        }
        mDevice = device;
        switch (state) {
            case Intent.EXTRA_DOCK_STATE_CAR:
            case Intent.EXTRA_DOCK_STATE_DESK:
            case Intent.EXTRA_DOCK_STATE_LE_DESK:
            case Intent.EXTRA_DOCK_STATE_HE_DESK:
                break;
            default:
                return;
        }

        StartForeground(0, new Notification());

        final AlertDialog.Builder ab = new AlertDialog->Builder(this);
        View view;
        LayoutInflater inflater = (LayoutInflater)GetSystemService(LAYOUT_INFLATER_SERVICE);

        mAudioMediaCheckbox = NULL;

        if (device != NULL) {
            // Device in a new dock.
            Boolean firstTime =
                    !LocalBluetoothPreferences->HasDockAutoConnectSetting(this, device->GetAddress());

            CharSequence[] items = InitBtSettings(device, state, firstTime);

            ab->SetTitle(GetString(R::string::bluetooth_dock_settings_title));

            // Profiles
            ab->SetMultiChoiceItems(items, mCheckedItems, mMultiClickListener);

            // Remember this settings
            view = inflater->Inflate(R.layout.remember_dock_setting, NULL);
            CheckBox rememberCheckbox = (CheckBox) view->FindViewById(R.id.remember);

            // check "Remember setting" by default if no value was saved
            Boolean checked = firstTime ||
                    LocalBluetoothPreferences->GetDockAutoConnectSetting(this, device->GetAddress());
            rememberCheckbox->SetChecked(checked);
            rememberCheckbox->SetOnCheckedChangeListener(mCheckedChangeListener);
            if (DEBUG) {
                Logger::D(TAG, "Auto connect = "
                  + LocalBluetoothPreferences->GetDockAutoConnectSetting(this, device->GetAddress()));
            }
        } else {
            ab->SetTitle(GetString(R::string::bluetooth_dock_settings_title));

            view = inflater->Inflate(R.layout.dock_audio_media_enable_dialog, NULL);
            mAudioMediaCheckbox =
                    (CheckBox) view->FindViewById(R.id.dock_audio_media_enable_cb);

            Boolean checked = Settings::Global::>GetInt(GetContentResolver(),
                                    Settings::Global::DOCK_AUDIO_MEDIA_ENABLED, 0) == 1;

            mAudioMediaCheckbox->SetChecked(checked);
            mAudioMediaCheckbox->SetOnCheckedChangeListener(mCheckedChangeListener);
        }

        Float pixelScaleFactor = GetResources()->GetDisplayMetrics().density;
        Int32 viewSpacingLeft = (Int32) (14 * pixelScaleFactor);
        Int32 viewSpacingRight = (Int32) (14 * pixelScaleFactor);
        ab->SetView(view, viewSpacingLeft, 0 /* top */, viewSpacingRight, 0 /* bottom */);

        // Ok Button
        ab->SetPositiveButton(GetString(android.R::string::ok), mClickListener);

        mStartIdAssociatedWithDialog = startId;
        mDialog = ab->Create();
        mDialog->GetWindow()->SetType(WindowManager.LayoutParams.TYPE_KEYGUARD_DIALOG);
        mDialog->SetOnDismissListener(mDismissListener);
        mDialog->Show();
    }

    // Called when the individual bt profiles are clicked.
    private final DialogInterface.OnMultiChoiceClickListener mMultiClickListener =
            new DialogInterface->OnMultiChoiceClickListener() {
                CARAPI OnClick(DialogInterface dialog, Int32 which, Boolean isChecked) {
                    if (DEBUG) {
                        Logger::D(TAG, "Item " + which + " changed to " + isChecked);
                    }
                    mCheckedItems[which] = isChecked;
                }
            };


    // Called when the "Remember" Checkbox is clicked
    private final CompoundButton.OnCheckedChangeListener mCheckedChangeListener =
            new CompoundButton->OnCheckedChangeListener() {
                CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
                    if (DEBUG) {
                        Logger::D(TAG, "onCheckedChanged: Remember Settings = " + isChecked);
                    }
                    if (mDevice != NULL) {
                        LocalBluetoothPreferences->SaveDockAutoConnectSetting(
                                DockService.this, mDevice->GetAddress(), isChecked);
                    } else {
                        Settings::Global::>PutInt(GetContentResolver(),
                                Settings::Global::DOCK_AUDIO_MEDIA_ENABLED, isChecked ? 1 : 0);
                    }
                }
            };


    // Called when the dialog is dismissed
    private final DialogInterface.OnDismissListener mDismissListener =
            new DialogInterface->OnDismissListener() {
                CARAPI OnDismiss(DialogInterface dialog) {
                    // NOTE: We MUST not call StopSelf() directly, since we need to
                    // make sure the wake lock acquired by the Receiver is released.
                    if (mPendingDevice == NULL) {
                        DockEventReceiver->FinishStartingService(
                                DockService.this, mStartIdAssociatedWithDialog);
                    }
                    StopForeground(TRUE);
                }
            };

    // Called when clicked on the OK button
    private final DialogInterface.OnClickListener mClickListener =
            new DialogInterface->OnClickListener() {
                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                    if (which == DialogInterface.BUTTON_POSITIVE) {
                        if (mDevice != NULL) {
                            if (!LocalBluetoothPreferences
                                    .HasDockAutoConnectSetting(
                                            DockService.this,
                                            mDevice->GetAddress())) {
                                LocalBluetoothPreferences
                                        .SaveDockAutoConnectSetting(
                                                DockService.this,
                                                mDevice->GetAddress(), TRUE);
                            }

                            ApplyBtSettings(mDevice, mStartIdAssociatedWithDialog);
                        } else if (mAudioMediaCheckbox != NULL) {
                            Settings::Global::>PutInt(GetContentResolver(),
                                    Settings::Global::DOCK_AUDIO_MEDIA_ENABLED,
                                    mAudioMediaCheckbox->IsChecked() ? 1 : 0);
                        }
                    }
                }
            };

    private CharSequence[] InitBtSettings(BluetoothDevice device,
            Int32 state, Boolean firstTime) {
        // TODO Avoid hardcoding dock and profiles. Read from system properties
        Int32 numOfProfiles;
        switch (state) {
            case Intent.EXTRA_DOCK_STATE_DESK:
            case Intent.EXTRA_DOCK_STATE_LE_DESK:
            case Intent.EXTRA_DOCK_STATE_HE_DESK:
                numOfProfiles = 1;
                break;
            case Intent.EXTRA_DOCK_STATE_CAR:
                numOfProfiles = 2;
                break;
            default:
                return NULL;
        }

        mProfiles = new LocalBluetoothProfile[numOfProfiles];
        mCheckedItems = new Boolean[numOfProfiles];
        CharSequence[] items = new CharSequence[numOfProfiles];

        // FIXME: convert switch to something else
        switch (state) {
            case Intent.EXTRA_DOCK_STATE_CAR:
                items[0] = GetString(R::string::bluetooth_dock_settings_headset);
                items[1] = GetString(R::string::bluetooth_dock_settings_a2dp);
                mProfiles[0] = mProfileManager->GetHeadsetProfile();
                mProfiles[1] = mProfileManager->GetA2dpProfile();
                if (firstTime) {
                    // Enable by default for car dock
                    mCheckedItems[0] = TRUE;
                    mCheckedItems[1] = TRUE;
                } else {
                    mCheckedItems[0] = mProfiles[0].IsPreferred(device);
                    mCheckedItems[1] = mProfiles[1].IsPreferred(device);
                }
                break;

            case Intent.EXTRA_DOCK_STATE_DESK:
            case Intent.EXTRA_DOCK_STATE_LE_DESK:
            case Intent.EXTRA_DOCK_STATE_HE_DESK:
                items[0] = GetString(R::string::bluetooth_dock_settings_a2dp);
                mProfiles[0] = mProfileManager->GetA2dpProfile();
                if (firstTime) {
                    // Disable by default for desk dock
                    mCheckedItems[0] = FALSE;
                } else {
                    mCheckedItems[0] = mProfiles[0].IsPreferred(device);
                }
                break;
        }
        return items;
    }

    // TODO: move to background thread to fix strict mode warnings
    private void HandleBtStateChange(Intent intent, Int32 startId) {
        Int32 btState = intent->GetIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
        synchronized(this) {
            If(DEBUG) Logger::D(TAG, "BtState = " + btState + " mPendingDevice = " + mPendingDevice);
            if (btState == BluetoothAdapter.STATE_ON) {
                HandleBluetoothStateOn(startId);
            } else if (btState == BluetoothAdapter.STATE_TURNING_OFF) {
                // Remove the flag to disable BT if someone is turning off bt.
                // The rational is that:
                // a) if BT is off at undock time, no work needs to be done
                // b) if BT is on at undock time, the user wants it on.
                GetPrefs()->Edit().Remove(KEY_DISABLE_BT_WHEN_UNDOCKED).Apply();
                DockEventReceiver->FinishStartingService(this, startId);
            } else if (btState == BluetoothAdapter.STATE_OFF) {
                // Bluetooth was turning off as we were trying to turn it on.
                // Let's try again
                If(DEBUG) Logger::D(TAG, "Bluetooth = OFF mPendingDevice = " + mPendingDevice);

                if (mPendingTurnOffStartId != INVALID_STARTID) {
                    DockEventReceiver->FinishStartingService(this, mPendingTurnOffStartId);
                    GetPrefs()->Edit().Remove(KEY_DISABLE_BT).Apply();
                    mPendingTurnOffStartId = INVALID_STARTID;
                }

                if (mPendingDevice != NULL) {
                    mLocalAdapter->Enable();
                    mPendingTurnOnStartId = startId;
                } else {
                    DockEventReceiver->FinishStartingService(this, startId);
                }
            }
        }
    }

    private void HandleBluetoothStateOn(Int32 startId) {
        if (mPendingDevice != NULL) {
            if (mPendingDevice->Equals(mDevice)) {
                If(DEBUG) {
                    Logger::D(TAG, "applying settings");
                }
                ApplyBtSettings(mPendingDevice, mPendingStartId);
            } else If(DEBUG) {
                Logger::D(TAG, "mPendingDevice  (" + mPendingDevice + ") != mDevice ("
                        + mDevice + ')');
            }

            mPendingDevice = NULL;
            DockEventReceiver->FinishStartingService(this, mPendingStartId);
        } else {
            final SharedPreferences prefs = GetPrefs();
            if (DEBUG) {
                Logger::D(TAG, "A DISABLE_BT_WHEN_UNDOCKED = "
                        + prefs->GetBoolean(KEY_DISABLE_BT_WHEN_UNDOCKED, FALSE));
            }
            // Reconnect if docked and bluetooth was enabled by user.
            Intent i = RegisterReceiver(NULL, new IntentFilter(IIntent::ACTION_DOCK_EVENT));
            if (i != NULL) {
                Int32 state = i->GetIntExtra(Intent.EXTRA_DOCK_STATE,
                        Intent.EXTRA_DOCK_STATE_UNDOCKED);
                if (state != Intent.EXTRA_DOCK_STATE_UNDOCKED) {
                    BluetoothDevice device = i
                            .GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    if (device != NULL) {
                        ConnectIfEnabled(device);
                    }
                } else if (prefs->GetBoolean(KEY_DISABLE_BT, FALSE)
                        && mLocalAdapter->Disable()) {
                    mPendingTurnOffStartId = startId;
                    prefs->Edit()->Remove(KEY_DISABLE_BT).Apply();
                    return;
                }
            }
        }

        if (mPendingTurnOnStartId != INVALID_STARTID) {
            DockEventReceiver->FinishStartingService(this, mPendingTurnOnStartId);
            mPendingTurnOnStartId = INVALID_STARTID;
        }

        DockEventReceiver->FinishStartingService(this, startId);
    }

    private synchronized void HandleUnexpectedDisconnect(BluetoothDevice disconnectedDevice,
            LocalBluetoothProfile profile, Int32 startId) {
        if (DEBUG) {
            Logger::D(TAG, "handling failed connect for " + disconnectedDevice);
        }

            // Reconnect if docked.
            if (disconnectedDevice != NULL) {
                // registerReceiver can't be called from a BroadcastReceiver
                Intent intent = RegisterReceiver(NULL, new IntentFilter(IIntent::ACTION_DOCK_EVENT));
                if (intent != NULL) {
                    Int32 state = intent->GetIntExtra(Intent.EXTRA_DOCK_STATE,
                            Intent.EXTRA_DOCK_STATE_UNDOCKED);
                    if (state != Intent.EXTRA_DOCK_STATE_UNDOCKED) {
                        BluetoothDevice dockedDevice = intent
                                .GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                        if (dockedDevice != NULL && dockedDevice->Equals(disconnectedDevice)) {
                            CachedBluetoothDevice cachedDevice = GetCachedBluetoothDevice(
                                    dockedDevice);
                            cachedDevice->ConnectProfile(profile);
                        }
                    }
                }
            }

            DockEventReceiver->FinishStartingService(this, startId);
    }

    private synchronized void ConnectIfEnabled(BluetoothDevice device) {
        CachedBluetoothDevice cachedDevice = GetCachedBluetoothDevice(
                device);
        List<LocalBluetoothProfile> profiles = cachedDevice->GetConnectableProfiles();
        for (LocalBluetoothProfile profile : profiles) {
            if (profile->GetPreferred(device) == BluetoothProfile.PRIORITY_AUTO_CONNECT) {
                cachedDevice->Connect(FALSE);
                return;
            }
        }
    }

    private synchronized void ApplyBtSettings(BluetoothDevice device, Int32 startId) {
        if (device == NULL || mProfiles == NULL || mCheckedItems == NULL
                || mLocalAdapter == NULL) {
            return;
        }

        // Turn on BT if something is enabled
        for (Boolean enable : mCheckedItems) {
            if (enable) {
                Int32 btState = mLocalAdapter->GetBluetoothState();
                if (DEBUG) {
                    Logger::D(TAG, "BtState = " + btState);
                }
                // May have race condition as the phone comes in and out and in the dock.
                // Always turn on BT
                mLocalAdapter->Enable();

                // if adapter was previously OFF, TURNING_OFF, or TURNING_ON
                if (btState != BluetoothAdapter.STATE_ON) {
                    if (mPendingDevice != NULL && mPendingDevice->Equals(mDevice)) {
                        return;
                    }

                    mPendingDevice = device;
                    mPendingStartId = startId;
                    if (btState != BluetoothAdapter.STATE_TURNING_ON) {
                        GetPrefs()->Edit().PutBoolean(
                                KEY_DISABLE_BT_WHEN_UNDOCKED, TRUE).Apply();
                    }
                    return;
                }
            }
        }

        mPendingDevice = NULL;

        Boolean callConnect = FALSE;
        CachedBluetoothDevice cachedDevice = GetCachedBluetoothDevice(
                device);
        for (Int32 i = 0; i < mProfiles.length; i++) {
            LocalBluetoothProfile profile = mProfiles[i];
            if (DEBUG) Logger::D(TAG, profile->ToString() + " = " + mCheckedItems[i]);

            if (mCheckedItems[i]) {
                // Checked but not connected
                callConnect = TRUE;
            } else if (!mCheckedItems[i]) {
                // Unchecked, may or may not be connected.
                Int32 status = profile->GetConnectionStatus(cachedDevice->GetDevice());
                if (status == BluetoothProfile.STATE_CONNECTED) {
                    if (DEBUG) Logger::D(TAG, "applyBtSettings - Disconnecting");
                    cachedDevice->Disconnect(mProfiles[i]);
                }
            }
            profile->SetPreferred(device, mCheckedItems[i]);
            if (DEBUG) {
                if (mCheckedItems[i] != profile->IsPreferred(device)) {
                    Logger::E(TAG, "Can't save preferred value");
                }
            }
        }

        if (callConnect) {
            if (DEBUG) Logger::D(TAG, "applyBtSettings - Connecting");
            cachedDevice->Connect(FALSE);
        }
    }

    private synchronized void HandleDocked(BluetoothDevice device, Int32 state,
            Int32 startId) {
        if (device != NULL &&
                LocalBluetoothPreferences->GetDockAutoConnectSetting(this, device->GetAddress())) {
            // Setting == auto connect
            InitBtSettings(device, state, FALSE);
            ApplyBtSettings(mDevice, startId);
        } else {
            CreateDialog(device, state, startId);
        }
    }

    private synchronized void HandleUndocked(BluetoothDevice device) {
        mRunnable = NULL;
        mProfileManager->RemoveServiceListener(this);
        if (mDialog != NULL) {
            mDialog->Dismiss();
            mDialog = NULL;
        }
        mDevice = NULL;
        mPendingDevice = NULL;
        if (device != NULL) {
            CachedBluetoothDevice cachedDevice = GetCachedBluetoothDevice(device);
            cachedDevice->Disconnect();
        }
    }

    private CachedBluetoothDevice GetCachedBluetoothDevice(BluetoothDevice device) {
        CachedBluetoothDevice cachedDevice = mDeviceManager->FindDevice(device);
        if (cachedDevice == NULL) {
            cachedDevice = mDeviceManager->AddDevice(mLocalAdapter, mProfileManager, device);
        }
        return cachedDevice;
    }

    public synchronized void OnServiceConnected() {
        if (mRunnable != NULL) {
            mRunnable->Run();
            mRunnable = NULL;
            mProfileManager->RemoveServiceListener(this);
        }
    }

    CARAPI OnServiceDisconnected() {
        // FIXME: shouldn't I do something on service disconnected too?
    }
}
