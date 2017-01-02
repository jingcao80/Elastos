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
 * Copyright (C) 2012 The Android Open Source Project
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
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Settings::IButtonBarHandler;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Setupwizard::Navigationbar::ISetupWizardNavBar;
using Elastos::Droid::Setupwizard::Navigationbar::SetupWizardNavBar::INavigationBarListener;

public class WifiSetupActivity extends WifiPickerActivity
        implements ButtonBarHandler, NavigationBarListener {
    private static const String TAG = "WifiSetupActivity";

    private static const String EXTRA_ALLOW_SKIP = "allowSkip";
    private static const String EXTRA_USE_IMMERSIVE_MODE = "useImmersiveMode";

    // this Boolean extra specifies whether to auto finish when connection is established
    private static const String EXTRA_AUTO_FINISH_ON_CONNECT = "wifi_auto_finish_on_connect";

    // Whether auto finish is suspended until user connects to an access point
    private static const String EXTRA_REQUIRE_USER_NETWORK_SELECTION =
            "wifi_require_user_network_selection";

    // Extra containing the resource name of the theme to be used
    private static const String EXTRA_THEME = "theme";
    private static const String THEME_HOLO = "holo";
    private static const String THEME_HOLO_LIGHT = "holo_light";
    private static const String THEME_MATERIAL = "material";
    private static const String THEME_MATERIAL_LIGHT = "material_light";

    // Key for whether the user selected network in saved instance state bundle
    private static const String PARAM_USER_SELECTED_NETWORK = "userSelectedNetwork";

    // Activity result when pressing the Skip button
    private static const Int32 RESULT_SKIP = Activity.RESULT_FIRST_USER;

    // From WizardManager (must match constants maintained there)
    private static const String ACTION_NEXT = "com.android.wizard.NEXT";
    private static const String EXTRA_SCRIPT_URI = "scriptUri";
    private static const String EXTRA_ACTION_ID = "actionId";
    private static const String EXTRA_RESULT_CODE = "com.android.setupwizard.ResultCode";
    private static const Int32 NEXT_REQUEST = 10000;

    // Whether we allow skipping without a valid network connection
    private Boolean mAllowSkip = TRUE;
    // Whether to auto finish when the user selected a network and successfully connected
    private Boolean mAutoFinishOnConnection;
    // Whether the user connected to a network. This excludes the auto-connecting by the system.
    private Boolean mUserSelectedNetwork;
    // Whether the device is connected to WiFi
    private Boolean mWifiConnected;

    private SetupWizardNavBar mNavigationBar;

    private final IntentFilter mFilter = new IntentFilter(WifiManager.NETWORK_STATE_CHANGED_ACTION);
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            // Refresh the connection state with the latest connection info. Use the connection info
            // from ConnectivityManager instead of the one attached in the intent to make sure
            // we have the most up-to-date connection state. b/17511772
            RefreshConnectionState();
        }
    };

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        final Intent intent = GetIntent();

        mAutoFinishOnConnection = intent->GetBooleanExtra(EXTRA_AUTO_FINISH_ON_CONNECT, FALSE);
        mAllowSkip = intent->GetBooleanExtra(EXTRA_ALLOW_SKIP, TRUE);
        // Behave like the user already selected a network if we do not require selection
        mUserSelectedNetwork = !intent->GetBooleanExtra(EXTRA_REQUIRE_USER_NETWORK_SELECTION, FALSE);
    }

    //@Override
    protected void OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);
        outState->PutBoolean(PARAM_USER_SELECTED_NETWORK, mUserSelectedNetwork);
    }

    //@Override
    protected void OnRestoreInstanceState(Bundle savedInstanceState) {
        super->OnRestoreInstanceState(savedInstanceState);
        mUserSelectedNetwork = savedInstanceState->GetBoolean(PARAM_USER_SELECTED_NETWORK, TRUE);
    }

    private void RefreshConnectionState() {
        final ConnectivityManager connectivity = (ConnectivityManager)
                GetSystemService(Context.CONNECTIVITY_SERVICE);
        Boolean connected = connectivity != NULL &&
                connectivity->GetNetworkInfo(ConnectivityManager.TYPE_WIFI).IsConnected();
        RefreshConnectionState(connected);
    }

    private void RefreshConnectionState(Boolean connected) {
        mWifiConnected = connected;
        if (connected) {
            if (mAutoFinishOnConnection && mUserSelectedNetwork) {
                Logger::D(TAG, "Auto-finishing with connection");
                FinishOrNext(Activity.RESULT_OK);
                // Require a user selection before auto-finishing next time we are here. The user
                // can either connect to a different network or press "next" to proceed.
                mUserSelectedNetwork = FALSE;
            }
            if (mNavigationBar != NULL) {
                mNavigationBar->GetNextButton()->SetText(R::string::setup_wizard_next_button_label);
                mNavigationBar->GetNextButton()->SetEnabled(TRUE);
            }
        } else {
            if (mNavigationBar != NULL) {
                mNavigationBar->GetNextButton()->SetText(R::string::skip_label);
                mNavigationBar->GetNextButton()->SetEnabled(mAllowSkip);
            }
        }
    }

    /* package */ void NetworkSelected() {
        Logger::D(TAG, "Network selected by user");
        mUserSelectedNetwork = TRUE;
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        RegisterReceiver(mReceiver, mFilter);
        RefreshConnectionState();
    }

    //@Override
    CARAPI OnPause() {
        UnregisterReceiver(mReceiver);
        super->OnPause();
    }

    //@Override
    protected void OnApplyThemeResource(Resources.Theme theme, Int32 resid, Boolean first) {
        String themeName = GetIntent()->GetStringExtra(EXTRA_THEME);
        if (THEME_HOLO_LIGHT->EqualsIgnoreCase(themeName) ||
                THEME_MATERIAL_LIGHT->EqualsIgnoreCase(themeName)) {
            resid = R.style.SetupWizardWifiTheme_Light;
        } else if (THEME_HOLO->EqualsIgnoreCase(themeName) ||
                THEME_MATERIAL->EqualsIgnoreCase(themeName)) {
            resid = R.style.SetupWizardWifiTheme;
        }
        super->OnApplyThemeResource(theme, resid, first);
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        return WifiSettingsForSetupWizard.class->GetName()->Equals(fragmentName);
    }

    //@Override
    /* package */ Class<? extends PreferenceFragment> GetWifiSettingsClass() {
        return WifiSettingsForSetupWizard.class;
    }

    /**
     * Complete this activity and return the results to the caller. If using WizardManager, this
     * will invoke the next scripted action; otherwise, we simply finish.
     */
    CARAPI FinishOrNext(Int32 resultCode) {
        Logger::D(TAG, "finishOrNext resultCode=" + resultCode
                + " isUsingWizardManager=" + IsUsingWizardManager());
        if (IsUsingWizardManager()) {
            SendResultsToSetupWizard(resultCode);
        } else {
            SetResult(resultCode);
            Finish();
        }
    }

    private Boolean IsUsingWizardManager() {
        return GetIntent()->HasExtra(EXTRA_SCRIPT_URI);
    }

    /**
     * Send the results of this activity to WizardManager, which will then send out the next
     * scripted activity. WizardManager does not actually return an activity result, but if we
     * invoke WizardManager without requesting a result, the framework will choose not to issue a
     * call to onActivityResult with RESULT_CANCELED when navigating backward.
     */
    private void SendResultsToSetupWizard(Int32 resultCode) {
        final Intent intent = GetIntent();
        final Intent nextIntent = new IIntent::ACTION_NEXT);
        nextIntent->PutExtra(EXTRA_SCRIPT_URI, intent->GetStringExtra(EXTRA_SCRIPT_URI));
        nextIntent->PutExtra(EXTRA_ACTION_ID, intent->GetStringExtra(EXTRA_ACTION_ID));
        nextIntent->PutExtra(EXTRA_THEME, intent->GetStringExtra(EXTRA_THEME));
        nextIntent->PutExtra(EXTRA_RESULT_CODE, resultCode);
        StartActivityForResult(nextIntent, NEXT_REQUEST);
    }

    //@Override
    CARAPI OnNavigationBarCreated(final SetupWizardNavBar bar) {
        mNavigationBar = bar;
        final Boolean useImmersiveMode =
                GetIntent()->GetBooleanExtra(EXTRA_USE_IMMERSIVE_MODE, FALSE);
        bar->SetUseImmersiveMode(useImmersiveMode);
        if (useImmersiveMode) {
            GetWindow()->SetNavigationBarColor(Color.TRANSPARENT);
            GetWindow()->SetStatusBarColor(Color.TRANSPARENT);
        }
    }

    //@Override
    CARAPI OnNavigateBack() {
        OnBackPressed();
    }

    //@Override
    CARAPI OnNavigateNext() {
        if (mWifiConnected) {
            FinishOrNext(RESULT_OK);
        } else {
            // Warn of possible data charges if there is a network connection, or lack of updates
            // if there is none.
            final Int32 message = IsNetworkConnected() ? R::string::wifi_skipped_message :
                    R::string::wifi_and_mobile_skipped_message;
            WifiSkipDialog->NewInstance(message).Show(GetFragmentManager(), "dialog");
        }
    }

    /**
     * @return True if there is a valid network connection, whether it is via WiFi, mobile data or
     *         other means.
     */
    private Boolean IsNetworkConnected() {
        final ConnectivityManager connectivity = (ConnectivityManager)
                GetSystemService(Context.CONNECTIVITY_SERVICE);
        if (connectivity == NULL) {
            return FALSE;
        }
        final NetworkInfo info = connectivity->GetActiveNetworkInfo();
        return info != NULL && info->IsConnected();
    }

    public static class WifiSkipDialog extends DialogFragment {
        public static WifiSkipDialog NewInstance(Int32 messageRes) {
            final Bundle args = new Bundle();
            args->PutInt("messageRes", messageRes);
            final WifiSkipDialog dialog = new WifiSkipDialog();
            dialog->SetArguments(args);
            return dialog;
        }

        public WifiSkipDialog() {
            // no-arg constructor for fragment
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            Int32 messageRes = GetArguments()->GetInt("messageRes");
            return new AlertDialog->Builder(GetActivity())
                    .SetMessage(messageRes)
                    .SetCancelable(FALSE)
                    .SetNegativeButton(R::string::wifi_skip_anyway,
                            new DialogInterface->OnClickListener() {
                        //@Override
                        CARAPI OnClick(DialogInterface dialog, Int32 id) {
                            WifiSetupActivity activity = (WifiSetupActivity) GetActivity();
                            activity->FinishOrNext(RESULT_SKIP);
                        }
                    })
                    .SetPositiveButton(R::string::wifi_dont_skip,
                            new DialogInterface->OnClickListener() {
                        //@Override
                        CARAPI OnClick(DialogInterface dialog, Int32 id) {
                        }
                    })
                    .Create();
        }
    }
}
