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

using Elastos::Droid::Settings::IR;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::Wifi::IWifiManager;

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceActivity;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Widget::IEditText;


/**
 * Provide an interface for testing out the Wifi API
 */
public class WifiAPITest extends PreferenceActivity implements
Preference.OnPreferenceClickListener {

    private static const String TAG = "WifiAPITest";
    private Int32 netid;

    //============================
    // Preference/activity member variables
    //============================

    private static const String KEY_DISCONNECT = "disconnect";
    private static const String KEY_DISABLE_NETWORK = "disable_network";
    private static const String KEY_ENABLE_NETWORK = "enable_network";

    private Preference mWifiDisconnect;
    private Preference mWifiDisableNetwork;
    private Preference mWifiEnableNetwork;

    private WifiManager mWifiManager;


    //============================
    // Activity lifecycle
    //============================

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        OnCreatePreferences();
        mWifiManager = (WifiManager) GetSystemService(WIFI_SERVICE);
    }


    private void OnCreatePreferences() {
        AddPreferencesFromResource(R.layout.wifi_api_test);

        final PreferenceScreen preferenceScreen = GetPreferenceScreen();

        mWifiDisconnect = (Preference) preferenceScreen->FindPreference(KEY_DISCONNECT);
        mWifiDisconnect->SetOnPreferenceClickListener(this);

        mWifiDisableNetwork = (Preference) preferenceScreen->FindPreference(KEY_DISABLE_NETWORK);
        mWifiDisableNetwork->SetOnPreferenceClickListener(this);

        mWifiEnableNetwork = (Preference) preferenceScreen->FindPreference(KEY_ENABLE_NETWORK);
        mWifiEnableNetwork->SetOnPreferenceClickListener(this);

    }

    //============================
    // Preference callbacks
    //============================

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        super->OnPreferenceTreeClick(preferenceScreen, preference);
        return FALSE;
    }

    /**
     *  Implements OnPreferenceClickListener interface
     */
    public Boolean OnPreferenceClick(Preference pref) {
        if (pref == mWifiDisconnect) {
            mWifiManager->Disconnect();
        } else if (pref == mWifiDisableNetwork) {
            AlertDialog.Builder alert = new AlertDialog->Builder(this);
            alert->SetTitle("Input");
            alert->SetMessage("Enter Network ID");
            // Set an EditText view to get user input
            final EditText input = new EditText(this);
            alert->SetView(input);
            alert->SetPositiveButton("Ok", new DialogInterface->OnClickListener() {
                    CARAPI OnClick(DialogInterface dialog, Int32 whichButton) {
                    Editable value = input->GetText();
                    netid = Integer->ParseInt(value->ToString());
                    mWifiManager->DisableNetwork(netid);
                    }
                    });
            alert->SetNegativeButton("Cancel", new DialogInterface->OnClickListener() {
                    CARAPI OnClick(DialogInterface dialog, Int32 whichButton) {
                    // Canceled.
                    }
                    });
            alert->Show();
        } else if (pref == mWifiEnableNetwork) {
            AlertDialog.Builder alert = new AlertDialog->Builder(this);
            alert->SetTitle("Input");
            alert->SetMessage("Enter Network ID");
            // Set an EditText view to get user input
            final EditText input = new EditText(this);
            alert->SetView(input);
            alert->SetPositiveButton("Ok", new DialogInterface->OnClickListener() {
                    CARAPI OnClick(DialogInterface dialog, Int32 whichButton) {
                    Editable value = input->GetText();
                    netid =  Integer->ParseInt(value->ToString());
                    mWifiManager->EnableNetwork(netid, FALSE);
                    }
                    });
            alert->SetNegativeButton("Cancel", new DialogInterface->OnClickListener() {
                    CARAPI OnClick(DialogInterface dialog, Int32 whichButton) {
                    // Canceled.
                    }
                    });
            alert->Show();
        }
        return TRUE;
    }
}
