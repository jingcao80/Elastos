/*
 * Copyright (C) 2010 The Android Open Source Project
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

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IWirelessSettings;
using Elastos::Droid::Settings::Search::IIndex;
using Elastos::Droid::Settings::Widget::ISwitchBar;

/**
 * BluetoothEnabler is a helper to manage the Bluetooth on/off checkbox
 * preference. It turns on/off Bluetooth and ensures the summary of the
 * preference reflects the current state.
 */
public class BluetoothEnabler implements SwitchBar.OnSwitchChangeListener {
    private Context mContext;
    private Switch mSwitch;
    private SwitchBar mSwitchBar;
    private Boolean mValidListener;
    private final LocalBluetoothAdapter mLocalAdapter;
    private final IntentFilter mIntentFilter;

    private static const String EVENT_DATA_IS_BT_ON = "is_bluetooth_on";
    private static const Int32 EVENT_UPDATE_INDEX = 0;

    private Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_UPDATE_INDEX:
                    final Boolean isBluetoothOn = msg->GetData()->GetBoolean(EVENT_DATA_IS_BT_ON);
                    Index->GetInstance(mContext).UpdateFromClassNameResource(
                            BluetoothSettings.class->GetName(), TRUE, isBluetoothOn);
                    break;
            }
        }
    };

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            // Broadcast receiver is always running on the UI thread here,
            // so we don't need consider thread synchronization.
            Int32 state = intent->GetIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
            HandleStateChanged(state);
        }
    };

    public BluetoothEnabler(Context context, SwitchBar switchBar) {
        mContext = context;
        mSwitchBar = switchBar;
        mSwitch = switchBar->GetSwitch();
        mValidListener = FALSE;

        LocalBluetoothManager manager = LocalBluetoothManager->GetInstance(context);
        if (manager == NULL) {
            // Bluetooth is not supported
            mLocalAdapter = NULL;
            mSwitch->SetEnabled(FALSE);
        } else {
            mLocalAdapter = manager->GetBluetoothAdapter();
        }
        mIntentFilter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
    }

    CARAPI SetupSwitchBar() {
        mSwitchBar->Show();
    }

    CARAPI TeardownSwitchBar() {
        mSwitchBar->Hide();
    }

    CARAPI Resume(Context context) {
        if (mLocalAdapter == NULL) {
            mSwitch->SetEnabled(FALSE);
            return;
        }

        if (mContext != context) {
            mContext = context;
        }

        // Bluetooth state is not sticky, so set it manually
        HandleStateChanged(mLocalAdapter->GetBluetoothState());

        mSwitchBar->AddOnSwitchChangeListener(this);
        mContext->RegisterReceiver(mReceiver, mIntentFilter);
        mValidListener = TRUE;
    }

    CARAPI Pause() {
        if (mLocalAdapter == NULL) {
            return;
        }

        mSwitchBar->RemoveOnSwitchChangeListener(this);
        mContext->UnregisterReceiver(mReceiver);
        mValidListener = FALSE;
    }

    void HandleStateChanged(Int32 state) {
        switch (state) {
            case BluetoothAdapter.STATE_TURNING_ON:
                mSwitch->SetEnabled(FALSE);
                break;
            case BluetoothAdapter.STATE_ON:
                SetChecked(TRUE);
                mSwitch->SetEnabled(TRUE);
                UpdateSearchIndex(TRUE);
                break;
            case BluetoothAdapter.STATE_TURNING_OFF:
                mSwitch->SetEnabled(FALSE);
                break;
            case BluetoothAdapter.STATE_OFF:
                SetChecked(FALSE);
                mSwitch->SetEnabled(TRUE);
                UpdateSearchIndex(FALSE);
                break;
            default:
                SetChecked(FALSE);
                mSwitch->SetEnabled(TRUE);
                UpdateSearchIndex(FALSE);
        }
    }

    private void SetChecked(Boolean isChecked) {
        if (isChecked != mSwitch->IsChecked()) {
            // set listener to NULL, so onCheckedChanged won't be called
            // if the checked status on Switch isn't changed by user click
            if (mValidListener) {
                mSwitchBar->RemoveOnSwitchChangeListener(this);
            }
            mSwitch->SetChecked(isChecked);
            if (mValidListener) {
                mSwitchBar->AddOnSwitchChangeListener(this);
            }
        }
    }

    private void UpdateSearchIndex(Boolean isBluetoothOn) {
        mHandler->RemoveMessages(EVENT_UPDATE_INDEX);

        Message msg = new Message();
        msg.what = EVENT_UPDATE_INDEX;
        msg->GetData()->PutBoolean(EVENT_DATA_IS_BT_ON, isBluetoothOn);
        mHandler->SendMessage(msg);
    }

    //@Override
    CARAPI OnSwitchChanged(Switch switchView, Boolean isChecked) {
        // Show toast message if Bluetooth is not allowed in airplane mode
        if (isChecked &&
                !WirelessSettings->IsRadioAllowed(mContext, Settings::Global::RADIO_BLUETOOTH)) {
            Toast->MakeText(mContext, R::string::wifi_in_airplane_mode, Toast.LENGTH_SHORT).Show();
            // Reset switch to off
            switchView->SetChecked(FALSE);
        }

        if (mLocalAdapter != NULL) {
            mLocalAdapter->SetBluetoothEnabled(isChecked);
        }
        mSwitch->SetEnabled(FALSE);
    }
}
