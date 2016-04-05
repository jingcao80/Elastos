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

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::App::IAlertActivity;
using Elastos::Droid::Internal::App::IAlertController;
using Elastos::Droid::Internal::Telephony::ISmsApplication;
using Elastos::Droid::Internal::Telephony::SmsApplication::ISmsApplicationData;
using Elastos::Droid::Settings::IR;

public class SmsDefaultDialog extends AlertActivity implements
        DialogInterface.OnClickListener {
    private ComponentName mNewDefault;
    private SmsApplicationData mNewSmsApplicationData;

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        Intent intent = GetIntent();
        String packageName = intent->GetStringExtra(Intents.EXTRA_PACKAGE_NAME);

        SetResult(RESULT_CANCELED);
        if (!BuildDialog(packageName)) {
            Finish();
        }
    }

    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        switch (which) {
            case BUTTON_POSITIVE:
                SmsApplication->SetDefaultApplication(mNewSmsApplicationData.mPackageName, this);
                SetResult(RESULT_OK);
                break;
            case BUTTON_NEGATIVE:
                break;
        }
    }

    private Boolean BuildDialog(String packageName) {
        TelephonyManager tm = (TelephonyManager)GetSystemService(Context.TELEPHONY_SERVICE);
        if (!tm->IsSmsCapable()) {
            // No phone, no SMS
            return FALSE;
        }

        mNewSmsApplicationData = SmsApplication->GetSmsApplicationData(packageName, this);
        if (mNewSmsApplicationData == NULL) {
            return FALSE;
        }

        SmsApplicationData oldSmsApplicationData = NULL;
        ComponentName oldSmsComponent = SmsApplication->GetDefaultSmsApplication(this, TRUE);
        if (oldSmsComponent != NULL) {
            oldSmsApplicationData =
                    SmsApplication->GetSmsApplicationData(oldSmsComponent->GetPackageName(), this);
            if (oldSmsApplicationData.mPackageName->Equals(mNewSmsApplicationData.mPackageName)) {
                return FALSE;
            }
        }

        // Compose dialog; get
        final AlertController.AlertParams p = mAlertParams;
        p.mTitle = GetString(R::string::sms_change_default_dialog_title);
        if (oldSmsApplicationData != NULL) {
            p.mMessage = GetString(R::string::sms_change_default_dialog_text,
                    mNewSmsApplicationData.mApplicationName,
                    oldSmsApplicationData.mApplicationName);
        } else {
            p.mMessage = GetString(R::string::sms_change_default_no_previous_dialog_text,
                    mNewSmsApplicationData.mApplicationName);
        }
        p.mPositiveButtonText = GetString(R::string::yes);
        p.mNegativeButtonText = GetString(R::string::no);
        p.mPositiveButtonListener = this;
        p.mNegativeButtonListener = this;
        SetupAlert();

        return TRUE;
    }
}