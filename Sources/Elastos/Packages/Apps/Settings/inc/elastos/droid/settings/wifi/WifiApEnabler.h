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

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IWirelessSettings;

using Elastos::Utility::IArrayList;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::Wifi::ISupplicantState;
using Elastos::Droid::Net::Wifi::IWifiConfiguration;
using Elastos::Droid::Net::Wifi::IWifiInfo;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::IToast;

public class WifiApEnabler {
    private final Context mContext;
    private final SwitchPreference mSwitch;
    private final CharSequence mOriginalSummary;

    private WifiManager mWifiManager;
    private final IntentFilter mIntentFilter;

    ConnectivityManager mCm;
    private String[] mWifiRegexs;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (WifiManager.WIFI_AP_STATE_CHANGED_ACTION->Equals(action)) {
                HandleWifiApStateChanged(intent->GetIntExtra(
                        WifiManager.EXTRA_WIFI_AP_STATE, WifiManager.WIFI_AP_STATE_FAILED));
            } else if (ConnectivityManager.ACTION_TETHER_STATE_CHANGED->Equals(action)) {
                ArrayList<String> available = intent->GetStringArrayListExtra(
                        ConnectivityManager.EXTRA_AVAILABLE_TETHER);
                ArrayList<String> active = intent->GetStringArrayListExtra(
                        ConnectivityManager.EXTRA_ACTIVE_TETHER);
                ArrayList<String> errored = intent->GetStringArrayListExtra(
                        ConnectivityManager.EXTRA_ERRORED_TETHER);
                UpdateTetherState(available->ToArray(), active->ToArray(), errored->ToArray());
            } else if (IIntent::ACTION_AIRPLANE_MODE_CHANGED->Equals(action)) {
                EnableWifiSwitch();
            }

        }
    };

    public WifiApEnabler(Context context, SwitchPreference switchPreference) {
        mContext = context;
        mSwitch = switchPreference;
        mOriginalSummary = switchPreference->GetSummary();
        switchPreference->SetPersistent(FALSE);

        mWifiManager = (WifiManager) context->GetSystemService(Context.WIFI_SERVICE);
        mCm = (ConnectivityManager)mContext->GetSystemService(Context.CONNECTIVITY_SERVICE);

        mWifiRegexs = mCm->GetTetherableWifiRegexs();

        mIntentFilter = new IntentFilter(WifiManager.WIFI_AP_STATE_CHANGED_ACTION);
        mIntentFilter->AddAction(ConnectivityManager.ACTION_TETHER_STATE_CHANGED);
        mIntentFilter->AddAction(IIntent::ACTION_AIRPLANE_MODE_CHANGED);
    }

    CARAPI Resume() {
        mContext->RegisterReceiver(mReceiver, mIntentFilter);
        EnableWifiSwitch();
    }

    CARAPI Pause() {
        mContext->UnregisterReceiver(mReceiver);
    }

    private void EnableWifiSwitch() {
        Boolean isAirplaneMode = Settings::Global::>GetInt(mContext->GetContentResolver(),
                Settings::Global::AIRPLANE_MODE_ON, 0) != 0;
        If(!isAirplaneMode) {
            mSwitch->SetEnabled(TRUE);
        } else {
            mSwitch->SetSummary(mOriginalSummary);
            mSwitch->SetEnabled(FALSE);
        }
    }

    CARAPI SetSoftapEnabled(Boolean enable) {
        final ContentResolver cr = mContext->GetContentResolver();
        /**
         * Disable Wifi if enabling tethering
         */
        Int32 wifiState = mWifiManager->GetWifiState();
        if (enable && ((wifiState == WifiManager.WIFI_STATE_ENABLING) ||
                    (wifiState == WifiManager.WIFI_STATE_ENABLED))) {
            mWifiManager->SetWifiEnabled(FALSE);
            Settings::Global::>PutInt(cr, Settings::Global::WIFI_SAVED_STATE, 1);
        }

        if (mWifiManager->SetWifiApEnabled(NULL, enable)) {
            /* Disable here, enabled on receiving success broadcast */
            mSwitch->SetEnabled(FALSE);
        } else {
            mSwitch->SetSummary(R::string::wifi_error);
        }

        /**
         *  If needed, restore Wifi on tether disable
         */
        if (!enable) {
            Int32 wifiSavedState = 0;
            try {
                wifiSavedState = Settings::Global::>GetInt(cr, Settings::Global::WIFI_SAVED_STATE);
            } catch (Settings.SettingNotFoundException e) {
                ;
            }
            if (wifiSavedState == 1) {
                mWifiManager->SetWifiEnabled(TRUE);
                Settings::Global::>PutInt(cr, Settings::Global::WIFI_SAVED_STATE, 0);
            }
        }
    }

    CARAPI UpdateConfigSummary(WifiConfiguration wifiConfig) {
        String s = mContext->GetString(
                R::string::wifi_tether_configure_ssid_default);
        mSwitch->SetSummary(String->Format(
                    mContext->GetString(R::string::wifi_tether_enabled_subtext),
                    (wifiConfig == NULL) ? s : wifiConfig.SSID));
    }

    private void UpdateTetherState(Object[] available, Object[] tethered, Object[] errored) {
        Boolean wifiTethered = FALSE;
        Boolean wifiErrored = FALSE;

        for (Object o : tethered) {
            String s = (String)o;
            for (String regex : mWifiRegexs) {
                if (s->Matches(regex)) wifiTethered = TRUE;
            }
        }
        for (Object o: errored) {
            String s = (String)o;
            for (String regex : mWifiRegexs) {
                if (s->Matches(regex)) wifiErrored = TRUE;
            }
        }

        if (wifiTethered) {
            WifiConfiguration wifiConfig = mWifiManager->GetWifiApConfiguration();
            UpdateConfigSummary(wifiConfig);
        } else if (wifiErrored) {
            mSwitch->SetSummary(R::string::wifi_error);
        }
    }

    private void HandleWifiApStateChanged(Int32 state) {
        switch (state) {
            case WifiManager.WIFI_AP_STATE_ENABLING:
                mSwitch->SetSummary(R::string::wifi_tether_starting);
                mSwitch->SetEnabled(FALSE);
                break;
            case WifiManager.WIFI_AP_STATE_ENABLED:
                /**
                 * Summary on enable is handled by tether
                 * broadcast notice
                 */
                mSwitch->SetChecked(TRUE);
                /* Doesnt need the airplane check */
                mSwitch->SetEnabled(TRUE);
                break;
            case WifiManager.WIFI_AP_STATE_DISABLING:
                mSwitch->SetSummary(R::string::wifi_tether_stopping);
                mSwitch->SetEnabled(FALSE);
                break;
            case WifiManager.WIFI_AP_STATE_DISABLED:
                mSwitch->SetChecked(FALSE);
                mSwitch->SetSummary(mOriginalSummary);
                EnableWifiSwitch();
                break;
            default:
                mSwitch->SetChecked(FALSE);
                mSwitch->SetSummary(R::string::wifi_error);
                EnableWifiSwitch();
        }
    }
}
