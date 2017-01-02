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

package com.android.settings.wifi;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Net::Wifi::IWifiConfiguration;
using Elastos::Utility::IList;

using Elastos::Droid::Settings::IR;


/**
 * Configuration details saved by the user on the WifiSettings screen
 */
public class WifiConfigInfo extends Activity {

    private TextView mConfigList;
    private WifiManager mWifiManager;

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        mWifiManager = (WifiManager) GetSystemService(WIFI_SERVICE);
        SetContentView(R.layout.wifi_config_info);
        mConfigList = (TextView) FindViewById(R.id.config_list);
    }

    //@Override
    protected void OnResume() {
        super->OnResume();
        if (mWifiManager->IsWifiEnabled()) {
            final List<WifiConfiguration> wifiConfigs = mWifiManager->GetConfiguredNetworks();
            StringBuffer configList  = new StringBuffer();
            for (Int32 i = wifiConfigs->Size() - 1; i >= 0; i--) {
                configList->Append(wifiConfigs->Get(i));
            }
            mConfigList->SetText(configList);
        } else {
            mConfigList->SetText(R::string::wifi_state_disabled);
        }
    }

}
