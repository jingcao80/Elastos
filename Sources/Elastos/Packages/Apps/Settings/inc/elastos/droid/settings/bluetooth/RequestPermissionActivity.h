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

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ILog;

/**
 * RequestPermissionActivity asks the user whether to enable discovery. This is
 * usually started by an application wanted to start bluetooth and or discovery
 */
public class RequestPermissionActivity extends Activity implements
        DialogInterface.OnClickListener {
    // Command line to test this
    // adb shell am start -a android.bluetooth.adapter.action.REQUEST_ENABLE
    // adb shell am start -a android.bluetooth.adapter.action.REQUEST_DISCOVERABLE

    private static const String TAG = "RequestPermissionActivity";

    private static const Int32 MAX_DISCOVERABLE_TIMEOUT = 3600; // 1 hr

    // Non-error return code: BT is starting or has started successfully. Used
    // by this Activity and RequestPermissionHelperActivity
    /* package */ static const Int32 RESULT_BT_STARTING_OR_STARTED = -1000;

    private static const Int32 REQUEST_CODE_START_BT = 1;

    private LocalBluetoothAdapter mLocalAdapter;

    private Int32 mTimeout = BluetoothDiscoverableEnabler.DEFAULT_DISCOVERABLE_TIMEOUT;

    /*
     * True if bluetooth wasn't enabled and RequestPermissionHelperActivity was
     * started to ask the user and start bt.
     *
     * If/when that activity returns successfully, display please wait msg then
     * go away when bt has started and discovery mode has been enabled.
     */
    private Boolean mNeededToEnableBluetooth;

    // True if requesting BT to be turned on
    // False if requesting BT to be turned on + discoverable mode
    private Boolean mEnableOnly;

    private Boolean mUserConfirmed;

    private AlertDialog mDialog;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {

        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (intent == NULL) {
                return;
            }
            if (mNeededToEnableBluetooth
                    && BluetoothAdapter.ACTION_STATE_CHANGED->Equals(intent->GetAction())) {
                Int32 state = intent->GetIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothDevice.ERROR);
                if (state == BluetoothAdapter.STATE_ON) {
                    if (mUserConfirmed) {
                        ProceedAndFinish();
                    }
                }
            }
        }
    };

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        // Note: initializes mLocalAdapter and returns TRUE on error
        if (ParseIntent()) {
            Finish();
            return;
        }

        Int32 btState = mLocalAdapter->GetState();

        switch (btState) {
            case BluetoothAdapter.STATE_OFF:
            case BluetoothAdapter.STATE_TURNING_OFF:
            case BluetoothAdapter.STATE_TURNING_ON:
                /*
                 * Strictly speaking STATE_TURNING_ON belong with STATE_ON;
                 * however, BT may not be ready when the user clicks yes and we
                 * would fail to turn on discovery mode. By kicking this to the
                 * RequestPermissionHelperActivity, this class will handle that
                 * case via the broadcast receiver.
                 */

                /*
                 * Start the helper activity to:
                 * 1) ask the user about enabling bt AND discovery
                 * 2) enable BT upon confirmation
                 */
                RegisterReceiver(mReceiver,
                        new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED));
                Intent intent = new Intent();
                intent->SetClass(this, RequestPermissionHelperActivity.class);
                if (mEnableOnly) {
                    intent->SetAction(RequestPermissionHelperActivity.ACTION_INTERNAL_REQUEST_BT_ON);
                } else {
                    intent->SetAction(RequestPermissionHelperActivity.
                            ACTION_INTERNAL_REQUEST_BT_ON_AND_DISCOVERABLE);
                    intent->PutExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, mTimeout);
                }
                StartActivityForResult(intent, REQUEST_CODE_START_BT);
                mNeededToEnableBluetooth = TRUE;
                break;
            case BluetoothAdapter.STATE_ON:
                if (mEnableOnly) {
                    // Nothing to do. Already enabled.
                    ProceedAndFinish();
                } else {
                    // Ask the user about enabling discovery mode
                    CreateDialog();
                }
                break;
            default:
                Logger::E(TAG, "Unknown adapter state: " + btState);
        }
    }

    private void CreateDialog() {
        AlertDialog.Builder builder = new AlertDialog->Builder(this);

        if (mNeededToEnableBluetooth) {
            // RequestPermissionHelperActivity has gotten confirmation from user
            // to turn on BT
            builder->SetMessage(GetString(R::string::bluetooth_turning_on));
            builder->SetCancelable(FALSE);
        } else {
            // Ask the user whether to turn on discovery mode or not
            // For lasting discoverable mode there is a different message
            if (mTimeout == BluetoothDiscoverableEnabler.DISCOVERABLE_TIMEOUT_NEVER) {
                builder->SetMessage(
                        GetString(R::string::bluetooth_ask_lasting_discovery));
            } else {
                builder->SetMessage(
                        GetString(R::string::bluetooth_ask_discovery, mTimeout));
            }
            builder->SetPositiveButton(GetString(R::string::allow), this);
            builder->SetNegativeButton(GetString(R::string::deny), this);
        }

        mDialog = builder->Create();
        mDialog->Show();

        if (GetResources()->GetBoolean(R.bool.auto_confirm_bluetooth_activation_dialog) == TRUE) {
            // dismiss dialog immediately if settings say so
            OnClick(NULL, DialogInterface.BUTTON_POSITIVE);
        }
    }

    //@Override
    protected void OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        if (requestCode != REQUEST_CODE_START_BT) {
            Logger::E(TAG, "Unexpected onActivityResult " + requestCode + ' ' + resultCode);
            SetResult(RESULT_CANCELED);
            Finish();
            return;
        }
        if (resultCode != RESULT_BT_STARTING_OR_STARTED) {
            SetResult(resultCode);
            Finish();
            return;
        }

        // Back from RequestPermissionHelperActivity. User confirmed to enable
        // BT and discoverable mode.
        mUserConfirmed = TRUE;

        if (mLocalAdapter->GetBluetoothState() == BluetoothAdapter.STATE_ON) {
            ProceedAndFinish();
        } else {
            // If BT is not up yet, show "Turning on Bluetooth..."
            CreateDialog();
        }
    }

    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                ProceedAndFinish();
                break;

            case DialogInterface.BUTTON_NEGATIVE:
                SetResult(RESULT_CANCELED);
                Finish();
                break;
        }
    }

    private void ProceedAndFinish() {
        Int32 returnCode;

        if (mEnableOnly) {
            // BT enabled. Done
            returnCode = RESULT_OK;
        } else if (mLocalAdapter->SetScanMode(
                BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE, mTimeout)) {
            // If already in discoverable mode, this will extend the timeout.
            Int64 endTime = System->CurrentTimeMillis() + (Int64) mTimeout * 1000;
            LocalBluetoothPreferences->PersistDiscoverableEndTimestamp(
                    this, endTime);
            if (0 < mTimeout) {
               BluetoothDiscoverableTimeoutReceiver->SetDiscoverableAlarm(this, endTime);
            }
            returnCode = mTimeout;
            // Activity.RESULT_FIRST_USER should be 1
            if (returnCode < RESULT_FIRST_USER) {
                returnCode = RESULT_FIRST_USER;
            }
        } else {
            returnCode = RESULT_CANCELED;
        }

        if (mDialog != NULL) {
            mDialog->Dismiss();
        }

        SetResult(returnCode);
        Finish();
    }

    /**
     * Parse the received Intent and initialize mLocalBluetoothAdapter.
     * @return TRUE if an error occurred; FALSE otherwise
     */
    private Boolean ParseIntent() {
        Intent intent = GetIntent();
        if (intent != NULL && intent->GetAction()->Equals(BluetoothAdapter.ACTION_REQUEST_ENABLE)) {
            mEnableOnly = TRUE;
        } else if (intent != NULL
                && intent->GetAction()->Equals(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE)) {
            mTimeout = intent->GetIntExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION,
                    BluetoothDiscoverableEnabler.DEFAULT_DISCOVERABLE_TIMEOUT);

            Logger::D(TAG, "Setting Bluetooth Discoverable Timeout = " + mTimeout);

            if (mTimeout < 0 || mTimeout > MAX_DISCOVERABLE_TIMEOUT) {
                mTimeout = BluetoothDiscoverableEnabler.DEFAULT_DISCOVERABLE_TIMEOUT;
            }
        } else {
            Logger::E(TAG, "Error: this activity may be started only with intent "
                    + BluetoothAdapter.ACTION_REQUEST_ENABLE + " or "
                    + BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
            SetResult(RESULT_CANCELED);
            return TRUE;
        }

        LocalBluetoothManager manager = LocalBluetoothManager->GetInstance(this);
        if (manager == NULL) {
            Logger::E(TAG, "Error: there's a problem starting Bluetooth");
            SetResult(RESULT_CANCELED);
            return TRUE;
        }
        mLocalAdapter = manager->GetBluetoothAdapter();

        return FALSE;
    }

    //@Override
    protected void OnDestroy() {
        super->OnDestroy();
        if (mNeededToEnableBluetooth) {
            UnregisterReceiver(mReceiver);
        }
    }

    //@Override
    CARAPI OnBackPressed() {
        SetResult(RESULT_CANCELED);
        super->OnBackPressed();
    }
}
