//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

using Elastos::Droid::Internal::App::IAlertActivity;
using Elastos::Droid::Internal::App::IAlertController;
using Elastos::Droid::Settings::IR;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;

/**
 * RequestPermissionHelperActivity asks the user whether to enable discovery.
 * This is usually started by RequestPermissionActivity.
 */
public class RequestPermissionHelperActivity extends AlertActivity implements
        DialogInterface.OnClickListener {
    private static const String TAG = "RequestPermissionHelperActivity";

    public static const String ACTION_INTERNAL_REQUEST_BT_ON =
        "com.android.settings.bluetooth.ACTION_INTERNAL_REQUEST_BT_ON";

    public static const String ACTION_INTERNAL_REQUEST_BT_ON_AND_DISCOVERABLE =
        "com.android.settings.bluetooth.ACTION_INTERNAL_REQUEST_BT_ON_AND_DISCOVERABLE";

    private LocalBluetoothAdapter mLocalAdapter;

    private Int32 mTimeout;

    // True if requesting BT to be turned on
    // False if requesting BT to be turned on + discoverable mode
    private Boolean mEnableOnly;

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        // Note: initializes mLocalAdapter and returns TRUE on error
        if (ParseIntent()) {
            Finish();
            return;
        }

        CreateDialog();

        if (GetResources()->GetBoolean(R.bool.auto_confirm_bluetooth_activation_dialog) == TRUE) {
            // dismiss dialog immediately if settings say so
            OnClick(NULL, BUTTON_POSITIVE);
            Dismiss();
        }
    }

    void CreateDialog() {
        final AlertController.AlertParams p = mAlertParams;

        if (mEnableOnly) {
            p.mMessage = GetString(R::string::bluetooth_ask_enablement);
        } else {
            if (mTimeout == BluetoothDiscoverableEnabler.DISCOVERABLE_TIMEOUT_NEVER) {
                p.mMessage = GetString(R::string::bluetooth_ask_enablement_and_lasting_discovery);
            } else {
                p.mMessage = GetString(R::string::bluetooth_ask_enablement_and_discovery, mTimeout);
            }
        }

        p.mPositiveButtonText = GetString(R::string::allow);
        p.mPositiveButtonListener = this;
        p.mNegativeButtonText = GetString(R::string::deny);
        p.mNegativeButtonListener = this;

        SetupAlert();
    }

    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        Int32 returnCode;
        // FIXME: fix this ugly switch logic!
        switch (which) {
            case BUTTON_POSITIVE:
                Int32 btState = 0;

                try {
                    // TODO There's a better way.
                    Int32 retryCount = 30;
                    do {
                        btState = mLocalAdapter->GetBluetoothState();
                        Thread->Sleep(100);
                    } while (btState == BluetoothAdapter.STATE_TURNING_OFF && --retryCount > 0);
                } catch (InterruptedException ignored) {
                    // don't care
                }

                if (btState == BluetoothAdapter.STATE_TURNING_ON
                        || btState == BluetoothAdapter.STATE_ON
                        || mLocalAdapter->Enable()) {
                    returnCode = RequestPermissionActivity.RESULT_BT_STARTING_OR_STARTED;
                } else {
                    returnCode = RESULT_CANCELED;
                }
                break;

            case BUTTON_NEGATIVE:
                returnCode = RESULT_CANCELED;
                break;
            default:
                return;
        }
        SetResult(returnCode);
    }

    /**
     * Parse the received Intent and initialize mLocalBluetoothAdapter.
     * @return TRUE if an error occurred; FALSE otherwise
     */
    private Boolean ParseIntent() {
        Intent intent = GetIntent();
        if (intent != NULL && intent->GetAction()->Equals(ACTION_INTERNAL_REQUEST_BT_ON)) {
            mEnableOnly = TRUE;
        } else if (intent != NULL
                && intent->GetAction()->Equals(ACTION_INTERNAL_REQUEST_BT_ON_AND_DISCOVERABLE)) {
            mEnableOnly = FALSE;
            // Value used for display purposes. Not range checking.
            mTimeout = intent->GetIntExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION,
                    BluetoothDiscoverableEnabler.DEFAULT_DISCOVERABLE_TIMEOUT);
        } else {
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
    CARAPI OnBackPressed() {
        SetResult(RESULT_CANCELED);
        super->OnBackPressed();
    }
}
