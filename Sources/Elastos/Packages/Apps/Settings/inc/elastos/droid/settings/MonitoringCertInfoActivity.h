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

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::DialogInterface::IOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerGlobal;

/**
 * Activity that shows a dialog explaining that a CA cert is allowing someone to monitor network
 * traffic.
 */
public class MonitoringCertInfoActivity extends Activity implements OnClickListener {

    private Boolean hasDeviceOwner = FALSE;

    //@Override
    protected void OnCreate(Bundle savedStates) {
        super->OnCreate(savedStates);

        DevicePolicyManager dpm =
                (DevicePolicyManager) GetSystemService(Context.DEVICE_POLICY_SERVICE);

        final AlertDialog.Builder builder = new AlertDialog->Builder(this);
        builder->SetTitle(R::string::ssl_ca_cert_dialog_title);
        builder->SetCancelable(TRUE);
        hasDeviceOwner = dpm->GetDeviceOwner() != NULL;
        Int32 buttonLabel;
        if (hasDeviceOwner) {
            // Institutional case.  Show informational message.
            String message = this->GetResources()->GetString(R::string::ssl_ca_cert_info_message,
                    dpm->GetDeviceOwnerName());
            builder->SetMessage(message);
            buttonLabel = R::string::done_button;
        } else {
            // Consumer case.  Show scary warning.
            builder->SetIcon(android.R.drawable.stat_notify_error);
            builder->SetMessage(R::string::ssl_ca_cert_warning_message);
            buttonLabel = R::string::ssl_ca_cert_settings_button;
        }

        builder->SetPositiveButton(buttonLabel, this);

        final Dialog dialog = builder->Create();
        dialog->GetWindow()->SetType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
        try {
            WindowManagerGlobal->GetWindowManagerService()->DismissKeyguard();
        } catch (RemoteException e) {
        }
        dialog->SetOnCancelListener(new DialogInterface->OnCancelListener() {
            //@Override CARAPI OnCancel(DialogInterface dialog) {
                Finish();
            }
        });

        dialog->Show();
    }

    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        if (hasDeviceOwner) {
            Finish();
        } else {
            Intent intent =
                    new Intent(android.provider.Settings.ACTION_TRUSTED_CREDENTIALS_USER);
            intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
            StartActivity(intent);
            Finish();
        }
    }
}
