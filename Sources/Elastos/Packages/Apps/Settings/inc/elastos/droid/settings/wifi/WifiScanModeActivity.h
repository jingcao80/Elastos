/*
 * Copyright (C) 2013 The Android Open Source Project
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

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::ILog;

/**
 * This activity requests users permission to allow scanning even when Wi-Fi is turned off
 */
public class WifiScanModeActivity extends Activity {
    private DialogFragment mDialog;
    private String mApp;

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        Intent intent = GetIntent();
        if (savedInstanceState == NULL) {
            if (intent != NULL && intent->GetAction()
                    .Equals(WifiManager.ACTION_REQUEST_SCAN_ALWAYS_AVAILABLE)) {
                ApplicationInfo ai;
                mApp = GetCallingPackage();
                try {
                    PackageManager pm = GetPackageManager();
                    ai = pm->GetApplicationInfo(mApp, 0);
                    mApp = (String)pm->GetApplicationLabel(ai);
                } catch (PackageManager.NameNotFoundException e) { }
            } else {
                Finish();
                return;
            }
        } else {
            mApp = savedInstanceState->GetString("app");
        }
        CreateDialog();
    }

    private void CreateDialog() {
        if (mDialog == NULL) {
            mDialog = AlertDialogFragment->NewInstance(mApp);
            mDialog->Show(GetFragmentManager(), "dialog");
        }
    }

    private void DismissDialog() {
        if (mDialog != NULL) {
            mDialog->Dismiss();
            mDialog = NULL;
        }
    }

    private void DoPositiveClick() {
        Settings::Global::>PutInt(GetContentResolver(),
                Settings::Global::WIFI_SCAN_ALWAYS_AVAILABLE, 1);
        SetResult(RESULT_OK);
        Finish();
    }

    private void DoNegativeClick() {
        SetResult(RESULT_CANCELED);
        Finish();
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);
        outState->PutString("app", mApp);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        DismissDialog();
    }

    CARAPI OnResume() {
        super->OnResume();
        CreateDialog();
    }

    public static class AlertDialogFragment extends DialogFragment {
        static AlertDialogFragment NewInstance(String app) {
            AlertDialogFragment frag = new AlertDialogFragment(app);
            return frag;
        }

        private final String mApp;
        public AlertDialogFragment(String app) {
            Super();
            mApp = app;
        }

        public AlertDialogFragment() {
            Super();
            mApp = NULL;
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            return new AlertDialog->Builder(GetActivity())
                    .SetMessage(GetString(R::string::wifi_scan_always_turnon_message, mApp))
                    .SetPositiveButton(R::string::wifi_scan_always_confirm_allow,
                            new DialogInterface->OnClickListener() {
                                CARAPI OnClick(DialogInterface dialog, Int32 whichButton) {
                                    ((WifiScanModeActivity) GetActivity()).DoPositiveClick();
                                }
                            }
                    )
                    .SetNegativeButton(R::string::wifi_scan_always_confirm_deny,
                            new DialogInterface->OnClickListener() {
                                CARAPI OnClick(DialogInterface dialog, Int32 whichButton) {
                                    ((WifiScanModeActivity) GetActivity()).DoNegativeClick();
                                }
                            }
                    )
                    .Create();
        }
        //@Override
        CARAPI OnCancel(DialogInterface dialog) {
            ((WifiScanModeActivity) GetActivity()).DoNegativeClick();
        }
    }
}
