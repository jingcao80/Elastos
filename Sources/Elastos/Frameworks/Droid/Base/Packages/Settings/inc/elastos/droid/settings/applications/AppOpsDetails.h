/**
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package com.android.settings.applications;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPermissionGroupInfo;
using Elastos::Droid::Content::Pm::IPermissionInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::IList;

public class AppOpsDetails extends Fragment {
    static const String TAG = "AppOpsDetails";

    public static const String ARG_PACKAGE_NAME = "package";

    private AppOpsState mState;
    private PackageManager mPm;
    private AppOpsManager mAppOps;
    private PackageInfo mPackageInfo;
    private LayoutInflater mInflater;
    private View mRootView;
    private TextView mAppVersion;
    private LinearLayout mOperationsSection;

    // Utility method to set application label and icon.
    private void SetAppLabelAndIcon(PackageInfo pkgInfo) {
        final View appSnippet = mRootView->FindViewById(R.id.app_snippet);
        appSnippet->SetPaddingRelative(0, appSnippet->GetPaddingTop(), 0, appSnippet->GetPaddingBottom());

        ImageView icon = (ImageView) appSnippet->FindViewById(R.id.app_icon);
        icon->SetImageDrawable(mPm->GetApplicationIcon(pkgInfo.applicationInfo));
        // Set application name.
        TextView label = (TextView) appSnippet->FindViewById(R.id.app_name);
        label->SetText(mPm->GetApplicationLabel(pkgInfo.applicationInfo));
        // Version number of application
        mAppVersion = (TextView) appSnippet->FindViewById(R.id.app_size);

        if (pkgInfo.versionName != NULL) {
            mAppVersion->SetVisibility(View.VISIBLE);
            mAppVersion->SetText(GetActivity()->GetString(R::string::version_text,
                    String->ValueOf(pkgInfo.versionName)));
        } else {
            mAppVersion->SetVisibility(View.INVISIBLE);
        }
    }

    private String RetrieveAppEntry() {
        final Bundle args = GetArguments();
        String packageName = (args != NULL) ? args->GetString(ARG_PACKAGE_NAME) : NULL;
        if (packageName == NULL) {
            Intent intent = (args == NULL) ?
                    GetActivity()->GetIntent() : (Intent) args->GetParcelable("intent");
            if (intent != NULL) {
                packageName = intent->GetData()->GetSchemeSpecificPart();
            }
        }
        try {
            mPackageInfo = mPm->GetPackageInfo(packageName,
                    PackageManager.GET_DISABLED_COMPONENTS |
                    PackageManager.GET_UNINSTALLED_PACKAGES);
        } catch (NameNotFoundException e) {
            Logger::E(TAG, "Exception when retrieving package:" + packageName, e);
            mPackageInfo = NULL;
        }

        return packageName;
    }

    private Boolean RefreshUi() {
        if (mPackageInfo == NULL) {
            return FALSE;
        }

        SetAppLabelAndIcon(mPackageInfo);

        Resources res = GetActivity()->GetResources();

        mOperationsSection->RemoveAllViews();
        String lastPermGroup = "";
        for (AppOpsState.OpsTemplate tpl : AppOpsState.ALL_TEMPLATES) {
            List<AppOpsState.AppOpEntry> entries = mState->BuildState(tpl,
                    mPackageInfo.applicationInfo.uid, mPackageInfo.packageName);
            for (final AppOpsState.AppOpEntry entry : entries) {
                final AppOpsManager.OpEntry firstOp = entry->GetOpEntry(0);
                final View view = mInflater->Inflate(R.layout.app_ops_details_item,
                        mOperationsSection, FALSE);
                mOperationsSection->AddView(view);
                String perm = AppOpsManager->OpToPermission(firstOp->GetOp());
                if (perm != NULL) {
                    try {
                        PermissionInfo pi = mPm->GetPermissionInfo(perm, 0);
                        if (pi.group != NULL && !lastPermGroup->Equals(pi.group)) {
                            lastPermGroup = pi.group;
                            PermissionGroupInfo pgi = mPm->GetPermissionGroupInfo(pi.group, 0);
                            if (pgi.icon != 0) {
                                ((ImageView)view->FindViewById(R.id.op_icon)).SetImageDrawable(
                                        pgi->LoadIcon(mPm));
                            }
                        }
                    } catch (NameNotFoundException e) {
                    }
                }
                ((TextView)view->FindViewById(R.id.op_name)).SetText(
                        entry->GetSwitchText(mState));
                ((TextView)view->FindViewById(R.id.op_time)).SetText(
                        entry->GetTimeText(res, TRUE));
                Switch sw = (Switch)view->FindViewById(R.id.switchWidget);
                final Int32 switchOp = AppOpsManager->OpToSwitch(firstOp->GetOp());
                sw->SetChecked(mAppOps->CheckOp(switchOp, entry->GetPackageOps()->GetUid(),
                        entry->GetPackageOps()->GetPackageName()) == AppOpsManager.MODE_ALLOWED);
                sw->SetOnCheckedChangeListener(new Switch->OnCheckedChangeListener() {
                    //@Override
                    CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
                        mAppOps->SetMode(switchOp, entry->GetPackageOps()->GetUid(),
                                entry->GetPackageOps()->GetPackageName(), isChecked
                                ? AppOpsManager.MODE_ALLOWED : AppOpsManager.MODE_IGNORED);
                    }
                });
            }
        }

        return TRUE;
    }

    private void SetIntentAndFinish(Boolean finish, Boolean appChanged) {
        Intent intent = new Intent();
        intent->PutExtra(ManageApplications.APP_CHG, appChanged);
        SettingsActivity sa = (SettingsActivity)GetActivity();
        sa->FinishPreferencePanel(this, Activity.RESULT_OK, intent);
    }

    /** Called when the activity is first created. */
    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mState = new AppOpsState(GetActivity());
        mPm = GetActivity()->GetPackageManager();
        mInflater = (LayoutInflater)GetActivity()->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mAppOps = (AppOpsManager)GetActivity()->GetSystemService(Context.APP_OPS_SERVICE);

        RetrieveAppEntry();

        SetHasOptionsMenu(TRUE);
    }

    //@Override
    public View OnCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        final View view = inflater->Inflate(R.layout.app_ops_details, container, FALSE);
        Utils->PrepareCustomPreferencesList(container, view, view, FALSE);

        mRootView = view;
        mOperationsSection = (LinearLayout)view->FindViewById(R.id.operations_section);
        return view;
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        if (!RefreshUi()) {
            SetIntentAndFinish(TRUE, TRUE);
        }
    }
}
