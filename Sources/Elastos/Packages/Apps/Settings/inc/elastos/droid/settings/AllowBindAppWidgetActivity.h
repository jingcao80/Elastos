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

package com.android.settings;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Appwidget::IAppWidgetManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::ICheckBox;

using Elastos::Droid::Internal::App::IAlertActivity;
using Elastos::Droid::Internal::App::IAlertController;

/**
 * This activity is displayed when an app launches the BIND_APPWIDGET intent. This allows apps
 * that don't have the BIND_APPWIDGET permission to bind specific widgets.
 */
public class AllowBindAppWidgetActivity extends AlertActivity implements
        DialogInterface.OnClickListener {

    private CheckBox mAlwaysUse;
    private Int32 mAppWidgetId;
    private UserHandle mProfile;
    private ComponentName mComponentName;
    private String mCallingPackage;
    private AppWidgetManager mAppWidgetManager;

    // Indicates whether this activity was closed because of a click
    private Boolean mClicked;

    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        if (which == AlertDialog.BUTTON_POSITIVE) {
            // By default, set the result to cancelled
            SetResult(RESULT_CANCELED);
            if (mAppWidgetId != -1 && mComponentName != NULL && mCallingPackage != NULL) {
                try {
                    final Boolean bound = mAppWidgetManager->BindAppWidgetIdIfAllowed(mAppWidgetId,
                            mProfile, mComponentName, NULL);
                    if (bound) {
                        Intent result = new Intent();
                        result->PutExtra(AppWidgetManager.EXTRA_APPWIDGET_ID, mAppWidgetId);
                        SetResult(RESULT_OK, result);
                    }
                } catch (Exception e) {
                    Logger::V("BIND_APPWIDGET", "Error binding widget with id "
                            + mAppWidgetId + " and component " + mComponentName);
                }

                final Boolean alwaysAllowBind = mAlwaysUse->IsChecked();
                if (alwaysAllowBind != mAppWidgetManager->HasBindAppWidgetPermission(
                        mCallingPackage)) {
                    mAppWidgetManager->SetBindAppWidgetPermission(mCallingPackage,
                            alwaysAllowBind);
                }
            }
        }
        Finish();
    }

    protected void OnPause() {
        if (IsDestroyed() && !mClicked) {
            SetResult(RESULT_CANCELED);
        }
        super->OnDestroy();
    }

    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        Intent intent = GetIntent();
        CharSequence label = "";
        if (intent != NULL) {
            try {
                mAppWidgetId = intent->GetIntExtra(AppWidgetManager.EXTRA_APPWIDGET_ID, -1);
                mProfile = intent->GetParcelableExtra(
                        AppWidgetManager.EXTRA_APPWIDGET_PROVIDER_PROFILE);
                if (mProfile == NULL) {
                    mProfile = android.os.Process->MyUserHandle();
                }
                mComponentName =
                        intent->GetParcelableExtra(AppWidgetManager.EXTRA_APPWIDGET_PROVIDER);
                mCallingPackage = GetCallingPackage();
                PackageManager pm = GetPackageManager();
                ApplicationInfo ai = pm->GetApplicationInfo(mCallingPackage, 0);
                label = pm->GetApplicationLabel(ai);
            } catch (Exception e) {
                mAppWidgetId = -1;
                mComponentName = NULL;
                mCallingPackage = NULL;
                Logger::V("BIND_APPWIDGET", "Error getting parameters");
                SetResult(RESULT_CANCELED);
                Finish();
                return;
            }
        }
        AlertController.AlertParams ap = mAlertParams;
        ap.mTitle = GetString(R::string::allow_bind_app_widget_activity_allow_bind_title);
        ap.mMessage = GetString(R::string::allow_bind_app_widget_activity_allow_bind, label);
        ap.mPositiveButtonText = GetString(R::string::create);
        ap.mNegativeButtonText = GetString(android.R::string::cancel);
        ap.mPositiveButtonListener = this;
        ap.mNegativeButtonListener = this;
        LayoutInflater inflater =
                (LayoutInflater) GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        ap.mView = inflater->Inflate(R.layout.always_use_checkbox, NULL);
        mAlwaysUse = (CheckBox) ap.mView->FindViewById(R.id.alwaysUse);
        mAlwaysUse->SetText(GetString(R::string::allow_bind_app_widget_activity_always_allow_bind, label));

        mAlwaysUse->SetPadding(mAlwaysUse->GetPaddingLeft(),
                mAlwaysUse->GetPaddingTop(),
                mAlwaysUse->GetPaddingRight(),
                (Int32) (mAlwaysUse->GetPaddingBottom() +
                        GetResources()->GetDimension(R.dimen.bind_app_widget_dialog_checkbox_bottom_padding)));

        mAppWidgetManager = AppWidgetManager->GetInstance(this);
        mAlwaysUse->SetChecked(mAppWidgetManager->HasBindAppWidgetPermission(mCallingPackage,
                mProfile->GetIdentifier()));

        SetupAlert();
    }
}
