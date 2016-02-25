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

package com.android.settings.location;

using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::Applications::IInstalledAppDetails;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * Retrieves the information of applications which accessed location recently.
 */
public class RecentLocationApps {
    private static const String TAG = RecentLocationApps.class->GetSimpleName();
    private static const String ANDROID_SYSTEM_PACKAGE_NAME = "android";

    private static const Int32 RECENT_TIME_INTERVAL_MILLIS = 15 * 60 * 1000;

    private final SettingsActivity mActivity;
    private final PackageManager mPackageManager;

    public RecentLocationApps(SettingsActivity activity) {
        mActivity = activity;
        mPackageManager = activity->GetPackageManager();
    }

    private class PackageEntryClickedListener
            implements Preference.OnPreferenceClickListener {
        private String mPackage;

        public PackageEntryClickedListener(String packageName) {
            mPackage = packageName;
        }

        //@Override
        public Boolean OnPreferenceClick(Preference preference) {
            // start new fragment to display extended information
            Bundle args = new Bundle();
            args->PutString(InstalledAppDetails.ARG_PACKAGE_NAME, mPackage);
            mActivity->StartPreferencePanel(InstalledAppDetails.class->GetName(), args,
                    R::string::application_info_label, NULL, NULL, 0);
            return TRUE;
        }
    }

    /**
     * Subclass of {@link Preference} to intercept views and allow content description to be set on
     * them for accessibility purposes.
     */
    private static class AccessiblePreference extends DimmableIconPreference {
        public CharSequence mContentDescription;

        public AccessiblePreference(Context context, CharSequence contentDescription) {
            Super(context);
            mContentDescription = contentDescription;
        }

        //@Override
        protected void OnBindView(View view) {
            super->OnBindView(view);
            if (mContentDescription != NULL) {
                final TextView titleView = (TextView) view->FindViewById(android.R.id.title);
                titleView->SetContentDescription(mContentDescription);
            }
        }
    }

    private AccessiblePreference CreateRecentLocationEntry(
            Drawable icon,
            CharSequence label,
            Boolean isHighBattery,
            CharSequence contentDescription,
            Preference.OnPreferenceClickListener listener) {
        AccessiblePreference pref = new AccessiblePreference(mActivity, contentDescription);
        pref->SetIcon(icon);
        pref->SetTitle(label);
        if (isHighBattery) {
            pref->SetSummary(R::string::location_high_battery_use);
        } else {
            pref->SetSummary(R::string::location_low_battery_use);
        }
        pref->SetOnPreferenceClickListener(listener);
        return pref;
    }

    /**
     * Fills a list of applications which queried location recently within specified time.
     */
    public List<Preference> GetAppList() {
        // Retrieve a location usage list from AppOps
        AppOpsManager aoManager =
                (AppOpsManager) mActivity->GetSystemService(Context.APP_OPS_SERVICE);
        List<AppOpsManager.PackageOps> appOps = aoManager->GetPackagesForOps(new Int32[] {
                AppOpsManager.OP_MONITOR_LOCATION, AppOpsManager.OP_MONITOR_HIGH_POWER_LOCATION, });

        // Process the AppOps list and generate a preference list.
        ArrayList<Preference> prefs = new ArrayList<Preference>();
        final Int64 now = System->CurrentTimeMillis();
        final UserManager um = (UserManager) mActivity->GetSystemService(Context.USER_SERVICE);
        final List<UserHandle> profiles = um->GetUserProfiles();

        final Int32 appOpsN = appOps->Size();
        for (Int32 i = 0; i < appOpsN; ++i) {
            AppOpsManager.PackageOps ops = appOps->Get(i);
            // Don't show the Android System in the list - it's not actionable for the user.
            // Also don't show apps belonging to background users except managed users.
            String packageName = ops->GetPackageName();
            Int32 uid = ops->GetUid();
            Int32 userId = UserHandle->GetUserId(uid);
            Boolean isAndroidOs =
                    (uid == Process.SYSTEM_UID) && ANDROID_SYSTEM_PACKAGE_NAME->Equals(packageName);
            if (isAndroidOs || !profiles->Contains(new UserHandle(userId))) {
                continue;
            }
            Preference preference = GetPreferenceFromOps(um, now, ops);
            if (preference != NULL) {
                prefs->Add(preference);
            }
        }

        return prefs;
    }

    /**
     * Creates a Preference entry for the given PackageOps.
     *
     * This method examines the time interval of the PackageOps first. If the PackageOps is older
     * than the designated interval, this method ignores the PackageOps object and returns NULL.
     * When the PackageOps is fresh enough, this method returns a Preference pointing to the App
     * Info page for that package.
     */
    private Preference GetPreferenceFromOps(final UserManager um, Int64 now,
            AppOpsManager.PackageOps ops) {
        String packageName = ops->GetPackageName();
        List<AppOpsManager.OpEntry> entries = ops->GetOps();
        Boolean highBattery = FALSE;
        Boolean normalBattery = FALSE;
        // Earliest time for a location request to end and still be shown in list.
        Int64 recentLocationCutoffTime = now - RECENT_TIME_INTERVAL_MILLIS;
        for (AppOpsManager.OpEntry entry : entries) {
            if (entry->IsRunning() || entry->GetTime() >= recentLocationCutoffTime) {
                switch (entry->GetOp()) {
                    case AppOpsManager.OP_MONITOR_LOCATION:
                        normalBattery = TRUE;
                        break;
                    case AppOpsManager.OP_MONITOR_HIGH_POWER_LOCATION:
                        highBattery = TRUE;
                        break;
                    default:
                        break;
                }
            }
        }

        if (!highBattery && !normalBattery) {
            if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                Logger::V(TAG, packageName + " hadn't used location within the time interval.");
            }
            return NULL;
        }

        // The package is fresh enough, continue.

        Int32 uid = ops->GetUid();
        Int32 userId = UserHandle->GetUserId(uid);

        AccessiblePreference preference = NULL;
        try {
            IPackageManager ipm = AppGlobals->GetPackageManager();
            ApplicationInfo appInfo =
                    ipm->GetApplicationInfo(packageName, PackageManager.GET_META_DATA, userId);
            if (appInfo == NULL) {
                Logger::W(TAG, "Null application info retrieved for package " + packageName
                        + ", userId " + userId);
                return NULL;
            }
            Resources res = mActivity->GetResources();

            final UserHandle userHandle = new UserHandle(userId);
            Drawable appIcon = mPackageManager->GetApplicationIcon(appInfo);
            Drawable icon = mPackageManager->GetUserBadgedIcon(appIcon, userHandle);
            CharSequence appLabel = mPackageManager->GetApplicationLabel(appInfo);
            CharSequence badgedAppLabel = mPackageManager->GetUserBadgedLabel(appLabel, userHandle);
            preference = CreateRecentLocationEntry(icon,
                    appLabel, highBattery, badgedAppLabel,
                    new PackageEntryClickedListener(packageName));
        } catch (RemoteException e) {
            Logger::W(TAG, "Error while retrieving application info for package " + packageName
                    + ", userId " + userId, e);
        }

        return preference;
    }
}
