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

package com.android.settings.wifi;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::Wifi::ISupplicantState;
using Elastos::Droid::Net::Wifi::IWifiInfo;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IWirelessSettings;
using Elastos::Droid::Settings::Search::IIndex;
using Elastos::Droid::Settings::Widget::ISwitchBar;

using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

public class WifiEnabler implements SwitchBar.OnSwitchChangeListener  {
    private Context mContext;
    private SwitchBar mSwitchBar;
    private Boolean mListeningToOnSwitchChange = FALSE;
    private AtomicBoolean mConnected = new AtomicBoolean(FALSE);

    private final WifiManager mWifiManager;
    private Boolean mStateMachineEvent;
    private final IntentFilter mIntentFilter;
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (WifiManager.WIFI_STATE_CHANGED_ACTION->Equals(action)) {
                HandleWifiStateChanged(intent->GetIntExtra(
                        WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_UNKNOWN));
            } else if (WifiManager.SUPPLICANT_STATE_CHANGED_ACTION->Equals(action)) {
                if (!mConnected->Get()) {
                    HandleStateChanged(WifiInfo->GetDetailedStateOf((SupplicantState)
                            intent->GetParcelableExtra(WifiManager.EXTRA_NEW_STATE)));
                }
            } else if (WifiManager.NETWORK_STATE_CHANGED_ACTION->Equals(action)) {
                NetworkInfo info = (NetworkInfo) intent->GetParcelableExtra(
                        WifiManager.EXTRA_NETWORK_INFO);
                mConnected->Set(info->IsConnected());
                HandleStateChanged(info->GetDetailedState());
            }
        }
    };

    private static const String EVENT_DATA_IS_WIFI_ON = "is_wifi_on";
    private static const Int32 EVENT_UPDATE_INDEX = 0;

    private Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_UPDATE_INDEX:
                    final Boolean isWiFiOn = msg->GetData()->GetBoolean(EVENT_DATA_IS_WIFI_ON);
                    Index->GetInstance(mContext).UpdateFromClassNameResource(
                            WifiSettings.class->GetName(), TRUE, isWiFiOn);
                    break;
            }
        }
    };

    public WifiEnabler(Context context, SwitchBar switchBar) {
        mContext = context;
        mSwitchBar = switchBar;

        mWifiManager = (WifiManager) context->GetSystemService(Context.WIFI_SERVICE);

        mIntentFilter = new IntentFilter(WifiManager.WIFI_STATE_CHANGED_ACTION);
        // The order matters! We really should not depend on this. :(
        mIntentFilter->AddAction(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION);
        mIntentFilter->AddAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);

        SetupSwitchBar();
    }

    CARAPI SetupSwitchBar() {
        final Int32 state = mWifiManager->GetWifiState();
        HandleWifiStateChanged(state);
        if (!mListeningToOnSwitchChange) {
            mSwitchBar->AddOnSwitchChangeListener(this);
            mListeningToOnSwitchChange = TRUE;
        }
        mSwitchBar->Show();
    }

    CARAPI TeardownSwitchBar() {
        if (mListeningToOnSwitchChange) {
            mSwitchBar->RemoveOnSwitchChangeListener(this);
            mListeningToOnSwitchChange = FALSE;
        }
        mSwitchBar->Hide();
    }

    CARAPI Resume(Context context) {
        mContext = context;
        // Wi-Fi state is sticky, so just let the receiver update UI
        mContext->RegisterReceiver(mReceiver, mIntentFilter);
        if (!mListeningToOnSwitchChange) {
            mSwitchBar->AddOnSwitchChangeListener(this);
            mListeningToOnSwitchChange = TRUE;
        }
    }

    CARAPI Pause() {
        mContext->UnregisterReceiver(mReceiver);
        if (mListeningToOnSwitchChange) {
            mSwitchBar->RemoveOnSwitchChangeListener(this);
            mListeningToOnSwitchChange = FALSE;
        }
    }

    private void HandleWifiStateChanged(Int32 state) {
        switch (state) {
            case WifiManager.WIFI_STATE_ENABLING:
                mSwitchBar->SetEnabled(FALSE);
                break;
            case WifiManager.WIFI_STATE_ENABLED:
                SetSwitchBarChecked(TRUE);
                mSwitchBar->SetEnabled(TRUE);
                UpdateSearchIndex(TRUE);
                break;
            case WifiManager.WIFI_STATE_DISABLING:
                mSwitchBar->SetEnabled(FALSE);
                break;
            case WifiManager.WIFI_STATE_DISABLED:
                SetSwitchBarChecked(FALSE);
                mSwitchBar->SetEnabled(TRUE);
                UpdateSearchIndex(FALSE);
                break;
            default:
                SetSwitchBarChecked(FALSE);
                mSwitchBar->SetEnabled(TRUE);
                UpdateSearchIndex(FALSE);
        }
    }

    private void UpdateSearchIndex(Boolean isWiFiOn) {
        mHandler->RemoveMessages(EVENT_UPDATE_INDEX);

        Message msg = new Message();
        msg.what = EVENT_UPDATE_INDEX;
        msg->GetData()->PutBoolean(EVENT_DATA_IS_WIFI_ON, isWiFiOn);
        mHandler->SendMessage(msg);
    }

    private void SetSwitchBarChecked(Boolean checked) {
        mStateMachineEvent = TRUE;
        mSwitchBar->SetChecked(checked);
        mStateMachineEvent = FALSE;
    }

    private void HandleStateChanged(@SuppressWarnings("unused") NetworkInfo.DetailedState state) {
        // After the refactoring from a CheckBoxPreference to a Switch, this method is useless since
        // there is nowhere to display a summary.
        // This code is kept in case a future change re-introduces an associated text.
        /*
        // WifiInfo is valid if and only if Wi-Fi is enabled.
        // Here we use the state of the switch as an optimization.
        if (state != NULL && mSwitch->IsChecked()) {
            WifiInfo info = mWifiManager->GetConnectionInfo();
            if (info != NULL) {
                //SetSummary(Summary->Get(mContext, info->GetSSID(), state));
            }
        }
        */
    }

    //@Override
    CARAPI OnSwitchChanged(Switch switchView, Boolean isChecked) {
        //Do nothing if called as a result of a state machine event
        if (mStateMachineEvent) {
            return;
        }
        // Show toast message if Wi-Fi is not allowed in airplane mode
        if (isChecked && !WirelessSettings->IsRadioAllowed(mContext, Settings::Global::RADIO_WIFI)) {
            Toast->MakeText(mContext, R::string::wifi_in_airplane_mode, Toast.LENGTH_SHORT).Show();
            // Reset switch to off. No infinite check/listenenr loop.
            mSwitchBar->SetChecked(FALSE);
            return;
        }

        // Disable tethering if enabling Wifi
        Int32 wifiApState = mWifiManager->GetWifiApState();
        if (isChecked && ((wifiApState == WifiManager.WIFI_AP_STATE_ENABLING) ||
                (wifiApState == WifiManager.WIFI_AP_STATE_ENABLED))) {
            mWifiManager->SetWifiApEnabled(NULL, FALSE);
        }

        if (!mWifiManager->SetWifiEnabled(isChecked)) {
            // Error
            mSwitchBar->SetEnabled(TRUE);
            Toast->MakeText(mContext, R::string::wifi_error, Toast.LENGTH_SHORT).Show();
        }
    }
}
